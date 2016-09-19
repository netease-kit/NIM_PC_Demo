#include "main.h"
#include "resource.h"
#include "gui/login/login_form.h"
#include "gui/main/main_form.h"
#include "app_dump.h"
#include "base/util/at_exit.h"
#include "shared/xml_util.h"
#include "base/util/string_number_conversions.h"
#include "callback/chatroom_callback.h"

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(kThreadUI);
	PreMessageLoop();

	std::wstring theme_dir = QPath::GetAppPath();
	ui::GlobalManager::Startup(theme_dir + L"themes\\default", ui::CreateControlCallback());

	nim_ui::UserConfig::GetInstance()->SetIcon(IDI_ICON);

	std::wstring app_crash = QCommand::Get(kCmdAppCrash);
	if( app_crash.empty() )
	{	
		nim_ui::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
	}
	else
	{
		std::wstring content(L"程序崩溃了，崩溃日志：");
		content.append(app_crash);

		MsgboxCallback cb = nbase::Bind(&MainThread::OnMsgBoxCallback, this, std::placeholders::_1);
		ShowMsgBox(NULL, content, cb, L"提示", L"打开", L"取消");
	}
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();

	PostMessageLoop();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();

	nim_chatroom::ChatRoom::Cleanup();
}

void MainThread::PreMessageLoop()
{
	misc_thread_.reset( new MiscThread(kThreadGlobalMisc, "Global Misc Thread") );
	misc_thread_->Start();

	screen_capture_thread_.reset(new MiscThread(kThreadScreenCapture, "screen capture"));
	screen_capture_thread_->Start();

	db_thread_.reset( new DBThread(kThreadDatabase, "Database Thread") );
	db_thread_->Start();
}

void MainThread::PostMessageLoop()
{
	misc_thread_->Stop();
	misc_thread_.reset(NULL);

	screen_capture_thread_->Stop();
	screen_capture_thread_.reset(NULL);

	db_thread_->Stop();
	db_thread_.reset(NULL);
}

void MainThread::OnMsgBoxCallback( MsgBoxRet ret )
{
	if(ret == MB_YES)
	{
		std::wstring dir = QPath::GetNimAppDataDir();
		QCommand::AppStartWidthCommand(dir, L"");
	}

	nim_ui::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
}

/**
* 全局函数，初始化云信。包括读取应用服务器地址，载入云信sdk，初始化安装目录和用户目录，注册收到推送时执行的回调函数。
* @return void 无返回值
*/
static void InitNim()
{
	std::wstring server_conf_path = QPath::GetAppPath();
	server_conf_path.append(L"server_conf.txt");
	nim::SDKConfig config;
	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, server_conf_path))
	{
		TiXmlElement* root = document.RootElement();
		if (root)
		{
			bool use_private_server = false;
			Json::Value srv_config;
			if (auto pchar = root->Attribute("kNIMLbsAddress")) {
				config.lbs_address_ = pchar;
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMNosLbsAddress")) {
				config.nos_lbs_address_ = pchar;
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMDefaultLinkAddress")) {
				config.default_link_address_.push_back(pchar);
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMDefaultNosUploadAddress")) {
				config.default_nos_upload_address_.push_back(pchar);
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMDefaultNosDownloadAddress")) {
				config.default_nos_download_address_.push_back(pchar);
			}
			if (auto pchar = root->Attribute("kNIMDefaultNosAccessAddress")) {
				config.default_nos_access_address_.push_back(pchar);
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMRsaPublicKeyModule")) {
				config.rsa_public_key_module_ = pchar;
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMRsaVersion")) {
				nbase::StringToInt((std::string)pchar, &config.rsa_version_);
				use_private_server = true;
			}
			if (auto pchar = root->Attribute("kNIMSDKLogLevel")){
				int log_level = 5;
				nbase::StringToInt((std::string)pchar, &log_level);
				config.sdk_log_level_ = (nim::NIMSDKLogLevel)log_level;
			}
			if (auto pchar = root->Attribute("kNIMPreloadImageQuality")){
				int quality = -1;
				nbase::StringToInt((std::string)pchar, &quality);
				config.preload_image_quality_ = quality;
			}
			if (auto pchar = root->Attribute("kNIMPreloadImageResize")){
				config.preload_image_resize_ = (std::string)pchar;
			}
			config.use_private_server_ = use_private_server;
		}
	}

	//sdk能力参数（必填）
	config.database_encrypt_key_ = "Netease"; //string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）

	bool ret = nim::Client::Init("Netease", "", config); // 载入云信sdk，初始化安装目录和用户目录
	assert(ret);
	ret = nim_chatroom::ChatRoom::Init();
	assert(ret);

	nim_ui::InitManager::GetInstance()->InitUiKit();
	nim_chatroom::ChatroomCallback::InitChatroomCallback();
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	nbase::AtExitManager at_manager;

	CComModule _Module;
	_Module.Init(NULL, hInst);

	_wsetlocale(LC_ALL, L"chs");

#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(L"Command:\n%s\n\n", lpszCmdLine);
#endif

	srand( (unsigned int) time(NULL) );

	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	QCommand::ParseCommand(lpszCmdLine);

	HRESULT hr = ::OleInitialize(NULL);
	if( FAILED(hr) )
		return 0;

	InitNim(); // 初始化云信和UI组件

	{
		MainThread thread; // 创建主线程
		thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop); // 执行主线程循环
	}
	QLOG_APP(L"exit ui loop");

	// 程序结束之前，清理云信sdk和UI组件
	nim_ui::InitManager::GetInstance()->CleanupUiKit();

	QLOG_APP(L"app exit");

	// 是否重新运行程序
	std::wstring restart = QCommand::Get(kCmdRestart);
	if( !restart.empty() )
	{
		std::wstring cmd;
		std::wstring acc = QCommand::Get(kCmdAccount);
		if( !acc.empty() )
			cmd.append( nbase::StringPrintf(L" /%s %s ", kCmdAccount.c_str(), acc.c_str()) );
		std::wstring exit_why = QCommand::Get(kCmdExitWhy);
		if( !exit_why.empty() )
			cmd.append( nbase::StringPrintf(L" /%s %s ", kCmdExitWhy.c_str(), exit_why.c_str()) );
		QCommand::RestartApp(cmd);
	}

	_Module.Term();
	::OleUninitialize();

	return 0;
}


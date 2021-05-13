#include "stdafx.h"
#include "nim_app.h"
#include "resource.h"
#include "app_dump.h"
#include "base/util/at_exit.h"
#include "base/util/string_number_conversions.h"
#include "gui/extern_ctrl/extern_ctrl_manager.h"
#include "shared/xml_util.h"
#include "gui/login/login_form.h"
#include "gui/main/main_form.h"
#include "callback/chatroom_callback.h"
#include "module/config/config_helper.h"
#include "cef/cef_module/manager/cef_manager.h"
#include "duilib/Utils/MultiLangSupport.h"
#include "nim_service\module\local\local_helper.h"
#include "tool_kits\ui_component\ui_kit\export\nim_ui_runtime_manager.h"
#include "app_sdk\app_config\app_sdk_config.h"
#include "shared/business_action_gateway/business_manager/business_manager.h"

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(kThreadUI);
	PreMessageLoop();

	std::wstring theme_dir = QPath::GetAppPath();
	bool adapt_api = ConfigHelper::GetInstance()->IsAdaptDpi();
	std::wstring language_res = nbase::UTF8ToUTF16(ConfigHelper::GetInstance()->GetLanguage());
	ui::LanguageSetting language_setting;
	language_setting.m_strResource = language_res;
	if (language_setting.m_strResource.compare(L"lang\\zh_CN") == 0)
		language_setting.m_enumType = ui::LanguageType::Simplified_Chinese;
	else if(language_setting.m_strResource.compare(L"lang\\en_US") == 0)
		language_setting.m_enumType = ui::LanguageType::American_English;
	else
		language_setting.m_enumType = ui::LanguageType::Simplified_Chinese;
	ui::GlobalManager::Startup(theme_dir + L"resources\\", ExternCtrlManager::CreateExternCtrl, adapt_api, 
		L"themes\\default", language_setting);
	nim_ui::UserConfig::GetInstance()->SetDefaultIcon(IDI_ICON);

	std::wstring app_crash = QCommand::Get(kCmdAppCrash);
	if (app_crash.empty())
	{
		nim_ui::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
	}
	else
	{
		std::wstring content = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_APP_DUMP_DUMP_TIP").c_str(), app_crash.c_str());
		MsgboxCallback cb = nbase::Bind(&MainThread::OnMsgBoxCallback, this, std::placeholders::_1,nbase::UTF16ToUTF8(app_crash));
		ShowMsgBox(NULL, cb, content, false, L"STRING_TIPS", true, L"STRID_APP_DUMP_OPEN", true, L"STRING_NO", true);
	}
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();

	PostMessageLoop();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();	
}

void MainThread::PreMessageLoop()
{
	misc_thread_.reset(new MiscThread(kThreadGlobalMisc, "Global Misc Thread"));
	misc_thread_->Start();

	screen_capture_thread_.reset(new MiscThread(kThreadScreenCapture, "screen capture"));
	screen_capture_thread_->Start();

	db_thread_.reset(new DBThread(kThreadDatabase, "Database Thread"));
	db_thread_->Start();

	app_sdk_thread_.reset(new MiscThread(kThreadApp, "App SDK Thread"));
	app_sdk_thread_->Start();

	nbase::BusinessManager::GetInstance()->Init();
}

void MainThread::PostMessageLoop()
{
	app_sdk_thread_->Stop();
	app_sdk_thread_.reset(NULL);

	misc_thread_->Stop();
	misc_thread_.reset(NULL);

	screen_capture_thread_->Stop();
	screen_capture_thread_.reset(NULL);

	db_thread_->Stop();
	db_thread_.reset(NULL);


}

void MainThread::OnMsgBoxCallback(MsgBoxRet ret, const std::string& dmp_path)
{
	if (ret == MB_YES)
	{
		std::string directory;
		shared::FilePathApartDirectory(dmp_path, directory);
		QCommand::AppStartWidthCommand(nbase::UTF8ToUTF16(directory), L"");
	}
	nim_ui::WindowsManager::SingletonShow<LoginForm>(LoginForm::kClassName);
}

/**
* 全局函数，初始化云信。包括读取应用服务器地址，载入云信sdk，初始化安装目录和用户目录，注册收到推送时执行的回调函数。
* @return bool是否初始化成功
*/
bool NimAPP::InitNim(const std::string& server_conf_file_path)
{
	nim::SDKConfig config;
	std::string app_key;
	InitGlobalConfig(server_conf_file_path,app_key,config);
	if (app_key.empty())
		app_key = app_sdk::AppSDKInterface::GetAppKey();
	//string（db key必填，目前只支持最多32个字符的加密密钥！建议使用32个字符）
#ifdef _DEBUG
	config.database_encrypt_key_ = "";
#endif

	
	std::wstring app_install;// = QPath::GetAppPath() + L"\\x64_dlls\\";
	bool ret = nim::Client::Init(app_key, "Netease", nbase::UTF16ToUTF8(app_install), config); // 载入云信sdk，初始化安装目录和用户目录
	if (ret)
	{
		nim_ui::RunTimeDataManager::GetInstance()->SetIMInited();
		nim::Client::SetCallbackFunction([](const StdClosure & task) {
			nbase::ThreadManager::PostTask(ThreadId::kThreadUI, task);
			});
	}
	assert(ret);
	//初始化聊天室
	{		
		ret = nim_chatroom::ChatRoom::Init(nbase::UTF16ToUTF8(app_install), "");
		if (ret)
		{
			nim_ui::RunTimeDataManager::GetInstance()->SetChatRoomInited();
#ifdef CPPWRAPPER_DLL
			nim_chatroom::ChatRoom::SetCallbackFunction([](const StdClosure & task) {
				nbase::ThreadManager::PostTask(ThreadId::kThreadUI, task);
				});
#endif
		}
		assert(ret);
	}
	// 初始化云信音视频
	ret = nim::VChat::Init(server_conf_file_path);
	if (ret)
	{
		nim_ui::RunTimeDataManager::GetInstance()->SetVChatInited();
	}
	assert(ret);
	
	// InitUiKit接口参数决定是否启用事件订阅模块，默认为false，如果是云信demo app则为true
	// 如果你的App开启了事件订阅功能，则此参数改为true
	//nim_ui::InitManager::GetInstance()->InitUiKit(app_sdk::AppSDKInterface::IsNimDemoAppKey(app_sdk::AppSDKInterface::GetAppKey())); 
	if (ret)
	{
		nim_ui::RunTimeDataManager::GetInstance()->SetSDKInited();
		nim_chatroom::ChatroomCallback::InitChatroomCallback();
		bool chatoom_msg_batch_report = false;
		if (app_sdk::AppSDKInterface::HasconfigValue("chatoom_msg_batch_report"))
			chatoom_msg_batch_report = (app_sdk::AppSDKInterface::GetConfigValue("chatoom_msg_batch_report").compare("0") != 0);
		nim_chatroom::ChatRoom::SetMsgsBatchReport(chatoom_msg_batch_report);
	}	
	else
	{
		CleanupSDKBeforLogin();
	}
	return ret;
}
void NimAPP::CleanupSDKBeforLogin()
{
	nim::VChat::Cleanup();
	nim_chatroom::ChatRoom::Cleanup();
	nim::Client::Cleanup2();	
}
int NimAPP::InitInstance(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	TCHAR module_path[MAX_PATH] = { 0 };	
	std::wstring nim_http_tool_path;
	if (GetModuleFileName(NULL, module_path, MAX_PATH) > 0)
	{
		std::wstring exe_path;
		exe_path = module_path;
		size_t pos = exe_path.find_last_of('\\');
		if (pos != std::wstring::npos)
			exe_path = exe_path.substr(0, pos + 1);
		nim_http_tool_path = exe_path + L"nim_tools_http_app.dll";
		std::wstring nim_http_tool_path_src = exe_path + L"nim_tools_http.dll";
		if (!nbase::FilePathIsExist(nim_http_tool_path, false))
			if (!nbase::CopyFile(nim_http_tool_path_src, nim_http_tool_path))
				nim_http_tool_path = L"";
		if (!nbase::FilePathIsExist(nim_http_tool_path, false))
				nim_http_tool_path = L"";
	}
	// 初始化云信http
	try {
		nim_http::Init(nim_http_tool_path);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "exception", 0);
	}
	
	g_need_restart_after_dump = true;
	return 0;
}
int NimAPP::ExitInstance()
{
	

	// 程序结束之前，清理云信sdk和UI组件
	nim_ui::InitManager::GetInstance()->CleanupUiKit();
	if (nim_ui::RunTimeDataManager::GetInstance()->IsSDKInited())
	{
		if(nim_ui::RunTimeDataManager::GetInstance()->IsChatRoomInited())
			nim_chatroom::ChatRoom::Cleanup();
		if (nim_ui::RunTimeDataManager::GetInstance()->IsVChatInited())
			nim::VChat::Cleanup();
		if (nim_ui::RunTimeDataManager::GetInstance()->IsIMInited())
			nim::Client::Cleanup2();
	}	
	nim_http::Uninit();

	nbase::BusinessManager::GetInstance()->UnInit();
	return 0;
}
//int NimAPP::InitRedistPackages()
//{
//	TCHAR path_envirom[4096] = { 0 };
//	GetEnvironmentVariable(L"path", path_envirom, 4096);
//
//	std::wstring redist_packages_path = QPath::GetAppPath();
//#ifdef _DEBUG
//	//cef_path += L"cef_debug"; // 现在即使在debug模式下也使用cef release版本的dll，为了屏蔽掉cef退出时的中断，如果不需要调试cef的功能不需要使用debug版本的dll
//	redist_packages_path += L"redist_packages";
//#else
//	redist_packages_path += L"redist_packages";
//#endif
//	if (!nbase::FilePathIsExist(redist_packages_path, true))
//	{
//		MessageBox(NULL, L"初始化 SDK环境不完整 ，可能导致SDK加载失败", L"提示", MB_OK);
//	}
//	std::wstring new_envirom(redist_packages_path);
//	new_envirom.append(L";").append(path_envirom);
//	return SetEnvironmentVariable(L"path", new_envirom.c_str()) != 0;
//}
int NimAPP::InitEnvironment(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	// 把cef dll文件的位置添加到程序的"path"环境变量中,这样可以把dll文件放到bin以外的目录，并且不需要手动频繁切换dll文件，这行代码必须写到main的开头
	nim_cef::CefManager::GetInstance()->AddCefDllToPath();
	_wsetlocale(LC_ALL, L"chs");
	srand((unsigned int)time(NULL));
	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	com_module_ = std::make_unique<CComModule>();
	com_module_->Init(NULL, hInst);	
	if (FAILED(::OleInitialize(NULL)))
		return 0;
	QCommand::ParseCommand(lpszCmdLine);
	//初始化cef
	CefSettings settings;
	if (!nim_cef::CefManager::GetInstance()->Initialize(QPath::GetNimAppDataDir(APPDATA_DIR), settings, atoi(GetConfigValue("kNIMCefOsrEnabled").c_str()) > 0))
		return 0;
	return 1;
}
void NimAPP::InitGlobalConfig(const std::string& server_conf_file_path,std::string& app_key, nim::SDKConfig& config)
{
	if (!server_conf_file_path.empty())
	{
		config.server_conf_file_path_ = server_conf_file_path;
	}
	else
	{
		std::wstring server_conf_path = QPath::GetAppPath();
		server_conf_path.append(L"global_conf.txt");
		if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMServerConfFilePath,false))
		{
			std::string nim_server_conf_file_path = GetConfigValue(nim::kNIMServerConfFilePath);
			if (nim_server_conf_file_path.empty())
				nim_server_conf_file_path = nbase::UTF16ToUTF8(QPath::GetAppPath().append(L"nim_server.conf"));
			if (nbase::FilePathIsExist(QPath::GetAppPath().append(L"nim_server.conf"), false))
				config.server_conf_file_path_ = nim_server_conf_file_path;
		}
	}
	if (!config.server_conf_file_path_.empty())
		app_sdk::AppSDKConfig::GetInstance()->SetAppConfigPath(config.server_conf_file_path_);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMAppKey))
		app_key = GetConfigValue(nim::kNIMAppKey);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMPreloadAttach))
		config.preload_attach_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMPreloadAttach).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMDataBaseEncryptKey))
		config.database_encrypt_key_ = app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMDataBaseEncryptKey);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMPreloadImageQuality))
		config.preload_image_quality_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMPreloadImageQuality).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMPreloadImageResize))
		config.preload_image_resize_ = app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMPreloadImageResize);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMPreloadAttachImageNameTemplate))
		config.preload_image_name_template_ = app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMPreloadAttachImageNameTemplate);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMSDKLogLevel))
		config.sdk_log_level_ = (nim::NIMSDKLogLevel)std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMSDKLogLevel).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMSyncSessionAck))
		config.sync_session_ack_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMSyncSessionAck).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMLoginRetryMaxTimes))
		config.login_max_retry_times_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMLoginRetryMaxTimes).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMUseHttps))
		config.use_https_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMUseHttps).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMTeamNotificationUnreadCount))
		config.team_notification_unread_count_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMTeamNotificationUnreadCount).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMVChatMissUnreadCount))
		config.vchat_miss_unread_count_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMVChatMissUnreadCount).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMResetUnreadCountWhenRecall))
		config.reset_unread_count_when_recall_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMResetUnreadCountWhenRecall).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMUploadSDKEventsAfterLogin))
		config.upload_sdk_events_after_login_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMUploadSDKEventsAfterLogin).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMAnimatedImageThumbnailEnabled))
		config.animated_image_thumbnail_enabled_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMAnimatedImageThumbnailEnabled).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMClientAntispam))
		config.client_antispam_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMClientAntispam).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMTeamMessageAckEnabled))
		config.team_msg_ack_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMTeamMessageAckEnabled).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMNeedUpdateLBSBeforRelogin))
		config.need_update_lbs_befor_relogin_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMNeedUpdateLBSBeforRelogin).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMCachingMarkreadEnabled))
		config.caching_markread_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMCachingMarkreadEnabled).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMCachingMarkreadTime))
		config.caching_markread_time_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMCachingMarkreadTime).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMCachingMarkreadCount))
		config.caching_markread_count_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMCachingMarkreadCount).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMUserDataFileLocalBackupFolder))
		config.user_datafile_localbackup_folder_ = app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMUserDataFileLocalBackupFolder);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMEnableUserDataFileLocalBackup))
		config.enable_user_datafile_backup_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMEnableUserDataFileLocalBackup).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMEnableUserDataFileLocalRestore))
		config.enable_user_datafile_restore_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMEnableUserDataFileLocalRestore).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMEnableUserDataFileDefRestoreProc))
		config.enable_user_datafile_defrestoreproc_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMEnableUserDataFileDefRestoreProc).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMDedicatedClusteFlag))
		config.dedicated_cluste_flag_ = (app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMDedicatedClusteFlag).compare("0") != 0);
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMNegoKeyNECA))
		config.nego_key_neca_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMNegoKeyNECA).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMCommNECA))
		config.comm_neca_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMCommNECA).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMIPProtVersion))
		config.ip_protocol_version_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMIPProtVersion).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMHandShakeType))
		config.hand_shake_type_ = std::atoi(app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMHandShakeType).c_str());
	if (app_sdk::AppSDKInterface::HasconfigValue(nim::kNIMPriorityUseCdnHost))
		config.hand_shake_type_ = app_sdk::AppSDKInterface::GetConfigValue(nim::kNIMPriorityUseCdnHost).compare("0") != 0;
}
int NimAPP::UninitEnvironment()
{
	com_module_->Term();
	com_module_.reset();
	::OleUninitialize();
	//清理cef
	nim_cef::CefManager::GetInstance()->UnInitialize();	
	return 0;
}
int NimAPP::Exec(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	if (InitEnvironment(hInst, hPrevInst, lpszCmdLine, nCmdShow) == 0)
		return 0;
	nbase::AtExitManager at_manager;	
	InitInstance(hInst, hPrevInst, lpszCmdLine, nCmdShow);
	// 创建主线程
	QLOG_APP(L"begin ui loop");
	MainThread().RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop); // 执行主线程循环
	QLOG_APP(L"exit ui loop");	
	UninitEnvironment();
	ExitInstance();
	QLOG_APP(L"app exit");
	// 是否重新运行程序
	CheckRestartApp();
	return 0;
}
void NimAPP::CheckRestartApp()
{
	std::wstring restart = QCommand::Get(kCmdRestart);
	if (!restart.empty())
	{
		std::wstring cmd;
		std::wstring acc = QCommand::Get(kCmdAccount);
		if (!acc.empty())
			cmd.append(nbase::StringPrintf(L" /%s %s ", kCmdAccount.c_str(), acc.c_str()));
		std::wstring exit_why = QCommand::Get(kCmdExitWhy);
		if (!exit_why.empty())
			cmd.append(nbase::StringPrintf(L" /%s %s ", kCmdExitWhy.c_str(), exit_why.c_str()));
		QCommand::RestartApp(cmd);
	}
}

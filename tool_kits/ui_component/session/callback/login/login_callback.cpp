#include "login_callback.h"
#include "export/nim_ui_all.h"
#include "gui/link/link_form.h"
#include "module/local/local_helper.h"
#include "module/login/login_manager.h"
#include "module/session/session_manager.h"
#include "module/service/user_service.h"
#include "util/user.h"
#include "shared/xml_util.h"

#include "gui/login/login_form.h"
#include "gui/main/main_form.h"

namespace nim_comp
{
void _InitUserFolder()
{
	nbase::CreateDirectory( GetUserDataPath() );
	nbase::CreateDirectory( GetUserImagePath() );
	nbase::CreateDirectory( GetUserAudioPath() );
}

void _InitLog()
{
#ifdef _DEBUG
	QLogImpl::GetInstance()->SetLogLevel(LV_PRO);
#else
	QLogImpl::GetInstance()->SetLogLevel(LV_APP);
#endif
	std::wstring dir = GetUserDataPath();
	QLogImpl::GetInstance()->SetLogFile(dir + kLogFile);
}

void _LogRobot()
{
	const long m2 = 2*1024*1024, m1 = 1024*1024;
	QLogImpl::GetInstance()->HalfTo(m2, m1);

	StdClosure task = nbase::Bind(&_LogRobot);
	nbase::ThreadManager::PostDelayedTask(kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(10));
}

//登陆之后的处理：比如读取数据
void _DoAfterLogin()
{
	TeamService::GetInstance()->QueryAllTeamInfo();

	StdClosure task = nbase::Bind(&_LogRobot);
	nbase::ThreadManager::PostDelayedTask(kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(1));
}

//退出程序前的处理：比如保存数据
void _DoBeforeAppExit()
{

}

//执行sdk退出函数
void NimLogout(nim::NIMLogoutType type = nim::kNIMLogoutAppExit)
{
	QLOG_APP(L"-----logout begin {0}-----") <<type;
	nim::Client::Logout( type, &LoginCallback::OnLogoutCallback );
}

void UILogoutCallback()
{
	if(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
	{
		LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);

		//依赖demo窗体
		LoginForm* login_form = dynamic_cast<LoginForm*>( WindowsManager::GetInstance()->GetWindow(LoginForm::kClassName, LoginForm::kClassName) );
		if(login_form)
			login_form->Reset();
	}
	else
	{
		PostQuitMessage(0);

		_DoBeforeAppExit();
	}
}

void LoginCallback::DoLogout(bool over, nim::NIMLogoutType type)
{
	QLOG_APP(L"DoLogout: {0} {1}") <<over <<type;

	LoginStatus status = LoginManager::GetInstance()->GetLoginStatus();
	if(status == LoginStatus_EXIT)
		return;
	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_EXIT);

	WindowsManager::GetInstance()->SetStopRegister(true);
	WindowsManager::GetInstance()->DestroyAllWindows();

	if(status == LoginStatus_NONE)
	{
		UILogoutCallback();
		return;
	}

	if(over)
	{
		if (type == nim::kNIMLogoutKickout || type == nim::kNIMLogoutRelogin)
		{
			QCommand::Set(kCmdAccount, nbase::UTF8ToUTF16(LoginManager::GetInstance()->GetAccount()));
			QCommand::Set(kCmdRestart, L"true");
			std::wstring param = nbase::StringPrintf(L"%d", type);
			QCommand::Set(kCmdExitWhy, param);
		}
		UILogoutCallback();
	}
	else
	{
		NimLogout(type);
	}
}

void LoginCallback::OnLogoutCallback(const std::string& json_params)
{
	QLOG_APP(L"OnLogoutCallback: {0}") <<json_params;
	QLOG_APP(L"-----logout end-----");
	UILogoutCallback();
}

void LoginCallback::OnKickoutCallback(const std::string& json_params)
{
	QLOG_APP(L"OnKickoutCallback: {0}") <<json_params;
	DoLogout(true, nim::kNIMLogoutKickout);
}

void LoginCallback::OnDisconnectCallback(const std::string& json_params)
{
	QLOG_APP(L"OnDisconnectCallback: {0}") <<json_params;
}


void LoginCallback::DoLogin( std::string user, std::string pass )
{
	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_NONE);
	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_LOGIN);

	LoginManager::GetInstance()->SetAccount(user);
	std::string pass_md5 = QString::GetMd5(pass);
	LoginManager::GetInstance()->SetPassword(pass_md5);

	_InitUserFolder();
	_InitLog();
	{
		int ver = 0;
		std::wstring vf;
		LocalHelper::GetAppLocalVersion(ver, vf);
		QLOG_APP(L"App Version {0}") << ver;
		QLOG_APP(L"Account {0}") << LoginManager::GetInstance()->GetAccount();
		QLOG_APP(L"UI ThreadId {0}") << GetCurrentThreadId();
		QLOG_APP(L"-----login begin-----");
	}

	//appkey是应用的标识，不同应用之间的数据（用户、消息、群组等）是完全隔离的。如需打网易云信Demo包，请勿修改appkey，
	//开发自己的应用时，请替换为自己的appkey，并请对应更换Demo代码中的获取好友列表、个人信息等网易云信SDK未提供的接口。
	std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";
	std::string new_app_key = GetConfigValue(g_AppKey);
	if (!new_app_key.empty())
	{
		app_key = new_app_key;
	}
	nim::Client::Login(app_key, LoginManager::GetInstance()->GetAccount(), LoginManager::GetInstance()->GetPassword(), &LoginCallback::OnLoginCallback, nullptr);
}

void LoginCallback::ReLogin()
{
	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_NONE);
	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_LOGIN);

	QLOG_APP(L"-----relogin begin-----");
	nim::Client::Relogin();
}

void LoginCallback::CacelLogin()
{
	assert(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_LOGIN);
	LoginManager::GetInstance()->SetLoginStatus(LoginStatus_CANCEL);
	QLOG_APP(L"-----login cancel begin-----");
}

void LoginCallback::UILoginCallback(int code, bool relogin)
{
	if(relogin)
	{
		QLOG_APP(L"-----relogin end {0}-----") <<code;

		if (code == nim::kNIMResSuccess)
		{
			LoginManager::GetInstance()->SetLoginStatus(LoginStatus_SUCCESS);
			LoginManager::GetInstance()->SetLinkActive(true);
		}
		else
		{
			LoginManager::GetInstance()->SetLoginStatus(LoginStatus_NONE);
			LoginManager::GetInstance()->SetLinkActive(false);

			ShowLinkForm();
		}
	}
	else
	{
		QLOG_APP(L"-----login end {0}-----") <<code;
//依赖demo部分
// 		if (nim_ui::LoginManager::GetInstance()->IsLoginFormValid())
// 		{
// 		 	if(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
// 		 	{
// 		 		QLOG_APP(L"-----login cancel end-----");
// 		 		if (code == nim::kNIMResSuccess)
// 		 			NimLogout(nim::kNIMLogoutChangeAccout);
// 		 		else
// 		 			UILogoutCallback();
// 		 		return;
// 		 	}
// 		 	else
// 		 		LoginManager::GetInstance()->SetLoginStatus(code == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);
// 		 
// 		 	if (code == nim::kNIMResSuccess)
// 		 	{
// 		 		//LoginManager::GetInstance()->GetLoginData()->status_ = kLoginDataStatusValid;
// 		 		//LoginManager::GetInstance()->GetLoginData()->remember_ = remember_pwd_ckb_->IsSelected() ? 1 : 0;
// 		 		//LoginManager::GetInstance()->GetLoginData()->auto_login_ = auto_login_ckb_->IsSelected() ? 1 : 0;
// 		 		//LoginManager::GetInstance()->SaveLoginData();
// 		 
// 				nim_ui::LoginManager::GetInstance()->InvokeHideWindow();
// 		 
// 		 		std::string acc = LoginManager::GetInstance()->GetAccount();
// 		 		assert(!acc.empty());
// 		 		std::wstring app_data_audio_path = QPath::GetUserAppDataDir(acc);
// 		 		nbase::CreateDirectory(app_data_audio_path);
// 		 		std::string res_audio_path = nbase::UTF16ToUTF8(app_data_audio_path);
// 		 		bool ret = nim::Audio::Init(res_audio_path);
// 		 		assert(ret);
// 		 		//audio
// 		 		nim::Audio::RegStartPlayCb(&AudioCallback::OnPlayAudioCallback);
// 		 		nim::Audio::RegStopPlayCb(&AudioCallback::OnStopAudioCallback);
// 		 
// 		 		_DoAfterLogin();
// 		 		// 登录成功，显示主界面
// 				nim_ui::WindowsManager::GetInstance()->InvokeShowMainForm();		 
// 				nim_ui::LoginManager::GetInstance()->InvokeDestroyWindow();
// 		 	}
// 		 	else
// 		 	{
// 				nim_ui::LoginManager::GetInstance()->InvokeLoginResult(code);
// 		 	}
// 		}
// 		else
// 		{
// 		 	QLOG_APP(L"login form has been closed");
// 		 	LoginManager::GetInstance()->SetLoginStatus(code == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);
// 		 	LoginCallback::DoLogout(false);
// 		}
		LoginForm* login_form = dynamic_cast<LoginForm*>( WindowsManager::GetInstance()->GetWindow(LoginForm::kClassName, LoginForm::kClassName) );
		if(login_form)
		{
			if(LoginManager::GetInstance()->GetLoginStatus() == LoginStatus_CANCEL)
			{
				QLOG_APP(L"-----login cancel end-----");
				if (code == nim::kNIMResSuccess)
					NimLogout(nim::kNIMLogoutChangeAccout);
				else
					UILogoutCallback();
				return;
			}
			else
				LoginManager::GetInstance()->SetLoginStatus(code == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);

			if (code == nim::kNIMResSuccess)
			{
				//LoginManager::GetInstance()->GetLoginData()->status_ = kLoginDataStatusValid;
				//LoginManager::GetInstance()->GetLoginData()->remember_ = remember_pwd_ckb_->IsSelected() ? 1 : 0;
				//LoginManager::GetInstance()->GetLoginData()->auto_login_ = auto_login_ckb_->IsSelected() ? 1 : 0;
				//LoginManager::GetInstance()->SaveLoginData();

				login_form->ShowWindow(false, false);

				std::string acc = LoginManager::GetInstance()->GetAccount();
				assert(!acc.empty());
				std::wstring app_data_audio_path = QPath::GetUserAppDataDir(acc);
				nbase::CreateDirectory(app_data_audio_path);
				std::string res_audio_path = nbase::UTF16ToUTF8(app_data_audio_path);
				bool ret = nim::Audio::Init(res_audio_path);
				assert(ret);
				//audio
				nim::Audio::RegStartPlayCb(&AudioCallback::OnPlayAudioCallback);
				nim::Audio::RegStopPlayCb(&AudioCallback::OnStopAudioCallback);

				_DoAfterLogin();
				// 登录成功，显示主界面
				WindowsManager::SingletonShow<MainForm>(MainForm::kClassName);

				::DestroyWindow( login_form->GetHWND() );
			}
			else
			{
				login_form->OnLoginResult(code);
			}
		}
		else
		{
			QLOG_APP(L"login form has been closed");
			LoginManager::GetInstance()->SetLoginStatus(code == nim::kNIMResSuccess ? LoginStatus_SUCCESS : LoginStatus_NONE);
			LoginCallback::DoLogout(false);
		}
	}
}

void LoginCallback::OnLoginCallback( const char *json_params, const void* user_data )
{
	QLOG_APP(L"OnLoginCallback: {0}") <<json_params;

	Json::Value json;
	Json::Reader reader;
	if( reader.parse(json_params, json) )
	{
		int code = json[nim::kNIMErrorCode].asInt();
		int login_step = json[nim::kNIMLoginStep].asInt();
		if (code == nim::kNIMResSuccess)
		{
			if (login_step == nim::kNIMLoginStepLogin)
			{
				Post2UI(nbase::Bind(&UILoginCallback, code, false));
				if (json[nim::kNIMOtherClientsPres].isArray())
				{
					Post2UI(nbase::Bind(LoginCallback::OnMultispotChange, true, json[nim::kNIMOtherClientsPres]));
				}
			}
		}
		else
		{
			Post2UI( nbase::Bind( &UILoginCallback, code, false) );
		}
	}
	else
	{
		QLOG_ERR(L"parse login params fail: {0}") <<json_params;
		Post2UI(nbase::Bind(&UILoginCallback, nim::kNIMResUnknownError, false));
	}
}

void LoginCallback::OnReLoginCallback( const std::string& json_params )
{
	QLOG_APP(L"OnReLoginCallback: {0}") <<json_params;

	Json::Value json;
	Json::Reader reader;
	if( reader.parse(json_params, json) )
	{
		int code = json[nim::kNIMErrorCode].asInt();
		int login_step = json[nim::kNIMLoginStep].asInt();
		if (code == nim::kNIMResSuccess)
		{
			if (login_step == nim::kNIMLoginStepLogin)
				UILoginCallback(code, true);
		}
		else
		{
			UILoginCallback(code, true);
		}
	}
	else
	{
		QLOG_ERR(L"parse relogin params fail: {0}") <<json_params;
		UILoginCallback(nim::kNIMResUnknownError, true);
	}
}

//多端
void LoginCallback::OnMultispotLoginCallback(const std::string& json_params)
{
	QLOG_APP(L"OnMultispotLoginCallback: {0}") << json_params;
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(json_params, json))
	{
		bool online = json[nim::kNIMMultiSpotNotiyType].asInt() == nim::kNIMMultiSpotNotiyTypeImIn;
		if (json[nim::kNIMOtherClientsPres].isArray())
		{
			Post2UI(nbase::Bind(LoginCallback::OnMultispotChange, online, json[nim::kNIMOtherClientsPres]));
		}
	}
}
void LoginCallback::OnMultispotChange(bool online, Json::Value& json)
{
	nim_ui::SessionListManager::GetInstance()->OnMultispotChange(online, json);
}

void LoginCallback::OnKickoutOtherClientCallback(const std::string& json_params)
{
	Json::Value json;
	Json::Reader reader;
	if (reader.parse(json_params, json))
	{
	 	bool success = json[nim::kNIMKickoutOtherResErrorCode].asInt() == nim::kNIMResSuccess;
	 	if (success && json[nim::kNIMKickoutOtherResDeviceIDs].isArray())
	 	{
	 		std::list<UTF8String> client_ids;
	 		for (uint32_t i = 0; i < json[nim::kNIMKickoutOtherResDeviceIDs].size(); i++)
	 		{
	 			client_ids.push_back(json[nim::kNIMKickoutOtherResDeviceIDs][i].asString());
	 		}
			nim_ui::SessionListManager::GetInstance()->OnMultispotKickout(client_ids);
	 	}
	}
}
}
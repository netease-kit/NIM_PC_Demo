/** @file nim_cpp_client.cpp
  * @brief 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/09/21
  */

#include "nim_cpp_client.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_cpp_win32_demo_helper.h"
#include "nim_string_util.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
SDKInstance *g_nim_sdk_instance = NULL;
typedef bool(*nim_client_init)(const char *app_data_dir, const char *app_install_dir, const char *json_extension);
typedef void(*nim_client_cleanup)(const char *json_extension);
typedef void(*nim_client_login)(const char *app_token, const char *account, const char *password, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef int (*nim_client_get_login_state)(const char *json_extension);
typedef void(*nim_client_relogin)(const char *json_extension);
typedef void(*nim_client_logout)(nim::NIMLogoutType logout_type, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_kick_other_client)(const char *json_extension);
typedef void(*nim_client_reg_auto_relogin_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_kickout_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_disconnect_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_multispot_login_notify_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_kickout_other_client_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_sync_multiport_push_config_cb)(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
typedef void(*nim_client_set_multiport_push_config)(const char *switch_content, const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
typedef void(*nim_client_get_multiport_push_config)(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
#else
#include "nim_client.h"
#endif
	SDKConfig g_nim_sdk_config_;
static void CallbackLogin(const char* json_res, const void *callback)
{
	if (callback != nullptr)
	{
		LoginRes res;
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.res_code_ = (NIMResCode)values[kNIMResCode].asInt();
			res.login_step_ = (NIMLoginStep)values[kNIMLoginStep].asUInt();
			res.relogin_ = values[kNIMRelogin].asBool();
			res.retrying_ = values[kNIMRetrying].asBool();
			ParseOtherClientsPres(values[kNIMOtherClientsPres], res.other_clients_);
		}
		Client::LoginCallback *cb = (Client::LoginCallback *)callback;
		PostTaskToUIThread(std::bind((*cb), res));
		//(*cb)(res);
	}
}

static void CallbackLogout(const char *json_res, const void *callback)
{
	if (callback != nullptr)
	{
		Client::LogoutCallback cb = *(Client::LogoutCallback *)callback;
		if (cb != nullptr)
		{		
			NIMResCode error_code = kNIMResSuccess;
			Json::Reader reader;
			Json::Value values;
			if (reader.parse(PCharToString(json_res), values) && values.isObject())
			{
				error_code = (NIMResCode)values[kNIMLogoutErrorCode].asInt();
			}
			PostTaskToUIThread(std::bind(cb, error_code));
		}		
	}
}

static void CallbackKickout(const char* json_res, const void *callback)
{
	if (callback != nullptr)
	{
		KickoutRes res;
		Json::Reader reader;
		Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.client_type_ = (NIMClientType)values[kNIMKickoutClientType].asUInt();
			res.kick_reason_ = (NIMKickReason)values[kNIMKickoutReasonCode].asUInt();
		}
		Client::KickoutCallback *cb = (Client::KickoutCallback *)callback;
		if (*cb != nullptr)
			PostTaskToUIThread(std::bind((*cb), res));
		//(*cb)(res);
	}
}

static void CallbackDisconnect(const char* json_res, const void* callback)
{
	if (callback != nullptr)
	{
		Client::DisconnectCallback *cb = (Client::DisconnectCallback *)callback;
		if (*cb != nullptr)
			PostTaskToUIThread(std::bind((*cb)));
		//(*cb)();
	}
}

static void CallbackMutliSpotLogin(const char* json_res, const void* callback)
{
	if (callback != nullptr)
	{
		MultiSpotLoginRes res;
		Json::Reader reader;
		Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.notify_type_ = (NIMMultiSpotNotifyType)values[kNIMMultiSpotNotifyType].asUInt();
			ParseOtherClientsPres(values[kNIMOtherClientsPres], res.other_clients_);
		}
		Client::MultiSpotLoginCallback *cb = (Client::MultiSpotLoginCallback *)callback;
		if (*cb != nullptr)
			PostTaskToUIThread(std::bind((*cb), res));
		//(*cb)(res);
	}
}

static void CallbackKickother(const char* json_res, const void* callback)
{
	if (callback != nullptr)
	{
		KickOtherRes res;
		Json::Reader reader;
		Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.res_code_ = (NIMResCode)values[kNIMKickoutOtherResErrorCode].asInt();
			JsonStrArrayToList(values[kNIMKickoutOtherResDeviceIDs], res.device_ids_);
		}
		Client::KickOtherCallback *cb = (Client::KickOtherCallback *)callback;
		if (*cb != nullptr)
			PostTaskToUIThread(std::bind((*cb), res));
		//(*cb)(res);
	}
}

bool Client::Init(const std::string& app_key
	, const std::string& app_data_dir
	, const std::string& app_install_dir
	, const SDKConfig &config)
{
#ifdef NIM_SDK_DLL_IMPORT

// #if !defined (WIN32)
// 	static const char *kSdkNimDll = "libnim.so";
// #elif defined (_DEBUG) || defined (DEBUG)
// 	static const char *kSdkNimDll = "nim_d.dll";
// #else
	static const char *kSdkNimDll = "nim.dll";
// #endif
	if (NULL == g_nim_sdk_instance)
	{
		g_nim_sdk_instance = new SDKInstance();
	}
	if (!g_nim_sdk_instance->LoadSdkDll(app_install_dir.c_str(), kSdkNimDll))
		return false;
#endif
	g_nim_sdk_config_ = config;
	Json::Value config_root;
	//sdk能力参数（必填）
	Json::Value config_values;
	config_values[nim::kNIMDataBaseEncryptKey] = config.database_encrypt_key_;
	config_values[nim::kNIMPreloadAttach] = config.preload_attach_;
	config_values[nim::kNIMPreloadImageQuality] = config.preload_image_quality_;
	config_values[nim::kNIMPreloadImageResize] = config.preload_image_resize_;
	config_values[nim::kNIMSDKLogLevel] = config.sdk_log_level_;
	config_values[nim::kNIMSyncSessionAck] = config.sync_session_ack_;
	config_values[nim::kNIMLoginRetryMaxTimes] = config.login_max_retry_times_;
#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY	
	config_values[nim::kNIMUseHttps] = config.use_https_;
	config_values[nim::kNIMTeamNotificationUnreadCount] = config.team_notification_unread_count_;
	config_values[nim::kNIMAnimatedImageThumbnailEnabled] = config.animated_image_thumbnail_enabled_;
	config_values[nim::kNIMClientAntispam] = config.client_antispam_;
	config_values[nim::kNIMTeamMessageAckEnabled] = config.team_msg_ack_;
#endif
	config_root[nim::kNIMGlobalConfig] = config_values;
	config_root[nim::kNIMAppKey] = app_key;
	if (!config.server_conf_file_path_.empty())
		config_root[nim::kNIMServerConfFilePath] = config.server_conf_file_path_;

	if (config.use_private_server_)
	{
		Json::Value srv_config;
		srv_config[nim::kNIMLbsAddress] = config.lbs_address_;
		srv_config[nim::kNIMNosLbsAddress] = config.nos_lbs_address_;
		srv_config[nim::kNIMDefaultLinkAddress] = config.default_link_address_;
		srv_config[nim::kNIMDefaultNosUploadAddress] = config.default_nos_upload_address_;
		srv_config[nim::kNIMDefaultNosUploadHost] = config.default_nos_upload_host_;
		srv_config[nim::kNIMRsaPublicKeyModule] = config.rsa_public_key_module_;
		srv_config[nim::kNIMRsaVersion] = config.rsa_version_;
		srv_config[nim::kNIMDownloadAddressTemplate] = config.nos_download_address_;
		srv_config[nim::kNIMAccelerateHost] = config.nos_accelerate_host_;
		srv_config[nim::kNIMAccelerateAddressTemplate] = config.nos_accelerate_address_;
		srv_config[nim::kNIMNtserverAddress] = config.ntserver_address_;
		config_values[kNIMUploadStatisticsData] = config.upload_statistics_data_;
		config_root[nim::kNIMPrivateServerSetting] = srv_config;
	}
	return NIM_SDK_GET_FUNC(nim_client_init)(app_data_dir.c_str(), app_install_dir.c_str(), GetJsonStringWithNoStyled(config_root).c_str());
}
const SDKConfig& Client::GetSDKConfig()
{
	return g_nim_sdk_config_;
}
void Client::Cleanup(const std::string& json_extension/* = ""*/)
{
	NIM_SDK_GET_FUNC(nim_client_cleanup)(json_extension.c_str());
#ifdef NIM_SDK_DLL_IMPORT
	g_nim_sdk_instance->UnLoadSdkDll();
	delete g_nim_sdk_instance;
	g_nim_sdk_instance = NULL;
#endif
}

bool Client::Login(const std::string& app_key
	, const std::string& account
	, const std::string& password
	, const LoginCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (app_key.empty() || account.empty() || password.empty())
		return false;

	LoginCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new LoginCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_client_login)(app_key.c_str()
										, account.c_str()
										, password.c_str()
										, json_extension.c_str()
										, &CallbackLogin
										, cb_pointer);

	return true;
}

NIMLoginState Client::GetLoginState( const std::string& json_extension /*= ""*/ )
{
	int login_state = NIM_SDK_GET_FUNC(nim_client_get_login_state)(json_extension.c_str());
	return (NIMLoginState)login_state;
}

void Client::Relogin(const std::string& json_extension/* = ""*/)
{
	return NIM_SDK_GET_FUNC(nim_client_relogin)(json_extension.c_str());
}
Client::LogoutCallback g_logou_cb = nullptr;
void Client::Logout(nim::NIMLogoutType logout_type
	, const LogoutCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	g_logou_cb = cb;
	//LogoutCallback* cb_pointer = nullptr;
	//if (cb)
	//{
	//	cb_pointer = new LogoutCallback(cb);
	//}
	return NIM_SDK_GET_FUNC(nim_client_logout)(logout_type, json_extension.c_str(), &CallbackLogout, &g_logou_cb);
}

bool Client::KickOtherClient(const std::list<std::string>& client_ids)
{
	if (client_ids.empty())
		return false;

	Json::Value values;
	for (auto it = client_ids.begin(); it != client_ids.end(); it++)
	{
		values[nim::kNIMKickoutOtherDeviceIDs].append(*it);
	}
	Json::FastWriter fs;
	std::string out = fs.write(values);
	NIM_SDK_GET_FUNC(nim_client_kick_other_client)(out.c_str());

	return true;
}

static Client::LoginCallback g_cb_relogin_ = nullptr;
void Client::RegReloginCb(const LoginCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_relogin_ = cb;
	return NIM_SDK_GET_FUNC(nim_client_reg_auto_relogin_cb)(json_extension.c_str(), &CallbackLogin, &g_cb_relogin_);
}

static Client::KickoutCallback g_cb_kickout_ = nullptr;
void Client::RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_kickout_ = cb;
	return NIM_SDK_GET_FUNC(nim_client_reg_kickout_cb)(json_extension.c_str(), &CallbackKickout, &g_cb_kickout_);
}

static Client::DisconnectCallback g_cb_disconnect_ = nullptr;
void Client::RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension/* = ""*/)
{	
	g_cb_disconnect_ = cb;
	return NIM_SDK_GET_FUNC(nim_client_reg_disconnect_cb)(json_extension.c_str(), &CallbackDisconnect, &g_cb_disconnect_);
}

static Client::MultiSpotLoginCallback g_cb_multispot_login_ = nullptr;
void Client::RegMultispotLoginCb(const MultiSpotLoginCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_multispot_login_ = cb;
	return NIM_SDK_GET_FUNC(nim_client_reg_multispot_login_notify_cb)(json_extension.c_str(), &CallbackMutliSpotLogin, &g_cb_multispot_login_);
}

static Client::KickOtherCallback g_cb_kickother_ = nullptr;
void Client::RegKickOtherClientCb(const KickOtherCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_kickother_ = cb;
	return NIM_SDK_GET_FUNC(nim_client_reg_kickout_other_client_cb)(json_extension.c_str(), &CallbackKickother, &g_cb_kickother_);
}
static void CallbackSyncMultiportPushConfig(int rescode, const char *content, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Client::MultiportPushConfigCallback* cb_pointer = (Client::MultiportPushConfigCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value values;
			Json::Reader reader;
			if (rescode == nim::kNIMResSuccess && reader.parse(PCharToString(content), values) && values.isObject())
			{
				bool open = values[kNIMMultiportPushConfigContentKeyOpen].asInt() == 1;
				PostTaskToUIThread(std::bind((*cb_pointer), rescode, open));
				//(*cb_pointer)(rescode, open);
				return;
			}
			(*cb_pointer)(rescode, false);
		}
	}
}

static Client::MultiportPushConfigCallback* g_cb_sync_multiport_push_switch_ = nullptr;
void Client::RegSyncMultiportPushConfigCb(const MultiportPushConfigCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_sync_multiport_push_switch_)
	{
		delete g_cb_sync_multiport_push_switch_;
		g_cb_sync_multiport_push_switch_ = nullptr;
	}
	g_cb_sync_multiport_push_switch_ = new MultiportPushConfigCallback(cb);
	return NIM_SDK_GET_FUNC(nim_client_reg_sync_multiport_push_config_cb)(json_extension.c_str(), &CallbackSyncMultiportPushConfig, g_cb_sync_multiport_push_switch_);
}

static void CallbackMultiportPushConfig(int rescode, const char *content, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Client::MultiportPushConfigCallback* cb_pointer = (Client::MultiportPushConfigCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value values;
			Json::Reader reader;
			if (rescode == nim::kNIMResSuccess && reader.parse(PCharToString(content), values) && values.isObject())
			{
				bool open = values[kNIMMultiportPushConfigContentKeyOpen].asInt() == 1;
				PostTaskToUIThread(std::bind((*cb_pointer), rescode, open));
				//(*cb_pointer)(rescode, open);
				delete cb_pointer;
				return;
			}
			(*cb_pointer)(rescode, false);
		}
		delete cb_pointer;
	}
}

void Client::SetMultiportPushConfigAsync(bool switch_on, const MultiportPushConfigCallback& cb, const std::string& json_extension/* = ""*/)
{
	MultiportPushConfigCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new MultiportPushConfigCallback(cb);
	}
	Json::Value values;
	Json::FastWriter fw;
	values[kNIMMultiportPushConfigContentKeyOpen] = switch_on ? 1 : 2;
	NIM_SDK_GET_FUNC(nim_client_set_multiport_push_config)(GetJsonStringWithNoStyled(values).c_str()
		, json_extension.c_str()
		, &CallbackMultiportPushConfig
		, cb_pointer);
}

void Client::GetMultiportPushConfigAsync(const MultiportPushConfigCallback& cb, const std::string& json_extension/* = ""*/)
{
	MultiportPushConfigCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new MultiportPushConfigCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_client_get_multiport_push_config)(json_extension.c_str()
		, &CallbackMultiportPushConfig
		, cb_pointer);
}

void Client::UnregClientCb()
{
	g_cb_relogin_ = nullptr;	
	g_cb_kickout_ = nullptr;	
	g_cb_disconnect_ = nullptr;
	g_cb_multispot_login_ = nullptr;
	g_cb_kickother_ = nullptr;	
}
}

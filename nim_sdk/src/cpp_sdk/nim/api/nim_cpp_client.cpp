/** @file nim_cpp_client.cpp
  * @brief 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/21
  */
#include "src/cpp_sdk/nim/api/nim_cpp_client.h"
#include <future>
#include <atomic>
#include "include/nim_cpp_api.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"

namespace nim {

#ifdef NIM_SDK_DLL_IMPORT
typedef bool(*nim_client_init)(const char *app_data_dir, const char *app_install_dir, const char *json_extension);
typedef void(*nim_client_cleanup)(const char *json_extension);
typedef void(*nim_client_cleanup2)(nim_json_transport_cb_func cb,const char *json_extension);
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
typedef const char* const(*nim_client_current_user_account)();
typedef const char* const(*nim_client_version)();
typedef void(*nim_client_get_server_current_time)(bool calc_local, nim_client_get_server_current_time_cb_func cb, const char *json_extension, const void *user_data);
#else
#include "nim_client.h"
#endif

SDKConfig g_nim_sdk_config_;

static void CallbackLogin(const char* json_res, const void *callback)
{
	
	CallbackProxy::DoSafeCallback<Client::LoginCallback>(callback, [=](const Client::LoginCallback& cb)
	{
		LoginRes res;
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.res_code_ = (NIMResCode)values[kNIMResCode].asInt();
			res.login_step_ = (NIMLoginStep)values[kNIMLoginStep].asUInt();
			res.relogin_ = values[kNIMRelogin].asBool();
			res.retrying_ = values[kNIMRetrying].asBool();
			ParseOtherClientsPres(values[kNIMOtherClientsPres], res.other_clients_);
		}
		CallbackProxy::Invoke(cb, res);
	});
}


static void CallbackLogout(const char *json_res, const void *callback)
{
	CallbackProxy::DoSafeCallback<Client::LogoutCallback>(callback, [=](const Client::LogoutCallback& cb){
		NIMResCode error_code = kNIMResSuccess;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			error_code = (NIMResCode)values[kNIMLogoutErrorCode].asInt();
		}		
		CallbackProxy::Invoke(cb, error_code);
	},true);
}

static void CallbackKickout(const char* json_res, const void *callback)
{
	CallbackProxy::DoSafeCallback<Client::KickoutCallback>(callback, [=](const Client::KickoutCallback& cb)
	{
		KickoutRes res;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.client_type_ = (NIMClientType)values[kNIMKickoutClientType].asUInt();
			res.kick_reason_ = (NIMKickReason)values[kNIMKickoutReasonCode].asUInt();
		}
		CallbackProxy::Invoke(cb, res);
	});
}

static void CallbackDisconnect(const char* json_res, const void* callback)
{
	CallbackProxy::DoSafeCallback<Client::DisconnectCallback>(callback, [=](const Client::DisconnectCallback& cb)
	{
		CallbackProxy::Invoke(cb);
	});

}

static void CallbackMutliSpotLogin(const char* json_res, const void* callback)
{

	CallbackProxy::DoSafeCallback<Client::MultiSpotLoginCallback>(callback, [=](const Client::MultiSpotLoginCallback& cb)
	{
		MultiSpotLoginRes res;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.notify_type_ = (NIMMultiSpotNotifyType)values[kNIMMultiSpotNotifyType].asUInt();
			ParseOtherClientsPres(values[kNIMOtherClientsPres], res.other_clients_);
		}
		CallbackProxy::Invoke(cb, res);
	});
}

static void CallbackKickother(const char* json_res, const void* callback)
{
	CallbackProxy::DoSafeCallback<Client::KickOtherCallback>(callback, [=](const Client::KickOtherCallback& cb){

		KickOtherRes res;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value values;
		if (reader.parse(PCharToString(json_res), values) && values.isObject())
		{
			res.res_code_ = (NIMResCode)values[kNIMKickoutOtherResErrorCode].asInt();
			JsonStrArrayToList(values[kNIMKickoutOtherResDeviceIDs], res.device_ids_);
		}
		CallbackProxy::Invoke(cb, res);

	});
}
void CallbackGetServerCurrentTime(int rescode, bool calc_local, uint64_t time, const void *callback)
{
	CallbackProxy::DoSafeCallback<Client::GetCurrentServerTimeCallback>(callback, [=](const Client::GetCurrentServerTimeCallback& cb) {		
		CallbackProxy::Invoke(cb, rescode,calc_local,time);
	},true);
}

/** @fn void UnregNIMCallback()
* 反注册NIM提供的所有回调
* @return void 无返回值
*/
void UnregNIMCallback()
{
	Client::UnregClientCb();
	DataSync::UnregDataSyncCb();
	Friend::UnregFriendCb();
	Session::UnregSessionCb();
	SystemMsg::UnregSysmsgCb();
	Talk::UnregTalkCb();
	Team::UnregTeamCb();
	User::UnregUserCb();
	SuperTeam::UnregSuperTeamCb();
	SessionOnLineService::UnregSessionOnLineServiceCb();
	MsgLog::UnregMsglogCb();
}
bool Client::Init(const std::string& app_key
	, const std::string& app_data_dir
	, const std::string& app_install_dir
	, const SDKConfig &config)
{
#ifdef NIM_SDK_DLL_IMPORT
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
		//define something for Windows (32-bit and 64-bit, this part is common)
		static const char *kSdkNimDll = "nim.dll";
	#ifdef _WIN64
		// define something for Windows (64-bit only)
	#else
		// define something for Windows (32-bit only)
	#endif
	#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		// iOS Simulator
		static const char *kSdkNimDll = "nim_sdk_iOS";
	#elif TARGET_OS_IPHONE
		// iOS device
		static const char *kSdkNimDll = "nim_sdk_iOS";
	#elif TARGET_OS_MAC
		// Other kinds of Mac OS
		static const char *kSdkNimDll = "nim_sdk_Mac";
	#else
	#   error "Unknown Apple platform"
		static const char *kSdkNimDll = nullptr;
	#endif
	#elif __ANDROID__
		// android
	#elif __linux__
		// linux
		static const char *kSdkNimDll = "libnim.so";
	#elif __unix__ // all unices not caught above
		// Unix
	#elif defined(_POSIX_VERSION)
		// POSIX
	#else
	#   error "Unknown compiler"
		static const char *kSdkNimDll = nullptr;
	#endif
		if (NULL == nim_sdk_instance)
		{
			nim_sdk_instance = new SDKInstance();
		}
		if (!nim_sdk_instance->LoadSdkDll(app_install_dir.c_str(), kSdkNimDll))
			return false;
#endif
	g_nim_sdk_config_ = config;
	nim_cpp_wrapper_util::Json::Value config_root;
	// SDK 能力参数（必填）
	nim_cpp_wrapper_util::Json::Value config_values;
	config_values[nim::kNIMDataBaseEncryptKey] = config.database_encrypt_key_;
	config_values[nim::kNIMPreloadAttach] = config.preload_attach_;	
	config_values[nim::kNIMPreloadAttachImageNameTemplate] = config.preload_image_name_template_;
	config_values[nim::kNIMPreloadImageQuality] = config.preload_image_quality_;
	config_values[nim::kNIMPreloadImageResize] = config.preload_image_resize_;
	config_values[nim::kNIMSDKLogLevel] = config.sdk_log_level_;
    config_values[nim::kNIMPushCerName] = config.push_cer_name_;
    config_values[nim::kNIMPushToken] = config.push_token_;
	config_values[nim::kNIMSyncSessionAck] = config.sync_session_ack_;
	config_values[nim::kNIMLoginRetryMaxTimes] = config.login_max_retry_times_;
	config_values[nim::kNIMUseHttps] = config.use_https_;
	config_values[nim::kNIMTeamNotificationUnreadCount] = config.team_notification_unread_count_;
	config_values[nim::kNIMVChatMissUnreadCount] = config.vchat_miss_unread_count_;
	config_values[nim::kNIMResetUnreadCountWhenRecall] = config.reset_unread_count_when_recall_;
	config_values[nim::kNIMUploadSDKEventsAfterLogin] = config.upload_sdk_events_after_login_;
	config_values[nim::kNIMAnimatedImageThumbnailEnabled] = config.animated_image_thumbnail_enabled_;
	config_values[nim::kNIMClientAntispam] = config.client_antispam_;
	config_values[nim::kNIMTeamMessageAckEnabled] = config.team_msg_ack_;
	config_values[nim::kNIMNeedUpdateLBSBeforRelogin] = config.need_update_lbs_befor_relogin_;
	//config_values[nim::kNIMMarkreadAfterSaveDBEnabled] = config.enable_markread_after_save_db_;
	config_values[nim::kNIMCachingMarkreadEnabled] = config.caching_markread_;
	config_values[nim::kNIMCachingMarkreadTime] = config.caching_markread_time_;
	config_values[nim::kNIMCachingMarkreadCount] = config.caching_markread_count_;
	config_values[nim::kNIMEnableUserDataFileLocalBackup] = config.enable_user_datafile_backup_;
	config_values[nim::kNIMEnableUserDataFileLocalRestore] = config.enable_user_datafile_restore_;
	config_values[nim::kNIMEnableUserDataFileDefRestoreProc] = config.enable_user_datafile_defrestoreproc_;
	config_values[nim::kNIMUserDataFileLocalBackupFolder] = config.user_datafile_localbackup_folder_;
	config_values[nim::kNIMDedicatedClusteFlag] = config.dedicated_cluste_flag_;
	config_values[nim::kNIMHandShakeType] = config.hand_shake_type_;
	config_values[nim::kNIMNegoKeyNECA] = config.nego_key_neca_;	
	config_values[nim::kNIMCommNECA] = config.comm_neca_;
	config_values[nim::kNIMIPProtVersion] = config.ip_protocol_version_;	

	if (!config.server_conf_file_path_.empty())
		config_root[nim::kNIMServerConfFilePath] = config.server_conf_file_path_;
	if (config.use_private_server_)
	{
		nim_cpp_wrapper_util::Json::Value srv_config;
		srv_config[nim::kNIMLbsAddress] = config.lbs_address_;
		srv_config[nim::kNIMNosLbsAddress] = config.nos_lbs_address_;
		srv_config[nim::kNIMDefaultLinkAddress] = config.default_link_address_;
		srv_config[nim::kNIMDefaultLinkAddressIPV6] = config.default_link_address_ipv6_;
		srv_config[nim::kNIMDefaultNosUploadAddress] = config.default_nos_upload_address_;
		srv_config[nim::kNIMDefaultNosUploadHost] = config.default_nos_upload_host_;
		srv_config[nim::kNIMRsaPublicKeyModule] = config.rsa_public_key_module_;
		srv_config[nim::kNIMRsaVersion] = config.rsa_version_;
		srv_config[nim::kNIMDownloadAddressTemplate] = config.nos_download_address_;
		srv_config[nim::kNIMAccelerateHost] = config.nos_accelerate_host_;
		srv_config[nim::kNIMAccelerateAddressTemplate] = config.nos_accelerate_address_;
		srv_config[nim::kNIMNegoKeyNECAKeyPA] = config.nego_key_neca_key_parta_;
		srv_config[nim::kNIMNegoKeyNECAKeyPB] = config.nego_key_neca_key_partb_;
		srv_config[nim::kNIMNegoKeyNECAKeyV] = config.nego_key_neca_key_version_;
		srv_config[nim::kNIMProbeIPV4URL] = config.probe_ipv4_url_;
		srv_config[nim::kNIMProbeIPV6URL] = config.probe_ipv6_url_;
		config_root[nim::kNIMPrivateServerSetting] = srv_config;
	}
	config_root[nim::kNIMGlobalConfig] = config_values;
	config_root[nim::kNIMAppKey] = app_key;	

	auto ret  = NIM_SDK_GET_FUNC(nim_client_init)(app_data_dir.c_str(), app_install_dir.c_str(), GetJsonStringWithNoStyled(config_root).c_str());
	if (ret && nim_sdk_instance)
		nim_sdk_instance->OnSDKInited();

	return ret;
}
void Client::SetCallbackFunction(const SDKClosure& callback)
{
	nim::CallbackProxy::docallback_async_ = callback;
}
const SDKConfig& Client::GetSDKConfig()
{
	return g_nim_sdk_config_;
}
void Client::Cleanup(const std::string& json_extension/* = ""*/)
{
	NIM_SDK_GET_FUNC(nim_client_cleanup)(json_extension.c_str());

#ifdef NIM_SDK_DLL_IMPORT
	nim_sdk_instance->UnLoadSdkDll();
	delete nim_sdk_instance;
	nim_sdk_instance = NULL;
#endif
}

void Client::Cleanup2(const std::string& json_extension /*= ""*/)
{
	static std::atomic<bool> cleanup_flag(false);
	auto cleanup_task = [&]() {
		NIM_SDK_GET_FUNC(nim_client_cleanup2)([](const char *json_params, const void *user_data) {
			cleanup_flag = true;
		}, json_extension.c_str());
		while (!cleanup_flag)
		{
            std::this_thread::sleep_for(std::chrono::seconds(0));
		}
	};
	std::thread cleanup_thread(cleanup_task);
	cleanup_thread.join();

#ifdef NIM_SDK_DLL_IMPORT
	nim_sdk_instance->UnLoadSdkDll();
	delete nim_sdk_instance;
	nim_sdk_instance = NULL;
	cleanup_flag = false;
#endif
}

void Client::LoginCustomDataToJson(const std::string& custom_data, std::string& strValue)
{
	nim_cpp_wrapper_util::Json::Value value;
	value[kNIMPresCustomTag] = custom_data;
	strValue = GetJsonStringWithNoStyled(value);
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
	NIM_SDK_GET_FUNC(nim_client_relogin)(json_extension.c_str());
}

void Client::Logout(nim::NIMLogoutType logout_type
	, const LogoutCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	LogoutCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new LogoutCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_client_logout)(logout_type, json_extension.c_str(), &CallbackLogout, cb_pointer);
}

bool Client::KickOtherClient(const std::list<std::string>& client_ids)
{
	if (client_ids.empty())
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	for (auto it = client_ids.begin(); it != client_ids.end(); it++)
	{
		values[nim::kNIMKickoutOtherDeviceIDs].append(*it);
	}
	nim_cpp_wrapper_util::Json::FastWriter fs;
	std::string out = fs.write(values);
	NIM_SDK_GET_FUNC(nim_client_kick_other_client)(out.c_str());

	return true;
}

static Client::LoginCallback g_cb_relogin_ = nullptr;
void Client::RegReloginCb(const LoginCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_relogin_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_auto_relogin_cb)(json_extension.c_str(), &CallbackLogin, &g_cb_relogin_);
}

static Client::KickoutCallback g_cb_kickout_ = nullptr;
void Client::RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_kickout_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_kickout_cb)(json_extension.c_str(), &CallbackKickout, &g_cb_kickout_);
}

static Client::DisconnectCallback g_cb_disconnect_ = nullptr;
void Client::RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_disconnect_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_disconnect_cb)(json_extension.c_str(), &CallbackDisconnect, &g_cb_disconnect_);
}

static Client::MultiSpotLoginCallback g_cb_multispot_login_ = nullptr;
void Client::RegMultispotLoginCb(const MultiSpotLoginCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_multispot_login_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_multispot_login_notify_cb)(json_extension.c_str(), &CallbackMutliSpotLogin, &g_cb_multispot_login_);
}

static Client::KickOtherCallback g_cb_kickother_ = nullptr;
void Client::RegKickOtherClientCb(const KickOtherCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_kickother_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_kickout_other_client_cb)(json_extension.c_str(), &CallbackKickother, &g_cb_kickother_);
}

static void CallbackSyncMultiportPushConfig(int rescode, const char *content, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Client::MultiportPushConfigCallback>(user_data, [=](const Client::MultiportPushConfigCallback& cb)
	{
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		bool open = false;
		if (rescode == nim::kNIMResSuccess && reader.parse(PCharToString(content), values) && values.isObject())
		{
			open = values[kNIMMultiportPushConfigContentKeyOpen].asInt() == 1;
		}
		CallbackProxy::Invoke(cb, rescode, open);
	});
}

static Client::MultiportPushConfigCallback g_cb_sync_multiport_push_switch_ = nullptr;
void Client::RegSyncMultiportPushConfigCb(const MultiportPushConfigCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_sync_multiport_push_switch_ = cb;
	NIM_SDK_GET_FUNC(nim_client_reg_sync_multiport_push_config_cb)(json_extension.c_str(), &CallbackSyncMultiportPushConfig, &g_cb_sync_multiport_push_switch_);
}

static void CallbackMultiportPushConfig(int rescode, const char *content, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Client::MultiportPushConfigCallback>(user_data, [=](const Client::MultiportPushConfigCallback& cb)
	{

		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		bool open = false;
		if (rescode == nim::kNIMResSuccess && reader.parse(PCharToString(content), values) && values.isObject())
		{
			open = values[kNIMMultiportPushConfigContentKeyOpen].asInt() == 1;
		}
		CallbackProxy::Invoke(cb, rescode, open);
	},true);
}

void Client::SetMultiportPushConfigAsync(bool switch_on, const MultiportPushConfigCallback& cb, const std::string& json_extension/* = ""*/)
{
	MultiportPushConfigCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new MultiportPushConfigCallback(cb);
	}
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::FastWriter fw;
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

std::string Client::GetSDKVersion()
{
	return NIM_SDK_GET_FUNC(nim_client_version)();
}
std::string Client::GetCurrentUserAccount()
{
	return NIM_SDK_GET_FUNC(nim_client_current_user_account)();
}
void Client::GetServerCurrentTime(const Client::GetCurrentServerTimeCallback& cb, bool calc_local/* = false*/)
{

	Client::GetCurrentServerTimeCallback* callback = nullptr;
	if (cb != nullptr)
	{
		callback = new Client::GetCurrentServerTimeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_client_get_server_current_time)(calc_local, &CallbackGetServerCurrentTime, "", callback);
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

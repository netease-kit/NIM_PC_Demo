#include "nim_client_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

typedef bool(*nim_client_init)(const char *app_data_dir, const char *app_install_dir, const char *json_extension);
typedef	void(*nim_client_cleanup)(const char *json_extension);
typedef	void(*nim_client_login)(const char *app_token, const char *account, const char *password, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef	void(*nim_client_relogin)(const char *json_extension);
typedef	void(*nim_client_logout)(nim::NIMLogoutType logout_type, const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_kick_other_client)(const char *json_extension);
typedef void(*nim_client_reg_auto_relogin_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_kickout_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_disconnect_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void* user_data);
typedef void(*nim_client_reg_multispot_login_notify_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_kickout_other_client_cb)(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);


static void CallbackWrapperDelete(const char *json, const void *user_data)
{
	if (user_data)
	{
		Client::LogoutCallback* cb_pointer = (Client::LogoutCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(json)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperNodelete(const char *json, const void *user_data)
{
	if (user_data)
	{
		Client::DisconnectCallback* cb_pointer = (Client::DisconnectCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(json)));
		}
	}
}

bool Client::Init(const std::string& app_data_dir, const std::string& app_install_dir, const std::string& json_extension)
{
	NimSdk::LoadSdkDll();
	return NIM_SDK_Get_Function(nim_client_init)(app_data_dir.c_str(), app_install_dir.c_str(), json_extension.c_str());
}

void Client::Cleanup(const std::string& json_extension)
{
	NIM_SDK_Get_Function(nim_client_cleanup)(json_extension.c_str());
	NimSdk::UnLoadSdkDll();
}

void Client::Login(const std::string& app_key, const std::string& account, const std::string& token, nim_json_transport_cb_func cb, const void* user_data, const std::string& json_extension)
{
	return NIM_SDK_Get_Function(nim_client_login)(app_key.c_str(), account.c_str(), token.c_str(), json_extension.c_str(), cb, user_data);
}

void Client::Relogin(const std::string& json_extension)
{
	return NIM_SDK_Get_Function(nim_client_relogin)(json_extension.c_str());
}

void Client::Logout(nim::NIMLogoutType logout_type, const LogoutCallback& cb, const std::string& json_extension)
{
	LogoutCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new LogoutCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_logout)(logout_type, json_extension.c_str(), &CallbackWrapperDelete, cb_pointer);
}

static Client::ReloginCallback* g_relogin_cb_pointer = nullptr;
void Client::RegReloginCb(const ReloginCallback& cb, const std::string& json_extension)
{
	delete g_relogin_cb_pointer;
	if (cb)
	{
		g_relogin_cb_pointer = new ReloginCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_reg_auto_relogin_cb)(json_extension.c_str(), &CallbackWrapperNodelete, g_relogin_cb_pointer);
}

static Client::DisconnectCallback* g_kickout_cb_pointer = nullptr;
void Client::RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension)
{
	delete g_kickout_cb_pointer;
	if (cb)
	{
		g_kickout_cb_pointer = new DisconnectCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_reg_kickout_cb)(json_extension.c_str(), &CallbackWrapperNodelete, g_kickout_cb_pointer);
}

static Client::DisconnectCallback* g_disconnect_cb_pointer = nullptr;
void Client::RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension)
{
	delete g_disconnect_cb_pointer;
	if (cb)
	{
		g_disconnect_cb_pointer = new DisconnectCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_reg_disconnect_cb)(json_extension.c_str(), &CallbackWrapperNodelete, g_disconnect_cb_pointer);
}

static Client::MultispotLoginCallback* g_multispot_login_cb_pointer = nullptr;
void Client::RegMultispotLoginCb(const MultispotLoginCallback& cb, const std::string& json_extension)
{
	delete g_multispot_login_cb_pointer;
	if (cb)
	{
		g_multispot_login_cb_pointer = new MultispotLoginCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_reg_multispot_login_notify_cb)(json_extension.c_str(), &CallbackWrapperNodelete, g_multispot_login_cb_pointer);
}

void Client::KickOtherClient(const std::list<std::string>& client_ids)
{
	Json::Value values;
	for (auto it = client_ids.begin(); it != client_ids.end(); it++)
	{
		values[nim::kNIMKickoutOtherDeviceIDs].append(*it);
	}
	Json::FastWriter fs;
	std::string out = fs.write(values);
	NIM_SDK_Get_Function(nim_client_kick_other_client)(out.c_str());
}

static Client::KickoutOtherClientCallback* g_kickout_other_client_cb_pointer = nullptr;
void Client::RegKickOtherClientCb(const KickoutOtherClientCallback& cb, const std::string& json_extension)
{
	delete g_kickout_other_client_cb_pointer;
	if (cb)
	{
		g_kickout_other_client_cb_pointer = new KickoutOtherClientCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_client_reg_kickout_other_client_cb)(json_extension.c_str(), &CallbackWrapperNodelete, g_kickout_other_client_cb_pointer);
}

}

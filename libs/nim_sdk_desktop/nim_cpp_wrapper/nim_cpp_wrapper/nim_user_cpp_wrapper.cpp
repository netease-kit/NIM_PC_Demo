#include "nim_user_cpp_wrapper.h"
#include "nim_sdk_function.h"



namespace nim
{

typedef	void (*nim_user_set_stat)(const unsigned int user_stat, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);
typedef	void (*nim_user_reg_special_relationship_changed_cb)(const char *json_extension, nim_user_special_relationship_change_cb_func cb, const void *user_data);
typedef	void (*nim_user_set_black)(const char *accid, bool set_black, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);
typedef	void (*nim_user_set_mute)(const char *accid, bool set_mute, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);
typedef	void (*nim_user_get_mute_blacklist)(const char *json_extension, nim_user_sync_muteandblacklist_cb_func cb, const void *user_data);

typedef void (*nim_user_reg_user_info_changed_cb)(const char *json_extension, nim_user_info_change_cb_func cb, const void *user_data);
typedef void (*nim_user_get_user_info)(const char *accids, const char *json_extension, nim_user_get_user_info_cb_func cb, const void *user_data);
typedef void (*nim_user_get_user_info_online)(const char *accids, const char *json_extension, nim_user_get_user_info_cb_func cb, const void *user_data);
typedef void (*nim_user_update_user_info)(const char *info_json, const char *json_extension, nim_user_update_info_cb_func cb, const void *user_data);

static void CallbackWrapperDelete(const char *json, const void *user_data)
{
	if (user_data)
	{
		User::SetStatCallback* cb_pointer = (User::SetStatCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(json)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDelete2(int res_code, const char *accid, bool opt, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::SetBlackCallback* cb_pointer = (User::SetBlackCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), res_code, PCharToString(accid), opt));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDelete3(int res_code, const char *mute_black_list_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::GetMuteAndBlacklistCallback* cb_pointer = (User::GetMuteAndBlacklistCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), res_code, PCharToString(mute_black_list_json)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDelete4(const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::GetUserInfoCallback* cb_pointer = (User::GetUserInfoCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(result_json)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDelete5(int res_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::UpdateUserInfoCallback* cb_pointer = (User::UpdateUserInfoCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), res_code));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperNoDelete1(NIMUserSpecialRelationshipChangeType type, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::SpecialRelationshipChangedCallback* cb_pointer = (User::SpecialRelationshipChangedCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), type, PCharToString(result_json)));
		}
	}
}

static void CallbackWrapperNoDelete2(const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		User::UserInfoChangedCallback* cb_pointer = (User::UserInfoChangedCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(result_json)));
		}
	}
}

void User::SetStat(unsigned int user_stat, const SetStatCallback& cb, const std::string& json_extension)
{
	SetStatCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetStatCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_set_stat)(user_stat, json_extension.c_str(), &CallbackWrapperDelete, cb_pointer);
}

static User::SpecialRelationshipChangedCallback* g_changed_cb_pointer = nullptr;
void User::RegSpecialRelationshipChangedCb(const SpecialRelationshipChangedCallback& cb, const std::string& json_extension)
{
	delete g_changed_cb_pointer;
	if (cb)
	{
		g_changed_cb_pointer = new SpecialRelationshipChangedCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_reg_special_relationship_changed_cb)(json_extension.c_str(), &CallbackWrapperNoDelete1, g_changed_cb_pointer);
}

void User::SetBlack(const std::string& accid, bool set_black, const SetBlackCallback& cb, const std::string& json_extension)
{
	SetBlackCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetBlackCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_set_black)(accid.c_str(), set_black, json_extension.c_str(), &CallbackWrapperDelete2, cb_pointer);
}

void User::SetMute(const std::string& accid, bool set_mute, const SetMuteCallback& cb, const std::string& json_extension)
{
	SetBlackCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetBlackCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_set_mute)(accid.c_str(), set_mute, json_extension.c_str(), &CallbackWrapperDelete2, cb_pointer);
}

void User::GetMuteAndBlacklist(const GetMuteAndBlacklistCallback& cb, const std::string& json_extension)
{
	GetMuteAndBlacklistCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetMuteAndBlacklistCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_get_mute_blacklist)(json_extension.c_str(), &CallbackWrapperDelete3, cb_pointer);
}



static User::UserInfoChangedCallback* g_uinfo_changed_cb_pointer = nullptr;
void User::RegUserInfoChangedCb(const UserInfoChangedCallback & cb, const std::string & json_extension)
{
	delete g_uinfo_changed_cb_pointer;
	if (cb)
	{
		g_uinfo_changed_cb_pointer = new UserInfoChangedCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_reg_user_info_changed_cb)(json_extension.c_str(), &CallbackWrapperNoDelete2, g_uinfo_changed_cb_pointer);
}

void User::GetUserInfo(const std::list<std::string>& accids, const GetUserInfoCallback& cb, const std::string& json_extension /*= ""*/)
{
	GetUserInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetUserInfoCallback(cb);
	}

	Json::Value values;
	for (auto iter = accids.cbegin(); iter != accids.cend(); ++iter)
		values.append(*iter);
	
	return NIM_SDK_Get_Function(nim_user_get_user_info)(values.toStyledString().c_str(), json_extension.c_str(), &CallbackWrapperDelete4, cb_pointer);
}

void User::GetUserInfoOnline(const std::list<std::string>& accids, const GetUserInfoCallback& cb, const std::string& json_extension /*= ""*/)
{
	GetUserInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetUserInfoCallback(cb);
	}

	Json::Value values;
	for (auto iter = accids.cbegin(); iter != accids.cend(); ++iter)
		values.append(*iter);

	return NIM_SDK_Get_Function(nim_user_get_user_info_online)(values.toStyledString().c_str(), json_extension.c_str(), &CallbackWrapperDelete4, cb_pointer);
}

void User::UpdateUserInfo(const std::string& info_json, const UpdateUserInfoCallback& cb, const std::string& json_extension /*= ""*/)
{
	UpdateUserInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new UpdateUserInfoCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_user_update_user_info)(info_json.c_str(), json_extension.c_str(), &CallbackWrapperDelete5, cb_pointer);
}

}

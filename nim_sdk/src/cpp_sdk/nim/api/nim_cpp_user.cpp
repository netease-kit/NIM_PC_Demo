/** @file nim_cpp_user.cpp
  * @brief NIM SDK提供的用户相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/8/17
  */

#include "src/cpp_sdk/nim/api//nim_cpp_user.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef	void (*nim_user_reg_special_relationship_changed_cb)(const char *json_extension, nim_user_special_relationship_change_cb_func cb, const void *user_data);
typedef	void (*nim_user_set_black)(const char *accid, bool set_black, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);
typedef	void (*nim_user_set_mute)(const char *accid, bool set_mute, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);
typedef	void (*nim_user_get_mute_blacklist)(const char *json_extension, nim_user_sync_muteandblacklist_cb_func cb, const void *user_data);

typedef void (*nim_user_reg_user_name_card_changed_cb)(const char *json_extension, nim_user_name_card_change_cb_func cb, const void *user_data);
typedef void (*nim_user_get_user_name_card)(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data);
typedef void (*nim_user_get_user_name_card_online)(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data);
typedef void (*nim_user_update_my_user_name_card)(const char *info_json, const char *json_extension, nim_user_update_my_name_card_cb_func cb, const void *user_data);
typedef void(*nim_user_query_by_keyword)(const char* keyword, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data);
typedef void (*nim_user_update_push_token)(const char* cer_name, const char* token, int type);
#else
#include "nim_user.h"
#endif

static void CallbackSetRelation(int res_code, const char *accid, bool opt, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::SetRelationCallback>(callback, [=](const User::SetRelationCallback& cb){
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(accid), opt);
	}, true);

}

static void CallbackGetBlackList(int res_code, const char *mute_black_list_json, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::GetBlackListCallback>(callback, [=](const User::GetBlackListCallback& cb){

		std::list<BlackMuteListInfo> black_list;
		std::list<BlackMuteListInfo> out_black_list;
		ParseSpecialListInfo(PCharToString(mute_black_list_json), black_list);
		for (auto black = black_list.begin(); black != black_list.end(); black++)
		{
			if (black->set_black_)
				out_black_list.push_back(*black);
		}
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, black_list);

	}, true);

}

static void CallbackGetMuteList(int res_code, const char *mute_black_list_json, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::GetMuteListCallback>(callback, [=](const User::GetMuteListCallback& cb){

		std::list<BlackMuteListInfo> mute_list;
		std::list<BlackMuteListInfo> out_mute_list;
		ParseSpecialListInfo(PCharToString(mute_black_list_json), mute_list);
		for (auto mute = mute_list.begin(); mute != mute_list.end(); mute++)
		{
			if (mute->set_mute_)
				out_mute_list.push_back(*mute);
		}
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, out_mute_list);

	}, true);


}

static void CallbackGetUserNameCard(const char *result_json, const char *json_extension, const void *callback)
{
	CallbackProxy::DoSafeCallback<User::GetUserNameCardCallback>(callback, [=](const User::GetUserNameCardCallback& cb){

		std::list<UserNameCard> users_info;
		ParseNameCards(PCharToString(result_json), users_info);
		CallbackProxy::Invoke(cb, users_info);

	}, true);

}

static void CallbackUpdateMyUserNameCard(int res_code, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::UpdateMyUserNameCardCallback>(callback, [=](const User::UpdateMyUserNameCardCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code);

	}, true);
}

static void CallbackSpecialRelationChange(NIMUserSpecialRelationshipChangeType type, const char *result_json, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::SpecialRelationshipChangedCallback>(callback, [=](const User::SpecialRelationshipChangedCallback& cb){

		SpecialRelationshipChangeEvent change_event;
		change_event.type_ = type;
		change_event.content_ = PCharToString(result_json);
		CallbackProxy::Invoke(cb, change_event);

	});
}

static void CallbackUserNameCardChange(const char *result_json, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<User::UserNameCardChangedCallback>(callback, [=](const User::UserNameCardChangedCallback& cb){

		std::list<UserNameCard> users_info;
		ParseNameCards(PCharToString(result_json), users_info);
		CallbackProxy::Invoke(cb, users_info);

	});

}

static User::SpecialRelationshipChangedCallback g_cb_relation_changed_ = nullptr;
void User::RegSpecialRelationshipChangedCb(const SpecialRelationshipChangedCallback& cb, const std::string& json_extension)
{
	g_cb_relation_changed_ = cb;
	NIM_SDK_GET_FUNC(nim_user_reg_special_relationship_changed_cb)(json_extension.c_str(), &CallbackSpecialRelationChange, &g_cb_relation_changed_);
}

bool User::SetBlack(const std::string& accid, bool set_black, const SetBlackCallback& cb, const std::string& json_extension)
{
	if (accid.empty())
		return false;

	SetBlackCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetBlackCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_user_set_black)(accid.c_str(), set_black, json_extension.c_str(), &CallbackSetRelation, cb_pointer);

	return true;
}

bool User::SetMute(const std::string& accid, bool set_mute, const SetMuteCallback& cb, const std::string& json_extension)
{
	if (accid.empty())
		return false;

	SetMuteCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetMuteCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_user_set_mute)(accid.c_str(), set_mute, json_extension.c_str(), &CallbackSetRelation, cb_pointer);

	return true;
}

void User::GetMutelist(const GetMuteListCallback& cb, const std::string& json_extension)
{
	GetMuteListCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetMuteListCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_user_get_mute_blacklist)(json_extension.c_str(), &CallbackGetMuteList, cb_pointer);
}

void User::GetBlacklist(const GetBlackListCallback& cb, const std::string& json_extension)
{
	GetBlackListCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetBlackListCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_user_get_mute_blacklist)(json_extension.c_str(), &CallbackGetBlackList, cb_pointer);
}

static User::UserNameCardChangedCallback g_cb_uinfo_changed_ = nullptr;
void User::RegUserNameCardChangedCb(const UserNameCardChangedCallback & cb, const std::string & json_extension)
{
	g_cb_uinfo_changed_ = cb;
	NIM_SDK_GET_FUNC(nim_user_reg_user_name_card_changed_cb)(json_extension.c_str(), &CallbackUserNameCardChange, &g_cb_uinfo_changed_);
}

bool User::GetUserNameCard(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension /*= ""*/)
{
	if (accids.empty())
		return false;

	GetUserNameCardCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetUserNameCardCallback(cb);
	}

	nim_cpp_wrapper_util::Json::Value values;
	for (auto iter = accids.cbegin(); iter != accids.cend(); ++iter)
		values.append(*iter);
	
	NIM_SDK_GET_FUNC(nim_user_get_user_name_card)(GetJsonStringWithNoStyled(values).c_str(), json_extension.c_str(), &CallbackGetUserNameCard, cb_pointer);

	return true;
}

bool User::GetUserNameCardOnline(const std::list<std::string>& accids, const GetUserNameCardCallback& cb, const std::string& json_extension /*= ""*/)
{
	if (accids.empty())
		return false;

	GetUserNameCardCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetUserNameCardCallback(cb);
	}

	nim_cpp_wrapper_util::Json::Value values;
	for (auto iter = accids.cbegin(); iter != accids.cend(); ++iter)
		values.append(*iter);

	NIM_SDK_GET_FUNC(nim_user_get_user_name_card_online)(GetJsonStringWithNoStyled(values).c_str(), json_extension.c_str(), &CallbackGetUserNameCard, cb_pointer);

	return true;
}

bool User::UpdateMyUserNameCard(const UserNameCard& namecard, const UpdateMyUserNameCardCallback& cb, const std::string& json_extension /*= ""*/)
{
	if (namecard.GetAccId().empty())
		return false;

	UpdateMyUserNameCardCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new UpdateMyUserNameCardCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_user_update_my_user_name_card)(namecard.ToJsonString().c_str(), json_extension.c_str(), &CallbackUpdateMyUserNameCard, cb_pointer);

	return true;
}

bool User::QueryUserListByKeyword(const std::string& keyword, const GetUserNameCardCallback& cb, const std::string& json_extension /*= ""*/)
{
	if (keyword.empty())
		return false;

	GetUserNameCardCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetUserNameCardCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_user_query_by_keyword)(keyword.c_str(), json_extension.c_str(), &CallbackGetUserNameCard, cb_pointer);

	return true;
}

bool User::ParseBlackListInfoChange(const SpecialRelationshipChangeEvent& change_event, BlackMuteListInfo& info)
{
	if (change_event.type_ != kNIMUserSpecialRelationshipChangeTypeMarkBlack)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(change_event.content_, values) && values.isObject())
	{
		info.accid_ = values["accid"].asString();
		info.set_black_ = values["black"].asBool();
		return true;
	}

	return false;
}

bool User::ParseMuteListInfoChange(const SpecialRelationshipChangeEvent& change_event, BlackMuteListInfo& info)
{
	if (change_event.type_ != kNIMUserSpecialRelationshipChangeTypeMarkMute)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(change_event.content_, values) && values.isObject())
	{
		info.accid_ = values["accid"].asString();
		info.set_mute_ = values["mute"].asBool();
		return true;
	}

	return false;
}

bool User::ParseSyncSpecialRelationshipChange(const SpecialRelationshipChangeEvent& change_event, std::list<BlackMuteListInfo>& black_mute_list)
{
	if (change_event.type_ != kNIMUserSpecialRelationshipChangeTypeSyncMuteAndBlackList)
		return false;

	return ParseSpecialListInfo(change_event.content_, black_mute_list);
}

void User::UpdatePushToken(const std::string& cer_name, const std::string& token, int type)
{
    NIM_SDK_GET_FUNC(nim_user_update_push_token)(cer_name.c_str(), token.c_str(), type);
}

void User::UnregUserCb()
{
	g_cb_relation_changed_ = nullptr;
	g_cb_uinfo_changed_ = nullptr;
}
}

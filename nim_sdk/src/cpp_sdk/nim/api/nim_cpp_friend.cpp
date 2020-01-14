/** @file nim_cpp_friend.cpp
  * @brief NIM 好友相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/8/17
  */

#include "src/cpp_sdk/nim/api//nim_cpp_friend.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_friend_reg_changed_cb)(const char *json_extension, nim_friend_change_cb_func cb, const void *user_data);
typedef void(*nim_friend_request)(const char *accid, NIMVerifyType verify_type, const char *msg, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_delete)(const char *accid, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_update)(const char *friend_json, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_get_list)(const char *json_extension, nim_friend_get_list_cb_func cb, const void *user_data);
typedef void(*nim_friend_get_profile)(const char *accid, const char *json_extension, nim_friend_get_profile_cb_func cb, const void *user_data);
typedef bool(*nim_friend_query_friendship_block)(const char *accid, const char *json_extension);
typedef void(*nim_friend_query_by_keyword)(const char *keyword, const char *json_extension, nim_friend_get_list_cb_func cb, const void *user_data);
#else
#include "nim_friend.h"
#endif

// 好友信息变化执行回调函数，不销毁该回调函数
static void CallbackFriendChange(NIMFriendChangeType type, const char *result_json, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Friend::FriendChangeCallback>(user_data, [=](const Friend::FriendChangeCallback& cb){
	
		FriendChangeEvent change_event;
		change_event.type_ = type;
		change_event.content_ = PCharToString(result_json);
		CallbackProxy::Invoke(cb, change_event);
	});

}

// 执行完好友操作回调函数后销毁之
static void CallbackFriendOpt(int res_code, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Friend::FriendOptCallback>(user_data, [=](const Friend::FriendOptCallback& cb){
	
		CallbackProxy::Invoke(cb, (NIMResCode)res_code);
	},true);
}

// 获取好友列表后执行回调函数，然后销毁该回调函数
static void CallbackGetFriendsList(int res_code, const char *result_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Friend::GetFriendsListCallback>(user_data, [=](const Friend::GetFriendsListCallback& cb){

		std::list<FriendProfile> friends_profile;
		ParseFriendsProfile(PCharToString(result_json), friends_profile);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, friends_profile);
	}, true);
}

// 获取好友信息后执行回调函数，然后销毁该回调函数
static void CallbackGetFriendProfile(const char *accid, const char *result_json, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Friend::GetFriendProfileCallback>(user_data, [=](const Friend::GetFriendProfileCallback& cb){

		FriendProfile friend_profile;
		ParseFriendProfile(PCharToString(result_json), friend_profile);
		CallbackProxy::Invoke(cb, PCharToString(accid), friend_profile);
	}, true);
}

static Friend::FriendChangeCallback g_cb_friend_changed_ = nullptr;
void Friend::RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension /* = "" */)
{
	g_cb_friend_changed_ = cb;
	NIM_SDK_GET_FUNC(nim_friend_reg_changed_cb)(json_extension.c_str(), &CallbackFriendChange, &g_cb_friend_changed_);
}

bool Friend::Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	if (accid.empty())
		return false;

	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb != nullptr)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_request)(accid.c_str(), verify_type, msg.c_str(), json_extension.c_str(), &CallbackFriendOpt, friend_opt_cb);

	return true;
}

bool Friend::Delete(const std::string &accid, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	if (accid.empty())
		return false;

	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_delete)(accid.c_str(), json_extension.c_str(), &CallbackFriendOpt, friend_opt_cb);

	return true;
}

bool Friend::DeleteEx(const std::string &accid, const DeleteFriendOption& option, const FriendOptCallback &cb)
{
	if (accid.empty())
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMFriendDeleteAlias] = option.delete_alias_;

	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_friend_delete)(accid.c_str(), values.toStyledString().c_str(), &CallbackFriendOpt, friend_opt_cb);

	return true;
}

bool Friend::Update(const FriendProfile &friend_profile, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	if (friend_profile.GetAccId().empty())
		return false;

	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_update)(friend_profile.ToJsonString().c_str(), json_extension.c_str(), &CallbackFriendOpt, friend_opt_cb);

	return true;
}

void Friend::GetList(const GetFriendsListCallback& cb, const std::string& json_extension)
{
	GetFriendsListCallback* get_friends_list_cb = nullptr;
	if (cb)
	{
		get_friends_list_cb = new GetFriendsListCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_get_list)(json_extension.c_str(), &CallbackGetFriendsList, get_friends_list_cb);
}

void Friend::GetFriendProfile(const std::string &accid, const GetFriendProfileCallback& cb, const std::string& json_extension/* = ""*/)
{
	GetFriendProfileCallback* get_profile_cb = nullptr;
	if (cb)
	{
		get_profile_cb = new GetFriendProfileCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_get_profile)(accid.c_str(), json_extension.c_str(), &CallbackGetFriendProfile, get_profile_cb);
}

bool Friend::ParseFriendAddEvent(const FriendChangeEvent& change_event, FriendAddEvent& out_event)
{
	if (change_event.type_ != kNIMFriendChangeTypeRequest)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(change_event.content_, values) && values.isObject())
	{
		out_event.accid_ = values["accid"].asString();
		out_event.add_type_ = (NIMVerifyType)values["type"].asUInt();
		out_event.msg_ = values["msg"].asString();
		return true;
	}

	return false;
}
	
bool Friend::ParseFriendDelEvent(const FriendChangeEvent& change_event, FriendDelEvent& out_event)
{
	if (change_event.type_ != kNIMFriendChangeTypeDel)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(change_event.content_, values) && values.isObject())
	{
		out_event.accid_ = values["accid"].asString();
		return true;
	}

	return false;
}

bool Friend::ParseFriendProfileUpdateEvent(const FriendChangeEvent& change_event, FriendProfileUpdateEvent& out_event)
{
	if (change_event.type_ != kNIMFriendChangeTypeUpdate)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(change_event.content_, values) && values.isObject())
	{
		ParseFriendProfile(values["info"], out_event.profile_);
		return true;
	}

	return false;
}

bool Friend::ParseFriendProfileSyncEvent(const FriendChangeEvent& change_event, FriendProfileSyncEvent& out_event)
{
	if (change_event.type_ != kNIMFriendChangeTypeSyncList)
		return false;

	return ParseFriendsProfile(change_event.content_, out_event.profiles_);
}

void Friend::UnregFriendCb()
{
	g_cb_friend_changed_ = nullptr;
}

bool Friend::QueryFriendshipBlock(const std::string& accid, const std::string& json_extension/* = ""*/)
{
	return 	NIM_SDK_GET_FUNC(nim_friend_query_friendship_block)(accid.c_str(), json_extension.c_str());
}

bool Friend::QueryFriendListByKeyword(std::string& keyword, const GetFriendsListCallback& cb, const std::string& json_extension /*= ""*/)
{
	if (keyword.empty())
		return false;

	GetFriendsListCallback* get_friends_list_cb = nullptr;
	if (cb)
	{
		get_friends_list_cb = new GetFriendsListCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_friend_query_by_keyword)(keyword.c_str(), json_extension.c_str(), &CallbackGetFriendsList, get_friends_list_cb);

	return true;
}

}

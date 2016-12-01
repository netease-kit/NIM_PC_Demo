/** @file nim_cpp_friend.cpp
  * @brief NIM 好友相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author caowei, Oleg, Harrison
  * @date 2015/8/17
  */

#include "nim_cpp_friend.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"

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
#else
#include "nim_friend.h"
#endif

// 好友信息变化执行回调函数，不销毁该回调函数
static void CallbackFriendChange(NIMFriendChangeType type, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::FriendChangeCallback* cb_pointer = (Friend::FriendChangeCallback*)user_data;
		if (*cb_pointer)
		{
			FriendChangeEvent change_event;
			change_event.type_ = type;
			change_event.content_ = PCharToString(result_json);
			(*cb_pointer)(change_event);
		}
	}
}

// 执行完好友操作回调函数后销毁之
static void CallbackFriendOpt(int res_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::FriendOptCallback* cb_pointer = (Friend::FriendOptCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((NIMResCode)res_code);
		}
		delete cb_pointer;
	}
}

// 获取好友列表后执行回调函数，然后销毁该回调函数
static void CallbackGetFriendsList(int res_code, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::GetFriendsListCallback* cb_pointer = (Friend::GetFriendsListCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<FriendProfile> friends_profile;
			ParseFriendsProfile(PCharToString(result_json), friends_profile);
			(*cb_pointer)((NIMResCode)res_code, friends_profile);
		}
		delete cb_pointer;
	}
}

// 获取好友信息后执行回调函数，然后销毁该回调函数
static void CallbackGetFriendProfile(const char *accid, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::GetFriendProfileCallback* cb_pointer = (Friend::GetFriendProfileCallback*)user_data;
		if (*cb_pointer)
		{
			FriendProfile friend_profile;
			ParseFriendProfile(PCharToString(result_json), friend_profile);
			(*cb_pointer)(accid, friend_profile);
		}
		delete cb_pointer;
	}
}

static Friend::FriendChangeCallback* g_cb_friend_changed_ = nullptr;
void Friend::RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension /* = "" */)
{
	if (g_cb_friend_changed_)
	{
		delete g_cb_friend_changed_;
		g_cb_friend_changed_ = nullptr;
	}
	g_cb_friend_changed_ = new FriendChangeCallback(cb);
	return NIM_SDK_GET_FUNC(nim_friend_reg_changed_cb)(json_extension.c_str(), &CallbackFriendChange, g_cb_friend_changed_);
}

bool Friend::Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	if (accid.empty())
		return false;

	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
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
	return NIM_SDK_GET_FUNC(nim_friend_get_list)(json_extension.c_str(), &CallbackGetFriendsList, get_friends_list_cb);
}

void Friend::GetFriendProfile(const std::string &accid, const GetFriendProfileCallback& cb, const std::string& json_extension/* = ""*/)
{
	GetFriendProfileCallback* get_profile_cb = nullptr;
	if (cb)
	{
		get_profile_cb = new GetFriendProfileCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_friend_get_profile)(accid.c_str(), json_extension.c_str(), &CallbackGetFriendProfile, get_profile_cb);
}

bool Friend::ParseFriendAddEvent(const FriendChangeEvent& change_event, FriendAddEvent& out_event)
{
	if (change_event.type_ != kNIMFriendChangeTypeRequest)
		return false;

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
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
	if (g_cb_friend_changed_)
	{
		delete g_cb_friend_changed_;
		g_cb_friend_changed_ = nullptr;
	}
}

bool Friend::QueryFriendshipBlock(const std::string& accid, const std::string& json_extension/* = ""*/)
{
	return 	NIM_SDK_GET_FUNC(nim_friend_query_friendship_block)(accid.c_str(), json_extension.c_str());
}
}

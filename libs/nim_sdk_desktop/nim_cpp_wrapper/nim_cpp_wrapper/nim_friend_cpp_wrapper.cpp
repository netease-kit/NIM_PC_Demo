#include "nim_friend_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{
typedef void(*nim_friend_reg_changed_cb)(const char *json_extension, nim_friend_change_cb_func cb, const void *user_data);
typedef void(*nim_friend_request)(const char *accid, NIMVerifyType verify_type, const char *msg, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_delete)(const char *accid, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_update)(const char *friend_json, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);
typedef void(*nim_friend_get_list)(const char *json_extension, nim_friend_get_list_cb_func cb, const void *user_data);

// 好友信息变化执行回调函数，不销毁该回调函数
static void FriendChangeCallbackWrapper(NIMFriendChangeType type, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::FriendChangeCallback* cb_pointer = (Friend::FriendChangeCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value json;
			Json::Reader reader;
			std::list<nim::UserProfile> user_profile_list;
			if (reader.parse(PCharToString(result_json), json))
			{
				if (type == kNIMFriendChangeTypeSyncList)
				{
					Json::Value json_list = json[kNIMFriendKey];
					int n = json_list.size();
					for (int i = 0; i < n; i++)
					{
						nim::UserProfile info;
						info.accid = json_list[i][nim::kNIMFriendKeyFaccid].asString();
						info.flag = NIMFriendFlag(json_list[i][nim::kNIMFriendKeyFlag].asInt());
						user_profile_list.push_back(info);
					}
				}
				else if (type == kNIMFriendChangeTypeRequest)
				{
					NIMVerifyType verify_type = (NIMVerifyType)json["type"].asInt();
					if (verify_type == kNIMVerifyTypeAdd || verify_type == kNIMVerifyTypeAgree)
					{
						nim::UserProfile info;
						info.accid = json[kNIMFriendKeyAccid].asString();
						info.flag = kNIMFriendFlagNormal;
						user_profile_list.push_back(info);
					}
				}
				else if (type == kNIMFriendChangeTypeDel)
				{
					nim::UserProfile info;
					info.accid = json[kNIMFriendKeyAccid].asString();
					info.flag = kNIMFriendFlagNotFriend;
					user_profile_list.push_back(info);
				}
			}
			PostTaskToUIThread(std::bind((*cb_pointer), type, user_profile_list));
		}
	}
}

// 执行完好友操作回调函数后销毁之
static void FriendOptCallbackWrapper(int res_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::FriendOptCallback* cb_pointer = (Friend::FriendOptCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), res_code));
		}
		delete cb_pointer;
	}
}

// 获取好友列表后执行回调函数，然后销毁该回调函数
static void GetFriendsListCallbackWrapper(int res_code, const char *result_json, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Friend::GetFriendsListCallback* cb_pointer = (Friend::GetFriendsListCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value json;
			Json::Reader reader;
			std::list<nim::UserProfile> user_profile_list;
			if (reader.parse(PCharToString(result_json), json))
			{
				Json::Value json_list = json[kNIMFriendKey];
				int n = json_list.size();
				for (int i = 0; i < n; i++)
				{
					nim::UserProfile info;
					info.accid = json_list[i][nim::kNIMFriendKeyFaccid].asString();
					info.flag = NIMFriendFlag(json_list[i][nim::kNIMFriendKeyFlag].asInt());
					user_profile_list.push_back(info);
				}
			}
			PostTaskToUIThread(std::bind((*cb_pointer), res_code, user_profile_list));
		}
		delete cb_pointer;
	}
}

static Friend::FriendChangeCallback* friend_change_cb = nullptr;
void Friend::RegChangeCb(const FriendChangeCallback &cb, const std::string& json_extension /* = "" */)
{
	delete friend_change_cb;
	if (cb)
	{
		friend_change_cb = new FriendChangeCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_friend_reg_changed_cb)(json_extension.c_str(), &FriendChangeCallbackWrapper, friend_change_cb);
}

void Friend::Request(const std::string &accid, NIMVerifyType verify_type, const std::string &msg, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_friend_request)(accid.c_str(), verify_type, msg.c_str(), json_extension.c_str(), &FriendOptCallbackWrapper, friend_opt_cb);
}

void Friend::Delete(const std::string &accid, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_friend_delete)(accid.c_str(), json_extension.c_str(), &FriendOptCallbackWrapper, friend_opt_cb);
}

void Friend::Update(const std::string &friend_json, const FriendOptCallback &cb, const std::string& json_extension /*= ""*/)
{
	FriendOptCallback* friend_opt_cb = nullptr;
	if (cb)
	{
		friend_opt_cb = new FriendOptCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_friend_update)(friend_json.c_str(), json_extension.c_str(), &FriendOptCallbackWrapper, friend_opt_cb);
}

void Friend::GetList(const GetFriendsListCallback& cb, const std::string& json_extension)
{
	GetFriendsListCallback* get_friends_list_cb = nullptr;
	if (cb)
	{
		get_friends_list_cb = new GetFriendsListCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_friend_get_list)(json_extension.c_str(), &GetFriendsListCallbackWrapper, get_friends_list_cb);
}

}

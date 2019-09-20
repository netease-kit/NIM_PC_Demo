/** @file nim_cpp_session.cpp
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "src/cpp_sdk/nim/api//nim_cpp_session.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_session_reg_change_cb)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_query_last_few_session_async)(int limit, const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data);
typedef void(*nim_session_query_all_recent_session_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void* user_data);
typedef void(*nim_session_query_all_recent_session_with_last_msg_excluded_type_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, enum NIMMessageType last_msg_excluded_type,const void* user_data);
typedef void(*nim_session_delete_recent_session_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_delete_all_recent_session_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_unread_count_zero_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_top)(enum NIMSessionType to_type, const char *id, bool top, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_extend_data)(enum NIMSessionType to_type, const char *id, const char *data, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_reset_all_unread_count_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_query_sessiondata_by_id_async)(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_query_sessiondata_by_id_cb_func cb, const void *user_data);
#else
#include "nim_session.h"
#endif

static void CallbackQuerySession(int total_unread_count, const char *result, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::QuerySessionListCallabck>(user_data, [=](const Session::QuerySessionListCallabck& cb){

		SessionDataList sl;
		ParseSessionList(PCharToString(result), sl);
		CallbackProxy::Invoke(cb, total_unread_count, sl);
	},true);

}

static void CallbackNotifySession(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Session::ChangeCallback>(user_data, [=](const Session::ChangeCallback& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session, total_unread_counts);
	}, true);

}

static void CallbackSessionChange(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::DeleteRecentSessionCallabck>(user_data, [=](const Session::DeleteRecentSessionCallabck& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session, total_unread_counts);
	});
}
static void CallbackQuerySessionData(int rescode, const char *result, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::QuerySessionDataCallback>(user_data, [=](const Session::QuerySessionDataCallback& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session);
	},true);
}

static Session::ChangeCallback g_cb_session_changed_ = nullptr;
void Session::RegChangeCb(const ChangeCallback& cb, const std::string& json_extension)
{
	g_cb_session_changed_ = cb;
	NIM_SDK_GET_FUNC(nim_session_reg_change_cb)(json_extension.c_str(), &CallbackSessionChange, &g_cb_session_changed_);
}

void Session::QueryLastFewSessionAsync(int limit, const QuerySessionListCallabck& cb, const std::string& json_extension /*= ""*/)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_last_few_session_async)(limit, json_extension.c_str(), &CallbackQuerySession, cb_pointer);
}

void Session::QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_all_recent_session_async)(json_extension.c_str(), &CallbackQuerySession, cb_pointer);
}
void Session::QueryAllRecentSessionAsync(NIMMessageType last_msg_excluded_type, const QuerySessionListCallabck& cb, const std::string& json_extension /*= ""*/)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_all_recent_session_with_last_msg_excluded_type_async)(json_extension.c_str(), &CallbackQuerySession, last_msg_excluded_type,cb_pointer);
}
bool Session::DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension)
{
	if (id.empty())
		return false;

	DeleteRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteRecentSessionCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_delete_recent_session_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackNotifySession, cb_pointer);

	return true;
}

void Session::DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb,  const std::string& json_extension)
{
	DeleteAllRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllRecentSessionCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_delete_all_recent_session_async)(json_extension.c_str(), &CallbackNotifySession, cb_pointer);
}

bool Session::SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension)
{
	if (id.empty())
		return false;

	SetUnreadCountZeroCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetUnreadCountZeroCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_unread_count_zero_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackNotifySession, cb_pointer);

	return true;
}

bool Session::SetSessionTop(enum NIMSessionType to_type, const std::string& id, bool top, const ChangeCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (id.empty())
		return false;

	ChangeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ChangeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_top)(to_type, id.c_str(), top, json_extension.c_str(), &CallbackSessionChange, cb_pointer);
	return true;
}

bool Session::SetSessionExtendData(enum NIMSessionType to_type, const std::string& id, const std::string& data, const ChangeCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (id.empty() && data.length() > 4096)
		return false;

	ChangeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ChangeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_extend_data)(to_type, id.c_str(), data.c_str(), json_extension.c_str(), &CallbackSessionChange, cb_pointer);
	return true;
}

bool Session::SetAllUnreadCountZeroAsync(const SetUnreadCountZeroCallback& cb, const std::string& json_extension/* = ""*/)
{
	SetUnreadCountZeroCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetUnreadCountZeroCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_reset_all_unread_count_async)(json_extension.c_str(), &CallbackSessionChange, cb_pointer);

	return true;
}
void Session::QuerySessionDataById(NIMSessionType to_type, const std::string& id, const QuerySessionDataCallback& cb, const std::string& json_extension/* = ""*/)
{
	Session::QuerySessionDataCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new Session::QuerySessionDataCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_sessiondata_by_id_async)(to_type,id.c_str(),json_extension.c_str(), &CallbackQuerySessionData, cb_pointer);

}
void Session::UnregSessionCb()
{
	g_cb_session_changed_ = nullptr;
}

}

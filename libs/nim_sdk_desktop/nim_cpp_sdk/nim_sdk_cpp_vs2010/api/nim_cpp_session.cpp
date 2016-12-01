/** @file nim_cpp_session.cpp
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/2/1
  */

#include "nim_cpp_session.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_session_reg_change_cb)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_query_all_recent_session_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void* user_data);
typedef void(*nim_session_delete_recent_session_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_delete_all_recent_session_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_unread_count_zero_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_top)(enum NIMSessionType to_type, const char *id, bool top, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_extend_data)(enum NIMSessionType to_type, const char *id, const char *data, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
#else
#include "nim_session.h"
#endif

static void CallbackQuerySession(int total_unread_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::QuerySessionListCallabck* cb_pointer = (Session::QuerySessionListCallabck*)user_data;
		if (*cb_pointer)
		{
			SessionDataList sl;
			ParseSessionList(PCharToString(result), sl);
			(*cb_pointer)(total_unread_count, sl);
		}
		delete cb_pointer;
	}
}

static void CallbackNotifySession(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::ChangeCallback* cb_pointer = (Session::ChangeCallback*)user_data;
		if (*cb_pointer)
		{
			SessionData session;
			ParseSession(PCharToString(result), session);
			(*cb_pointer)((nim::NIMResCode)rescode, session, total_unread_counts);
		}
		delete cb_pointer;
	}
}

static void CallbackSessionChange(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::DeleteRecentSessionCallabck* cb_pointer = (Session::DeleteRecentSessionCallabck*)user_data;
		if (*cb_pointer)
		{
			SessionData session;
			ParseSession(PCharToString(result), session);
			(*cb_pointer)((nim::NIMResCode)rescode, session, total_unread_counts);
		}
	}
}

static Session::ChangeCallback* g_cb_session_changed_ = nullptr;
void Session::RegChangeCb(const ChangeCallback& cb, const std::string& json_extension)
{
	if (g_cb_session_changed_)
	{
		delete g_cb_session_changed_;
		g_cb_session_changed_ = nullptr;
	}
	g_cb_session_changed_ = new ChangeCallback(cb);
	return NIM_SDK_GET_FUNC(nim_session_reg_change_cb)(json_extension.c_str(), &CallbackSessionChange, g_cb_session_changed_);
}

void Session::QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	return NIM_SDK_GET_FUNC(nim_session_query_all_recent_session_async)(json_extension.c_str(), &CallbackQuerySession, cb_pointer);
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
	return NIM_SDK_GET_FUNC(nim_session_delete_all_recent_session_async)(json_extension.c_str(), &CallbackNotifySession, cb_pointer);
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

void Session::UnregSessionCb()
{
	if (g_cb_session_changed_)
	{
		delete g_cb_session_changed_;
		g_cb_session_changed_ = nullptr;
	}
}

}
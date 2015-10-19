#include "nim_session_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

typedef void(*nim_session_reg_change_cb)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_query_all_recent_session_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void* user_data);
typedef void(*nim_session_delete_recent_session_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_delete_all_recent_session_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_unread_count_zero_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);


static void CallbackWrapper(int total_unread_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::QuerySessionListCallabck* cb_pointer = (Session::QuerySessionListCallabck*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), total_unread_count, PCharToString(result)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper2(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::DeleteRecentSessionCallabck* cb_pointer = (Session::DeleteRecentSessionCallabck*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)rescode, PCharToString(result), total_unread_counts));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper3NoDelete(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Session::DeleteRecentSessionCallabck* cb_pointer = (Session::DeleteRecentSessionCallabck*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)rescode, PCharToString(result), total_unread_counts));
		}
	}
}


static Session::ChangeCallback* g_chang_cb_pointer = nullptr;
void Session::RegChangeCb(const ChangeCallback& cb, const std::string& json_extension)
{
	delete g_chang_cb_pointer;
	if (cb)
	{
		g_chang_cb_pointer = new ChangeCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_session_reg_change_cb)(json_extension.c_str(), &CallbackWrapper3NoDelete, g_chang_cb_pointer);
}

void Session::QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	return NIM_SDK_Get_Function(nim_session_query_all_recent_session_async)(json_extension.c_str(), &CallbackWrapper, cb_pointer);
}

void Session::DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension)
{
	DeleteRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteRecentSessionCallabck(cb);
	}
	return NIM_SDK_Get_Function(nim_session_delete_recent_session_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void Session::DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb,  const std::string& json_extension)
{
	DeleteAllRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllRecentSessionCallabck(cb);
	}
	return NIM_SDK_Get_Function(nim_session_delete_all_recent_session_async)(json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void Session::SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension)
{
	SetUnreadCountZeroCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetUnreadCountZeroCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_session_set_unread_count_zero_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

}
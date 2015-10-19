#include "nim_sysmsg_cpp_wrapper.h"
#include "nim_sdk_function.h"


namespace nim
{

typedef void(*nim_sysmsg_reg_sysmsg_cb)(const char *json_extension, nim_sysmsg_receive_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_msg_async)(int limit_count, __int64 last_time, const char *json_extension, nim_sysmsg_query_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_unread_count)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_set_status_async)(__int64 msg_id, nim::NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_read_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_delete_async)(__int64 msg_id, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);


static void CallbackWrapper1NoDelete(const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::SysmsgCallback* cb_pointer = (SystemMsg::SysmsgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(result)));
		}
	}
}

static void CallbackWrapper2(int count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::QueryMsgCallback* cb_pointer = (SystemMsg::QueryMsgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), count, PCharToString(result)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper3(int res_code, __int64 msg_id, int unread_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::SetStatusCallback* cb_pointer = (SystemMsg::SetStatusCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, msg_id, unread_count));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper4(int res_code, int unread_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::ReadAllCallback* cb_pointer = (SystemMsg::ReadAllCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, unread_count));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperSysmsgUnread(int res_code, int unread_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::QuerySysmsgUnreadCallback* cb_pointer = (SystemMsg::QuerySysmsgUnreadCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, unread_count));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDel(int res_code, __int64 msg_id, int unread_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::DeleteCallback* cb_pointer = (SystemMsg::DeleteCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, msg_id, unread_count));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperDelAll(int res_code, int unread_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		SystemMsg::DeleteAllCallback* cb_pointer = (SystemMsg::DeleteAllCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, unread_count));
		}
		delete cb_pointer;
	}
}

SystemMsg::SysmsgCallback* cb_pointer = nullptr;
 void SystemMsg::RegSysmsgCb(const SysmsgCallback& cb, const std::string& json_extension)
 {
	 delete cb_pointer;
	 if (cb)
	 {
		 cb_pointer = new SysmsgCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_reg_sysmsg_cb)(json_extension.c_str(), &CallbackWrapper1NoDelete, cb_pointer);
 }

 void SystemMsg::QueryMsgAsync(int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension)
 {
	 QueryMsgCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new QueryMsgCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_query_msg_async)(limit_count, last_time, json_extension.c_str(), &CallbackWrapper2, cb_pointer);
 }

 void SystemMsg::QueryUnreadCount(const QuerySysmsgUnreadCallback& cb, const std::string& json_extension /*= ""*/)
 {
	 QuerySysmsgUnreadCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new QuerySysmsgUnreadCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_query_unread_count)(json_extension.c_str(), &CallbackWrapperSysmsgUnread, cb_pointer);
 }

 void SystemMsg::SetStatusAsync(__int64 msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension)
 {
	 SetStatusCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new SetStatusCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_set_status_async)(msg_id, status, json_extension.c_str(), &CallbackWrapper3, cb_pointer);
 }

 void SystemMsg::ReadAllAsync(const ReadAllCallback& cb, const std::string& json_extension)
 {
	 ReadAllCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new ReadAllCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_read_all_async)(json_extension.c_str(), &CallbackWrapper4, cb_pointer);
 }

 void SystemMsg::DeleteAsync( __int64 msg_id, const DeleteCallback& cb, const std::string& json_extension /*= ""*/ )
 {
	 DeleteCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new DeleteCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_delete_async)(msg_id, json_extension.c_str(), &CallbackWrapperDel, cb_pointer);
 }

 void SystemMsg::DeleteAllAsync( const DeleteAllCallback& cb, const std::string& json_extension /*= ""*/ )
 {
	 DeleteAllCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new DeleteAllCallback(cb);
	 }
	 return NIM_SDK_Get_Function(nim_sysmsg_delete_all_async)(json_extension.c_str(), &CallbackWrapperDelAll, cb_pointer);
 }

}
#include "nim_cpp_sysmsg.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"

namespace nim
{

typedef void(*nim_sysmsg_reg_sysmsg_cb)(const char *json_extension, nim_sysmsg_receive_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_msg_async)(int limit_count, __int64 last_time, const char *json_extension, nim_sysmsg_query_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_unread_count)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_set_status_async)(__int64 msg_id, nim::NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_read_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_delete_async)(__int64 msg_id, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_reg_custom_notification_arc_cb)(const char *json_extension, nim_custom_sysmsg_arc_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_send_custom_notification)(const char *json_msg, const char *json_extension);
typedef void(*nim_sysmsg_set_logs_status_by_type_async)(NIMSysMsgType type, NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_logs_by_type_async)(NIMSysMsgType type, const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);

static void CallbackSysmsgChange(const char *result, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::ReceiveSysmsgCallback* cb_pointer = (SystemMsg::ReceiveSysmsgCallback*)callback;
		if (*cb_pointer)
		{
			SysMessage msg;
			if (ParseSysMessage(PCharToString(result), msg))
			{
				PostTaskToUIThread(std::bind((*cb_pointer), msg));
				//(*cb_pointer)(msg);
			}
		}
	}
}

static void CallbackSendCustomSysmsg(const char *result, const void *callback)
{
	if (callback)
	{
		SystemMsg::SendCustomSysmsgCallback* cb_pointer = (SystemMsg::SendCustomSysmsgCallback*)callback;
		if (*cb_pointer)
		{
			SendMessageArc arc;
			if (ParseSendMessageArc(PCharToString(result), arc))
			{
				PostTaskToUIThread(std::bind((*cb_pointer), arc));
				//(*cb_pointer)(arc);
			}
		}
	}
}

static void CallbackQuerySysmsg(int count, const char *result, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::QueryMsgCallback* cb_pointer = (SystemMsg::QueryMsgCallback*)callback;
		if (*cb_pointer)
		{
			std::list<SysMessage> msgs;
			int unread = 0;
			if (ParseSysMessages(PCharToString(result), msgs, &unread))
			{
				PostTaskToUIThread(std::bind((*cb_pointer), count, unread, msgs));
				//(*cb_pointer)(count, unread, msgs);
			}
		}
		delete cb_pointer;
	}
}

static void CallbackNotifySingleSysmsg(int res_code, __int64 msg_id, int unread_count, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::SetStatusCallback* cb_pointer = (SystemMsg::SetStatusCallback*)callback;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, msg_id, unread_count));
			//(*cb_pointer)((nim::NIMResCode)res_code, msg_id, unread_count);
		}
		delete cb_pointer;
	}
}

static void CallbackNotifySysmsgRes(int res_code, int unread_count, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::ReadAllCallback* cb_pointer = (SystemMsg::ReadAllCallback*)callback;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, unread_count));
			//(*cb_pointer)((nim::NIMResCode)res_code, unread_count);
		}
		delete cb_pointer;
	}
}

SystemMsg::ReceiveSysmsgCallback* cb_pointer = nullptr;
 void SystemMsg::RegSysmsgCb(const ReceiveSysmsgCallback& cb, const std::string& json_extension)
 {
	 delete cb_pointer;
	 if (cb)
	 {
		 cb_pointer = new ReceiveSysmsgCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_reg_sysmsg_cb)(json_extension.c_str(), &CallbackSysmsgChange, cb_pointer);
 }

 static SystemMsg::SendCustomSysmsgCallback* g_send_custom_sysmsg_cb_pointer = nullptr;
 void SystemMsg::RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension)
 {
	 delete g_send_custom_sysmsg_cb_pointer;
	 if (cb)
	 {
		 g_send_custom_sysmsg_cb_pointer = new SendCustomSysmsgCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_reg_custom_notification_arc_cb)(json_extension.c_str(), &CallbackSendCustomSysmsg, g_send_custom_sysmsg_cb_pointer);
 }

 void SystemMsg::SendCustomNotificationMsg(const std::string& json_msg)
 {
	 return NIM_SDK_GET_FUNC(nim_sysmsg_send_custom_notification)(json_msg.c_str(), nullptr);
 }

 std::string SystemMsg::CreateCustomNotificationMsg(const std::string& receiver_id
	 , const NIMSysMsgType type
	 , const std::string& client_msg_id
	 , const std::string& content
	 , bool support_offline
	 , const std::string& apns_text/* = ""*/)
 {
	 Json::Value values;
	 values[kNIMSysMsgKeyToAccount] = receiver_id;
	 values[kNIMSysMsgKeyType] = type;
	 values[kNIMSysMsgKeyAttach] = content;
	 values[kNIMSysMsgKeyLocalClientMsgId] = client_msg_id;
	 values[kNIMSysMsgKeyCustomSaveFlag] = support_offline ? 1 : 0;
	 values[kNIMSysMsgKeyCustomApnsText] = apns_text;
	 return values.toStyledString();
 }

 bool SystemMsg::QueryMsgAsync(int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension)
 {
	 if (limit_count <= 0)
		 return false;
	 QueryMsgCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new QueryMsgCallback(cb);
	 }
	 NIM_SDK_GET_FUNC(nim_sysmsg_query_msg_async)(limit_count, last_time, json_extension.c_str(), &CallbackQuerySysmsg, cb_pointer);

	 return true;
 }

 void SystemMsg::QueryUnreadCount(const QuerySysmsgUnreadCallback& cb, const std::string& json_extension /*= ""*/)
 {
	 QuerySysmsgUnreadCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new QuerySysmsgUnreadCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_query_unread_count)(json_extension.c_str(), &CallbackNotifySysmsgRes, cb_pointer);
 }

 bool SystemMsg::SetStatusAsync(__int64 msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension)
 {
	 SetStatusCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new SetStatusCallback(cb);
	 }
	 NIM_SDK_GET_FUNC(nim_sysmsg_set_status_async)(msg_id, status, json_extension.c_str(), &CallbackNotifySingleSysmsg, cb_pointer);

	 return true;
 }

 void SystemMsg::ReadAllAsync(const ReadAllCallback& cb, const std::string& json_extension)
 {
	 ReadAllCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new ReadAllCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_read_all_async)(json_extension.c_str(), &CallbackNotifySysmsgRes, cb_pointer);
 }

 bool SystemMsg::DeleteAsync(__int64 msg_id, const DeleteCallback& cb, const std::string& json_extension /*= ""*/)
 {
	 DeleteCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new DeleteCallback(cb);
	 }
	 NIM_SDK_GET_FUNC(nim_sysmsg_delete_async)(msg_id, json_extension.c_str(), &CallbackNotifySingleSysmsg, cb_pointer);

	 return true;
 }

 void SystemMsg::DeleteAllAsync( const DeleteAllCallback& cb, const std::string& json_extension /*= ""*/ )
 {
	 DeleteAllCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new DeleteAllCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_delete_all_async)(json_extension.c_str(), &CallbackNotifySysmsgRes, cb_pointer);
 }

 void SystemMsg::SetStatusByTypeAsync(NIMSysMsgType type, NIMSysMsgStatus status, const BatchSetCallback& cb, const std::string& json_extension/* = ""*/)
 {
	 BatchSetCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new BatchSetCallback(cb);
	 } 
	 return NIM_SDK_GET_FUNC(nim_sysmsg_set_logs_status_by_type_async)(type, status, json_extension.c_str(), &CallbackNotifySysmsgRes, cb_pointer);
 }

 void SystemMsg::DeleteStatusByTypeAsync(NIMSysMsgType type, const BatchSetCallback& cb, const std::string& json_extension/* = ""*/)
 {
	 BatchSetCallback* cb_pointer = nullptr;
	 if (cb)
	 {
		 cb_pointer = new BatchSetCallback(cb);
	 }
	 return NIM_SDK_GET_FUNC(nim_sysmsg_delete_logs_by_type_async)(type, json_extension.c_str(), &CallbackNotifySysmsgRes, cb_pointer);
 }

}
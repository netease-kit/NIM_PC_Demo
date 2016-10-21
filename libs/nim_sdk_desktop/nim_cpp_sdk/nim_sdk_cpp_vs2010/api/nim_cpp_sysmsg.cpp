/** @file nim_cpp_sysmsg.cpp
  * @brief 系统消息接口；主要包括查询系统消息、删除系统消息等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/2/1
  */

#include "nim_cpp_sysmsg.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_sysmsg_reg_sysmsg_cb)(const char *json_extension, nim_sysmsg_receive_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_msg_async)(int limit_count, int64_t last_time, const char *json_extension, nim_sysmsg_query_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_unread_count)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_set_status_async)(int64_t msg_id, nim::NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_read_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_delete_async)(int64_t msg_id, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_all_async)(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_reg_custom_notification_ack_cb)(const char *json_extension, nim_custom_sysmsg_ack_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_send_custom_notification)(const char *json_msg, const char *json_extension);
typedef void(*nim_sysmsg_set_logs_status_by_type_async)(NIMSysMsgType type, NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_logs_by_type_async)(NIMSysMsgType type, const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
#else
#include "nim_sysmsg.h"
#endif

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
				(*cb_pointer)(msg);
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
			if (ParseSendMessageAck(PCharToString(result), arc))
			{
				(*cb_pointer)(arc);
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
				(*cb_pointer)(count, unread, msgs);
			}
		}
		delete cb_pointer;
	}
}

static void CallbackNotifySingleSysmsg(int res_code, int64_t msg_id, int unread_count, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::SetStatusCallback* cb_pointer = (SystemMsg::SetStatusCallback*)callback;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)res_code, msg_id, unread_count);
		}
		delete cb_pointer;
	}
}

static void CallbackNotifySysmsgRes(int res_code, int unread_count, const char *json_extension, const void *callback)
{
	if (callback)
	{
		SystemMsg::NotifySysmsgResCallback* cb_pointer = (SystemMsg::NotifySysmsgResCallback*)callback;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)res_code, unread_count);
		}
		delete cb_pointer;
	}
}

SystemMsg::ReceiveSysmsgCallback* g_cb_receive_sysmsg_ = nullptr;
 void SystemMsg::RegSysmsgCb(const ReceiveSysmsgCallback& cb, const std::string& json_extension)
 {
	 if (g_cb_receive_sysmsg_)
	 {
		 delete g_cb_receive_sysmsg_;
		 g_cb_receive_sysmsg_ = nullptr;
	 }
	 g_cb_receive_sysmsg_ = new ReceiveSysmsgCallback(cb);
	 return NIM_SDK_GET_FUNC(nim_sysmsg_reg_sysmsg_cb)(json_extension.c_str(), &CallbackSysmsgChange, g_cb_receive_sysmsg_);
 }

 static SystemMsg::SendCustomSysmsgCallback* g_cb_send_custom_sysmsg_ = nullptr;
 void SystemMsg::RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension)
 {
	 if (g_cb_send_custom_sysmsg_)
	 {
		 delete g_cb_send_custom_sysmsg_;
		 g_cb_send_custom_sysmsg_ = nullptr;
	 }
	 g_cb_send_custom_sysmsg_ = new SendCustomSysmsgCallback(cb);
	 return NIM_SDK_GET_FUNC(nim_sysmsg_reg_custom_notification_ack_cb)(json_extension.c_str(), &CallbackSendCustomSysmsg, g_cb_send_custom_sysmsg_);
 }

 void SystemMsg::SendCustomNotificationMsg(const std::string& json_msg)
 {
	 return NIM_SDK_GET_FUNC(nim_sysmsg_send_custom_notification)(json_msg.c_str(), nullptr);
 }

 std::string SystemMsg::CreateCustomNotificationMsg(const std::string& receiver_id
	 , const NIMSysMsgType type
	 , const std::string& client_msg_id
	 , const std::string& content
	 , const SysMessageSetting& msg_setting
	 , int64_t timetag/* = 0*/)
 {
	 Json::Value values;
	 values[kNIMSysMsgKeyToAccount] = receiver_id;
	 values[kNIMSysMsgKeyType] = type;
	 values[kNIMSysMsgKeyAttach] = content;
	 values[kNIMSysMsgKeyLocalClientMsgId] = client_msg_id;
	 msg_setting.ToJsonValue(values);
	 if (timetag > 0)
		 values[kNIMSysMsgKeyTime] = timetag;
	 return GetJsonStringWithNoStyled(values);
 }

 bool SystemMsg::QueryMsgAsync(int limit_count, int64_t last_time, const QueryMsgCallback& cb, const std::string& json_extension)
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

 bool SystemMsg::SetStatusAsync(int64_t msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension)
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

 bool SystemMsg::DeleteAsync(int64_t msg_id, const DeleteCallback& cb, const std::string& json_extension /*= ""*/)
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

void SystemMsg::UnregSysmsgCb()
{
	if (g_cb_receive_sysmsg_)
	{
		delete g_cb_receive_sysmsg_;
		g_cb_receive_sysmsg_ = nullptr;
	}

	if (g_cb_send_custom_sysmsg_)
	{
		delete g_cb_send_custom_sysmsg_;
		g_cb_send_custom_sysmsg_ = nullptr;
	}

}

}
/** @file nim_chatroom_cpp.cpp
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/12/29
  */

#include "nim_chatroom_cpp.h"
#include "nim_chatroom_sdk_helper.h"

namespace nim_chatroom
{
typedef void(*nim_chatroom_reg_enter_cb)(const char *json_extension, nim_chatroom_enter_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_exit_cb)(const char *json_extension, nim_chatroom_exit_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_send_msg_ack_cb)(const char *json_extension, nim_chatroom_sendmsg_arc_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_receive_msg_cb)(const char *json_extension, nim_chatroom_receive_msg_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_receive_notification_cb)(const char *json_extension, nim_chatroom_receive_notification_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_link_condition_cb)(const char *json_extension, nim_chatroom_link_condition_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_init)(const char *json_extension);
typedef bool(*nim_chatroom_enter)(const __int64 room_id, const char *request_enter_data, const char *enter_info, const char *json_extension);
typedef void(*nim_chatroom_exit)(const __int64 room_id, const char *json_extension);
typedef void(*nim_chatroom_cleanup)(const char *json_extension);
typedef void(*nim_chatroom_send_msg)(const __int64 room_id, const char *msg, const char *json_extension);
typedef void(*nim_chatroom_get_members_online_async)(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_msg_history_online_async)(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_msg_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_set_member_attribute_async)(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_set_member_attribute_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_info_async)(const __int64 room_id, const char *json_extension, nim_chatroom_get_info_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_members_by_ids_online_async)(const __int64 room_id, const char *ids_json_array_string, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_kick_member_async)(const __int64 room_id, const char *id, const char *notify_ext, const char *json_extension, nim_chatroom_kick_member_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_set_proxy)(NIMChatRoomProxyType type, const char *host, int port, const char *user, const char *password);
typedef void(*nim_chatroom_temp_mute_member_async)(const __int64 room_id, const char *accid, const __int64 duration, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_temp_mute_member_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_update_room_info_async)(const __int64 room_id, const char *room_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_room_info_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_update_my_role_async)(const __int64 room_id, const char *role_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_my_role_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_offer_async)(const __int64 room_id, const char *element_key, const char *element_value, const char *json_extension, nim_chatroom_queue_offer_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_poll_async)(const __int64 room_id, const char *element_key, const char *json_extension, nim_chatroom_queue_poll_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_list_async)(const __int64 room_id, const char *json_extension, nim_chatroom_queue_list_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_drop_async)(const __int64 room_id, const char *json_extension, nim_chatroom_queue_drop_cb_func cb, const void *user_data);

static void CallbackEnter(__int64 room_id, int step, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::EnterCallback* cb_pointer = (ChatRoom::EnterCallback* )user_data;
		if (*cb_pointer)
		{
			ChatRoomInfo info;
			ChatRoomMemberInfo my_info;
			ParseChatRoomEnterCallbackResultInfo(PCharToString(result), info, my_info);
			//如果错误码error_code为kResRoomLocalNeedRequestAgain，聊天室重连机制结束，则需要向IM服务器重新请求进入该聊天室权限
			(*cb_pointer)(room_id, (NIMChatRoomEnterStep)step, error_code, info, my_info);
		}
	}
}

static void CallbackExit(__int64 room_id, int error_code, int exit_reason, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::ExitCallback* cb_pointer = (ChatRoom::ExitCallback* )user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(room_id, error_code, (NIMChatRoomExitReason)exit_reason);
		}
	}
}

static void CallbackSendMsgAck(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::SendMsgAckCallback* cb_pointer = (ChatRoom::SendMsgAckCallback* )user_data;
		if (*cb_pointer)
		{
			Json::Value value;
			if (ParseJsonValue(PCharToString(result), value))
			{
				ChatRoomMessage msg;
				msg.ParseFromJsonValue(value);
				(*cb_pointer)(room_id, error_code, msg);
			}
		}
	}
}

static void CallbackReceiveMsg(__int64 room_id, const char *content, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::ReceiveMsgCallback* cb_pointer = (ChatRoom::ReceiveMsgCallback* )user_data;
		if (*cb_pointer)
		{
			Json::Value value;
			if (ParseJsonValue(PCharToString(content), value))
			{
				ChatRoomMessage msg;
				msg.ParseFromJsonValue(value);
				(*cb_pointer)(room_id, msg);
			}
		}
	}
}

static void CallbackReceiveNotification(__int64 room_id, const char *content, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::NotificationCallback* cb_pointer = (ChatRoom::NotificationCallback* )user_data;
		if (*cb_pointer)
		{
			Json::Value value;
			if (ParseJsonValue(PCharToString(content), value))
			{
				ChatRoomNotification msg;
				msg.ParseFromJsonValue(value);
				(*cb_pointer)(room_id, msg);
			}
		}
	}
}

static void CallbackLinkCondition(__int64 room_id, int condition, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::LinkConditionCallback* cb_pointer = (ChatRoom::LinkConditionCallback* )user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(room_id, (NIMChatRoomLinkCondition)condition);
		}
	}
}

static ChatRoom::EnterCallback* g_cb_enter_ = nullptr;
void ChatRoom::RegEnterCb(const EnterCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_enter_)
	{
		delete g_cb_enter_;
		g_cb_enter_ = nullptr;
	}
	g_cb_enter_ = new EnterCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_enter_cb)(json_extension.c_str(), &CallbackEnter, g_cb_enter_);
}

static ChatRoom::ExitCallback* g_cb_exit_ = nullptr;
void ChatRoom::RegExitCb(const ExitCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_exit_)
	{
		delete g_cb_exit_;
		g_cb_exit_ = nullptr;
	}
	g_cb_exit_ = new ExitCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_exit_cb)(json_extension.c_str(), &CallbackExit, g_cb_exit_);
}

static ChatRoom::SendMsgAckCallback* g_cb_send_msg_ack_ = nullptr;
void ChatRoom::RegSendMsgAckCb(const SendMsgAckCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_send_msg_ack_)
	{
		delete g_cb_send_msg_ack_;
		g_cb_send_msg_ack_ = nullptr;
	}
	g_cb_send_msg_ack_ = new SendMsgAckCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_send_msg_ack_cb)(json_extension.c_str(), &CallbackSendMsgAck, g_cb_send_msg_ack_);
}

static ChatRoom::ReceiveMsgCallback* g_cb_receive_msg_ = nullptr;
void ChatRoom::RegReceiveMsgCb(const ReceiveMsgCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_receive_msg_)
	{
		delete g_cb_receive_msg_;
		g_cb_receive_msg_ = nullptr;
	}
	g_cb_receive_msg_ = new ReceiveMsgCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_receive_msg_cb)(json_extension.c_str(), &CallbackReceiveMsg, g_cb_receive_msg_);
}

static ChatRoom::NotificationCallback* g_cb_notification_ = nullptr;
void ChatRoom::RegNotificationCb(const NotificationCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_notification_)
	{
		delete g_cb_notification_;
		g_cb_notification_ = nullptr;
	}
	g_cb_notification_ = new NotificationCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_receive_notification_cb)(json_extension.c_str(), &CallbackReceiveNotification, g_cb_notification_);
}

static ChatRoom::LinkConditionCallback* g_cb_link_condition_ = nullptr;
void ChatRoom::RegLinkConditionCb(const LinkConditionCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (g_cb_link_condition_)
	{
		delete g_cb_link_condition_;
		g_cb_link_condition_ = nullptr;
	}
	g_cb_link_condition_ = new LinkConditionCallback(cb);
	return NIM_SDK_GET_FUNC(nim_chatroom_reg_link_condition_cb)(json_extension.c_str(), &CallbackLinkCondition, g_cb_link_condition_);
}

bool ChatRoom::Init(const std::string& json_extension/* = ""*/)
{
	if (!SDKFunction::LoadSdkDll())
		return false;

	NIM_SDK_GET_FUNC(nim_chatroom_init)(json_extension.c_str());
	return true;
}

bool ChatRoom::Enter(const __int64 room_id, const std::string& request_enter_data, const ChatRoomEnterInfo& info/* = ChatRoomEnterInfo()*/, const std::string& json_extension/* = ""*/)
{
	return NIM_SDK_GET_FUNC(nim_chatroom_enter)(room_id, request_enter_data.c_str(), info.ToJsonString().c_str(), json_extension.c_str());
}

void ChatRoom::Exit(const __int64 room_id, const std::string& json_extension/* = ""*/)
{
	return NIM_SDK_GET_FUNC(nim_chatroom_exit)(room_id, json_extension.c_str());
}

void ChatRoom::Cleanup(const std::string& json_extension/* = ""*/)
{
	NIM_SDK_GET_FUNC(nim_chatroom_cleanup)(json_extension.c_str());
	SDKFunction::UnLoadSdkDll();
}

void ChatRoom::SendMsg(const __int64 room_id, const std::string& json_msg, const std::string& json_extension/* = ""*/)
{
	return NIM_SDK_GET_FUNC(nim_chatroom_send_msg)(room_id, json_msg.c_str(), json_extension.c_str());
}

std::string ChatRoom::CreateRoomMessage(const NIMChatRoomMsgType msg_type
	, const std::string& client_msg_id
	, const std::string& attach
	, const ChatRoomMessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
	values[kNIMChatRoomMsgKeyType] = msg_type;
	values[kNIMChatRoomMsgKeyAttach] = attach;
	values[kNIMChatRoomMsgKeyClientMsgid] = client_msg_id;
	msg_setting.ToJsonValue(values);
	return GetJsonStringWithNoStyled(values);
}

static void CallbackGetMembersOnline(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::GetMembersCallback *cb = (ChatRoom::GetMembersCallback*)user_data;
		if (*cb)
		{
			std::list<ChatRoomMemberInfo> infos;
			ParseChatRoomMemberInfos(PCharToString(result), infos);
			(*cb)(room_id, error_code, infos);
		}
	}
}

static void CallbackGetMsgHistoryOnline(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::GetMsgHistoryCallback *cb = (ChatRoom::GetMsgHistoryCallback*)user_data;
		if (*cb)
		{
			std::list<ChatRoomMessage> msgs;
			ParseChatRoomMsgs(PCharToString(result), msgs);
			(*cb)(room_id, error_code, msgs);
		}
	}
}

static void CallbackSetMemberAtribute(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::SetMemberAttributeCallback *cb = (ChatRoom::SetMemberAttributeCallback*)user_data;
		if (*cb)
		{
			Json::Value value;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), value) && value.isObject())
			{
				ChatRoomMemberInfo info;
				info.ParseFromJsonValue(value);
				(*cb)(room_id, error_code, info);
				return;
			}
			(*cb)(room_id, error_code, ChatRoomMemberInfo());
		}
	}
}

void ChatRoom::GetMembersOnlineAsync(const __int64 room_id, 
	const ChatRoomGetMembersParameters &parameters, 
	const GetMembersCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	GetMembersCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new GetMembersCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_get_members_online_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackGetMembersOnline,
		cb_pointer);
}

void ChatRoom::GetMessageHistoryOnlineAsync(const __int64 room_id, 
	const ChatRoomGetMsgHistoryParameters &parameters, 
	const GetMsgHistoryCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	GetMsgHistoryCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new GetMsgHistoryCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_get_msg_history_online_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackGetMsgHistoryOnline,
		cb_pointer);
}

void ChatRoom::SetMemberAttributeOnlineAsync(const __int64 room_id, 
	const ChatRoomSetMemberAttributeParameters &parameters, 
	const SetMemberAttributeCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	SetMemberAttributeCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new SetMemberAttributeCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_set_member_attribute_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackSetMemberAtribute,
		cb_pointer);
}

static void CallbackGetChatRoomInfo(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::GetChatRoomInfoCallback *cb = (ChatRoom::GetChatRoomInfoCallback*)user_data;
		if (*cb)
		{
			Json::Value value;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), value) && value.isObject())
			{
				ChatRoomInfo info;
				info.ParseFromJsonValue(value);
				(*cb)(room_id, error_code, info);
				return;
			}
			(*cb)(room_id, error_code, ChatRoomInfo());
		}
	}
}

static void CallbackKickMember(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::KickMemberCallback *cb = (ChatRoom::KickMemberCallback*)user_data;
		if (*cb)
			(*cb)(room_id, error_code);
	}
}

void ChatRoom::GetInfoAsync(const __int64 room_id, 
	const GetChatRoomInfoCallback& callback ,
	const std::string &json_extension/* = ""*/)
{
	GetChatRoomInfoCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new GetChatRoomInfoCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_get_info_async)(room_id, 
		json_extension.c_str(),
		&CallbackGetChatRoomInfo,
		cb_pointer);
}

void ChatRoom::GetMemberInfoByIDsAsync(const __int64 room_id, 
	const std::list<std::string>& ids, 
	const GetMembersCallback& callback,
	const std::string &json_extension/* = ""*/)
{
	GetMembersCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new GetMembersCallback(callback);

	std::string ids_json;
	StrListToJsonString(ids, ids_json);

	return NIM_SDK_GET_FUNC(nim_chatroom_get_members_by_ids_online_async)(room_id, 
		ids_json.c_str(),
		json_extension.c_str(),
		&CallbackGetMembersOnline,
		cb_pointer);
}

void ChatRoom::KickMemberAsync(const __int64 room_id, 
	const std::string& id, 
	const std::string& notify_ext, 
	const KickMemberCallback &callback, 
	const std::string &json_extension/* = ""*/)
{
	KickMemberCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new KickMemberCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_kick_member_async)(room_id, 
		id.c_str(), 
		notify_ext.c_str(), 
		json_extension.c_str(),
		&CallbackKickMember,
		cb_pointer);
}

void ChatRoom::SetProxy(NIMChatRoomProxyType type,
	const std::string& host,
	int				   port,
	const std::string& user,
	const std::string& password)
{
	return NIM_SDK_GET_FUNC(nim_chatroom_set_proxy)(type, host.c_str(), port, user.c_str(), password.c_str());
}

static void CallbackTempMuteMember(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::TempMuteMemberCallback *cb = (ChatRoom::TempMuteMemberCallback*)user_data;
		if (*cb)
		{
			Json::Value value;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), value) && value.isObject())
			{
				ChatRoomMemberInfo info;
				info.ParseFromJsonValue(value);
				(*cb)(room_id, error_code, info);
				return;
			}
			(*cb)(room_id, error_code, ChatRoomMemberInfo());
		}
	}
}

void ChatRoom::TempMuteMemberAsync(const __int64 room_id
	, const std::string& accid
	, const __int64 duration
	, bool need_notify
	, const std::string& notify_ext
	, const TempMuteMemberCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	TempMuteMemberCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new TempMuteMemberCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_temp_mute_member_async)(room_id
		, accid.c_str()
		, duration
		, need_notify
		, notify_ext.c_str()
		, json_extension.c_str()
		, &CallbackTempMuteMember
		, cb_pointer);
}

static void CallbackUpdateRoomInfo(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::UpdateRoomInfoCallback *cb = (ChatRoom::UpdateRoomInfoCallback*)user_data;
		if (*cb)
			(*cb)(room_id, error_code);
	}
}

static void CallbackUpdateMyRoomRole(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::UpdateMyRoomRoleCallback *cb = (ChatRoom::UpdateMyRoomRoleCallback*)user_data;
		if (*cb)
			(*cb)(room_id, error_code);
	}
}

static void CallbackQueueOffer(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::QueueOfferCallback *cb = (ChatRoom::QueueOfferCallback*)user_data;
		if (*cb)
			(*cb)(room_id, error_code);
	}
}

static void CallbackQueueDrop(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::QueueDropCallback *cb = (ChatRoom::QueueDropCallback*)user_data;
		if (*cb)
			(*cb)(room_id, error_code);
	}
}

void ChatRoom::UpdateRoomInfoAsync(const __int64 room_id
	, const ChatRoomInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateRoomInfoCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	UpdateRoomInfoCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new UpdateRoomInfoCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_update_room_info_async)(room_id, 
		info.ToJsonString().c_str(), 
		need_notify,
		notify_ext.c_str(), 
		json_extension.c_str(),
		&CallbackUpdateRoomInfo,
		cb_pointer);
}

void ChatRoom::UpdateMyRoomRoleAsync(const __int64 room_id
	, const ChatRoomMemberInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateMyRoomRoleCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	UpdateMyRoomRoleCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new UpdateMyRoomRoleCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_update_my_role_async)(room_id, 
		info.ToJsonString().c_str(), 
		need_notify,
		notify_ext.c_str(), 
		json_extension.c_str(),
		&CallbackUpdateMyRoomRole,
		cb_pointer);
}

void ChatRoom::QueueOfferAsync(const __int64 room_id
	, const ChatRoomQueueElement& element
	, const QueueOfferCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueOfferCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new QueueOfferCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_queue_offer_async)(room_id, 
		element.key_.c_str(), 
		element.value_.c_str(), 
		json_extension.c_str(),
		&CallbackQueueOffer,
		cb_pointer);
}

static void CallbackQueuePoll(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::QueuePollCallback *cb = (ChatRoom::QueuePollCallback*)user_data;
		if (*cb)
		{
			Json::Value value;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), value) && value.isObject())
			{
				ChatRoomQueueElement element;
				element.key_ = value[kNIMChatRoomQueueElementKey].asString();
				element.value_ = value[kNIMChatRoomQueueElementValue].asString();
				(*cb)(room_id, error_code, element);
				return;
			}
			(*cb)(room_id, error_code, ChatRoomQueueElement());
		}
	}
}

static void CallbackQueueList(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		ChatRoom::QueueListCallback *cb = (ChatRoom::QueueListCallback*)user_data;
		if (*cb)
		{
			Json::Value value;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), value) && value.isArray())
			{
				ChatRoomQueue queue;
				auto size = value.size();
				for (size_t i = 0; i < size; i++)
				{
					ChatRoomQueueElement element;
					element.key_ = value[i][kNIMChatRoomQueueElementKey].asString();
					element.value_ = value[i][kNIMChatRoomQueueElementValue].asString();
					queue.push_back(element);
				}
				(*cb)(room_id, error_code, queue);
				return;
			}
			(*cb)(room_id, error_code, ChatRoomQueue());
		}
	}
}

void ChatRoom::QueuePollAsync(const __int64 room_id
	, const std::string& element_key
	, const QueuePollCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueuePollCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new QueuePollCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_queue_poll_async)(room_id, 
		element_key.c_str(), 
		json_extension.c_str(),
		&CallbackQueuePoll,
		cb_pointer);
}

void ChatRoom::QueueListAsync(const __int64 room_id
	, const QueueListCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueListCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new QueueListCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_queue_list_async)(room_id, 
		json_extension.c_str(),
		&CallbackQueueList,
		cb_pointer);
}

void ChatRoom::QueueDropAsync(const __int64 room_id
	, const QueueDropCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueDropCallback* cb_pointer = nullptr;
	if (callback)
		cb_pointer = new QueueDropCallback(callback);

	return NIM_SDK_GET_FUNC(nim_chatroom_queue_drop_async)(room_id, 
		json_extension.c_str(),
		&CallbackQueueDrop,
		cb_pointer);
}

}
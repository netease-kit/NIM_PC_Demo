/** @file nim_chatroom_cpp.cpp
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/12/29
  */


#include "include/nim_chatroom_cpp_api.h"
#include "src/cpp_sdk/nim_chatroom/helper/nim_chatroom_loader_helper.h"


namespace nim_chatroom
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_chatroom_reg_enter_cb)(const char *json_extension, nim_chatroom_enter_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_exit_cb)(const char *json_extension, nim_chatroom_exit_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_send_msg_ack_cb)(const char *json_extension, nim_chatroom_sendmsg_arc_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_receive_msg_cb)(const char *json_extension, nim_chatroom_receive_msg_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_receive_notification_cb)(const char *json_extension, nim_chatroom_receive_notification_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_reg_link_condition_cb)(const char *json_extension, nim_chatroom_link_condition_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_init)(const char *json_extension);
typedef bool(*nim_chatroom_enter)(const int64_t room_id, const char *request_enter_data, const char *enter_info, const char *json_extension);
typedef void(*nim_chatroom_exit)(const int64_t room_id, const char *json_extension);
typedef int (*nim_chatroom_get_login_state)(const int64_t room_id, const char *json_extension);
typedef void(*nim_chatroom_cleanup)(const char *json_extension);
typedef void(*nim_chatroom_send_msg)(const int64_t room_id, const char *msg, const char *json_extension);
typedef void(*nim_chatroom_get_members_online_async)(const int64_t room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_msg_history_online_async)(const int64_t room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_msg_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_set_member_attribute_async)(const int64_t room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_set_member_attribute_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_info_async)(const int64_t room_id, const char *json_extension, nim_chatroom_get_info_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_get_members_by_ids_online_async)(const int64_t room_id, const char *ids_json_array_string, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_kick_member_async)(const int64_t room_id, const char *id, const char *notify_ext, const char *json_extension, nim_chatroom_kick_member_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_set_proxy)(NIMChatRoomProxyType type, const char *host, int port, const char *user, const char *password);
typedef void(*nim_chatroom_temp_mute_member_async)(const int64_t room_id, const char *accid, const int64_t duration, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_temp_mute_member_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_update_room_info_async)(const int64_t room_id, const char *room_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_room_info_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_update_my_role_async)(const int64_t room_id, const char *role_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_my_role_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_offer_async)(const int64_t room_id, const char *element_key, const char *element_value, const char *json_extension, nim_chatroom_queue_offer_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_poll_async)(const int64_t room_id, const char *element_key, const char *json_extension, nim_chatroom_queue_poll_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_list_async)(const int64_t room_id, const char *json_extension, nim_chatroom_queue_list_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_header_async)(const int64_t room_id, const char *json_extension, nim_chatroom_queue_header_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_queue_drop_async)(const int64_t room_id, const char *json_extension, nim_chatroom_queue_drop_cb_func cb, const void *user_data);
typedef bool(*nim_chatroom_enter_with_anoymity)(const int64_t room_id, const char *anonymity_info, const char *enter_info, const char *json_extension);
typedef char* (*nim_chatroom_query_all_robots_block)(const int64_t room_id, const char *json_extension);
typedef char* (*nim_chatroom_query_robot_by_accid_block)(const int64_t room_id, const char *accid, const char *json_extension);
typedef void (*nim_chatroom_get_robots_async)(const int64_t room_id, int64_t timetag, const char *json_extension, nim_chatroom_query_robots_cb_func cb, const void *user_data);
typedef void (*nim_chatroom_set_msgs_batch_report)(bool set_batch, const char *json_extension);
typedef void (*nim_chatroom_reg_receive_msgs_cb)(const char *json_extension, nim_chatroom_receive_msg_cb_func cb, const void *user_data);
typedef void(*nim_chatroom_batch_upate_async)(const int64_t room_id, const char *element_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_batch_update_cb cb, const void *user_data);

#else
#include "nim_chatroom.h"
#endif

static void CallbackEnter(int64_t room_id, int step, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::EnterCallback>(user_data, [=](const ChatRoom::EnterCallback& cb){

		ChatRoomInfo info;
		ChatRoomMemberInfo my_info;
		ParseChatRoomEnterCallbackResultInfo(PCharToString(result), info, my_info);
		//如果错误码error_code为kResRoomLocalNeedRequestAgain，聊天室重连机制结束，则需要向IM服务器重新请求进入该聊天室权限
		nim::CallbackProxy::Invoke(cb, room_id, (NIMChatRoomEnterStep)step, error_code, info, my_info);
	});
}

static void CallbackExit(int64_t room_id, int error_code, int exit_reason, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::ExitCallback>(user_data, [=](const ChatRoom::ExitCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code, (NIMChatRoomExitReason)exit_reason);
	});

}
static void CallbackExit_2(int64_t room_id, int error_code, int exit_reason, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::ExitCallback_2>(user_data, [=](const ChatRoom::ExitCallback_2& cb){

		NIMChatRoomExitReasonInfo exit_info;
		exit_info.code_ = (NIMChatRoomExitReason)exit_reason;
		if (json_extension != nullptr)
			exit_info.notify_ext_ = json_extension;
		nim::CallbackProxy::Invoke(cb, room_id, error_code, exit_info);
	});
}
static void CallbackSendMsgAck(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::SendMsgAckCallback>(user_data, [=](const ChatRoom::SendMsgAckCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		ChatRoomMessage msg;
		if (nim::ParseJsonValue(PCharToString(result), value))
		{
			msg.ParseFromJsonValue(value);
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, msg);
	});
}

static void CallbackReceiveMsg(int64_t room_id, const char *content, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::ReceiveMsgCallback>(user_data, [=](const ChatRoom::ReceiveMsgCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		if (nim::ParseJsonValue(PCharToString(content), value))
		{
			ChatRoomMessage msg;
			msg.ParseFromJsonValue(value);
			nim::CallbackProxy::Invoke(cb, room_id, msg);
		}
	});

}

static void CallbackReceiveMsgs(int64_t room_id, const char *content, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::ReceiveMsgsCallback>(user_data, [=](const ChatRoom::ReceiveMsgsCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		if (nim::ParseJsonValue(PCharToString(content), value) && value.isArray())
		{
			int cnt = (int)value.size();
			std::list<ChatRoomMessage> msgs;
			for (auto i = 0; i < cnt; i++)
			{
				ChatRoomMessage msg;
				msg.ParseFromJsonValue(value[i]);
				msgs.push_back(msg);
			}
			nim::CallbackProxy::Invoke(cb, room_id, msgs);
		}
	});
}

static void CallbackReceiveNotification(int64_t room_id, const char *content, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::NotificationCallback>(user_data, [=](const ChatRoom::NotificationCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		if (nim::ParseJsonValue(PCharToString(content), value))
		{
			ChatRoomNotification msg;
			msg.ParseFromJsonValue(value);
			nim::CallbackProxy::Invoke(cb, room_id, msg);
		}
	});
}

static void CallbackLinkCondition(int64_t room_id, int condition, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::LinkConditionCallback>(user_data, [=](const ChatRoom::LinkConditionCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, (NIMChatRoomLinkCondition)condition);
	});

}

static ChatRoom::EnterCallback g_cb_enter_ = nullptr;
void ChatRoom::RegEnterCb(const EnterCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_enter_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_enter_cb)(json_extension.c_str(), &CallbackEnter, &g_cb_enter_);
}

static ChatRoom::ExitCallback g_cb_exit_ = nullptr;
void ChatRoom::RegExitCb(const ExitCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_exit_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_exit_cb)(json_extension.c_str(), &CallbackExit, &g_cb_exit_);
}
static ChatRoom::ExitCallback_2 g_cb_exit_2_ = nullptr;
void ChatRoom::RegExitCb_2(const ChatRoom::ExitCallback_2& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_exit_2_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_exit_cb)(json_extension.c_str(), &CallbackExit_2, &g_cb_exit_2_);
}
static ChatRoom::SendMsgAckCallback g_cb_send_msg_ack_ = nullptr;
void ChatRoom::RegSendMsgAckCb(const SendMsgAckCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_send_msg_ack_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_send_msg_ack_cb)(json_extension.c_str(), &CallbackSendMsgAck, &g_cb_send_msg_ack_);
}

static ChatRoom::ReceiveMsgCallback g_cb_receive_msg_ = nullptr;
void ChatRoom::RegReceiveMsgCb(const ReceiveMsgCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_receive_msg_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_receive_msg_cb)(json_extension.c_str(), &CallbackReceiveMsg, &g_cb_receive_msg_);
}

static ChatRoom::ReceiveMsgsCallback g_cb_receive_msgs_ = nullptr;
void ChatRoom::RegReceiveMsgsCb(const ReceiveMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_receive_msgs_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_receive_msgs_cb)(json_extension.c_str(), &CallbackReceiveMsgs, &g_cb_receive_msgs_);
}

static ChatRoom::NotificationCallback g_cb_notification_ = nullptr;
void ChatRoom::RegNotificationCb(const NotificationCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_notification_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_receive_notification_cb)(json_extension.c_str(), &CallbackReceiveNotification, &g_cb_notification_);
}

static ChatRoom::LinkConditionCallback g_cb_link_condition_ = nullptr;
void ChatRoom::RegLinkConditionCb(const LinkConditionCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_link_condition_ = cb;
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_reg_link_condition_cb)(json_extension.c_str(), &CallbackLinkCondition, &g_cb_link_condition_);
}

bool ChatRoom::Init(const std::string& app_install_dir, const std::string& json_extension/* = ""*/)
{
#ifdef NIM_SDK_DLL_IMPORT

#if defined (OS_POSIX)
	static const char *kSdkNimDll = "libnim_chatroom.so";
#elif defined(WIN32)
	static const char *kSdkNimDll = "nim_chatroom.dll";
#else
    static const char *kSdkNimDll = nullptr;
#endif

	if (NULL == nim_chatroom_sdk_instance)
	{
		nim_chatroom_sdk_instance = new nim::SDKInstance();
	}
	if (!nim_chatroom_sdk_instance->LoadSdkDll(app_install_dir.c_str(), kSdkNimDll))
		return false;

#endif

	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_init)(json_extension.c_str());
#ifdef NIM_SDK_DLL_IMPORT
    nim_chatroom_sdk_instance->OnSDKInited();
#endif
	return true;
}
#ifdef CPPWRAPPER_DLL
void ChatRoom::SetCallbackFunction(const ChatRoom::SDKClosure& callback)
{
	nim::CallbackProxy::docallback_async_ = callback;
}
#endif
bool ChatRoom::Enter(const int64_t room_id, const std::string& request_enter_data, const ChatRoomEnterInfo& info/* = ChatRoomEnterInfo()*/, const std::string& json_extension/* = ""*/)
{
	return NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_enter)(room_id, request_enter_data.c_str(), info.ToJsonString().c_str(), json_extension.c_str());
}

bool ChatRoom::AnonymousEnter(const int64_t room_id, const ChatRoomAnoymityEnterInfo& anonymity_info, const ChatRoomEnterInfo& info, const std::string& json_extension/* = ""*/)
{
	return NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_enter_with_anoymity)(room_id, anonymity_info.ToJsonString().c_str(), info.ToJsonString().c_str(), json_extension.c_str());
}

void ChatRoom::Exit(const int64_t room_id, const std::string& json_extension/* = ""*/)
{
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_exit)(room_id, json_extension.c_str());
}

NIMChatRoomLoginState ChatRoom::GetLoginState( const int64_t room_id, const std::string& json_extension /*= ""*/ )
{
	int login_state = NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_login_state)(room_id, json_extension.c_str());
	return (NIMChatRoomLoginState)login_state;
}

void ChatRoom::Cleanup(const std::string& json_extension/* = ""*/)
{
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_cleanup)(json_extension.c_str());
#ifdef NIM_SDK_DLL_IMPORT
	nim_chatroom_sdk_instance->UnLoadSdkDll();
	delete nim_chatroom_sdk_instance;
	nim_chatroom_sdk_instance = NULL;
#endif
}

void ChatRoom::SetMsgsBatchReport(bool set_batch, const std::string& json_extension/* = ""*/)
{
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_set_msgs_batch_report)(set_batch, json_extension.c_str());
}

void ChatRoom::SendMsg(const int64_t room_id, const std::string& json_msg, const std::string& json_extension/* = ""*/)
{
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_send_msg)(room_id, json_msg.c_str(), json_extension.c_str());
}

std::string ChatRoom::CreateRoomMessage(const NIMChatRoomMsgType msg_type
	, const std::string& client_msg_id
	, const std::string& attach
	, const std::string& msg_body
	, const ChatRoomMessageSetting& msg_setting
	, int64_t timetag/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMChatRoomMsgKeyType] = msg_type;
	values[kNIMChatRoomMsgKeyAttach] = attach;
	values[kNIMChatRoomMsgKeyClientMsgid] = client_msg_id;
	values[kNIMChatRoomMsgKeyBody] = msg_body;
	msg_setting.ToJsonValue(values);
	return nim::GetJsonStringWithNoStyled(values);
}

static void CallbackGetMembersOnline(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::GetMembersCallback>(user_data, [=](const ChatRoom::GetMembersCallback& cb){

		std::list<ChatRoomMemberInfo> infos;
		ParseChatRoomMemberInfos(PCharToString(result), infos);
		nim::CallbackProxy::Invoke(cb, room_id, error_code, infos);
	});
}

static void CallbackGetMsgHistoryOnline(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::GetMsgHistoryCallback>(user_data, [=](const ChatRoom::GetMsgHistoryCallback& cb){

		std::list<ChatRoomMessage> msgs;
		ParseChatRoomMsgs(PCharToString(result), msgs);
		nim::CallbackProxy::Invoke(cb, room_id, error_code, msgs);
	});

}

static void CallbackSetMemberAtribute(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::SetMemberAttributeCallback>(user_data, [=](const ChatRoom::SetMemberAttributeCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomMemberInfo info;
		if (reader.parse(PCharToString(result), value) && value.isObject())
		{

			info.ParseFromJsonValue(value);
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, info);
	});

}

void ChatRoom::GetMembersOnlineAsync(const int64_t room_id, 
	const ChatRoomGetMembersParameters &parameters, 
	const GetMembersCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	GetMembersCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new GetMembersCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_members_online_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackGetMembersOnline,
		cb_pointer);
}

void ChatRoom::GetMessageHistoryOnlineAsync(const int64_t room_id, 
	const ChatRoomGetMsgHistoryParameters &parameters, 
	const GetMsgHistoryCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	GetMsgHistoryCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new GetMsgHistoryCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_msg_history_online_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackGetMsgHistoryOnline,
		cb_pointer);
}

void ChatRoom::SetMemberAttributeOnlineAsync(const int64_t room_id, 
	const ChatRoomSetMemberAttributeParameters &parameters, 
	const SetMemberAttributeCallback &callback,
	const std::string& json_extension/* = ""*/)
{
	SetMemberAttributeCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new SetMemberAttributeCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_set_member_attribute_async)(room_id, 
		parameters.ToJsonString().c_str(), 
		json_extension.c_str(),
		&CallbackSetMemberAtribute,
		cb_pointer);
}

static void CallbackGetChatRoomInfo(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::GetChatRoomInfoCallback>(user_data, [=](const ChatRoom::GetChatRoomInfoCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomInfo info;
		if (reader.parse(PCharToString(result), value) && value.isObject())
		{
			info.ParseFromJsonValue(value);
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, info);
	});

}

static void CallbackKickMember(int64_t room_id, int error_code, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::KickMemberCallback>(user_data, [=](const ChatRoom::KickMemberCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code);
	});
}

void ChatRoom::GetInfoAsync(const int64_t room_id, 
	const GetChatRoomInfoCallback& callback ,
	const std::string &json_extension/* = ""*/)
{
	GetChatRoomInfoCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new GetChatRoomInfoCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_info_async)(room_id, 
		json_extension.c_str(),
		&CallbackGetChatRoomInfo,
		cb_pointer);
}

void ChatRoom::GetMemberInfoByIDsAsync(const int64_t room_id, 
	const std::list<std::string>& ids, 
	const GetMembersCallback& callback,
	const std::string &json_extension/* = ""*/)
{
	GetMembersCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new GetMembersCallback(callback);

	std::string ids_json;
	nim::StrListToJsonString(ids, ids_json);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_members_by_ids_online_async)(room_id, 
		ids_json.c_str(),
		json_extension.c_str(),
		&CallbackGetMembersOnline,
		cb_pointer);
}

void ChatRoom::KickMemberAsync(const int64_t room_id, 
	const std::string& id, 
	const std::string& notify_ext, 
	const KickMemberCallback &callback, 
	const std::string &json_extension/* = ""*/)
{
	KickMemberCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new KickMemberCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_kick_member_async)(room_id, 
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
		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_set_proxy)(type, host.c_str(), port, user.c_str(), password.c_str());
}

static void CallbackTempMuteMember(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::TempMuteMemberCallback>(user_data, [=](const ChatRoom::TempMuteMemberCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomMemberInfo info;
		if (reader.parse(PCharToString(result), value) && value.isObject())
		{
			info.ParseFromJsonValue(value);
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, info);
	});

}

void ChatRoom::TempMuteMemberAsync(const int64_t room_id
	, const std::string& accid
	, const int64_t duration
	, bool need_notify
	, const std::string& notify_ext
	, const TempMuteMemberCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	TempMuteMemberCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new TempMuteMemberCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_temp_mute_member_async)(room_id
		, accid.c_str()
		, duration
		, need_notify
		, notify_ext.c_str()
		, json_extension.c_str()
		, &CallbackTempMuteMember
		, cb_pointer);
}

static void CallbackUpdateRoomInfo(int64_t room_id, int error_code, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::UpdateRoomInfoCallback>(user_data, [=](const ChatRoom::UpdateRoomInfoCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code);
	});

}

static void CallbackUpdateMyRoomRole(int64_t room_id, int error_code, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::UpdateMyRoomRoleCallback>(user_data, [=](const ChatRoom::UpdateMyRoomRoleCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code);
	});

}

static void CallbackQueueOffer(int64_t room_id, int error_code, const char *json_extension, const void *user_data)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueueOfferCallback>(user_data, [=](const ChatRoom::QueueOfferCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code);
	});
}

static void CallbackQueueDrop(int64_t room_id, int error_code, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueueDropCallback>(user_data, [=](const ChatRoom::QueueDropCallback& cb){

		nim::CallbackProxy::Invoke(cb, room_id, error_code);
	});
}

void ChatRoom::UpdateRoomInfoAsync(const int64_t room_id
	, const ChatRoomInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateRoomInfoCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	UpdateRoomInfoCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new UpdateRoomInfoCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_update_room_info_async)(room_id, 
		info.ToJsonString().c_str(), 
		need_notify,
		notify_ext.c_str(), 
		json_extension.c_str(),
		&CallbackUpdateRoomInfo,
		cb_pointer);
}

void ChatRoom::UpdateMyRoomRoleAsync(const int64_t room_id
	, const ChatRoomMemberInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateMyRoomRoleCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	UpdateMyRoomRoleCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new UpdateMyRoomRoleCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_update_my_role_async)(room_id, 
		info.ToJsonString().c_str(), 
		need_notify,
		notify_ext.c_str(), 
		json_extension.c_str(),
		&CallbackUpdateMyRoomRole,
		cb_pointer);
}

void ChatRoom::QueueOfferAsync(const int64_t room_id
	, const ChatRoomQueueElement& element
	, const QueueOfferCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueOfferCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new QueueOfferCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_queue_offer_async)(room_id, 
		element.key_.c_str(), 
		element.value_.c_str(), 
		json_extension.c_str(),
		&CallbackQueueOffer,
		cb_pointer);
}

static void CallbackQueuePoll(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueuePollCallback>(user_data, [=](const ChatRoom::QueuePollCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomQueueElement element;
		if (reader.parse(PCharToString(result), value) && value.isObject())
		{

			element.key_ = value[kNIMChatRoomQueueElementKey].asString();
			element.value_ = value[kNIMChatRoomQueueElementValue].asString();
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, element);
	});

}

static void CallbackQueueList(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueueListCallback>(user_data, [=](const ChatRoom::QueueListCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomQueue queue;
		if (reader.parse(PCharToString(result), value) && value.isArray())
		{

			auto size = value.size();
			for (int i = 0; i < (int)size; i++)
			{
				ChatRoomQueueElement element;
				element.key_ = value[i][kNIMChatRoomQueueElementKey].asString();
				element.value_ = value[i][kNIMChatRoomQueueElementValue].asString();
				queue.push_back(element);
			}
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, queue);
	});
}

static void CallbackQueueHeader(int64_t room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueueHeaderCallback>(user_data, [=](const ChatRoom::QueueHeaderCallback& cb){

		nim_cpp_wrapper_util::Json::Value value;
		nim_cpp_wrapper_util::Json::Reader reader;
		ChatRoomQueueElement element;
		if (reader.parse(PCharToString(result), value) && value.isObject())
		{
			element.key_ = value[kNIMChatRoomQueueElementKey].asString();
			element.value_ = value[kNIMChatRoomQueueElementValue].asString();
		}
		nim::CallbackProxy::Invoke(cb, room_id, error_code, element);
	});
}

void ChatRoom::QueuePollAsync(const int64_t room_id
	, const std::string& element_key
	, const QueuePollCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueuePollCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new QueuePollCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_queue_poll_async)(room_id, 
		element_key.c_str(), 
		json_extension.c_str(),
		&CallbackQueuePoll,
		cb_pointer);
}

void ChatRoom::QueueListAsync(const int64_t room_id
	, const QueueListCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueListCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new QueueListCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_queue_list_async)(room_id, 
		json_extension.c_str(),
		&CallbackQueueList,
		cb_pointer);
}

void ChatRoom::QueueHeaderAsync(const int64_t room_id
	, const QueueHeaderCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueHeaderCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new QueueHeaderCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_queue_header_async)(room_id, 
		json_extension.c_str(),
		&CallbackQueueHeader,
		cb_pointer);
}

void ChatRoom::QueueDropAsync(const int64_t room_id
	, const QueueDropCallback& callback
	, const std::string &json_extension/* = ""*/)
{
	QueueDropCallback* cb_pointer = nullptr;
	if (callback != nullptr)
		cb_pointer = new QueueDropCallback(callback);

		NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_queue_drop_async)(room_id, 
		json_extension.c_str(),
		&CallbackQueueDrop,
		cb_pointer);
}

void ChatRoom::UnregChatroomCb()
{
	g_cb_enter_ = nullptr;
	g_cb_exit_ = nullptr;
	g_cb_exit_2_ = nullptr;
	g_cb_send_msg_ack_ = nullptr;
	g_cb_receive_msg_ = nullptr;
	g_cb_notification_ = nullptr;
	g_cb_link_condition_ = nullptr;
}

RobotInfos ChatRoom::QueryAllRobotInfosBlock(const int64_t room_id, const std::string &json_extension/* = ""*/)
{
	char *res = NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_query_all_robots_block)(room_id, json_extension.c_str());
	RobotInfos infos;
	ParseRobotInfosStringToRobotInfos(PCharToString(res), infos);
	return infos;
}

RobotInfo ChatRoom::QueryRobotInfoByAccidBlock(const int64_t room_id, const std::string &accid, const std::string &json_extension/* = ""*/)
{
	char *res = NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_query_robot_by_accid_block)(room_id, accid.c_str(), json_extension.c_str());
	RobotInfo info;
	ParseRobotInfoStringToRobotInfo(PCharToString(res), info);
	return info;
}

static void CallbackRobotQuery(int rescode, const char *res, const char *json_extension, const void *callback)
{

	nim::CallbackProxy::DoSafeCallback<ChatRoom::RobotQueryCallback>(callback, [=](const ChatRoom::RobotQueryCallback& cb){

		RobotInfos infos;
		ParseRobotInfosStringToRobotInfos(PCharToString(res), infos);
		nim::CallbackProxy::Invoke(cb, (NIMResCode)rescode, infos);
	},true);

}



void ChatRoom::GetRobotInfoAsync(const int64_t room_id, const int64_t timetag, const RobotQueryCallback &callback, const std::string &json_extension/* = ""*/)
{
	RobotQueryCallback* cb_pointer = nullptr;
	if (callback != nullptr)
	{
		cb_pointer = new RobotQueryCallback(callback);
	}
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_get_robots_async)(room_id, timetag, json_extension.c_str(), &CallbackRobotQuery, cb_pointer);
}


static void CallbackBatchUpdate(int64_t room_id, int error_code ,const char *res, const char *json_extension, const void *callback)
{
	nim::CallbackProxy::DoSafeCallback<ChatRoom::QueueBatchUpdateCallback>(callback, [=](const ChatRoom::QueueBatchUpdateCallback& cb){

		std::list<std::string> lststring;
		ParseBatchInfosStringToNotMembers(PCharToString(res), lststring);
		nim::CallbackProxy::Invoke(cb, room_id, error_code, lststring);
	}, true);

}


void ChatRoom::QueueBatchUpdateAsync(const int64_t room_id
	, const ChatRoomBatchMembers& batch_members
	, bool need_notify
	, const std::string& notify_ext
	, const QueueBatchUpdateCallback& callback
	, const std::string &json_extension)
{
	QueueBatchUpdateCallback* cb_pointer = nullptr;
	if (callback != nullptr)
	{
		cb_pointer = new QueueBatchUpdateCallback(callback);
	}
	std::string elements = batch_members.ToJsonString();
	NIM_CHATROOM_SDK_GET_FUNC(nim_chatroom_batch_upate_async)(room_id, elements.c_str(), need_notify, notify_ext.c_str() ,json_extension.c_str(), &CallbackBatchUpdate, cb_pointer);
}

}

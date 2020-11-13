/** @file nim_cpp_talk.cpp
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "src/cpp_sdk/nim/api//nim_cpp_talk.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "src/cpp_sdk/nim/api//nim_cpp_global.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_talk_reg_ack_cb)(const char *json_extension, nim_talk_ack_cb_func cb, const void *user_data);
typedef void(*nim_talk_send_msg)(const char* json_msg, const char *json_extension, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_talk_stop_send_msg)(const char *json_msg, const char *json_extension);
typedef void(*nim_talk_reg_receive_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);
typedef void(*nim_talk_reg_receive_msgs_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);
typedef void(*nim_talk_reg_notification_filter_cb)(const char *json_extension, nim_talk_team_notification_filter_func cb, const void *user_data);
typedef char*(*nim_talk_create_retweet_msg)(const char* src_msg_json, const char* client_msg_id, const NIMSessionType retweet_to_session_type, const char* retweet_to_session_id, const char* msg_setting, int64_t timetag);
typedef void(*nim_talk_recall_msg)(const char *json_msg, const char *notify, const char *json_extension, nim_talk_recall_msg_func cb, const void *user_data);
typedef void(*nim_talk_recall_msg2)(const char *json_msg, const char *notify, const char *apnstext, const char *pushpayload, const char *json_extension, nim_talk_recall_msg_func cb, const void *user_data);
typedef void(*nim_talk_recall_msg_ex)(const char *json_msg, const char *notify, nim_talk_recall_msg_func cb, nim_talk_recall_extra_params& extra_params, const void* user_data);
typedef void(*nim_talk_reg_recall_msg_cb)(const char *json_extension, nim_talk_recall_msg_func cb, const void *user_data);
typedef char*(*nim_talk_get_attachment_path_from_msg)(const char *json_msg);
typedef void(*nim_talk_reg_receive_broadcast_cb)(const char *json_extension, nim_talk_receive_broadcast_cb_func cb, const void *user_data);
typedef void(*nim_talk_reg_receive_broadcast_msgs_cb)(const char *json_extension, nim_talk_receive_broadcast_cb_func cb, const void *user_data);
typedef void(*nim_talk_reg_message_filter_cb)(const char *json_extension, nim_talk_message_filter_func cb, const void *user_data);
typedef void(*nim_talk_reply_msg)(const char* json_msg, const char* json_reply_msg, const char* json_extension, nim_nos_upload_prg_cb_func prg_cb, const void* prg_user_data);
#else
#include "nim_talk.h"
#endif

static void CallbackSendMsgAck(const char *result, const void *callback)
{

	CallbackProxy::DoSafeCallback<Talk::SendMsgAckCallback>(callback, [=](const Talk::SendMsgAckCallback& cb){

		SendMessageArc arc;
		ParseSendMessageAck(PCharToString(result), arc);
		CallbackProxy::Invoke(cb, arc);
	});
}

static void CallbackReceiveMsg(const char *content, const char *json_extension, const void *callback)
{
	CallbackProxy::DoSafeCallback<Talk::ReceiveMsgCallback>(callback, [=](const Talk::ReceiveMsgCallback& cb){

		IMMessage msg;
		ParseReceiveMessage(PCharToString(content), msg);
		CallbackProxy::Invoke(cb, msg);
	});
}

static void CallbackReceiveMessages(const char *content, const char *json_extension, const void *callback)
{
	CallbackProxy::DoSafeCallback<Talk::ReceiveMsgsCallback>(callback, [=](const Talk::ReceiveMsgsCallback& cb){
		std::list<IMMessage> msgs;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value value;
		if (reader.parse(PCharToString(content), value) && value.isArray())
		{
			int size = value.size();
			for (int i = 0; i < size; i++)
			{
				IMMessage msg;
				ParseReceiveMessage(value[i], msg);
				msgs.push_back(msg);
			}
		}
		CallbackProxy::Invoke(cb, msgs);
	});
}

static void CallbackFileUploadProcess(int64_t uploaded_size, int64_t file_size, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Talk::FileUpPrgCallback>(callback, [=](const Talk::FileUpPrgCallback& cb){
		CallbackProxy::Invoke(cb, uploaded_size, file_size);
	});
}

static bool FilterTeamNotification(const char *content, const char *json_extension, const void *callback)
{

	return CallbackProxy::DoSafeCallback<Talk::TeamNotificationFilter>(callback, [=](const Talk::TeamNotificationFilter& cb){
		IMMessage msg;
		ParseReceiveMessage(PCharToString(content), msg);
		return CallbackProxy::Invoke(cb, msg);
	});

}

static Talk::SendMsgAckCallback g_cb_send_msg_ack_ = nullptr;
void Talk::RegSendMsgCb(const SendMsgAckCallback& cb, const std::string& json_extension)
{
	g_cb_send_msg_ack_ = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_ack_cb)(json_extension.c_str(), &CallbackSendMsgAck, &g_cb_send_msg_ack_);
}

void Talk::SendMsg(const std::string& json_msg, const std::string& json_extension/* = ""*/, FileUpPrgCallback* pcb/* = nullptr*/)
{
	if (pcb)
	{
		NIM_SDK_GET_FUNC(nim_talk_send_msg)(json_msg.c_str(), nullptr, &CallbackFileUploadProcess, pcb);
	} 
	else
	{
		NIM_SDK_GET_FUNC(nim_talk_send_msg)(json_msg.c_str(), nullptr, nullptr, nullptr);
	}
}

bool Talk::StopSendMsg(const std::string& client_msg_id, const NIMMessageType& type, const std::string& json_extension/* = ""*/)
{
	if (client_msg_id.empty())
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyType] = type;
	NIM_SDK_GET_FUNC(nim_talk_stop_send_msg)(GetJsonStringWithNoStyled(values).c_str(), json_extension.c_str());

	return true;
}

static Talk::ReceiveMsgCallback g_cb_pointer = nullptr;
void Talk::RegReceiveCb(const ReceiveMsgCallback& cb, const std::string& json_extension)
{
	g_cb_pointer = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_receive_cb)(json_extension.c_str(), &CallbackReceiveMsg, &g_cb_pointer);
}

static Talk::ReceiveMsgsCallback g_cb_msgs_pointer = nullptr;
void Talk::RegReceiveMessagesCb(const ReceiveMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_msgs_pointer = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_receive_msgs_cb)(json_extension.c_str(), &CallbackReceiveMessages, &g_cb_msgs_pointer);
}

std::string Talk::CreateTextMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const std::string& content
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyBody] = content;
	values[kNIMMsgKeyType] = kNIMMessageTypeText;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateImageMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMImage& image
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = image.ToJsonString();
	values[kNIMMsgKeyType] = kNIMMessageTypeImage;
	values[kNIMMsgKeyLocalFilePath] = file_path;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	values[kNIMMsgKeyLocalResId] = client_msg_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateFileMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMFile& file
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = file.ToJsonString();
	values[kNIMMsgKeyType] = kNIMMessageTypeFile;
	values[kNIMMsgKeyLocalFilePath] = file_path;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	values[kNIMMsgKeyLocalResId] = client_msg_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateAudioMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMAudio& audio
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = audio.ToJsonString();
	values[kNIMMsgKeyType] = kNIMMessageTypeAudio;
	values[kNIMMsgKeyLocalFilePath] = file_path;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	values[kNIMMsgKeyLocalResId] = client_msg_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateVideoMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMVideo& video
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = video.ToJsonString();
	values[kNIMMsgKeyType] = kNIMMessageTypeVideo;
	values[kNIMMsgKeyLocalFilePath] = file_path;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	values[kNIMMsgKeyLocalResId] = client_msg_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateLocationMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMLocation& location
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = location.ToJsonString();
	values[kNIMMsgKeyType] = kNIMMessageTypeLocation;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateTipMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const std::string& tip_content
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyBody] = tip_content;
	values[kNIMMsgKeyType] = kNIMMessageTypeTips;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	msg_setting.ToJsonValue(values);
	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateBotRobotMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const std::string& content
	, const IMBotRobot& bot_msg
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyBody] = content;
	values[kNIMMsgKeyType] = kNIMMessageTypeRobot;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	values[kNIMMsgKeyAttach] = bot_msg.ToJsonString();
	if(sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateG2NetCallMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const std::string& msg_attach
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/
	, int32_t sub_type/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyAttach] = msg_attach;
	values[kNIMMsgKeyType] = kNIMMessageTypeG2NetCall;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;
	if (sub_type > 0)
		values[kNIMMsgKeySubType] = sub_type;
	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateRetweetMessage(const std::string& src_msg_json
	, const std::string& client_msg_id
	, const NIMSessionType retweet_to_session_type
	, const std::string& retweet_to_session_id
	, const MessageSetting& msg_setting
	, int64_t timetag/* = 0*/)
{
	nim_cpp_wrapper_util::Json::Value setting;
	msg_setting.ToJsonValue(setting);
	nim_cpp_wrapper_util::Json::FastWriter fw;
	const char *msg = NIM_SDK_GET_FUNC(nim_talk_create_retweet_msg)(src_msg_json.c_str(), client_msg_id.c_str(), retweet_to_session_type, retweet_to_session_id.c_str(), fw.write(setting).c_str(), timetag);
	std::string out_msg = (std::string)msg;	
	Global::FreeBuf((void *)msg);
	return out_msg;
}

bool Talk::ParseIMMessage(const std::string& json_msg, IMMessage& msg)
{
	return ParseMessage(json_msg, msg);
}

bool Talk::ParseImageMessageAttach(const IMMessage& msg, IMImage& image)
{
	if (msg.type_ != kNIMMessageTypeImage)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		image.display_name_ = values[kNIMImgMsgKeyDisplayName].asString();
		image.file_extension_ = values[kNIMImgMsgKeyExt].asString();
		image.height_ = values[kNIMImgMsgKeyHeight].asUInt();
		image.width_ = values[kNIMImgMsgKeyWidth].asUInt();
		image.md5_ = values[kNIMImgMsgKeyMd5].asString();
		image.size_ = values[kNIMImgMsgKeySize].asUInt64();
		image.url_ = values[kNIMImgMsgKeyUrl].asString();
		return true;
	}
	return false;
}

bool Talk::ParseFileMessageAttach(const IMMessage& msg, IMFile& file)
{
	if (msg.type_ != kNIMMessageTypeFile)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		file.display_name_ = values[kNIMFileMsgKeyDisplayName].asString();
		file.file_extension_ = values[kNIMFileMsgKeyExt].asString();
		file.md5_ = values[kNIMFileMsgKeyMd5].asString();
		file.size_ = values[kNIMFileMsgKeySize].asUInt64();
		file.url_ = values[kNIMFileMsgKeyUrl].asString();
		return true;
	}
	return false;
}

bool Talk::ParseAudioMessageAttach(const IMMessage& msg, IMAudio& audio)
{
	if (msg.type_ != kNIMMessageTypeAudio)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		audio.display_name_ = values[kNIMAudioMsgKeyDisplayName].asString();
		audio.file_extension_ = values[kNIMAudioMsgKeyExt].asString();
		audio.md5_ = values[kNIMAudioMsgKeyMd5].asString();
		audio.size_ = values[kNIMAudioMsgKeySize].asUInt64();
		audio.url_ = values[kNIMAudioMsgKeyUrl].asString();
		audio.duration_ = values[kNIMAudioMsgKeyDuration].asUInt();
		return true;
	}
	return false;
}

bool Talk::ParseVideoMessageAttach(const IMMessage& msg, IMVideo& video)
{
	if (msg.type_ != kNIMMessageTypeVideo)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		video.display_name_ = values[kNIMVideoMsgKeyDisplayName].asString();
		video.file_extension_ = values[kNIMVideoMsgKeyExt].asString();
		video.md5_ = values[kNIMVideoMsgKeyMd5].asString();
		video.size_ = values[kNIMVideoMsgKeySize].asUInt64();
		video.url_ = values[kNIMVideoMsgKeyUrl].asString();
		video.duration_ = values[kNIMVideoMsgKeyDuration].asUInt();
		video.height_ = values[kNIMVideoMsgKeyHeight].asUInt();
		video.width_ = values[kNIMVideoMsgKeyWidth].asUInt();
		return true;
	}
	return false;
}

bool Talk::ParseLocationMessageAttach(const IMMessage& msg, IMLocation& location)
{
	if (msg.type_ != kNIMMessageTypeLocation)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		location.description_ = values[kNIMLocationMsgKeyTitle].asString();
		location.latitude_ = values[kNIMLocationMsgKeyLatitude].asDouble();
		location.longitude_ = values[kNIMLocationMsgKeyLongitude].asDouble();
		return true;
	}
	return false;
}

bool Talk::ParseBotRobotMessageAttach(const IMMessage& msg, IMBotRobot& robot_msg)
{
	if (msg.type_ != kNIMMessageTypeRobot)
		return false;

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		robot_msg.robot_accid_ = values[kNIMBotRobotMsgKeyRobotID].asString();
		robot_msg.related_msg_id_ = values[kNIMBotRobotReceivedMsgKeyClientMsgID].asString();
		robot_msg.out_msg_ = values[kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
		robot_msg.robot_msg_flag_ = values[kNIMBotRobotReceivedMsgKeyRobotMsg][kNIMBotRobotReceivedMsgKeyRobotMsgFlag].asString();
		robot_msg.robot_msg_content_ = values[kNIMBotRobotReceivedMsgKeyRobotMsg][kNIMBotRobotReceivedMsgKeyRobotMsgMessage];
		return true;
	}
	return false;
}

static Talk::TeamNotificationFilter g_team_notification_filter_ = nullptr;
void Talk::RegTeamNotificationFilter(const TeamNotificationFilter& filter, const std::string& json_extension/* = ""*/)
{
	
	g_team_notification_filter_ = filter;
	NIM_SDK_GET_FUNC(nim_talk_reg_notification_filter_cb)(json_extension.c_str(), &FilterTeamNotification, &g_team_notification_filter_);
}

static Talk::MessageFilter g_message_filter_ = nullptr;
void Talk::RegMessageFilter(const MessageFilter& filter, const std::string& json_extension/* = ""*/)
{
	g_message_filter_ = filter;
	NIM_SDK_GET_FUNC(nim_talk_reg_message_filter_cb)(json_extension.c_str(),
		[](const char *content, const char *json_extension, const void *callback) ->bool{
			return CallbackProxy::DoSafeCallback<Talk::MessageFilter>(callback, [=](const Talk::MessageFilter& cb) {
				IMMessage msg;
				ParseReceiveMessage(PCharToString(content), msg);
				return CallbackProxy::Invoke(cb, msg);
				});
		}, &g_message_filter_);
}
static void ReceiveRecallMsg(int rescode, const char *content, const char *json_extension, const void *callback)
{
	CallbackProxy::DoSafeCallback<Talk::RecallMsgsCallback>(callback, [=](const Talk::RecallMsgsCallback& cb){
		std::list<RecallMsgNotify> notifys;
		ParseRecallMsgNotify(PCharToString(content), notifys);
		CallbackProxy::Invoke(cb, kNIMResSuccess, notifys);
	});
}

static Talk::RecallMsgsCallback g_recall_msg_cb_ = nullptr;
void Talk::RegRecallMsgsCallback(const RecallMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_recall_msg_cb_ = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_recall_msg_cb)(json_extension.c_str(), &ReceiveRecallMsg, &g_recall_msg_cb_);
}

static void CallbackRecallMsg(int rescode, const char *content, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Talk::RecallMsgsCallback>(callback, [=](const Talk::RecallMsgsCallback& cb){
		std::list<RecallMsgNotify> notifys;
		ParseRecallMsgNotify(PCharToString(content), notifys);
		CallbackProxy::Invoke(cb, (NIMResCode)rescode, notifys);
	},true);
}

void Talk::RecallMsg(const IMMessage &msg, const std::string &notify, const RecallMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	RecallMsgsCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new RecallMsgsCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_talk_recall_msg)(msg.ToJsonString(false).c_str(), notify.c_str(), json_extension.c_str(), &CallbackRecallMsg, cb_pointer);
}

void Talk::RecallMsg2(const IMMessage &msg, const std::string &notify, const RecallMsgsCallback& cb, const std::string& apnstext /*= ""*/, const std::string& pushpayloadconst /*= ""*/, const std::string& json_extension/* = ""*/)
{
	RecallMsgsCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new RecallMsgsCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_talk_recall_msg2)(msg.ToJsonString(false).c_str(), notify.c_str(), apnstext.c_str(), pushpayloadconst.c_str(), json_extension.c_str(),  &CallbackRecallMsg, cb_pointer);
}

void Talk::RecallMsgEx(const IMMessage& msg, const std::string& notify, const RecallMsgsCallback& cb, nim_talk_recall_extra_params& extra_param)
{
	RecallMsgsCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new RecallMsgsCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_talk_recall_msg_ex)(msg.ToJsonString(false).c_str(), notify.c_str(),
											 &CallbackRecallMsg, extra_param, cb_pointer);

}
std::string Talk::GetAttachmentPathFromMsg(const IMMessage& msg)
{
	const char* file_path = NIM_SDK_GET_FUNC(nim_talk_get_attachment_path_from_msg)(msg.ToJsonString(false).c_str());
	std::string out_path = (std::string)file_path;
	Global::FreeBuf((void *)file_path);
	return out_path;
}

static void CallbackReceiveBroadcastMsg(const char *content, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Talk::ReceiveBroadcastMsgCallback>(callback, [=](const Talk::ReceiveBroadcastMsgCallback& cb){
		BroadcastMessage msg;
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		if (reader.parse(PCharToString(content), values) && values.isObject())
		{
			msg.body_ = values[kNIMBroadcastMsgKeyBody].asString();
			msg.from_id_ = values[kNIMBroadcastMsgKeyFromAccid].asString();
			msg.id_ = values[kNIMBroadcastMsgKeyID].asUInt64();
			msg.time_ = values[kNIMBroadcastMsgKeyTime].asUInt64();
		}
		CallbackProxy::Invoke(cb, msg);
	});

}

static void CallbackReceiveBroadcastMessages(const char *content, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Talk::ReceiveBroadcastMsgsCallback>(callback, [=](const Talk::ReceiveBroadcastMsgsCallback& cb){
		std::list<BroadcastMessage> msgs;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value value;
		if (reader.parse(PCharToString(content), value) && value.isArray())
		{
			int size = value.size();
			for (int i = 0; i < size; i++)
			{
				BroadcastMessage msg;
				msg.body_ = value[i][kNIMBroadcastMsgKeyBody].asString();
				msg.from_id_ = value[i][kNIMBroadcastMsgKeyFromAccid].asString();
				msg.id_ = value[i][kNIMBroadcastMsgKeyID].asUInt64();
				msg.time_ = value[i][kNIMBroadcastMsgKeyTime].asUInt64();
				msgs.push_back(msg);
			}
		}
		CallbackProxy::Invoke(cb, msgs);
	});
}

static Talk::ReceiveBroadcastMsgCallback g_cb_broadcast = nullptr;
void Talk::RegReceiveBroadcastMsgCb(const ReceiveBroadcastMsgCallback& cb, const std::string& json_extension)
{
	g_cb_broadcast = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_receive_broadcast_cb)(json_extension.c_str(), &CallbackReceiveBroadcastMsg, &g_cb_broadcast);
}

static Talk::ReceiveBroadcastMsgsCallback g_cb_broadcast_msgs = nullptr;
void Talk::RegReceiveBroadcastMsgsCb(const ReceiveBroadcastMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	g_cb_broadcast_msgs = cb;
	NIM_SDK_GET_FUNC(nim_talk_reg_receive_broadcast_msgs_cb)(json_extension.c_str(), &CallbackReceiveBroadcastMessages, &g_cb_broadcast_msgs);
}
void Talk::ReplyMessage(const IMMessage& msg, const std::string& json_reply_msg, FileUpPrgCallback* prg_cb /*= nullptr*/)
{
	auto json_msg = msg.ToJsonString(false);
	if (prg_cb)
	{
		
		NIM_SDK_GET_FUNC(nim_talk_reply_msg)(json_msg.c_str(), json_reply_msg.c_str(), nullptr, &CallbackFileUploadProcess, prg_cb);
	}
	else
	{
		NIM_SDK_GET_FUNC(nim_talk_reply_msg)(json_msg.c_str(), json_reply_msg.c_str(), nullptr, nullptr, nullptr);
	}
}

void Talk::UnregTalkCb()
{
	g_cb_send_msg_ack_ = nullptr;
	g_team_notification_filter_ = nullptr;
	g_cb_pointer = nullptr;
	g_cb_msgs_pointer = nullptr;
	g_recall_msg_cb_ = nullptr;
	g_cb_broadcast_msgs = nullptr;
	g_cb_broadcast = nullptr;
}

}

/** @file nim_cpp_talk.cpp
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#include "nim_cpp_talk.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"

namespace nim
{

typedef void(*nim_talk_reg_ack_cb)(const char *json_extension, nim_talk_ack_cb_func cb, const void *user_data);
typedef void(*nim_talk_send_msg)(const char* json_msg, const char *json_extension, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_talk_stop_send_msg)(const char *json_msg, const char *json_extension);
typedef void(*nim_talk_reg_receive_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);
typedef void(*nim_talk_reg_receive_msgs_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);
typedef void(*nim_talk_reg_notification_filter_cb)(const char *json_extension, nim_talk_team_notification_filter_func cb, const void *user_data);

static void CallbackSendMsgAck(const char *result, const void *callback)
{
	if (callback)
	{
		Talk::SendMsgAckCallback* cb_pointer = (Talk::SendMsgAckCallback*)callback;
		if (*cb_pointer)
		{
			SendMessageArc arc;
			ParseSendMessageAck(PCharToString(result), arc);
			(*cb_pointer)(arc);
		}
	}
}

static void CallbackReceiveMsg(const char *content, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Talk::ReceiveMsgCallback* cb_pointer = (Talk::ReceiveMsgCallback*)callback;
		if (*cb_pointer)
		{
			IMMessage msg;
			ParseReceiveMessage(PCharToString(content), msg);
			(*cb_pointer)(msg);
		}
	}
}

static void CallbackReceiveMessages(const char *content, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Talk::ReceiveMsgsCallback* cb_pointer = (Talk::ReceiveMsgsCallback*)callback;
		if (*cb_pointer)
		{
			std::list<IMMessage> msgs;
			Json::Reader reader;
			Json::Value value;
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
			(*cb_pointer)(msgs);
		}
	}
}

static void CallbackFileUploadProcess(__int64 uploaded_size, __int64 file_size, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Talk::FileUpPrgCallback* cb_pointer = (Talk::FileUpPrgCallback*)callback;
		if (*cb_pointer)
		{
			(*cb_pointer)(uploaded_size, file_size);
		}
	}
}

static bool FilterTeamNotification(const char *content, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Talk::TeamNotificationFilter* cb_point = (Talk::TeamNotificationFilter *)callback;
		if (*cb_point)
		{
			IMMessage msg;
			ParseReceiveMessage(PCharToString(content), msg);
			return (*cb_point)(msg);
		}
	}
	return false;
}

static Talk::SendMsgAckCallback* g_cb_send_msg_ack_ = nullptr;
void Talk::RegSendMsgCb(const SendMsgAckCallback& cb, const std::string& json_extension)
{
	if (g_cb_send_msg_ack_)
	{
		delete g_cb_send_msg_ack_;
		g_cb_send_msg_ack_ = nullptr;
	}
	g_cb_send_msg_ack_ = new SendMsgAckCallback(cb);
	return NIM_SDK_GET_FUNC(nim_talk_reg_ack_cb)(json_extension.c_str(), &CallbackSendMsgAck, g_cb_send_msg_ack_);
}

void Talk::SendMsg(const std::string& json_msg, const std::string& json_extension/* = ""*/, FileUpPrgCallback* pcb/* = nullptr*/)
{
	if (pcb)
	{
		return NIM_SDK_GET_FUNC(nim_talk_send_msg)(json_msg.c_str(), nullptr, &CallbackFileUploadProcess, pcb);
	} 
	else
	{
		return NIM_SDK_GET_FUNC(nim_talk_send_msg)(json_msg.c_str(), nullptr, nullptr, nullptr);
	}
}

bool Talk::StopSendMsg(const std::string& client_msg_id, const NIMMessageType& type, const std::string& json_extension/* = ""*/)
{
	if (client_msg_id.empty())
		return false;

	Json::Value values;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyType] = type;
	NIM_SDK_GET_FUNC(nim_talk_stop_send_msg)(GetJsonStringWithNoStyled(values).c_str(), json_extension.c_str());

	return true;
}

static Talk::ReceiveMsgCallback* g_cb_pointer = nullptr;
void Talk::RegReceiveCb(const ReceiveMsgCallback& cb, const std::string& json_extension)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new ReceiveMsgCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_talk_reg_receive_cb)(json_extension.c_str(), &CallbackReceiveMsg, g_cb_pointer);
}

static Talk::ReceiveMsgsCallback* g_cb_msgs_pointer = nullptr;
void Talk::RegReceiveMessagesCb(const ReceiveMsgsCallback& cb, const std::string& json_extension/* = ""*/)
{
	delete g_cb_msgs_pointer;
	if (cb)
	{
		g_cb_msgs_pointer = new ReceiveMsgsCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_talk_reg_receive_msgs_cb)(json_extension.c_str(), &CallbackReceiveMessages, g_cb_msgs_pointer);
}

std::string Talk::CreateTextMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const std::string& content
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateImageMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMImage& image
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateFileMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMFile& file
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateAudioMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMAudio& audio
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateVideoMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMVideo& video
	, const std::string& file_path
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateLocationMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const IMLocation& location
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
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

	return GetJsonStringWithNoStyled(values);
}

std::string Talk::CreateTipMessage(const std::string& receiver_id
	, const NIMSessionType session_type
	, const std::string& client_msg_id
	, const Json::Value& tips
	, const MessageSetting& msg_setting
	, __int64 timetag/* = 0*/)
{
	Json::Value values;
	values[kNIMMsgKeyToAccount] = receiver_id;
	values[kNIMMsgKeyToType] = session_type;
	values[kNIMMsgKeyClientMsgid] = client_msg_id;
	values[kNIMMsgKeyServerExt] = GetJsonStringWithNoStyled(tips);
	values[kNIMMsgKeyType] = kNIMMessageTypeTips;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;

	return GetJsonStringWithNoStyled(values);
}


bool Talk::ParseIMMessage(const std::string& json_msg, IMMessage& msg)
{
	return ParseMessage(json_msg, msg);
}

bool Talk::ParseImageMessageAttach(const IMMessage& msg, IMImage& image)
{
	if (msg.type_ != kNIMMessageTypeImage)
		return false;

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
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

	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		location.description_ = values[kNIMLocationMsgKeyTitle].asString();
		location.latitude_ = values[kNIMLocationMsgKeyLatitude].asDouble();
		location.longitude_ = values[kNIMLocationMsgKeyLongitude].asDouble();
		return true;
	}
	return false;
}

static Talk::TeamNotificationFilter* g_team_notification_filter_ = nullptr;
void Talk::RegTeamNotificationFilter(const TeamNotificationFilter& filter, const std::string& json_extension/* = ""*/)
{
	if (g_team_notification_filter_)
	{
		delete g_team_notification_filter_;
		g_team_notification_filter_ = nullptr;
	}
	g_team_notification_filter_ = new Talk::TeamNotificationFilter(filter);
	return NIM_SDK_GET_FUNC(nim_talk_reg_notification_filter_cb)(json_extension.c_str(), &FilterTeamNotification, g_team_notification_filter_);
}

void Talk::UnregTalkCb()
{
	if (g_cb_send_msg_ack_)
	{
		delete g_cb_send_msg_ack_;
		g_cb_send_msg_ack_ = nullptr;
	}
	if (g_team_notification_filter_)
	{
		delete g_team_notification_filter_;
		g_team_notification_filter_ = nullptr;
	}
	if (g_cb_pointer)
	{
		delete g_cb_pointer;
		g_cb_pointer = nullptr;
	}
	if (g_cb_msgs_pointer)
	{
		delete g_cb_msgs_pointer;
		g_cb_msgs_pointer = nullptr;
	}
}

}
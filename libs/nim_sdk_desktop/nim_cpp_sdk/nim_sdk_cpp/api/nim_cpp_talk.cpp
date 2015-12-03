#include "nim_cpp_talk.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"

namespace nim
{

typedef void(*nim_talk_reg_arc_cb)(const char *json_extension, nim_talk_arc_cb_func cb, const void *user_data);
typedef void(*nim_talk_send_msg)(const char* json_msg, const char *json_extension, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_talk_stop_send_msg)(const char *json_msg, const char *json_extension);
typedef void(*nim_talk_reg_receive_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);

static void CallbackSendMsgArc(const char *result, const void *callback)
{
	if (callback)
	{
		Talk::SendMsgArcCallback* cb_pointer = (Talk::SendMsgArcCallback*)callback;
		if (*cb_pointer)
		{
			SendMessageArc arc;
			ParseSendMessageArc(PCharToString(result), arc);
			PostTaskToUIThread(std::bind((*cb_pointer), arc));
			//(*cb_pointer)(arc);
		}
	}
}

static void CallbackReceiveMsg(const char *content, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Talk::ReveiveMsgCallback* cb_pointer = (Talk::ReveiveMsgCallback*)callback;
		if (*cb_pointer)
		{
			IMMessage msg;
			ParseReceiveMessage(PCharToString(content), msg);
			PostTaskToUIThread(std::bind((*cb_pointer), msg));
			//(*cb_pointer)(msg);
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
			PostTaskToUIThread(std::bind((*cb_pointer), uploaded_size, file_size));
			//(*cb_pointer)(uploaded_size, file_size);
		}
	}
}

static Talk::SendMsgArcCallback* g_send_msg_cb_pointer = nullptr;
void Talk::RegSendMsgCb(const SendMsgArcCallback& cb, const std::string& json_extension)
{
	delete g_send_msg_cb_pointer;
	if (cb)
	{
		g_send_msg_cb_pointer = new SendMsgArcCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_talk_reg_arc_cb)(json_extension.c_str(), &CallbackSendMsgArc, g_send_msg_cb_pointer);
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
	NIM_SDK_GET_FUNC(nim_talk_stop_send_msg)(values.toStyledString().c_str(), json_extension.c_str());

	return true;
}

static Talk::ReveiveMsgCallback* g_cb_pointer = nullptr;
void Talk::RegReceiveCb(const ReveiveMsgCallback& cb, const std::string& json_extension)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new ReveiveMsgCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_talk_reg_receive_cb)(json_extension.c_str(), &CallbackReceiveMsg, g_cb_pointer);
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

	return values.toStyledString();
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

	return values.toStyledString();
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

	return values.toStyledString();
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

	return values.toStyledString();
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

	return values.toStyledString();
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

	return values.toStyledString();
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
	values[kNIMMsgKeyServerExt] = tips.toStyledString();
	values[kNIMMsgKeyType] = kNIMMessageTypeTips;
	values[kNIMMsgKeyLocalTalkId] = receiver_id;

	msg_setting.ToJsonValue(values);

	//选填
	if (timetag > 0)
		values[kNIMMsgKeyTime] = timetag;

	return values.toStyledString();
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



}
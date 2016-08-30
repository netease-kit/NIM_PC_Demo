/** @file nim_talk_helper.cpp
  * @brief Talk 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/16
  */

#include "nim_talk_helper.h"

namespace nim
{

bool ParseMessage(const std::string& msg_json, IMMessage& message)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msg_json, values) && values.isObject())
	{
		ParseMessage(values, message);
		return true;
	}
	return false;
}

bool ParseReceiveMessage(const std::string& msg_json, IMMessage& message)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msg_json, values) && values.isObject())
	{
		ParseReceiveMessage(values, message);
		return true;
	}
	
	return false;
}

void ParseReceiveMessage(const Json::Value& msg_json_value, IMMessage& message)
{
	message.rescode_ = (NIMResCode)msg_json_value[kNIMMsgKeyLocalRescode].asUInt();
	message.feature_ = (NIMMessageFeature)msg_json_value[kNIMMsgKeyLocalMsgFeature].asUInt();

	ParseMessage(msg_json_value[kNIMMsgKeyLocalReceiveMsgContent], message);
}

void ParseMessage(const Json::Value& msg_json, IMMessage& message)
{
	message.session_type_ = (NIMSessionType)msg_json[kNIMMsgKeyToType].asUInt();
	message.receiver_accid_ = msg_json[kNIMMsgKeyToAccount].asString();
	message.sender_accid_ = msg_json[kNIMMsgKeyFromAccount].asString();
	message.readonly_sender_client_type_ = (NIMClientType)msg_json[kNIMMsgKeyFromClientType].asUInt();
	message.readonly_sender_device_id_ = msg_json[kNIMMsgKeyFromDeviceId].asString();
	message.readonly_sender_nickname_ = msg_json[kNIMMsgKeyFromNick].asString();
	message.timetag_ = msg_json[kNIMMsgKeyTime].asUInt64();

	message.type_ = (NIMMessageType)msg_json[kNIMMsgKeyType].asUInt();
	message.content_ = msg_json[kNIMMsgKeyBody].asString();
	message.attach_ = msg_json[kNIMMsgKeyAttach].asString();
	message.client_msg_id_ = msg_json[kNIMMsgKeyClientMsgid].asString();
	message.readonly_server_id_ = msg_json[kNIMMsgKeyServerMsgid].asUInt64();

	message.local_res_path_ = msg_json[kNIMMsgKeyLocalFilePath].asString();
	message.local_talk_id_ = msg_json[kNIMMsgKeyLocalTalkId].asString();
	message.local_res_id_ = msg_json[kNIMMsgKeyLocalResId].asString();
	message.status_ = (NIMMsgLogStatus)msg_json[kNIMMsgKeyLocalLogStatus].asUInt();
	message.sub_status_ = (NIMMsgLogSubStatus)msg_json[kNIMMsgKeyLocalLogSubStatus].asUInt();

	message.msg_setting_.ParseMessageSetting(msg_json);
}
}
/** @file nim_msg_helper.cpp
  * @brief Message 通用辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */
#if !defined(WIN32)
#include <pthread.h>
#endif
#include "src/cpp_sdk/nim/helper/nim_msg_helper.h"
#include "include/depend_lib/include/nim_json_util.h"
#include "public_define/defines/nim_define/nim_talk_def.h"
#include "public_define/defines/nim_define/nim_msglog_def.h"
namespace nim
{
	bool ParseRecallMsgNotify(const nim_cpp_wrapper_util::Json::Value& json_value, RecallMsgNotify& notify)
	{
		notify.from_id_ = json_value[kNIMRecallMsgKeyFromAccID].asString();
		notify.to_id_ = json_value[kNIMRecallMsgKeyToAccID].asString();
		notify.msg_id_ = json_value[kNIMRecallMsgKeyMsgId].asString();
		notify.operator_id_ = json_value[kNIMRecallMsgKeyOpeAccID].asString();
		notify.notify_ = json_value[kNIMRecallMsgKeyNotify].asString();
		notify.session_type_ = (NIMSessionType)json_value[kNIMRecallMsgKeyToType].asInt();
		notify.notify_timetag_ = json_value[kNIMRecallMsgKeyTime].asUInt64();
		notify.notify_feature_ = (NIMMessageFeature)json_value[kNIMRecallMsgKeyNotifyFeature].asInt();
		notify.msglog_exist_ = json_value[kNIMRecallMsgKeyMsgExist].asBool();
		notify.msglog_timetag_ = json_value[kNIMRecallMsgKeyMsgTime].asUInt64();
		notify.from_nick_ = json_value[kNIMRecallMsgKeyMsgFromNick].asString();
		return true;
	}
	bool ParseRecallMsgNotify(const std::string json_value, RecallMsgNotify& notify)
	{
		nim_cpp_wrapper_util::Json::Value value;
		if (nim_cpp_wrapper_util::Json::Reader().parse(json_value, value))
		{
			return ParseRecallMsgNotify(value, notify);
		}
		return false;
	}
bool ParseRecallMsgNotify(const std::string& notify_json, std::list<RecallMsgNotify>& notifys)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(notify_json, values) && values.isArray())
	{
		auto size = values.size();
		for (auto it : values )
		{
			RecallMsgNotify notify;
			if(ParseRecallMsgNotify(it,notify))
				notifys.emplace_back(notify);
		}
		return true;
	}
	return false;
}

bool ParseSendMessageAck(const std::string& arc_json, SendMessageArc& arc)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(arc_json, values) && values.isObject())
	{
		arc.rescode_ = (NIMResCode)values[kNIMSendAckKeyRescode].asUInt();
		arc.msg_id_ = values[kNIMSendAckKeyMsgId].asString();
		arc.talk_id_ = values[kNIMSendAckKeyTalkId].asString();
		arc.msg_timetag_ = values[kNIMSendAckKeyTimetag].asInt64();
		if(values.isMember(kNIMSendAckKeyCallbackExt))
			arc.third_party_callback_ext_ = values[kNIMSendAckKeyCallbackExt].asString();
		return true;
	}
	return false;
}

}

/** @file nim_msg_helper.cpp
  * @brief Message 通用辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#include "nim_msg_helper.h"

namespace nim
{
#include "nim_talk_def.h"
#include "nim_msglog_def.h"

bool ParseRecallMsgNotify(const std::string& notify_json, std::list<RecallMsgNotify>& notifys)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(notify_json, values) && values.isArray())
	{
		auto size = values.size();
		for (int i = 0; i < size; i++ )
		{
			Json::Value value = values[i];
			RecallMsgNotify notify;
			notify.from_id_ = value[kNIMRecallMsgKeyFromAccID].asString();
			notify.to_id_ = value[kNIMRecallMsgKeyToAccID].asString();
			notify.msg_id_ = value[kNIMRecallMsgKeyMsgId].asString();
			notify.operator_id_ = value[kNIMRecallMsgKeyOpeAccID].asString();
			//if (value.isMember(kNIMRecallMsgKeyNotify))
			notify.notify_ = value[kNIMRecallMsgKeyNotify].asString();
			notify.session_type_ = (NIMSessionType)value[kNIMRecallMsgKeyToType].asInt();
			notify.notify_timetag_ = value[kNIMRecallMsgKeyTime].asUInt64();
			notify.notify_feature_ = (NIMMessageFeature)value[kNIMRecallMsgKeyNotifyFeature].asInt();
			notify.msglog_exist_ = value[kNIMRecallMsgKeyMsgExist].asBool();
			notify.msglog_timetag_ = value[kNIMRecallMsgKeyMsgTime].asUInt64();
			notify.from_nick_ = value[kNIMRecallMsgKeyMsgFromNick].asString();
			notifys.push_back(notify);
		}
		return true;
	}
	return false;
}

bool ParseSendMessageAck(const std::string& arc_json, SendMessageArc& arc)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(arc_json, values) && values.isObject())
	{
		arc.rescode_ = (NIMResCode)values[kNIMSendAckKeyRescode].asUInt();
		arc.msg_id_ = values[kNIMSendAckKeyMsgId].asString();
		arc.talk_id_ = values[kNIMSendAckKeyTalkId].asString();
		arc.msg_timetag_ = values[kNIMSendAckKeyTimetag].asInt64();
		return true;
	}
	return false;
}

}

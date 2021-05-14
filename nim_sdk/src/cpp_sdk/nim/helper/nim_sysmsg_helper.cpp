/** @file nim_sysmsg_helper.cpp
  * @brief sysmsg 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/20
  */

#include "assert.h"
#include "src/cpp_sdk/nim/helper/nim_sysmsg_helper.h"

namespace nim
{

bool ParseSysMessage(const std::string& sysmsg_json, SysMessage& msg)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(sysmsg_json, values) && values.isObject())
	{
		msg.rescode_ = (NIMResCode)values[kNIMSysMsgKeyLocalRescode].asUInt();
		msg.feature_ = (NIMMessageFeature)values[kNIMSysMsgKeyLocalFeature].asUInt();
		msg.total_unread_count_ = values[kNIMSysMsgKeyLocalUnreadCount].asUInt();

		nim_cpp_wrapper_util::Json::Value content = values[kNIMSysMsgKeyLocalContent];
		ParseSysMessageContent(content, msg);

		return true;
	}

	return false;
}

bool ParseSysMessages(const std::string& sysmsgs_json, std::list<SysMessage>& msgs, int* unread)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(sysmsgs_json, values) && values.isObject())
	{
		*unread = values[kNIMSysMsglogQueryKeyUnreadCount].asUInt();
		nim_cpp_wrapper_util::Json::Value content = values[kNIMSysMsglogQueryKeyContent];
		int len = content.size();
		for (int i = 0; i < len; i++)
		{
			SysMessage msg;
			ParseSysMessageContent(content[i], msg);
			msgs.push_back(msg);
		}
		return true;
	}

	return false;
}

void ParseSysMessageContent(const nim_cpp_wrapper_util::Json::Value& content_json, SysMessage& msg)
{
	msg.timetag_ = content_json[kNIMSysMsgKeyTime].asUInt64();
	msg.type_ = (NIMSysMsgType)content_json[kNIMSysMsgKeyType].asUInt();
	msg.receiver_accid_ = content_json[kNIMSysMsgKeyToAccount].asString();
	msg.sender_accid_ = content_json[kNIMSysMsgKeyFromAccount].asString();
	msg.content_ = content_json[kNIMSysMsgKeyMsg].asString();
	msg.attach_ = content_json[kNIMSysMsgKeyAttach].asString();
	msg.id_ = content_json[kNIMSysMsgKeyMsgId].asUInt64();
	msg.status_ = (NIMSysMsgStatus)content_json[kNIMSysMsgKeyLocalStatus].asUInt();
	msg.callbac_ext_ = content_json[kNIMSysMsgKeyCallbackExt].asString();
	msg.msg_setting_.ParseMessageSetting(content_json);
}

}
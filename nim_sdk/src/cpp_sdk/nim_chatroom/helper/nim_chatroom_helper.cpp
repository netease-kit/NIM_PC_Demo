/** @file nim_chatroom_helper.cpp
  * @brief 聊天室SDK辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/12/29
  */

#include "src/cpp_sdk/nim_chatroom/helper/nim_chatroom_helper.h"

namespace nim_chatroom
{
std::vector<NIMChatRoomMsgType> ChatRoomGetMsgHistoryParameters::kMsg_Types_List;
bool operator == (const NIMChatRoomExitReasonInfo& info,NIMChatRoomExitReason code)
{
	return info.code_ == code;
}
	
bool ParseChatRoomEnterCallbackResultInfo(const std::string& result, ChatRoomInfo& room_info, ChatRoomMemberInfo& my_info)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(result, values) && values.isObject())
	{
		room_info.ParseFromJsonValue(values[kNIMChatRoomEnterCallbackKeyRoomInfo]);
		my_info.ParseFromJsonValue(values[kNIMChatRoomEnterCallbackKeyMyInfo]);
		return true;
	}
	return false;
}

bool ParseChatRoomMemberInfos(const std::string& infos_json_str, std::list<ChatRoomMemberInfo>& infos)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(infos_json_str, values) && values.isArray())
	{
		int count = values.size();
		for (int i = 0; i < count; i++)
		{
			ChatRoomMemberInfo info;
			info.ParseFromJsonValue(values[i]);
			infos.push_back(info);
		}
		return true;
	}
	return false;
}

bool ParseChatRoomMsgs(const std::string& msgs_json_str, std::list<ChatRoomMessage>& msgs)
{
	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::Reader reader;
	if (reader.parse(msgs_json_str, values) && values.isArray())
	{
		int count = values.size();
		for (int i = 0; i < count; i++)
		{
			ChatRoomMessage msg;
			msg.ParseFromJsonValue(values[i]);
			msgs.push_back(msg);
		}
		return true;
	}
	return false;
}

bool ParseRobotInfosStringToRobotInfos(const std::string &infos_json, RobotInfos &infos)
{
	nim_cpp_wrapper_util::Json::Reader reader;
	nim_cpp_wrapper_util::Json::Value values;
	if (reader.parse(infos_json, values) && values.isArray())
	{
		int len = values.size();
		for (int i = 0; i < len; i++)
		{
			RobotInfo info;
			if (ParseRobotInfoStringToRobotInfo(values[i].asString(), info))
				infos.push_back(info);
		}
		return true;
	}
	return false;
}

bool ParseRobotInfoStringToRobotInfo(const std::string &info_json, RobotInfo &info)
{
	nim_cpp_wrapper_util::Json::Reader reader;
	nim_cpp_wrapper_util::Json::Value value;
	if (reader.parse(info_json, value) && value.isObject())
	{
		info.SetAccid(value[kNIMRobotInfoKeyAccid].asString());
		info.SetName(value[kNIMRobotInfoKeyName].asString());
		info.SetIcon(value[kNIMRobotInfoKeyIcon].asString());
		info.SetIntro(value[kNIMRobotInfoKeyIntro].asString());
		info.SetRobotID(value[kNIMRobotInfoKeyRobotId].asString());
		info.SetCreateTime(value[kNIMRobotInfoKeyCreateTime].asUInt64());
		info.SetUpdateTime(value[kNIMRobotInfoKeyUpdateTime].asUInt64());
		return true;
	}
	return false;
}


bool ParseBatchInfosStringToNotMembers(const std::string &infos_json, std::list<std::string> &lst_members)
{
	nim_cpp_wrapper_util::Json::Reader reader;
	nim_cpp_wrapper_util::Json::Value values;
	if (reader.parse(infos_json, values) && values.isArray())
	{
		int len = values.size();
		for (int i = 0; i < len; i++)
		{
			lst_members.push_back(values[i].asString());
		}
		return true;
	}
	return false;
}




}

/** @file nim_chatroom_helper.cpp
  * @brief 聊天室SDK辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg, Harrison
  * @date 2015/12/29
  */

#include "nim_chatroom_helper.h"

namespace nim_chatroom
{
	std::vector<NIMChatRoomMsgType> ChatRoomGetMsgHistoryParameters::kMsg_Types_List;
	const char * ChatRoomPlatformConfig::kPlatformConfigToken = "platform_config_token";//平台配置标签
	const char * ChatRoomPlatformConfig::kNtserverAddress = "nt_server";//部分 IM 错误信息统计上报地址,
	const char * ChatRoomPlatformConfig::kUploadStatisticsData = "is_upload_statistics_data";//错误信息统计是否上报,私有化如果不上传相应数据，此项配置应为false
bool ParseChatRoomEnterCallbackResultInfo(const std::string& result, ChatRoomInfo& room_info, ChatRoomMemberInfo& my_info)
{
	Json::Value values;
	Json::Reader reader;
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
	Json::Value values;
	Json::Reader reader;
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
	Json::Value values;
	Json::Reader reader;
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
	Json::Reader reader;
	Json::Value values;
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
	Json::Reader reader;
	Json::Value value;
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
}

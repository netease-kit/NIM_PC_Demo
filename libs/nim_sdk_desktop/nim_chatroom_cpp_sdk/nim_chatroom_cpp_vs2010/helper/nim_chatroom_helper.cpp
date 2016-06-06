/** @file nim_chatroom_helper.cpp
  * @brief 聊天室SDK辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/12/29
  */

#include "nim_chatroom_helper.h"

namespace nim_chatroom
{

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

}

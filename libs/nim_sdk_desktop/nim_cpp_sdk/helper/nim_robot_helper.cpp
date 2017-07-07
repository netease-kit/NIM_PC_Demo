/** @file nim_robot_helper.cpp
  * @brief SDK波特机器人辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/06/26
  */

#include "nim_robot_helper.h"

namespace nim
{
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
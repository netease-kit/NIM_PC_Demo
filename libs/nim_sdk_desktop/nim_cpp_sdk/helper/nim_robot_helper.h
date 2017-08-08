/** @file nim_robot_helper.h
  * @brief SDK波特机器人辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/06/26
  */

#ifndef _NIM_SDK_CPP_ROBOT_HELPER_H_
#define _NIM_SDK_CPP_ROBOT_HELPER_H_

#include <string>
#include <list>
#include <assert.h>
#include "json.h"
#include "nim_json_util.h"
#include "nim_base_types.h"
#include "nim_sdk_defines.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @brief 机器人信息 */
struct RobotInfo
{
public:
	/** 设置机器人云信id */
	void SetAccid(const std::string& accid)
	{
		robot_info_[kNIMRobotInfoKeyAccid] = accid;
	}
	
	/** 获取机器人云信id */
	std::string GetAccid() const
	{
		return robot_info_[kNIMRobotInfoKeyAccid].asString();
	}

	/** 设置机器人名字 */
	void SetName(const std::string& name)
	{
		robot_info_[kNIMRobotInfoKeyName] = name;
	}

	/** 获取机器人名字 */
	std::string GetName() const
	{
		return robot_info_[kNIMRobotInfoKeyName].asString();
	}

	/** 设置机器人头像 */
	void SetIcon(const std::string& icon)
	{
		robot_info_[kNIMRobotInfoKeyIcon] = icon;
	}

	/** 获取机器人头像 */
	std::string GetIcon() const
	{
		return robot_info_[kNIMRobotInfoKeyIcon].asString();
	}

	/** 设置机器人介绍 */
	void SetIntro(const std::string& intro)
	{
		robot_info_[kNIMRobotInfoKeyIntro] = intro;
	}

	/** 获取机器人介绍 */
	std::string GetIntro() const
	{
		return robot_info_[kNIMRobotInfoKeyIntro].asString();
	}

	/** 设置机器人波特平台ID */
	void SetRobotID(const std::string& id)
	{
		robot_info_[kNIMRobotInfoKeyRobotId] = id;
	}

	/** 获取机器人波特平台ID */
	std::string GetRobotID() const
	{
		return robot_info_[kNIMRobotInfoKeyRobotId].asString();
	}

	/** 设置机器人创建时间 */
	void SetCreateTime(const uint64_t time)
	{
		robot_info_[kNIMRobotInfoKeyCreateTime] = time;
	}

	/** 获取机器人创建时间 */
	uint64_t GetCreateTime() const
	{
		return robot_info_[kNIMRobotInfoKeyCreateTime].asUInt64();
	}

	/** 设置机器人更新时间 */
	void SetUpdateTime(const uint64_t time)
	{
		robot_info_[kNIMRobotInfoKeyUpdateTime] = time;
	}

	/** 获取机器人更新时间 */
	uint64_t GetUpdateTime() const
	{
		return robot_info_[kNIMRobotInfoKeyUpdateTime].asUInt64();
	}

private:
	Json::Value robot_info_;
};

typedef std::list<RobotInfo> RobotInfos;

/** @fn bool ParseRobotInfosStringToRobotInfos(const std::string& infos_json, RobotInfos &infos)
  * @brief 解析机器人信息
  * @param[in] infos_json 机器人信息（Json Value数据字符串）
  * @param[out] infos 机器人信息
  * @return bool 解析成功或失败 
  */
bool ParseRobotInfosStringToRobotInfos(const std::string &infos_json, RobotInfos &infos);

/** @fn bool ParseRobotInfoStringToRobotInfo(const std::string& info_json, RobotInfos &info)
  * @brief 解析机器人信息
  * @param[in] info_json 机器人信息
  * @param[out] info 机器人信息
  * @return bool 解析成功或失败 
  */
bool ParseRobotInfoStringToRobotInfo(const std::string &info_json, RobotInfo &info);

}

#endif //_NIM_SDK_CPP_ROBOT_HELPER_H_
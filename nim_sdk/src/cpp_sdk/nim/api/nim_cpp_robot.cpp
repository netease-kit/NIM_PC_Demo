/** @file nim_cpp_robot.cpp
  * @brief NIM SDK提供的Robot接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2017.06.26
  */

#include "src/cpp_sdk/nim/api//nim_cpp_robot.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void (*nim_robot_reg_changed_callback)(const char *json_extension, nim_robot_change_cb_func cb, const void *user_data);
typedef char* (*nim_robot_query_all_robots_block)(const char *json_extension);
typedef char* (*nim_robot_query_robot_by_accid_block)(const char *accid, const char *json_extension);
typedef void (*nim_robot_get_robots_async)(__int64 timetag, const char *json_extension, nim_robot_query_cb_func cb, const void *user_data);
#else
#include "nim_robot.h"
#endif

static void CallbackRobotInfoChange(int rescode, NIMRobotInfoChangeType type, const char *res, const char *json_extension, const void *callback)
{
	CallbackProxy::DoSafeCallback<Robot::RobotChangedCallback>(callback, [=](const Robot::RobotChangedCallback& cb){

		RobotInfos infos;
		ParseRobotInfosStringToRobotInfos(PCharToString(res), infos);
		CallbackProxy::Invoke(cb, (NIMResCode)rescode,type, infos);
	});
}

static Robot::RobotChangedCallback g_cb_changed_ = nullptr;
void Robot::RegChangedCallback(const RobotChangedCallback &callback, const std::string &json_extension)
{
	g_cb_changed_ = callback;
	NIM_SDK_GET_FUNC(nim_robot_reg_changed_callback)(json_extension.c_str(), &CallbackRobotInfoChange, &g_cb_changed_);
}

RobotInfos Robot::QueryAllRobotInfosBlock(const std::string &json_extension/* = ""*/)
{
	char *res = NIM_SDK_GET_FUNC(nim_robot_query_all_robots_block)(json_extension.c_str());
	RobotInfos infos;
	ParseRobotInfosStringToRobotInfos(PCharToString(res), infos);
	return infos;
}

RobotInfo Robot::QueryRobotInfoByAccidBlock(const std::string &accid, const std::string &json_extension/* = ""*/)
{
	char *res = NIM_SDK_GET_FUNC(nim_robot_query_robot_by_accid_block)(accid.c_str(), json_extension.c_str());
	RobotInfo info;
	ParseRobotInfoStringToRobotInfo(PCharToString(res), info);
	return info;
}

static void CallbackRobotQuery(int rescode, const char *res, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Robot::RobotQueryCallback>(callback, [=](const Robot::RobotQueryCallback& cb){

		RobotInfos infos;
		ParseRobotInfosStringToRobotInfos(PCharToString(res), infos);
		CallbackProxy::Invoke(cb, (NIMResCode)rescode, infos);
	},true);

}

void Robot::GetRobotInfoAsync(const __int64 timetag, const RobotQueryCallback &callback, const std::string &json_extension/* = ""*/)
{
	RobotQueryCallback* cb_pointer = nullptr;
	if (callback)
	{
		cb_pointer = new RobotQueryCallback(callback);
	}
	NIM_SDK_GET_FUNC(nim_robot_get_robots_async)(timetag, json_extension.c_str(), &CallbackRobotQuery, cb_pointer);
}
void Robot::UnregRobotCb()
{
	g_cb_changed_ = nullptr;
}
}
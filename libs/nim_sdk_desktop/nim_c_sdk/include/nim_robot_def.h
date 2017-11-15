/** @file nim_robot_def.h
  * @brief 智能机器人 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/06/24
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_ROBOT_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_ROBOT_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif

/** @name 机器人信息 Json Keys
* 例子  { "accid" : "", "create_timetag" : 1430101821372, "icon" : "", "intro":"", "name" : "", "update_timetag" : 1430101821372, "rid":"" }
  * @{
  */
//协议定义
static const char *kNIMRobotInfoKeyAccid	= "accid";	/**< string 帐号*/
static const char *kNIMRobotInfoKeyName		= "name";	/**< string 名字*/
static const char *kNIMRobotInfoKeyIcon		= "icon";	/**< string 头像*/
static const char *kNIMRobotInfoKeyIntro	= "intro";	/**< string 介绍*/
static const char *kNIMRobotInfoKeyRobotId	= "rid";	/**< string 机器人ID*/
static const char *kNIMRobotInfoKeyCreateTime	= "create_timetag";	/**< long 创建时间戳 毫秒*/
static const char *kNIMRobotInfoKeyUpdateTime	= "update_timetag";	/**< long 更新时间戳 毫秒*/
/** @}*/ //机器人信息 Json Keys

/** @enum NIMRobotInfoChangeType*/
enum NIMRobotInfoChangeType
{
	kNIMRobotInfoChangeTypeAll      = 0,			/**< 全量更新*/
};

/** @typedef void (*nim_robot_change_cb_func)(int rescode, NIMRobotInfoChangeType type, const char *result, const char *json_extension, const void *user_data)
  * 机器人变更的通知函数定义
  * @param[out] rescode			错误码,200:一切正常
  * @param[out] type			更新类型 0：全量更新
  * @param[out] result			机器人信息 Json string array
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_robot_change_cb_func)(int rescode, NIMRobotInfoChangeType type, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_robot_query_cb_func)(int rescode, const char *result, const char *json_extension, const void *user_data)
  * 获取机器人信息的函数定义
  * @param[out] rescode			错误码,200:一切正常
  * @param[out] result			机器人信息 Json string array
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_robot_query_cb_func)(int rescode, const char *result, const char *json_extension, const void *user_data);

#ifdef __cplusplus
}
#endif

#endif //NIM_SDK_DLL_EXPORT_HEADERS_ROBOT_DEF_H_

/** @file nim_robot.h
  * @brief 智能机器人 接口头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/06/24
  */

#ifndef NIM_SDK_DLL_API_NIM_ROBOT_H_
#define NIM_SDK_DLL_API_NIM_ROBOT_H_

#include "nim_sdk_dll.h"
#include "nim_robot_def.h"
#include "util/stdbool.h"

#ifdef __cplusplus
extern"C"
{
#endif
#if NIMAPI_UNDER_WIN_DESKTOP_ONLY

/** @fn void nim_robot_reg_changed_callback(const char *json_extension, nim_robot_change_cb_func cb, const void *user_data)
  * 注册机器人变更广播通知
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			nim_robot_change_cb_func回调函数定义见nim_robot_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_robot_reg_changed_callback(const char *json_extension, nim_robot_change_cb_func cb, const void *user_data);

/** @fn char *nim_robot_query_all_robots_block(const char *json_extension)
  * 获取全部机器人信息(同步接口，堵塞NIM内部线程)
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return char 机器人信息 json string array
  */
NIM_SDK_DLL_API char *nim_robot_query_all_robots_block(const char *json_extension);

/** @fn char *nim_robot_query_robot_by_accid_block(const char *accid, const char *json_extension)
  * 获取指定机器人信息(同步接口，堵塞NIM内部线程)
  * @param[in] accid 机器人accid
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return char 机器人信息 json string
  */
NIM_SDK_DLL_API char *nim_robot_query_robot_by_accid_block(const char *accid, const char *json_extension);
#endif
#ifdef __cplusplus
}
#endif

#endif //NIM_SDK_DLL_API_NIM_ROBOT_H_
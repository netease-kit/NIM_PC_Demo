/** @file nim_session.h
  * @brief NIM SDK提供的session接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_SESSION_H_
#define NIM_SDK_DLL_API_NIM_SESSION_H_

#include "nim_sdk_dll.h"
#include "nim_session_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_session_reg_change_cb(const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 注册最近会话列表项变更通知
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_reg_change_cb(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_query_all_recent_session_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data)
  * 查询会话列表
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询会话列表的回调函数， nim_session_query_recent_session_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_all_recent_session_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data);

/** @fn void nim_session_delete_recent_session_async(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 删除最近联系人
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_delete_recent_session_async(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_delete_all_recent_session_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 删除全部最近联系人
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_delete_all_recent_session_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_set_unread_count_zero_async(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 最近联系人项未读数清零
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_set_unread_count_zero_async(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_SESSION_H_
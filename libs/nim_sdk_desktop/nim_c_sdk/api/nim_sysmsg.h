/** @file nim_sysmsg.h
  * @brief NIM SDK提供的系统消息历史接口 
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_SYSMSG_H_
#define NIM_SDK_DLL_API_NIM_SYSMSG_H_

#include "nim_sdk_dll.h"
#include "nim_sysmsg_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_sysmsg_reg_sysmsg_cb(const char *json_extension, nim_sysmsg_receive_cb_func cb, const void *user_data)
  * 注册接收系统通知回调接口
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			操作结果的回调函数， nim_sysmsg_receive_cb_func回调函数定义见nim_sysmsg_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_reg_sysmsg_cb(const char *json_extension, nim_sysmsg_receive_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_query_msg_async(int limit_count, __int64 last_time, const char *json_extension, nim_sysmsg_query_cb_func cb, const void *user_data)
  * 查询本地系统消息（按时间逆序起查，逆序排列）
  * @param[in] limit_count	一次查询数量，建议20
  * @param[in] last_time	上次查询最后一条消息的时间戳（按时间逆序起查，即最小的时间戳）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询本地系统消息的回调函数， nim_sysmsg_query_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_query_msg_async(int limit_count, __int64 last_time, const char *json_extension, nim_sysmsg_query_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_query_unread_count(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data)
  * 查询未读消息数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询未读消息数的回调函数， nim_sysmsg_res_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_query_unread_count(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_set_status_async(__int64 msg_id, NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data)
  * 设置消息状态
  * @param[in] msg_id		消息id
  * @param[in] status		消息状态
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			设置消息状态的回调函数， nim_sysmsg_res_ex_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_set_status_async(__int64 msg_id, NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_read_all_async(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data)
  * 设置全部消息为已读
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			设置全部消息为已读的回调函数， nim_sysmsg_res_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_read_all_async(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_delete_async(__int64 msg_id, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data)
  * 删除消息
  * @param[in] msg_id		消息id
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			删除消息的回调函数， nim_sysmsg_res_ex_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_delete_async(__int64 msg_id, const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);

/** @fn void nim_sysmsg_delete_all_async(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data)
  * 全部删除
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			删除消息的回调函数， nim_sysmsg_res_ex_cb_func回调函数定义见nim_msglog_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_sysmsg_delete_all_async(const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_SYSMSG_H_
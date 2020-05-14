/** @file nim_session.h
  * @brief 最近会话列表 接口头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_SESSION_H_
#define NIM_SDK_DLL_API_NIM_SESSION_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/defines/nim_define/nim_session_def.h"
#include "public_define/defines/nim_define/nim_msglog_def.h"
#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_session_reg_change_cb(const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * (全局回调)注册最近会话列表项变更通知
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  */
NIM_SDK_DLL_API void nim_session_reg_change_cb(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_reg_badge_count_cb(const char *json_extension, nim_session_badge_count_cb_func cb, const void *user_data)
* (全局回调)注册获取badge数量的回调(仅iOS platform有效)
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb            最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
* @param[in] user_data    APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
* @note 错误码    200:成功
*/
NIM_SDK_DLL_API void nim_session_reg_badge_count_cb(const char *json_extension, nim_session_badge_count_cb_func cb, const void *user_data);

/** @fn void nim_session_query_all_recent_session_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data)
  * 查询指定数量的最后会话数据
  * @param[in] limit		要返回的最大数量
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询会话列表的回调函数， nim_session_query_recent_session_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_last_few_session_async(int limit, const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data);

/** @fn void nim_session_query_all_recent_session_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data)
  * 查询会话列表
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询会话列表的回调函数， nim_session_query_recent_session_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_all_recent_session_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data);

/** @fn void nim_session_query_all_recent_session_with_last_msg_excluded_type_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, enum NIMMessageType last_msg_excluded_type,const void *user_data)
  * 查询会话列表,可指定最后一条会话消息要排除掉的类型
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			查询会话列表的回调函数， nim_session_query_recent_session_cb_func回调函数定义见nim_session_def.h
  * @param[in] last_msg_excluded_type 最后一条会话消息要排除掉的类型,如果不排除任何消息，参数请传入NIMMessageType::kNIMMessageTypeUnknown
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_all_recent_session_with_last_msg_excluded_type_async(const char *json_extension, nim_session_query_recent_session_cb_func cb, enum NIMMessageType last_msg_excluded_type,const void *user_data);



/** @fn void nim_session_delete_recent_session_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 删除最近联系人
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_delete_recent_session_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_delete_all_recent_session_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 删除全部最近联系人
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_delete_all_recent_session_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_set_unread_count_zero_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 最近联系人项未读数清零
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_set_unread_count_zero_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_set_top(enum NIMSessionType to_type, const char *id, bool top, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 设置会话项是否置顶(置顶属性只保存在本地)
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] top			true - 置顶 false - 取消置顶
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			回调函数
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_set_top(enum NIMSessionType to_type, const char *id, bool top, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_set_extend_data(enum NIMSessionType to_type, const char *id, const char *data, const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 设置会话项扩展数据(扩展数据只保存在本地)
  * @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] data			扩展数据,建议使用灵活可扩展的数据结构,例如Json
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			回调函数
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_set_extend_data(enum NIMSessionType to_type, const char *id, const char *data, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_reset_all_unread_count_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data)
  * 最近联系人项全部未读数清零
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_reset_all_unread_count_async(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);

/** @fn void nim_session_query_sessiondata_by_id_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_query_sessiondata_by_id_cb_func cb, const void *user_data);
  * 根据给定的id查询相应会话的信息
* @param[in] to_type		会话类型
  * @param[in] id			对方的account id或者群组tid。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			最近会话列表项变更的回调函数， nim_session_query_sessiondata_by_id_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_query_sessiondata_by_id_async(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_query_sessiondata_by_id_cb_func cb, const void *user_data);

/** @fn void nim_session_query_stick_top_session_list(nim_session_query_stick_top_session_list_cb_func cb, const void* user_data)
  * 获取置顶会话列表
  * @param[in] cb		获取置顶会话列表回调， 定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_query_stick_top_session_list(nim_session_query_stick_top_session_list_cb_func cb, const void* user_data);

/** @fn void nim_session_set_to_stick_top(const char* session_id, enum NIMSessionType to_type,const char* ext,nim_session_set_to_stick_top_cb_func cb, const void* user_data)
  * 置顶会话
  * @param[in] session_id		会话ID
  * @param[in] to_type		会话类型
  * @param[in] ext		扩展信息
  * @param[in] cb		置顶会话回调， 定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_set_to_stick_top(const char* session_id, enum NIMSessionType to_type, const char* ext, nim_session_set_to_stick_top_cb_func cb, const void* user_data);


/** @fn void nim_session_cancel_stick_top(const char* session_id, enum NIMSessionType to_type, nim_session_cancel_stick_top_cb_func cb, const void* user_data)
  * 取消置顶会话
  * @param[in] session_id		会话ID
  * @param[in] to_type		会话类型
  * @param[in] cb		取消置顶会话回调， 定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_cancel_stick_top(const char* session_id, enum NIMSessionType to_type, nim_session_cancel_stick_top_cb_func cb, const void* user_data);

/** @fn void nim_session_update_stick_top(const char* session_id, enum NIMSessionType to_type,const char* ext, nim_session_update_stick_top_cb_func cb, const void* user_data)
  * 更新置顶会话
  * @param[in] session_id		会话ID
  * @param[in] to_type		会话类型
  * @param[in] ext		扩展信息
  * @param[in] cb		更新置顶会话回调， 定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				0:失败
  */
NIM_SDK_DLL_API void nim_session_update_stick_top(const char* session_id, enum NIMSessionType to_type,const char* ext, nim_session_update_stick_top_cb_func cb, const void* user_data);

/** @fn void nim_session_reg_set_to_stick_top_notify_cb(nim_session_set_to_stick_top_notify_cb_func cb, const void* user_data)
  * (全局回调)注册会话置顶通知回调
  * @param[in] cb			会话置顶通知回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_reg_set_to_stick_top_notify_cb(nim_session_set_to_stick_top_notify_cb_func cb, const void* user_data);

/** @fn void nim_session_reg_cancell_stick_top_notify_cb(nim_session_cancel_stick_top_notify_cb_func cb, const void* user_data)
  * (全局回调)注册取消置顶通知回调
  * @param[in] cb	取消会话置顶通知回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_reg_cancell_stick_top_notify_cb(nim_session_cancel_stick_top_notify_cb_func cb, const void* user_data);

/** @fn void void nim_session_reg_update_stick_top_notify_cb(nim_session_update_stick_top_notify_cb_func cb, const void* user_data)
  * (全局回调)注册更新置顶通知回调
  * @param[in] cb		更新会话置顶通知回调函数， nim_session_change_cb_func回调函数定义见nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_reg_update_stick_top_notify_cb(nim_session_update_stick_top_notify_cb_func cb, const void* user_data);

/** @fn void nim_session_query_session_hasmore_roammsg(const SessionMainTagInfo* session_main_tag_info, nim_session_query_session_hasmore_roammsg_cb_func cb, const void* user_data)
  * 查询会话是漫游消息未拉取信息
  * @param[in] session_main_tag_info 要查询会话信息
  * @param[in] cb	结果回调，详见 nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_session_hasmore_roammsg(const SessionMainTagInfo* session_main_tag_info, nim_session_query_session_hasmore_roammsg_cb_func cb, const void* user_data);

/** @fn void nim_session_query_all_session_hasmore_roammsg(nim_session_query_all_session_hasmore_roammsg_cb_func cb, const void* user_data)
  * 查询所有漫游消息未拉取完全的会话
  * @param[in] cb	结果回调，详见 nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_query_all_session_hasmore_roammsg(nim_session_query_all_session_hasmore_roammsg_cb_func cb, const void* user_data);

/** @fn void nim_session_update_session_hasmore_roammsg(const char *json_msg, nim_session_update_session_hasmore_roammsg_cb_func cb, const void* user_data)
  * 更新会话漫游消息未拉取信息
  * @param[in] json_msg 对应的消息内容
  * @param[in] cb	结果回调，详见 nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_update_session_hasmore_roammsg(const char *json_msg, nim_session_update_session_hasmore_roammsg_cb_func cb, const void* user_data);

/** @fn void nim_session_update_session_hasmore_roammsg(const char *json_msg, nim_session_update_session_hasmore_roammsg_cb_func cb, const void* user_data)
  * 删除会话漫游消息未拉取信息
  * @param[in] session_main_tag_info 要删除的会话信息
  * @param[in] cb	结果回调，详见 nim_session_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_session_delete_session_hasmore_roammsg(const SessionMainTagInfo* session_main_tag_info, nim_session_delete_session_hasmore_roammsg_cb_func cb, const void* user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_SESSION_H_

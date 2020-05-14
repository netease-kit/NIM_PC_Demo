/** @file nim_talkex_def_pin_message.h
  * @brief PIN Message接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/19
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_PIN_MESSAGE_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_PIN_MESSAGE_H_

#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_talkex_pin_message_add_pin_message_cb_func)(int code,const char *session,int to_type, const char *pin_info, const void *user_data)
* 添加Pin Message回调函数定义
* @param[out] code	执行结果
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] pin_info	Pin Message 内容
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_add_pin_message_cb_func)(int code,const char *session,int to_type, const char *pin_info, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_unpin_message_cb_func)(int code, const char *session, int to_type, const char *id, const void *user_data)
* 添加Pin Message回调函数定义
* @param[out] code	执行结果
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] id	Pin Message id
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_unpin_message_cb_func)(int code, const char *session, int to_type, const char *id, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_update_pin_message_cb_func)(int code, const char *session, int to_type, const char *pin_info, const void *user_data)
* 更新 Pin Message ext字段回调函数定义
* @param[out] code	执行结果
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] pin_info	Pin Message 内容
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_update_pin_message_cb_func)(int code, const char *session, int to_type, const char *pin_info, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_query_all_pin_message_cb_func)(int code, const char *session, int to_type, const char *pin_list, const void *user_data)
* 查询会话的所有 Pin Message回调函数定义
* @param[out] code	执行结果
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] pin_list	Pin Message list
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_query_all_pin_message_cb_func)(int code, const char *session, int to_type, const char *pin_list, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_add_pin_message_notify_cb_func)(const char *session, int to_type, const char *pin_info, const void *user_data)
* 添加Pin Message通知回调函数定义
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] pin_info	Pin Message 内容
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_add_pin_message_notify_cb_func)(const char *session, int to_type, const char *pin_info, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_unpin_message_notify_cb_func)(const char *session, int to_type, const char *id, const void *user_data)
* 添加UnPin Message通知回调函数定义
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] id	Pin Message id
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_unpin_message_notify_cb_func)(const char *session, int to_type, const char *id, const void *user_data);

/** @typedef void (*nim_talkex_pin_message_update_pin_message_notify_cb_func)(const char *session, int to_type, const char *pin_info, const void *user_data)
* 添加 更新 Pin Message通知回调函数定义
* @param[out] session	消息所属会话
* @param[out] to_type	 会话类型
* @param[out] pin_info	Pin Message 内容
* @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void (*nim_talkex_pin_message_update_pin_message_notify_cb_func)(const char *session, int to_type, const char *pin_info, const void *user_data);

/** @name Pin Message 属性Json Keys
  * @{
  */
static const char* kNIMPinMessageInfoKeyID = "id"; /**< string pin消息的ID 客户端生成服务端没有*/
static const char* kNIMPinMessageInfoKeySessionID = "session_id"; /**< string pin消息对应的会话ID 客户端数据*/
static const char* kNIMPinMessageInfoKeyServerID = "server_id"; /**< uint64_t 被 pin 消息的服务端ID*/
static const char* kNIMPinMessageInfoKeyClientID = "client_id";/**< string 被 pin 消息的客户商dID*/
static const char* kNIMPinMessageInfoKeyToType = "to_type";/**< int 被 pin 消息的会话类型*/
static const char* kNIMPinMessageInfoKeyFromAccID = "from_account";/**< string 被 pin 消息的发送方ID*/
static const char* kNIMPinMessageInfoKeyToAccID = "to_account";/**< string 被 pin 消息的接收方ID*/
static const char* kNIMPinMessageInfoKeyMSGTime = "message_time";/**< uint64_t 被 pin 消息的时间戳*/
static const char* kNIMPinMessageInfoKeyOPAccID = "operator_account";/**< string pin 消息的操作ID*/
static const char* kNIMPinMessageInfoKeyExt = "ext";/**< pin string 消息的扩展字段*/
static const char* kNIMPinMessageInfoKeyCreateTime = "create_time";/**< uint64_t pin 消息的创建时间戳*/
static const char* kNIMPinMessageInfoKeyUpdateTime = "update_time";/**< uint64_t pin 消息的更新时间戳*/
/** @}*/ //Pin Message 属性Json Keys

/** @name UnPin 参数定义Json Keys
  * @{
  */
static const char* kNIMPinMessageModifyParamKeySession = "session";/**< UnPin 消息所属会话*/
static const char* kNIMPinMessageModifyParamKeyToType = "to_type";/**< UnPin 消息所属会话类型*/
static const char* kNIMPinMessageModifyParamKeyID = "id";/**< Pin Message 的ID 客户端生成服务端没有*/
static const char* kNIMPinMessageModifyParamKeyExt = "ext";/**< 扩展字段*/
/** @}*/ //UnPin 参数定义Json Keys

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_PIN_MESSAGE_H_
/** @file nim_talk.h
  * @brief NIM SDK提供的talk接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_TALK_H_
#define NIM_SDK_DLL_API_NIM_TALK_H_

#include "nim_sdk_dll.h"
#include "nim_talk_def.h"
#include "nim_nos_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_talk_send_msg(const char *json_msg, const char *json_extension, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data)
  * 发送消息
  * @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] prg_cb		上传进度的回调函数, 如果发送的消息里包含了文件资源，则通过此回调函数通知上传进度, nim_sysmsg_receive_cb_func回调函数定义见nim_nos_def.h
  * @param[in] prg_user_data APP的自定义用户数据，SDK只负责传回给回调函数prg_cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_talk_send_msg(const char *json_msg, const char *json_extension, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);

/** @fn void nim_talk_stop_send_msg(const char *json_msg, const char *json_extension)
  * 停止正在发送中的消息（目前只支持发送文件消息时的终止）
  * @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_talk_stop_send_msg(const char *json_msg, const char *json_extension);

/** @fn void nim_talk_reg_ack_cb(const char *json_extension, nim_talk_ack_cb_func cb, const void *user_data)
  * 注册发送消息结果回调函数 （必须全局注册，统一接受回调后分发消息到具体的会话。注意：客户端发包之后，服务器不一定会返回！）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		发送消息的回调函数, nim_talk_ack_cb_func回调函数定义见nim_talk_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_talk_reg_ack_cb(const char *json_extension, nim_talk_ack_cb_func cb, const void *user_data);

/** @fn void nim_talk_reg_receive_cb(const char *json_extension, nim_talk_receive_cb_func cb, const void *user_data)
  * 注册接收消息回调 （建议全局注册，统一接受回调后分发消息到具体的会话）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		接收消息的回调函数, nim_talk_receive_cb_func回调函数定义见nim_talk_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_talk_reg_receive_cb(const char *json_extension, nim_talk_receive_cb_func cb, const void *user_data);

/** @fn void nim_talk_reg_receive_msgs_cb(const char *json_extension, nim_talk_receive_cb_func cb, const void *user_data)
  * 注册接收批量消息回调 （如果在注册了接收消息回调的同时也注册了该批量接口，当有批量消息时，会改走这个接口通知应用层，例如登录后接收到的离线消息等）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb		接收消息的回调函数, nim_talk_receive_cb_func回调函数定义见nim_talk_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_talk_reg_receive_msgs_cb(const char *json_extension, nim_talk_receive_cb_func cb, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_TALK_H_
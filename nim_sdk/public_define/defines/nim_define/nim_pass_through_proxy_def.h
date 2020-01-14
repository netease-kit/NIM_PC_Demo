/** @file nim_pass_through_proxy_def.h
  * @brief 透传服务代理相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Dylan
  * @date 2019/12/20
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_PASS_THROUGH_PROXY_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_PASS_THROUGH_PROXY_DEF_H_

#include "nim_global_def.h"
#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif

enum NIMSendHttpRequestMethods
{
	kNIMHttpGet = 1,
	kNIMHttpPost,
	kNIMHttpPut,
	kNIMHttpDelete
};

/** @typedef void (*nim_send_http_request_cb_func)(int res_code, const char *header, const char *body, const char *json_extension, const void *user_data)
 * 透传HTTP 请求发送回调
 * @param[out] res_code			结果代码，一切正常200
 * @param[out] header			返回的 header 内容 json 格式数据
 * @param[out] body				返回的 body 内容 json 格式数据
 * @param[out] json_extension	json扩展数据（备用）
 * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
 * @return void 无返回值
 */
typedef void(*nim_send_http_request_cb_func)(int res_code, const char *header, const char *body, const char *json_extension, const void *user_data);

/** @typedef void(*nim_received_http_msg_cb_func)(const char *from_accid, const char *body, uint64_t timestamp, const void *user_data)
 * 透传HTTP请求接收回调
 * @param[out] from_accid		发送方accid
 * @param[out] body				透传内容
 * @param[out] timestamp		发送时间, 毫秒
 * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
 * @return void 无返回值
 */
typedef void(*nim_received_http_msg_cb_func)(const char *from_accid, const char *body, uint64_t timestamp, const void *user_data);
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_PASS_THROUGH_PROXY_DEF_H_
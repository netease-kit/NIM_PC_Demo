/** @file nim_pass_through_proxy.h
  * @brief 透传服务代理 接口头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Dylan
  * @date 2019/12/20
  */
#ifndef NIM_SDK_DLL_API_NIM_PASS_THROUGH_PROXY_H_
#define NIM_SDK_DLL_API_NIM_PASS_THROUGH_PROXY_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/defines/nim_define/nim_pass_through_proxy_def.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn void nim_reg_received_http_msg_cb(nim_received_http_msg_cb_func cb, const char* json_extension, const void* user_data)
 * 注册收到 HTTP 透传消息的回调函数
 * @param[in] cb				URL 中除了 Host 的 Path
 * @param[in] json_extension	拓展参数，暂时无用
 * @param[in] user_data			用户自定义数据，SDK 只负责透传
 * @return void 无返回值
 */
NIM_SDK_DLL_API	void nim_reg_received_http_msg_cb(nim_received_http_msg_cb_func cb,
	const char* json_extension, const void* user_data);

/** @fn void nim_pass_through_proxy_send_http_request(const char* host, const char* path, NIMSendHttpRequestMethods method, const char* header, const char* body, const char* json_extension, nim_send_http_request_cb_func user_callback,const void* user_data)
  * 发送一个透传 HTTP 请求
  * @param[in] host				要发送的 HTTP 请求 Host 地址
  * @param[in] path				URL 中除了 Host 的 Path
  * @param[in] method			要发送的 HTTP 请求方式
  * @param[in] header			要发送的 HTTP 请求 Header 内容
  * @param[in] body				要发送的 HTTP 请求 Body 内容
  * @param[in] json_extension	拓展字段，暂时无用
  * @param[in] user_callback	发送请求后的回调信息，见 nim_pass_through_proxy_def.h
  * @param[in] user_data		用户自定义数据，SDK 只负责透传
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_pass_through_proxy_send_http_request(const char* host, 
	const char* path, 
	NIMSendHttpRequestMethods method, 
	const char* header, 
	const char* body, 
	const char* json_extension, 
	nim_send_http_request_cb_func user_callback,
	const void* user_data);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //NIM_SDK_DLL_API_NIM_PASS_THROUGH_PROXY_H_
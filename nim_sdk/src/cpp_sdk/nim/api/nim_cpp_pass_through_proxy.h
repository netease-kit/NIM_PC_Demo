/** @file nim_cpp_pass_through_proxy.h
  * @brief 透传服务代理
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Dylan
  * @date 2019/12/20
  */
#ifndef _NIM_SDK_CPP_PASS_THROUGH_PROXY_H_
#define _NIM_SDK_CPP_PASS_THROUGH_PROXY_H_

#include <functional>
#include <string>
#include "public_define/nim_util_include.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
#include "public_define/defines/nim_define/nim_pass_through_proxy_def.h"
#include <map>
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

class NIM_SDK_CPPWRAPPER_DLL_API PassThroughProxy
{
public:
	typedef std::function<void(int res_code, const std::string& header, const std::string& body, const std::string& json_extension)> SendHttpRequestCallback;
	typedef std::function<void(const std::string& from_accid, const std::string& body, uint64_t timestamp)> ReceivedHttpMsgCb;

public:
	/** @fn void RegReceivedHttpMsgCb(const ReceivedHttpMsgCb& cb, const std::string& json_extension)
	 * 注册接受到 HTTP 透传消息的回调函数
	 * @param[in] cb				要注册的回调函数
	 * @param[in] json_extension	拓展字段，暂时无用
	 */
	static void RegReceivedHttpMsgCb(const ReceivedHttpMsgCb& cb, const std::string& json_extension);

	/** @fn void SendHttpRequest(const std::string& host, const std::string& path, NIMSendHttpRequestMethods method, const std::string& headers, const std::string& body, const std::string& json_extension, SendHttpRequestCallback callback)
	 * 发送一个透传 HTTP 请求
	 * @param[in] host		要发送的 HTTP 请求 Host 地址
	 * @param[in] path		URL 中除了 Host 的 Path
	 * @param[in] method	要发送的 HTTP 请求方式
	 * @param[in] header	要发送的 HTTP 请求 Header 内容
	 * @param[in] body		要发送的 HTTP 请求 Body 内容
	 * @param[in] callback	发送请求后的数据回调函数
	 * @return void 无返回值	
	 */
	static void SendHttpRequest(const std::string& host,
		const std::string& path,
		NIMSendHttpRequestMethods method,
		const std::string& headers,
		const std::string& body,
		const std::string& json_extension,
		SendHttpRequestCallback callback);
};

} 

#endif //_NIM_SDK_CPP_PASS_THROUGH_PROXY_H_

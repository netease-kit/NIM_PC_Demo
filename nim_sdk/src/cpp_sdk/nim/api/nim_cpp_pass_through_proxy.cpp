/** @file nim_cpp_global.cpp
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "src/cpp_sdk/nim/api/nim_cpp_pass_through_proxy.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "public_define/defines/nim_define/nim_pass_through_proxy_def.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_reg_received_http_msg_cb)(nim_received_http_msg_cb_func cb, const char *json_extension, const void *user_data);
typedef	void(*nim_pass_through_proxy_send_http_request)(const char* host, const char* path,	NIMSendHttpRequestMethods method, const char* header, const char* body, const char* json_extension, nim_send_http_request_cb_func user_callback, const void* user_data);
#else
#include "nim_pass_through_proxy.h"
#endif

static void CallbackReceivedHttpMsg(const char* from_accid, const char* body, uint64_t timestamp, const void *user_data)
{
	CallbackProxy::DoSafeCallback<PassThroughProxy::ReceivedHttpMsgCb>(user_data, [=](const PassThroughProxy::ReceivedHttpMsgCb& cb) {
		CallbackProxy::Invoke(cb, PCharToString(from_accid), PCharToString(body), timestamp);
	}, true);
}

static void CallbackSendHttpRequest(int res_code, const char *headers, const char *body, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<PassThroughProxy::SendHttpRequestCallback>(user_data, [=](const PassThroughProxy::SendHttpRequestCallback& cb) {
		CallbackProxy::Invoke(cb, res_code, PCharToString(headers), PCharToString(body), PCharToString(json_extension));
	}, true);
}

static PassThroughProxy::ReceivedHttpMsgCb g_cb_received_http_msg_ = nullptr;
void PassThroughProxy::RegReceivedHttpMsgCb(const ReceivedHttpMsgCb& cb, const std::string& json_extension)
{
	g_cb_received_http_msg_ = cb;
	NIM_SDK_GET_FUNC(nim_reg_received_http_msg_cb)(&CallbackReceivedHttpMsg, json_extension.c_str(), &g_cb_received_http_msg_);
}

void PassThroughProxy::SendHttpRequest(const std::string& host,
	const std::string& path,
	NIMSendHttpRequestMethods method,
	const std::string& headers,
	const std::string& body,
	const std::string& json_extension,
	SendHttpRequestCallback callback)
{
	SendHttpRequestCallback* cb_pointer = nullptr;
	if (callback)
	{
		cb_pointer = new SendHttpRequestCallback(callback);
	}

	NIM_SDK_GET_FUNC(nim_pass_through_proxy_send_http_request)(host.c_str(),
		path.c_str(),
		method,
		headers.c_str(),
		body.c_str(),
		json_extension.c_str(),
		&CallbackSendHttpRequest,
		cb_pointer);
}

}

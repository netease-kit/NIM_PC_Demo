/** @file nim_cpp_global.cpp
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "nim_cpp_global.h"
#include "nim_sdk_util.h"
#include "nim_client_def.h"
#include "nim_string_util.h"
#include "callback_proxy.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef	void (*nim_global_free_str_buf)(char *str);
typedef	void (*nim_global_free_buf)(void *data);
typedef void (*nim_global_set_proxy)(NIMProxyType, const char*, int, const char*, const char*);
typedef void (*nim_global_detect_proxy)(enum NIMProxyType type, const char *host, int port, const char *user, const char *password, nim_global_detect_proxy_cb_func cb, const void *user_data);
typedef void (*nim_global_reg_exception_report_cb)(const char *json_extension, nim_sdk_exception_cb_func cb, const void *user_data);
typedef void (*nim_global_get_sdk_cache_file_info_async)(const char *login_id, const char *file_type, int64_t end_timestamp, const char *json_extension, nim_sdk_get_cache_file_info_cb_func cb, const void *user_data);
typedef void (*nim_global_del_sdk_cache_file_async)(const char *login_id, const char *file_type, int64_t end_timestamp, const char *json_extension, nim_sdk_del_cache_file_cb_func cb, const void *user_data);
typedef void (*nim_global_sdk_feedback_async)(const char *url, const char *json_extension, nim_sdk_feedback_cb_func cb, const void *user_data);
#else
#include "nim_global.h"
#endif

void Global::FreeStrBuf(char *str)
{
	NIM_SDK_GET_FUNC(nim_global_free_str_buf)(str);
}

void Global::FreeBuf(void *data)
{
	NIM_SDK_GET_FUNC(nim_global_free_buf)(data);
}

void Global::SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
{
	NIM_SDK_GET_FUNC(nim_global_set_proxy)(type, host.c_str(), port, user.c_str(), password.c_str());
}

static void CallbackDetectProxy(bool connect, NIMProxyDetectStep step, const char *json_extention, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Global::DetectProxyCallback>(user_data, [=](const Global::DetectProxyCallback& cb){

		CallbackProxy::Invoke(cb, connect, step, PCharToString(json_extention));
	},true);
}

void Global::DetectProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password, const Global::DetectProxyCallback& callback)
{
	Global::DetectProxyCallback *cb_pointer = nullptr;
	if (callback)
	{
		cb_pointer = new Global::DetectProxyCallback(callback);
	}
	NIM_SDK_GET_FUNC(nim_global_detect_proxy)(type, host.c_str(), port, user.c_str(), password.c_str(), &CallbackDetectProxy, cb_pointer);
}

static void CallbackException(NIMSDKException exception, const char *log, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Global::ExceptionCallback>(user_data, [=](const Global::ExceptionCallback& cb){

		std::string a;
		a.append(log);
		CallbackProxy::Invoke(cb, exception, a);
	});
}

void Global::SetExceptionReportCallback(const std::string&json_extension, const ExceptionCallback& cb)
{
	ExceptionCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ExceptionCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_global_reg_exception_report_cb)(json_extension.c_str(), &CallbackException, cb_pointer);
}

static void CallbackGetCachedFileInfo(const char *info, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Global::GetCachedFileInfoCallback>(user_data, [=](const Global::GetCachedFileInfoCallback& cb){

		Global::CachedFileInfo out;
		Json::Reader reader;
		Json::Value values;
		if (reader.parse(PCharToString(info), values) && values.isObject())
		{
			out.file_type_ = values[kNIMCacheFileType].asString();
			out.file_path_ = values[kNIMCacheFilePath].asString();
			out.file_count_ = values[kNIMCacheFileCount].asInt();
			out.file_total_size_ = values[kNIMCacheFileTotalSize].asUInt64();
			CallbackProxy::Invoke(cb, kNIMResSuccess, out);
		}
	},true);
}

static void CallbackDeleteCachedFile(enum NIMResCode code, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Global::DeleteCachedFileCallback>(user_data, [=](const Global::DeleteCachedFileCallback& cb){

		CallbackProxy::Invoke(cb, code);
	});
}

void Global::GetSDKCachedFileInfoAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const GetCachedFileInfoCallback &cb)
{
	GetCachedFileInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetCachedFileInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_global_get_sdk_cache_file_info_async)(login_id.c_str(), file_type.c_str(), end_timestamp, json_extension.c_str(), &CallbackGetCachedFileInfo, cb_pointer);
}

void Global::DeleteSDKCachedFileAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const DeleteCachedFileCallback &cb)
{
	DeleteCachedFileCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteCachedFileCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_global_del_sdk_cache_file_async)(login_id.c_str(), file_type.c_str(), end_timestamp, json_extension.c_str(), &CallbackDeleteCachedFile, cb_pointer);
}

static void CallbackSDKFeedback(enum NIMResCode code, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Global::SDKFeedbackCallback>(user_data, [=](const Global::SDKFeedbackCallback& cb){

		CallbackProxy::Invoke(cb, code);
	});
}

void Global::SDKFeedbackAsync(const std::string &url, const std::string &json_extension, const SDKFeedbackCallback &cb)
{
	SDKFeedbackCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SDKFeedbackCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_global_sdk_feedback_async)(url.c_str(), json_extension.c_str(), &CallbackSDKFeedback, cb_pointer);
}
}

/** @file nim_cpp_doc_trans.h
  * @brief NIM SDK提供的文档转换相关接口，其中文档上传下载,请使用nos接口nim_nos_upload_ex/nim_nos_download/nim_nos_download_ex
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/12/15
  */

#include "nim_cpp_doc_trans.h"
#include "nim_sdk_util.h"
#include "nim_cpp_global.h"
#include "callback_proxy.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef	void (*nim_doctrans_reg_notify_cb)(const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);
typedef	void (*nim_doctrans_get_info)(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);
typedef	void (*nim_doctrans_get_info_list)(const char *id, int32_t limit, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);
typedef void (*nim_doctrans_del_info)(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);
typedef char* (*nim_doctrans_get_source_file_url)(const char *url_prefix, NIMDocTranscodingFileType file_type);
typedef char* (*nim_doctrans_get_page_url)(const char *url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num);
#else
#include "nim_doc_trans.h"
#endif

static void CallbackNotify(int32_t code, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<DocTrans::DocInfoCallback>(user_data, [=](const DocTrans::DocInfoCallback& cb){

		std::string json;
		json.append(json_extension);
		DocTransInfo info;
		ParseDocTransInfo(json, info);
		CallbackProxy::Invoke(cb, code, info);

	});
}

static void CallbackDocInfo(int32_t code, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<DocTrans::DocInfoCallback>(user_data, [=](const DocTrans::DocInfoCallback& cb){
		std::string json;
		json.append(json_extension);
		DocTransInfo info;
		ParseDocTransInfo(json, info);
		CallbackProxy::Invoke(cb, code, info);

	},true);
}

static void CallbackDocInfos(int32_t code, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<DocTrans::DocInfosCallback>(user_data, [=](const DocTrans::DocInfosCallback& cb){
		std::string json;
		json.append(json_extension);
		std::list<DocTransInfo> infos;
		int32_t count = ParseDocTransInfos(json, infos);
		CallbackProxy::Invoke(cb, code, count, infos);

	}, true);
}

static DocTrans::DocInfoCallback* g_notify_cb_pointer_ = nullptr;
void DocTrans::RegNotifyCb(const DocInfoCallback& cb)
{
	if (g_notify_cb_pointer_)
	{
		delete g_notify_cb_pointer_;
		g_notify_cb_pointer_ = nullptr;
	}
	g_notify_cb_pointer_ = new DocTrans::DocInfoCallback(cb);
	NIM_SDK_GET_FUNC(nim_doctrans_reg_notify_cb)("", &CallbackNotify, g_notify_cb_pointer_);
}

void DocTrans::GetInfo( const std::string& id, const std::string& json_extension, const DocInfoCallback& cb )
{
	DocTrans::DocInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DocTrans::DocInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_doctrans_get_info)(id.c_str(), json_extension.c_str(), &CallbackDocInfo, cb_pointer);
}

void DocTrans::GetInfoList( const std::string& id, int32_t limit, const std::string& json_extension, const DocInfosCallback& cb )
{
	DocTrans::DocInfosCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DocTrans::DocInfosCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_doctrans_get_info_list)(id.c_str(), limit, json_extension.c_str(), &CallbackDocInfos, cb_pointer);
}

void DocTrans::DeleteInfo( const std::string& id, const std::string& json_extension, const DocInfoCallback& cb )
{
	DocTrans::DocInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DocTrans::DocInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_doctrans_del_info)(id.c_str(), json_extension.c_str(), &CallbackDocInfo, cb_pointer);
}

std::string DocTrans::GetSourceFileUrl( const std::string& url_prefix, NIMDocTranscodingFileType file_type )
{
	const char *url = NIM_SDK_GET_FUNC(nim_doctrans_get_source_file_url)(url_prefix.c_str(), file_type);
	std::string out_url = url;
	Global::FreeBuf((void *)url);
	return out_url;
}

std::string DocTrans::GetPageUrl( const std::string& url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num )
{
	const char *url = NIM_SDK_GET_FUNC(nim_doctrans_get_page_url)(url_prefix.c_str(), img_type, quality, page_num);
	std::string out_url = url;
	Global::FreeBuf((void *)url);
	return out_url;
}

}
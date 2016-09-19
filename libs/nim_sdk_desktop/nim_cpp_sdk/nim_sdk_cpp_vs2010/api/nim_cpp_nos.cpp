/** @file nim_cpp_nos.cpp
  * @brief NIM SDK提供的NOS云存储服务接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#include "nim_cpp_nos.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"
#include "nim_cpp_talk.h"

namespace nim
{

typedef void(*nim_nos_reg_download_cb)(nim_nos_download_cb_func cb, const void *user_data);
typedef void(*nim_nos_download_media)(const char *json_msg, nim_nos_download_cb_func callback_result, const void *download_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_stop_download_media)(const char *json_msg);
typedef void(*nim_nos_upload)(const char *local_file, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_download)(const char *nos_url, nim_nos_download_cb_func callback_result, const void *res_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);

struct UploadCallbackUserData
{
	UploadCallbackUserData() :	callback_result(),	callback_progress_pointer(nullptr) {}
	NOS::UploadMediaCallback callback_result; 	/**< 上传结果回调 */
	NOS::ProgressCallback* callback_progress_pointer; /**< 过程回调 */
};

struct DownloadCallbackUserData
{
	DownloadCallbackUserData() : callback_result(),	callback_progress_pointer(nullptr) {}
	NOS::DownloadMediaCallback callback_result;	/**< 下载结果回调 */
	NOS::ProgressCallback* callback_progress_pointer; /**< 过程回调 */
};

static void CallbackUpload(int res_code, const char *url, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		NOS::UploadMediaCallback cb = ((UploadCallbackUserData*)user_data)->callback_result;
		if (cb)
		{
			cb((NIMResCode)res_code, PCharToString(url));
		}
		delete ((UploadCallbackUserData*)user_data)->callback_progress_pointer;
		delete user_data;
	}
}

static void CallbackDownload(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		NOS::DownloadMediaCallback cb = ((DownloadCallbackUserData*)user_data)->callback_result;
		if (cb)
		{
			cb((NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id));
		}
		delete ((DownloadCallbackUserData*)user_data)->callback_progress_pointer;
		delete user_data;
	}
}

static void CallbackProgress(__int64 completed_size, __int64 total_size, const char *json_extension, const void *callback)
{
	if (callback)
	{
		NOS::ProgressCallback* cb = (NOS::ProgressCallback*)callback;
		if (*cb)
		{
			(*cb)(completed_size, total_size);
		}
	}
}

static void CallbackMediaDownloadResult(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		NOS::DownloadMediaCallback* cb = (NOS::DownloadMediaCallback*)user_data;
		if (*cb)
		{
			(*cb)((NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id));
		}
	}
}

static NOS::DownloadMediaCallback* g_cb_pointer = nullptr;
void NOS::RegDownloadCb(const DownloadMediaCallback& cb)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new DownloadMediaCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_nos_reg_download_cb)(&CallbackMediaDownloadResult, g_cb_pointer);
}

bool NOS::FetchMedia(const IMMessage& msg, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress)
{
	ProgressCallback* callback_progress_pt = nullptr;
	if (callback_progress)
		callback_progress_pt = new ProgressCallback(callback_progress);

	DownloadCallbackUserData *user_data = nullptr;
	if (callback_result)
	{
		user_data = new DownloadCallbackUserData();
		user_data->callback_result = callback_result;
		user_data->callback_progress_pointer = callback_progress_pt;
	}
	
	NIM_SDK_GET_FUNC(nim_nos_download_media)(msg.ToJsonString(false).c_str(), &CallbackDownload, user_data, &CallbackProgress, callback_progress_pt);

	return true;
}

bool NOS::StopFetchMedia(const IMMessage& msg)
{
	if (msg.client_msg_id_.empty() || msg.local_res_id_.empty())
		return false;

	NIM_SDK_GET_FUNC(nim_nos_stop_download_media)(msg.ToJsonString(false).c_str());

	return true;
}

bool NOS::UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress)
{
	if (local_file.empty())
		return false;

	ProgressCallback* callback_progress_pointer = nullptr;
	if (callback_progress)
	{
		callback_progress_pointer = new ProgressCallback(callback_progress);
	}

	UploadCallbackUserData* callback_result_userdata = nullptr;
	if (callback_result)
	{
		callback_result_userdata = new UploadCallbackUserData();
		callback_result_userdata->callback_result = callback_result;
		callback_result_userdata->callback_progress_pointer = callback_progress_pointer;
	}
	NIM_SDK_GET_FUNC(nim_nos_upload)(local_file.c_str(), &CallbackUpload, callback_result_userdata, &CallbackProgress, callback_progress_pointer);

	return true;
}

bool NOS::DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress)
{
	if (nos_url.empty())
		return false;

	ProgressCallback* callback_progress_pointer = nullptr;
	if (callback_progress)
		callback_progress_pointer = new ProgressCallback(callback_progress);

	DownloadCallbackUserData* callback_result_userdata = nullptr;
	if (callback_result)
	{
		callback_result_userdata = new DownloadCallbackUserData();
		callback_result_userdata->callback_result = callback_result;
		callback_result_userdata->callback_progress_pointer = callback_progress_pointer;
	}

	NIM_SDK_GET_FUNC(nim_nos_download)(nos_url.c_str(), &CallbackDownload, callback_result_userdata, &CallbackProgress, callback_progress_pointer);

	return true;
}

}
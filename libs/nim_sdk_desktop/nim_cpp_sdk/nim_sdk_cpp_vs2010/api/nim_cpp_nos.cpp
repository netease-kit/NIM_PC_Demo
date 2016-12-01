/** @file nim_cpp_nos.cpp
  * @brief NIM SDK提供的NOS云存储服务接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/2/1
  */

#include "nim_cpp_nos.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"
#include "nim_cpp_talk.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_nos_reg_download_cb)(nim_nos_download_cb_func cb, const void *user_data);
typedef void(*nim_nos_download_media)(const char *json_msg, nim_nos_download_cb_func callback_result, const void *download_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_download_media_ex)(const char *json_msg, const char *json_extension, nim_nos_download_cb_func callback_result, const void *download_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_download_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_download_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_stop_download_media)(const char *json_msg);
typedef void(*nim_nos_upload)(const char *local_file, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_upload_ex)(const char *local_file, const char *json_extension, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_upload_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_upload_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_download)(const char *nos_url, nim_nos_download_cb_func callback_result, const void *res_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_download_ex)(const char *nos_url, const char *json_extension, nim_nos_download_cb_func callback_result, const void *res_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_download_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_download_info_cb_func info_cb, const void *info_user_data);
#else
#include "nim_nos.h"
#endif
struct UploadCallbackUserData
{
	UploadCallbackUserData() :	callback_result(),	callback_progress_pointer(nullptr), callback_speed_pointer(nullptr), callback_transfer_pointer(nullptr) {}
	NOS::UploadMediaCallback callback_result; 	/**< 上传结果回调 */
	NOS::ProgressCallback* callback_progress_pointer; /**< 过程回调 */
	NOS::SpeedCallback* callback_speed_pointer; /**< 速度回调 */
	NOS::TransferInfoCallback* callback_transfer_pointer; /**< 最终上传信息回调 */
};

struct DownloadCallbackUserData
{
	DownloadCallbackUserData() : callback_result(),	callback_progress_pointer(nullptr), callback_speed_pointer(nullptr), callback_transfer_pointer(nullptr) {}
	NOS::DownloadMediaCallback callback_result;	/**< 下载结果回调 */
	NOS::ProgressCallback* callback_progress_pointer; /**< 过程回调 */
	NOS::SpeedCallback* callback_speed_pointer; /**< 速度回调 */
	NOS::TransferInfoCallback* callback_transfer_pointer; /**< 最终下载信息回调 */
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
		delete ((UploadCallbackUserData*)user_data)->callback_speed_pointer;
		delete ((UploadCallbackUserData*)user_data)->callback_transfer_pointer;
		delete (UploadCallbackUserData*)user_data;
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
		delete ((DownloadCallbackUserData*)user_data)->callback_speed_pointer;
		delete ((DownloadCallbackUserData*)user_data)->callback_transfer_pointer;
		delete (DownloadCallbackUserData*)user_data;
	}
}

static void CallbackProgress(int64_t completed_size, int64_t total_size, const char *json_extension, const void *callback)
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

static void CallbackSpeed(int64_t speed, const char *json_extension, const void *callback)
{
	if (callback)
	{
		NOS::SpeedCallback* cb = (NOS::SpeedCallback*)callback;
		if (*cb)
		{
			(*cb)(speed);
		}
	}
}

static void CallbackTransferInfo(int64_t actual_size, int64_t speed, const char *json_extension, const void *callback)
{
	if (callback)
	{
		NOS::TransferInfoCallback* cb = (NOS::TransferInfoCallback*)callback;
		if (*cb)
		{
			(*cb)(actual_size, speed);
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

bool NOS::FetchMediaEx(const IMMessage& msg, const std::string& json_extension, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress, const SpeedCallback& callback_speed /*= SpeedCallback()*/, const TransferInfoCallback& callback_transfer /*= TransferInfoCallback()*/)
{
	ProgressCallback* callback_progress_pt = nullptr;
	if (callback_progress)
		callback_progress_pt = new ProgressCallback(callback_progress);

	SpeedCallback* callback_speed_pointer = nullptr;
	if (callback_speed)
		callback_speed_pointer = new SpeedCallback(callback_speed);

	TransferInfoCallback* callback_transfer_pointer = nullptr;
	if (callback_transfer)
		callback_transfer_pointer = new TransferInfoCallback(callback_transfer);

	DownloadCallbackUserData *user_data = nullptr;
	if (callback_result)
	{
		user_data = new DownloadCallbackUserData();
		user_data->callback_result = callback_result;
		user_data->callback_progress_pointer = callback_progress_pt;
		user_data->callback_speed_pointer = callback_speed_pointer;
		user_data->callback_transfer_pointer = callback_transfer_pointer;
	}

	NIM_SDK_GET_FUNC(nim_nos_download_media_ex)(msg.ToJsonString(false).c_str(), json_extension.c_str(), &CallbackDownload, user_data, &CallbackProgress, callback_progress_pt, &CallbackSpeed, callback_speed_pointer, &CallbackTransferInfo, callback_transfer_pointer);

	return true;
}

bool NOS::StopFetchMedia(const IMMessage& msg)
{
	if (msg.client_msg_id_.empty() || msg.local_res_id_.empty())
		return false;

	NIM_SDK_GET_FUNC(nim_nos_stop_download_media)(msg.ToJsonString(false).c_str());

	return true;
}

bool NOS::UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress /*= ProgressCallback()*/)
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

bool NOS::UploadResourceEx(const std::string& local_file, const std::string& json_extension, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress, const SpeedCallback& callback_speed, const TransferInfoCallback& callback_transfer)
{
	if (local_file.empty())
		return false;

	ProgressCallback* callback_progress_pointer = nullptr;
	if (callback_progress)
	{
		callback_progress_pointer = new ProgressCallback(callback_progress);
	}

	SpeedCallback* callback_speed_pointer = nullptr;
	if (callback_speed)
		callback_speed_pointer = new SpeedCallback(callback_speed);

	TransferInfoCallback* callback_transfer_pointer = nullptr;
	if (callback_transfer)
		callback_transfer_pointer = new TransferInfoCallback(callback_transfer);

	UploadCallbackUserData* callback_result_userdata = nullptr;
	if (callback_result)
	{
		callback_result_userdata = new UploadCallbackUserData();
		callback_result_userdata->callback_result = callback_result;
		callback_result_userdata->callback_progress_pointer = callback_progress_pointer;
		callback_result_userdata->callback_speed_pointer = callback_speed_pointer;
		callback_result_userdata->callback_transfer_pointer = callback_transfer_pointer;
	}
	NIM_SDK_GET_FUNC(nim_nos_upload_ex)(local_file.c_str(), json_extension.c_str(), &CallbackUpload, callback_result_userdata, &CallbackProgress, callback_progress_pointer, &CallbackSpeed, callback_speed_pointer, &CallbackTransferInfo, callback_transfer_pointer);

	return true;
}

bool NOS::DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress /*= ProgressCallback()*/)
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

bool NOS::DownloadResourceEx(const std::string& nos_url, const std::string& json_extension, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress, const SpeedCallback& callback_speed, const TransferInfoCallback& callback_transfer)
{
	if (nos_url.empty())
		return false;

	ProgressCallback* callback_progress_pointer = nullptr;
	if (callback_progress)
		callback_progress_pointer = new ProgressCallback(callback_progress);

	SpeedCallback* callback_speed_pointer = nullptr;
	if (callback_speed)
		callback_speed_pointer = new SpeedCallback(callback_speed);

	TransferInfoCallback* callback_transfer_pointer = nullptr;
	if (callback_transfer)
		callback_transfer_pointer = new TransferInfoCallback(callback_transfer);
	
	DownloadCallbackUserData* callback_result_userdata = nullptr;
	if (callback_result)
	{
		callback_result_userdata = new DownloadCallbackUserData();
		callback_result_userdata->callback_result = callback_result;
		callback_result_userdata->callback_progress_pointer = callback_progress_pointer;
		callback_result_userdata->callback_speed_pointer = callback_speed_pointer;
		callback_result_userdata->callback_transfer_pointer = callback_transfer_pointer;
	}

	NIM_SDK_GET_FUNC(nim_nos_download_ex)(nos_url.c_str(), json_extension.c_str(), &CallbackDownload, callback_result_userdata, &CallbackProgress, callback_progress_pointer, &CallbackSpeed, callback_speed_pointer, &CallbackTransferInfo, callback_transfer_pointer);

	return true;
}

}
/** @file nim_cpp_nos.cpp
  * @brief NIM SDK提供的NOS云存储服务接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "nim_cpp_nos.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"
#include "nim_cpp_talk.h"
#include "callback_proxy.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_nos_init_config)(const char* json_tags, nim_nos_init_config_cb_func cb, const char *json_extension, const void *user_data);
typedef void(*nim_nos_reg_download_cb)(nim_nos_download_cb_func cb, const void *user_data);
typedef void(*nim_nos_reg_upload_cb)(nim_nos_upload_cb_func cb, const void *user_data);
typedef void(*nim_nos_download_media)(const char *json_msg, nim_nos_download_cb_func callback_result, const void *download_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_download_media_ex)(const char *json_msg, const char *json_extension, nim_nos_download_cb_func callback_result, const void *download_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_download_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_download_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_stop_download_media)(const char *json_msg);
typedef void(*nim_nos_upload)(const char *local_file, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_upload_ex)(const char *local_file, const char *json_extension, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_upload_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_upload_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_upload2)(const char *local_file, const char *tag, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_upload_ex2)(const char *local_file, const char *tag, const char *json_extension, nim_nos_upload_cb_func callback_result, const void *res_user_data, nim_nos_upload_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_upload_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_upload_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_stop_upload_ex)(const char *task_id, const char *json_extension);
typedef void(*nim_nos_download)(const char *nos_url, nim_nos_download_cb_func callback_result, const void *res_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_nos_download_ex)(const char *nos_url, const char *json_extension, nim_nos_download_cb_func callback_result, const void *res_user_data, nim_nos_download_prg_cb_func prg_cb, const void *prg_user_data, nim_nos_download_speed_cb_func speed_cb, const void *speed_user_data, nim_nos_download_info_cb_func info_cb, const void *info_user_data);
typedef void(*nim_nos_stop_download_ex)(const char *task_id, const char *json_extension);
typedef void(*nim_nos_set_quick_trans)(int quick_trans);

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

struct UploadCallbackExUserData
{
	UploadCallbackExUserData() :	callback_result(),	callback_progress_pointer(nullptr), callback_speed_pointer(nullptr), callback_transfer_pointer(nullptr) {}
	NOS::UploadMediaExCallback callback_result; 	/**< 扩展上传结果回调 */
	NOS::ProgressExCallback* callback_progress_pointer; /**< 扩展过程回调 */
	NOS::SpeedCallback* callback_speed_pointer; /**< 速度回调 */
	NOS::TransferInfoCallback* callback_transfer_pointer; /**< 最终上传信息回调 */
};

struct DownloadCallbackExUserData
{
	DownloadCallbackExUserData() : callback_result(),	callback_progress_pointer(nullptr), callback_speed_pointer(nullptr), callback_transfer_pointer(nullptr) {}
	NOS::DownloadMediaExCallback callback_result;	/**< 扩展下载结果回调 */
	NOS::ProgressExCallback* callback_progress_pointer; /**< 扩展过程回调 */
	NOS::SpeedCallback* callback_speed_pointer; /**< 速度回调 */
	NOS::TransferInfoCallback* callback_transfer_pointer; /**< 最终下载信息回调 */
};
static void CallbackInitConfig(enum NIMNosInitConfigResultType rescode, const char* json_result, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<NOS::InitNosResultCallback>(user_data, [=](const NOS::InitNosResultCallback& cb){

		InitNosResult result;
		result.result_ = rescode;
		result.FromJsonString(json_result);
		CallbackProxy::Invoke(cb, result);
	},true);

}
static void CallbackUpload(int res_code, const char *url, const char *json_extension, const void *user_data)
{

	NOS::UploadMediaCallback cb_result = ((UploadCallbackUserData*)user_data)->callback_result;
	CallbackProxy::DoSafeCallback<NOS::UploadMediaCallback>(&cb_result, [=](const NOS::UploadMediaCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(url));
		delete ((UploadCallbackUserData*)user_data)->callback_progress_pointer;
		delete ((UploadCallbackUserData*)user_data)->callback_speed_pointer;
		delete ((UploadCallbackUserData*)user_data)->callback_transfer_pointer;
		delete (UploadCallbackUserData*)user_data;
	});
}

static void CallbackDownload(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	NOS::DownloadMediaCallback cb_result = ((DownloadCallbackUserData*)user_data)->callback_result;
	CallbackProxy::DoSafeCallback<NOS::DownloadMediaCallback>(&cb_result, [=](const NOS::DownloadMediaCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id));
		delete ((DownloadCallbackUserData*)user_data)->callback_progress_pointer;
		delete ((DownloadCallbackUserData*)user_data)->callback_speed_pointer;
		delete ((DownloadCallbackUserData*)user_data)->callback_transfer_pointer;
		delete (DownloadCallbackUserData*)user_data;
	});
}

static void CallbackProgress(int64_t completed_size, int64_t total_size, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<NOS::ProgressCallback>(callback, [=](const NOS::ProgressCallback& cb){

		CallbackProxy::Invoke(cb, completed_size, total_size);
	});
}

static void CallbackUploadEx(int res_code, const char *url, const char *json_extension, const void *user_data)
{
	NOS::UploadMediaExCallback cb_result = ((UploadCallbackExUserData*)user_data)->callback_result;
	CallbackProxy::DoSafeCallback<NOS::UploadMediaExCallback>(&cb_result, [=](const NOS::UploadMediaExCallback& cb){

		UploadMediaResult result;
		ParseUploadResult(PCharToString(url), PCharToString(json_extension), result);

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, result);
		delete ((UploadCallbackExUserData*)user_data)->callback_progress_pointer;
		delete ((UploadCallbackExUserData*)user_data)->callback_speed_pointer;
		delete ((UploadCallbackExUserData*)user_data)->callback_transfer_pointer;
		delete (UploadCallbackExUserData*)user_data;
	});
}



static void CallbackDownloadEx(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	NOS::DownloadMediaExCallback cb_result = ((DownloadCallbackExUserData*)user_data)->callback_result;
	CallbackProxy::DoSafeCallback<NOS::DownloadMediaExCallback>(&cb_result, [=](const NOS::DownloadMediaExCallback& cb){

		DownloadMediaResult result;
		ParseDownloadResult(PCharToString(file_path), PCharToString(call_id), PCharToString(res_id), result);
	
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, result);
		delete ((DownloadCallbackExUserData*)user_data)->callback_progress_pointer;
		delete ((DownloadCallbackExUserData*)user_data)->callback_speed_pointer;
		delete ((DownloadCallbackExUserData*)user_data)->callback_transfer_pointer;
		delete (DownloadCallbackExUserData*)user_data;
	});
}

static void CallbackProgressEx(int64_t completed_size, int64_t total_size, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<NOS::ProgressExCallback>(callback, [=](const NOS::ProgressExCallback& cb){

		ProgressData data;
		ParseProgressData(PCharToString(json_extension), data);
		CallbackProxy::Invoke(cb, completed_size, total_size, data);
	});
}

static void CallbackSpeed(int64_t speed, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<NOS::SpeedCallback>(callback, [=](const NOS::SpeedCallback& cb){

		CallbackProxy::Invoke(cb, speed);
	});
}

static void CallbackTransferInfo(int64_t actual_size, int64_t speed, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<NOS::TransferInfoCallback>(callback, [=](const NOS::TransferInfoCallback& cb){

		CallbackProxy::Invoke(cb, actual_size, speed);
	});
}

static void CallbackMediaDownloadResult(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<NOS::DownloadMediaCallback>(user_data, [=](const NOS::DownloadMediaCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id));
	});
}

static void CallbackMediaUploadResult(int res_code, const char *url, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<NOS::UploadMediaExCallback>(user_data, [=](const NOS::UploadMediaExCallback& cb){

		UploadMediaResult result;
		ParseUploadResult(PCharToString(url), PCharToString(json_extension), result);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, result);
	});
}
class UploadCallbackUserDataMaker
{
public:
	static UploadCallbackUserData* MakeUserData(const NOS::UploadMediaCallback& callback_result, const NOS::ProgressCallback& callback_progress)
	{
		UploadCallbackUserData* callback_result_userdata = nullptr;
		if (callback_result)
		{
			callback_result_userdata = new UploadCallbackUserData();
			callback_result_userdata->callback_result = callback_result;
			callback_result_userdata->callback_progress_pointer = (callback_progress == nullptr ? nullptr : (new NOS::ProgressCallback(callback_progress)));
		}
		return callback_result_userdata;
	}
	static UploadCallbackExUserData* MakeUserData(const NOS::UploadMediaExCallback& callback_result, const NOS::ProgressExCallback& callback_progress, \
		const NOS::SpeedCallback& callback_speed, const NOS::TransferInfoCallback& callback_transfer)
	{
		UploadCallbackExUserData* callback_result_userdata = nullptr;
		if (callback_result)
		{
			callback_result_userdata = new UploadCallbackExUserData();
			callback_result_userdata->callback_result = callback_result;
			callback_result_userdata->callback_progress_pointer = (callback_progress == nullptr ? nullptr : (new NOS::ProgressExCallback(callback_progress)));
			callback_result_userdata->callback_speed_pointer = (callback_speed == nullptr ? nullptr : (new NOS::SpeedCallback(callback_speed)));
			callback_result_userdata->callback_transfer_pointer = (callback_transfer == nullptr ? nullptr : (new NOS::TransferInfoCallback(callback_transfer)));
		}
		return callback_result_userdata;
	}
};
void NOS::InitConfig(const InitNosConfigParam& param, const InitNosResultCallback& cb)
{
	Json::Value json_tags;
	auto&& tag_list = param.GetTagList();
	if (tag_list.empty())
		return;
	auto it = tag_list.begin();
	while (it != tag_list.end())
	{
		Json::Value json_tag;
		json_tag[kNIMNosUploadTagName] = it->first;
		json_tag[kNIMNosUploadTagSurvivalTime] = it->second;
		json_tags.append(json_tag);
		it++;
	}	
	NIM_SDK_GET_FUNC(nim_nos_init_config)(Json::FastWriter().write(json_tags).c_str(), CallbackInitConfig, param.GetExtension().c_str(), (new InitNosResultCallback(cb)));
}
static NOS::DownloadMediaCallback g_cb_pointer = nullptr;
void NOS::RegDownloadCb(const DownloadMediaCallback& cb)
{
	g_cb_pointer = cb;
	NIM_SDK_GET_FUNC(nim_nos_reg_download_cb)(&CallbackMediaDownloadResult, &g_cb_pointer);
}

static NOS::UploadMediaExCallback g_cb_upload_pointer = nullptr;
void NOS::RegUploadCb(const UploadMediaExCallback& cb)
{
	g_cb_upload_pointer = cb;
	NIM_SDK_GET_FUNC(nim_nos_reg_upload_cb)(&CallbackMediaUploadResult, &g_cb_upload_pointer);
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
	if (msg.client_msg_id_.empty())
		return false;

	NIM_SDK_GET_FUNC(nim_nos_stop_download_media)(msg.ToJsonString(false).c_str());

	return true;
}
bool NOS::UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress /*= ProgressCallback()*/)
{
	if (local_file.empty())
		return false;
	auto callback_result_userdata = UploadCallbackUserDataMaker::MakeUserData(callback_result, callback_progress);
	NIM_SDK_GET_FUNC(nim_nos_upload)(local_file.c_str(), 
		&CallbackUpload, callback_result_userdata, 
		&CallbackProgress, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_progress_pointer));

	return true;
}
bool NOS::UploadResource2(const std::string& local_file, const std::string& tag, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress)
{
	if (local_file.empty())
		return false;
	auto callback_result_userdata = UploadCallbackUserDataMaker::MakeUserData(callback_result, callback_progress);
	NIM_SDK_GET_FUNC(nim_nos_upload2)(local_file.c_str(), tag.c_str(), 
		&CallbackUpload, callback_result_userdata, 
		&CallbackProgress, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_progress_pointer));

	return true;
}
bool NOS::UploadResourceEx( const std::string& local_file, const std::string& json_extension, const UploadMediaExCallback& callback_result, const ProgressExCallback& callback_progress /*= ProgressExCallback()*/, const SpeedCallback& callback_speed /*= SpeedCallback()*/, const TransferInfoCallback& callback_transfer /*= TransferInfoCallback()*/ )
{
	if (local_file.empty())
		return false;
	auto callback_result_userdata = UploadCallbackUserDataMaker::MakeUserData(callback_result, callback_progress, callback_speed, callback_transfer);	
	NIM_SDK_GET_FUNC(nim_nos_upload_ex)(local_file.c_str(), json_extension.c_str(), 
		&CallbackUploadEx, callback_result_userdata, 
		&CallbackProgressEx,( callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_progress_pointer),
		&CallbackSpeed, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_speed_pointer),
		&CallbackTransferInfo, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_transfer_pointer));
	return true;
}
bool NOS::UploadResourceEx2(const std::string& local_file, const std::string& tag, const std::string& json_extension, const UploadMediaExCallback& callback_result, const ProgressExCallback& callback_progress, const SpeedCallback& callback_speed, const TransferInfoCallback& callback_transfer)
{
	if (local_file.empty())
		return false;
	auto callback_result_userdata = UploadCallbackUserDataMaker::MakeUserData(callback_result, callback_progress, callback_speed, callback_transfer);
	NIM_SDK_GET_FUNC(nim_nos_upload_ex2)(local_file.c_str(), json_extension.c_str(), tag.c_str(),
		&CallbackUploadEx, callback_result_userdata,
		&CallbackProgressEx, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_progress_pointer),
		&CallbackSpeed, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_speed_pointer),
		&CallbackTransferInfo, (callback_result_userdata == nullptr ? nullptr : callback_result_userdata->callback_transfer_pointer));
	return true;
}
bool NOS::StopUploadResourceEx(const std::string& task_id, const std::string& json_extension/* = ""*/)
{
	NIM_SDK_GET_FUNC(nim_nos_stop_upload_ex)(task_id.c_str(), json_extension.c_str());
	return true;
}

bool NOS::DownloadResource( const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress /*= ProgressCallback()*/ )
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

bool NOS::DownloadResourceEx( const std::string& nos_url, const std::string& json_extension, const DownloadMediaExCallback& callback_result, const ProgressExCallback& callback_progress /*= ProgressExCallback()*/, const SpeedCallback& callback_speed /*= SpeedCallback()*/, const TransferInfoCallback& callback_transfer /*= TransferInfoCallback()*/ )
{
	if (nos_url.empty())
		return false;

	ProgressExCallback* callback_progress_pointer = nullptr;
	if (callback_progress)
		callback_progress_pointer = new ProgressExCallback(callback_progress);

	SpeedCallback* callback_speed_pointer = nullptr;
	if (callback_speed)
		callback_speed_pointer = new SpeedCallback(callback_speed);

	TransferInfoCallback* callback_transfer_pointer = nullptr;
	if (callback_transfer)
		callback_transfer_pointer = new TransferInfoCallback(callback_transfer);
	
	DownloadCallbackExUserData* callback_result_userdata = nullptr;
	if (callback_result)
	{
		callback_result_userdata = new DownloadCallbackExUserData();
		callback_result_userdata->callback_result = callback_result;
		callback_result_userdata->callback_progress_pointer = callback_progress_pointer;
		callback_result_userdata->callback_speed_pointer = callback_speed_pointer;
		callback_result_userdata->callback_transfer_pointer = callback_transfer_pointer;
	}

	NIM_SDK_GET_FUNC(nim_nos_download_ex)(nos_url.c_str(), json_extension.c_str(), &CallbackDownloadEx, callback_result_userdata, &CallbackProgressEx, callback_progress_pointer, &CallbackSpeed, callback_speed_pointer, &CallbackTransferInfo, callback_transfer_pointer);

	return true;
}

bool NOS::StopDownloadResourceEx(const std::string& task_id, const std::string& json_extension/* = ""*/)
{
	NIM_SDK_GET_FUNC(nim_nos_stop_download_ex)(task_id.c_str(), json_extension.c_str());
	return true;
}
void NOS::UnregNosCb()
{
	g_cb_pointer = nullptr;
	g_cb_upload_pointer = nullptr;
}

void NOS::SetSupportQuickTrans(bool bquick)
{
	NIM_SDK_GET_FUNC(nim_nos_set_quick_trans)((int)bquick);
}
}
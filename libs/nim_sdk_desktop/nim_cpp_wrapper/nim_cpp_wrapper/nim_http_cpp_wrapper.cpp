#include "nim_http_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

typedef void(*nim_http_reg_dl_cb)(nim_http_dl_cb_func cb, const void *user_data);
typedef void(*nim_http_fetch_media)(const char *json_msg, nim_http_dl_cb_func res_cb, const void *dl_user_data, nim_http_dl_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_http_stop_fetch_media)(const char *json_msg);
typedef void(*nim_http_up)(const char *local_file, nim_http_up_cb_func res_cb, const void *res_user_data, nim_http_up_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_http_dl)(const char *nos_url, nim_http_dl_cb_func res_cb, const void *res_user_data, nim_http_dl_prg_cb_func prg_cb, const void *prg_user_data);


struct UploadCallbackUserData
{
	UploadCallbackUserData() :
		response_cb(),
		progress_cb_pointer(nullptr)
	{

	}
	Http::UploadResourceCallback response_cb;
	Http::UploadResourceProgressCallback* progress_cb_pointer;
};

struct DownloadCallbackUserData
{
	DownloadCallbackUserData() :
	response_cb(),
	progress_cb_pointer(nullptr)
{

}
Http::DownloadResourceCallback response_cb;
Http::DownloadResourceProgressCallback* progress_cb_pointer;
};

struct FetchMediaCallbackUserData
{
	FetchMediaCallbackUserData() :
		response_cb(),
		progress_cb_pointer(nullptr)
	{

	}
	Http::FetchMediaCallback response_cb;
	Http::FetchMediaProgressCallback* progress_cb_pointer;
};

static void FetchMediaCallbackWrapper(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::FetchMediaCallback response_cb = ((FetchMediaCallbackUserData*)user_data)->response_cb;
		if (response_cb)
		{
			PostTaskToUIThread(std::bind(response_cb, (nim::NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id)));
		}
		delete ((FetchMediaCallbackUserData*)user_data)->progress_cb_pointer;
		delete user_data;
	}
}

static void FetchMediaProgressCallbackWrapper(__int64 completed_size, __int64 total_size, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::FetchMediaProgressCallback* cb = (Http::FetchMediaProgressCallback*)user_data;
		if (*cb)
		{
			PostTaskToUIThread(std::bind((*cb), completed_size, total_size));
		}
	}
}

static void UploadCallbackWrapper(int res_code, const char *url, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::UploadResourceCallback cb = ((UploadCallbackUserData*)user_data)->response_cb;
		if (cb)
		{
			PostTaskToUIThread(std::bind(cb, (nim::NIMResCode)res_code, PCharToString(url)));
		}
		delete ((UploadCallbackUserData*)user_data)->progress_cb_pointer;
		delete user_data;
	}
}

static void DownloadCallbackWrapper(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::DownloadResourceCallback cb = ((DownloadCallbackUserData*)user_data)->response_cb;
		if (cb)
		{
			PostTaskToUIThread(std::bind(cb, (nim::NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id)));
		}
		delete ((DownloadCallbackUserData*)user_data)->progress_cb_pointer;
		delete user_data;
	}
}

static void ProgressCallbackWrapper(__int64 completed_size, __int64 total_size, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::UploadResourceProgressCallback* cb = (Http::UploadResourceProgressCallback*)user_data;
		if (*cb)
		{
			PostTaskToUIThread(std::bind((*cb), completed_size, total_size));
		}
	}
}

static void CallbackWrapperNodelete(int res_code, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Http::DownloadCallback* cb_pointer = (Http::DownloadCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, PCharToString(file_path), PCharToString(call_id), PCharToString(res_id)));
		}
	}
}

static Http::DownloadCallback* g_cb_pointer = nullptr;
void Http::RegDownloadCb(const DownloadCallback& cb)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new DownloadCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_http_reg_dl_cb)(&CallbackWrapperNodelete, g_cb_pointer);
}

void Http::FetchMedia(const std::string& json_msg, const FetchMediaCallback& res_cb, const FetchMediaProgressCallback& prg_cb)
{
	FetchMediaProgressCallback* progress_cb_pointer = nullptr;
	if (prg_cb)
	{
		progress_cb_pointer = new FetchMediaProgressCallback(prg_cb);
	}

	FetchMediaCallbackUserData* response_cb_userdata = nullptr;
	if (res_cb)
	{
		response_cb_userdata = new FetchMediaCallbackUserData();
		response_cb_userdata->response_cb = res_cb;
		response_cb_userdata->progress_cb_pointer = progress_cb_pointer;
	}
	return NIM_SDK_Get_Function(nim_http_fetch_media)(json_msg.c_str(), &FetchMediaCallbackWrapper, response_cb_userdata, &FetchMediaProgressCallbackWrapper, progress_cb_pointer);
}
void Http::StopFetchMedia(const std::string& json_msg)
{
	NIM_SDK_Get_Function(nim_http_stop_fetch_media)(json_msg.c_str());
}

void Http::UploadResource(const std::string& local_file, const UploadResourceCallback& response_cb, const UploadResourceProgressCallback& progress_cb)
{
	UploadResourceProgressCallback* progress_cb_pointer = nullptr;
	if (progress_cb)
	{
		progress_cb_pointer = new UploadResourceProgressCallback(progress_cb);
	}

	UploadCallbackUserData* response_cb_userdata = nullptr;
	if (response_cb)
	{
		response_cb_userdata = new UploadCallbackUserData();
		response_cb_userdata->response_cb = response_cb;
		response_cb_userdata->progress_cb_pointer = progress_cb_pointer;
	}
	return NIM_SDK_Get_Function(nim_http_up)(local_file.c_str(), &UploadCallbackWrapper, response_cb_userdata, &ProgressCallbackWrapper, progress_cb_pointer);
}

void Http::DownloadResource(const std::string& nos_url, const DownloadResourceCallback& response_cb, const DownloadResourceProgressCallback& progress_cb)
{
	DownloadResourceProgressCallback* progress_cb_pointer = nullptr;
	if (progress_cb)
	{
		progress_cb_pointer = new DownloadResourceProgressCallback(progress_cb);
	}

	DownloadCallbackUserData* response_cb_userdata = nullptr;
	if (response_cb)
	{
		response_cb_userdata = new DownloadCallbackUserData();
		response_cb_userdata->response_cb = response_cb;
		response_cb_userdata->progress_cb_pointer = progress_cb_pointer;
	}

	return NIM_SDK_Get_Function(nim_http_dl)(nos_url.c_str(), &DownloadCallbackWrapper, response_cb_userdata, &ProgressCallbackWrapper, progress_cb_pointer);
}

}
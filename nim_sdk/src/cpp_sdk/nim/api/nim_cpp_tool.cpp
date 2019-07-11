/** @file nim_cpp_tool.cpp
  * @brief NIM SDK提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5等
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/21
  */

#include "src/cpp_sdk/nim/api//nim_cpp_tool.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "src/cpp_sdk/nim/api//nim_cpp_global.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef	const char * (*nim_tool_get_user_appdata_dir)(const char * app_account);
typedef	const char * (*nim_tool_get_user_specific_appdata_dir)(const char *app_account,enum NIMAppDataType appdata_type);
typedef	const char * (*nim_tool_get_local_appdata_dir)();
typedef	const char * (*nim_tool_get_cur_module_dir)();
typedef	const char * (*nim_tool_get_md5)(const char *input);
typedef	const char * (*nim_tool_get_file_md5)(const char *file_path);
typedef	const char * (*nim_tool_get_uuid)();
typedef void(*nim_tool_get_audio_text_async)(const char *json_audio_info, const char *json_extension, nim_tool_get_audio_text_cb_func cb, const void *user_data);
typedef void(*nim_tool_filter_client_antispam_async)(const char* text, const char* replace_str, const char* lib_name, const char *json_extension, nim_tool_filter_client_antispam_cb_func cb, const void *user_data);
#else
#include "nim_tools.h"
#endif

static void CallbackGetAudioText(int res_code, const char *text, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Tool::GetAudioTextCallback>(callback, [=](const Tool::GetAudioTextCallback& cb){
		CallbackProxy::Invoke(cb, res_code, PCharToString(text));
	}, true);
}

std::string Tool::GetUserAppdataDir(const std::string& app_account)
{
	const char *dir = NIM_SDK_GET_FUNC(nim_tool_get_user_appdata_dir)(app_account.c_str());
	std::string dir_str = (std::string)dir;
	Global::FreeBuf((void *)dir);
	return dir_str; 
}

std::string Tool::GetSpecificAppdataDir(const std::string app_account,enum NIMAppDataType appdata_type)
{
	const char *dir = NIM_SDK_GET_FUNC(nim_tool_get_user_specific_appdata_dir)(app_account.c_str(), appdata_type);
	std::string dir_str = (std::string)dir;
	Global::FreeBuf((void *)dir);
	return dir_str; 
}

std::string Tool::GetLocalAppdataDir()
{
	const char *dir = NIM_SDK_GET_FUNC(nim_tool_get_local_appdata_dir)();
	std::string dir_str = (std::string)dir;
	Global::FreeBuf((void *)dir);
	return dir_str;
}

std::string Tool::GetCurModuleDir()
{
	const char *dir = NIM_SDK_GET_FUNC(nim_tool_get_cur_module_dir)();
	std::string dir_str = (std::string)dir;
	Global::FreeBuf((void *)dir);
	return dir_str;
}

std::string Tool::GetMd5(const std::string& input)
{
	const char *md5 = NIM_SDK_GET_FUNC(nim_tool_get_md5)(input.c_str());
	std::string md5_str = (std::string)md5;
	Global::FreeBuf((void *)md5);
	return md5_str;
}

std::string Tool::GetFileMd5(const std::string& file_path)
{
	const char *md5 = NIM_SDK_GET_FUNC(nim_tool_get_file_md5)(file_path.c_str());
	std::string md5_str = (std::string)md5;
	Global::FreeBuf((void *)md5);
	return md5_str;
}

std::string Tool::GetUuid()
{
	const char *uuid = NIM_SDK_GET_FUNC(nim_tool_get_uuid)();
	std::string uuid_str = (std::string)uuid;
	Global::FreeBuf((void *)uuid);
	return uuid_str;
}

bool Tool::GetAudioTextAsync(const AudioInfo& audio_info, const GetAudioTextCallback& cb, const std::string& json_extension)
{
	if (audio_info.mime_type_.empty() || audio_info.samplerate_.empty() || audio_info.url_.empty() || audio_info.duration_ <= 0)
		return false;

	GetAudioTextCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetAudioTextCallback(cb);
	}

	nim_cpp_wrapper_util::Json::Value json_value;
	json_value[nim::kNIMTransAudioKeyMime] = audio_info.mime_type_;
	json_value[nim::kNIMTransAudioKeySample] = audio_info.samplerate_;
	json_value[nim::kNIMTransAudioKeyAudioUrl] = audio_info.url_;
	json_value[nim::kNIMTransAudioKeyDuration] = audio_info.duration_;
	NIM_SDK_GET_FUNC(nim_tool_get_audio_text_async)(GetJsonStringWithNoStyled(json_value).c_str(), json_extension.c_str(), &CallbackGetAudioText, cb_pointer);

	return true;
}

static void CallbackFilterClientAntispam(bool succedd, int ret, const char *text, const char *json_extension, const void *callback)
{

	CallbackProxy::DoSafeCallback<Tool::FilterClientAntispamCallback>(callback, [=](const Tool::FilterClientAntispamCallback& cb){
		CallbackProxy::Invoke(cb, succedd, ret, PCharToString(text));
	}, true);
}

void Tool::FilterClientAntispam(const std::string& text, const std::string& replace_str, const std::string& lib_name, const FilterClientAntispamCallback& callback)
{
	FilterClientAntispamCallback* cb_pointer = nullptr;
	if (callback)
	{
		cb_pointer = new FilterClientAntispamCallback(callback);
	}
	NIM_SDK_GET_FUNC(nim_tool_filter_client_antispam_async)((char *)text.c_str(), (char *)replace_str.c_str(), (char *)lib_name.c_str(), nullptr, &CallbackFilterClientAntispam, cb_pointer);
}

}
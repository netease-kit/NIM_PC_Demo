#include "nim_tool_cpp_wrapper.h"
#include "nim_sdk_function.h"


namespace nim
{

typedef	const wchar_t * (*nim_tool_get_user_appdata_dir)(const char * app_account);
typedef	const wchar_t * (*nim_tool_get_local_appdata_dir)();
typedef	const wchar_t * (*nim_tool_get_cur_module_dir)();
typedef	const char * (*nim_tool_get_md5)(const char *input);
typedef	const char * (*nim_tool_get_file_md5)(const char *file_path);
typedef	const char * (*nim_tool_get_uuid)();
typedef void(*nim_tool_get_audio_text_async)(const char *json_audio_info, const char *json_extension, nim_tool_get_audio_text_cb_func cb, const void *user_data);

static void CallbackWrapper1(int res_code, const char *text, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Tool::GetAudioTextCallback* cb_pointer = (Tool::GetAudioTextCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, PCharToString(text)));
		}
		delete cb_pointer;
	}
}

const wchar_t * Tool::GetUserAppdataDir(const std::string& app_account)
{
	return NIM_SDK_Get_Function(nim_tool_get_user_appdata_dir)(app_account.c_str());
}

const wchar_t * Tool::GetLocalAppdataDir()
{
	return NIM_SDK_Get_Function(nim_tool_get_local_appdata_dir)();
}

const wchar_t * Tool::GetCurModuleDir()
{
	return NIM_SDK_Get_Function(nim_tool_get_cur_module_dir)();
}

const char * Tool::GetMd5(const std::string& input)
{
	return NIM_SDK_Get_Function(nim_tool_get_md5)(input.c_str());
}

const char * Tool::GetFileMd5(const std::string& file_path)
{
	return NIM_SDK_Get_Function(nim_tool_get_file_md5)(file_path.c_str());
}

const char * Tool::GetUuid()
{
	return NIM_SDK_Get_Function(nim_tool_get_uuid)();
}

void Tool::GetAudioTextAsync(const AudioInfo& audio_info, const GetAudioTextCallback& cb, const std::string& json_extension)
{
	GetAudioTextCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new GetAudioTextCallback(cb);
	}

	Json::Value json_value;
	json_value[nim::kNIMTransAudioKeyMime] = audio_info.mime;
	json_value[nim::kNIMTransAudioKeySample] = audio_info.samp;
	json_value[nim::kNIMTransAudioKeyAudioUrl] = audio_info.url;
	json_value[nim::kNIMTransAudioKeyDuration] = audio_info.dur;
	NIM_SDK_Get_Function(nim_tool_get_audio_text_async)(json_value.toStyledString().c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

}
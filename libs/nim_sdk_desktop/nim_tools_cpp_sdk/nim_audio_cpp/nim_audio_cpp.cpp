/** @file nim_audio_cpp.cpp
* @brief NIM 提供的语音播放工具接口(CPP接口)
* @copyright (c) 2015-2016, NetEase Inc. All rights reserved
* @author towik, Oleg
* @date 2015/2/10
*/

#include "nim_audio_cpp.h"

namespace nim_audio
{

static const std::wstring kSdkAudioDll_d = L"nim_audio_d.dll";
static const std::wstring kSdkAudioDll = L"nim_audio.dll";

HINSTANCE Audio::instance_audio_ = NULL;

//init
typedef bool(*nim_audio_init_module)(const wchar_t* user_data_parent_path);
typedef bool(*nim_audio_uninit_module)();

//play and stop
typedef bool(*nim_audio_play_audio)(const wchar_t* file_path, const char* call_id, const char* res_id, int audio_format);
typedef bool(*nim_audio_stop_play_audio)();

//capture
typedef bool(*nim_audio_start_capture)(const char* call_id, const char* res_id, int audio_format, int volume, const wchar_t* capture_device);
typedef bool(*nim_audio_stop_capture)();
typedef bool(*nim_audio_cancel_audio)(const wchar_t* file_path);
typedef bool(*nim_audio_enum_capture_device)();

//reg callback
typedef bool(*nim_audio_reg_start_play_cb)(nim_rescode_id_cb cb);
typedef bool(*nim_audio_reg_stop_play_cb)(nim_rescode_id_cb cb);
typedef bool(*nim_audio_reg_start_capture_cb)(nim_rescode_cb cb);
typedef bool(*nim_audio_reg_stop_capture_cb)(nim_stop_capture_cb cb);
typedef bool(*nim_audio_reg_cancel_audio_cb)(nim_rescode_cb cb);
typedef bool(*nim_audio_reg_enum_capture_device_cb)(nim_enum_capture_device_cb cb);

bool Audio::Init(const std::wstring& user_data_parent_path)
{
#ifdef _DEBUG
	instance_audio_ = ::LoadLibraryW(kSdkAudioDll_d.c_str());
#else
	instance_audio_ = ::LoadLibraryW(kSdkAudioDll.c_str());
#endif
	assert(instance_audio_);
	if (instance_audio_ == NULL)
		return false;

	if (instance_audio_)
	{
		nim_audio_init_module f_init = Function<nim_audio_init_module>("nim_audio_init_module");
		bool ret = f_init(user_data_parent_path.c_str());
		return ret;
	}
	return false;
}

void Audio::Cleanup()
{
	if (instance_audio_)
	{
		nim_audio_uninit_module f_uninit = Function<nim_audio_uninit_module>("nim_audio_uninit_module");
		f_uninit();
		::FreeLibrary(instance_audio_);
		instance_audio_ = NULL;
	}
}

bool Audio::PlayAudio(const wchar_t* file_path, const char* call_id, const char* res_id, nim_audio_type audio_format)
{
	nim_audio_play_audio f_uninit = Function<nim_audio_play_audio>("nim_audio_play_audio");
	return f_uninit(file_path, call_id, res_id, audio_format);
}

bool Audio::StopPlayAudio()
{
	nim_audio_stop_play_audio f_uninit = Function<nim_audio_stop_play_audio>("nim_audio_stop_play_audio");
	return f_uninit();
}

bool Audio::RegStartPlayCb(nim_rescode_id_cb cb)
{
	nim_audio_reg_start_play_cb f_uninit = Function<nim_audio_reg_start_play_cb>("nim_audio_reg_start_play_cb");
	return f_uninit(cb);
}

bool Audio::RegStopPlayCb(nim_rescode_id_cb cb)
{
	nim_audio_reg_stop_play_cb f_uninit = Function<nim_audio_reg_stop_play_cb>("nim_audio_reg_stop_play_cb");
	return f_uninit(cb);
}

bool Audio::RegStartCaptureCb(nim_rescode_cb cb)
{
	nim_audio_reg_start_capture_cb f_uninit = Function<nim_audio_reg_start_capture_cb>("nim_audio_reg_start_capture_cb");
	return f_uninit(cb);
}

bool Audio::RegStopCaptureCb(nim_stop_capture_cb cb)
{
	nim_audio_reg_stop_capture_cb f_uninit = Function<nim_audio_reg_stop_capture_cb>("nim_audio_reg_stop_capture_cb");
	return f_uninit(cb);
}

bool Audio::RegCancelAudioCb(nim_rescode_cb cb)
{
	nim_audio_reg_cancel_audio_cb f_uninit = Function<nim_audio_reg_cancel_audio_cb>("nim_audio_reg_cancel_audio_cb");
	return f_uninit(cb);
}

bool Audio::StartCapture(const char* call_id, const char* res_id, nim_audio_type audio_format/* = AAC*/, int volume/* = 180*/, const wchar_t* capture_device/* = nullptr*/)
{
	nim_audio_start_capture f_uninit = Function<nim_audio_start_capture>("nim_audio_start_capture");
	return f_uninit(call_id, res_id, audio_format, volume, capture_device);
}

bool Audio::StopCapture()
{
	nim_audio_stop_capture f_uninit = Function<nim_audio_stop_capture>("nim_audio_stop_capture");
	return f_uninit();
}

bool Audio::CancelAudio(const wchar_t* file_path)
{
	nim_audio_cancel_audio f_uninit = Function<nim_audio_cancel_audio>("nim_audio_cancel_audio");
	return f_uninit(file_path);
}

bool Audio::RegEnumCaptureDeviceCb(nim_enum_capture_device_cb cb)
{
	nim_audio_reg_enum_capture_device_cb fun = Function<nim_audio_reg_enum_capture_device_cb>("nim_audio_reg_enum_capture_device_cb");
	return fun(cb);
}

bool Audio::EnumCaptureDevice()
{
	nim_audio_enum_capture_device fun = Function<nim_audio_enum_capture_device>("nim_audio_enum_capture_device");
	return fun();
}

}
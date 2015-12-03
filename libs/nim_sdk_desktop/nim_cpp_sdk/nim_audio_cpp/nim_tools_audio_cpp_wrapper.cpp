#include "nim_tools_audio_cpp_wrapper.h"
#include "module/login/login_manager.h"



namespace nim_audio
{

static const std::wstring kSdkAudioDll_d = L"nim_audio_d.dll";
static const std::wstring kSdkAudioDll = L"nim_audio.dll";

HINSTANCE Audio::instance_audio_ = NULL;


//init
typedef bool(*nim_audio_init_module)(const char* user_data_parent_path);
typedef bool(*nim_audio_uninit_module)();

//play and stop
typedef bool(*nim_audio_play_audio)(const char* call_id, const char* res_id, const char* file_path);
typedef bool(*nim_audio_stop_play_audio)();

//reg
typedef bool(*nim_audio_reg_start_play_cb)(nim_rescode_id_cb cb);

typedef bool(*nim_audio_reg_stop_play_cb)(nim_rescode_id_cb cb);


bool Audio::Init(const std::string& res_audio_path)
{
	std::wstring dir = QPath::GetAppPath();
#ifdef _DEBUG
	dir.append(kSdkAudioDll_d);
#else
	dir.append(kSdkAudioDll);
#endif
	instance_audio_ = ::LoadLibraryW(dir.c_str());
	if (instance_audio_ == NULL)
	{
		QLOG_ERR(L"sdk audio dll load fail {0} {1}") << dir << GetLastError();
		return false;
	}

	assert(instance_audio_);
	if (instance_audio_)
	{
		nim_audio_init_module f_init = Function<nim_audio_init_module>("nim_audio_init_module");
		bool ret = f_init(res_audio_path.c_str());
		if (!ret)
		{
			QLOG_ERR(L"init audio fail: {0}") << res_audio_path;
		}
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
		QLOG_APP(L"uninit audio module");

		::FreeLibrary(instance_audio_);
		instance_audio_ = NULL;
		QLOG_APP(L"free audio library");
	}
}

bool Audio::PlayAudio(const char* call_id, const char* res_id, const char* file_path)
{
	nim_audio_play_audio f_uninit = Function<nim_audio_play_audio>("nim_audio_play_audio");
	return f_uninit(call_id, res_id, file_path);
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

}
#include "audio_manager.h"
#include "callback/audio/audio_callback.h"

namespace nim_comp
{
AudioManager::AudioManager()
{

}

bool AudioManager::InitAudio(const std::string user_data_path)
{	
	bool ret = nim_audio::Audio::Init(user_data_path);
	assert(ret);

	nim_audio::Audio::RegStartPlayCb(&AudioCallback::OnPlayAudioCallback);
	nim_audio::Audio::RegStopPlayCb(&AudioCallback::OnStopAudioCallback);

	nim_audio::Audio::RegStartCaptureCb(&AudioCallback::OnStartCaptureCallback);
	nim_audio::Audio::RegStopCaptureCb(&AudioCallback::OnStopCaptureCallback);
	nim_audio::Audio::RegCancelAudioCb(&AudioCallback::OnCancelCaptureCallback);
	nim_audio::Audio::RegEnumCaptureDeviceCb(&AudioCallback::OnEnumCaptureDeviceCallback);

	return ret;
}

bool AudioManager::PlayAudio(const std::string file_path, const std::string session_id, const std::string msg_id, nim_audio::nim_audio_type audio_format)
{
	play_sid_ = session_id;
	play_cid_ = msg_id;
	return nim_audio::Audio::PlayAudio(file_path.c_str(), session_id.c_str(), msg_id.c_str(), audio_format);
}

bool AudioManager::StopPlayAudio(const std::string session_id)
{
	if (session_id == play_sid_)
	{
		ClearPlayId();
		return nim_audio::Audio::StopPlayAudio();
	}

	return false;
}

bool AudioManager::IsPlaying()
{
	return !play_sid_.empty();
}

bool AudioManager::StartCapture(const std::string session_id, const std::string msg_id, nim_audio::nim_audio_type audio_format /*= nim_audio::AAC*/, int volume /*= 180*/, int loudness /*= 0*/, const wchar_t* capture_device /*= nullptr*/)
{
	// 如果当前已经在录制了，就无法调用录制接口
	if (!capture_sid_.empty())
		return false;

	capture_sid_ = session_id;
	capture_cid_ = msg_id;
	return nim_audio::Audio::StartCapture(session_id.c_str(), msg_id.c_str(), audio_format, volume, loudness, capture_device);
}

bool AudioManager::StopCapture(const std::string session_id)
{
	if (session_id == capture_sid_)
	{
		ClearCaptureId();
		return nim_audio::Audio::StopCapture();
	}

	return false;
}

bool AudioManager::CancelCapture(const std::string session_id, const std::string file_path)
{
	if (session_id == capture_sid_)
	{
		ClearCaptureId();
		return nim_audio::Audio::CancelAudio(file_path.c_str());
	}

	return false;
}

bool AudioManager::IsCapturing()
{
	return !capture_sid_.empty();
}

bool AudioManager::EnumCaptureDevice()
{
	return nim_audio::Audio::EnumCaptureDevice();
}

UnregisterCallback AudioManager::RegStartCapture(const StartCaptureCallback& callback)
{
	StartCaptureCallback* new_callback = new StartCaptureCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	start_capture_cb_list_[cb_id].reset(new_callback);

	auto cb = ToWeakCallback([this, cb_id]() {
		start_capture_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback AudioManager::RegStopCapture(const StopCaptureCallback& callback)
{
	StopCaptureCallback* new_callback = new StopCaptureCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	stop_capture_cb_list_[cb_id].reset(new_callback);

	auto cb = ToWeakCallback([this, cb_id]() {
		stop_capture_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback AudioManager::RegCancelCapture(const CancelCaptureCallback& callback)
{
	CancelCaptureCallback* new_callback = new CancelCaptureCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	cancel_capture_cb_list_[cb_id].reset(new_callback);

	auto cb = ToWeakCallback([this, cb_id]() {
		cancel_capture_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback AudioManager::RegEnumCaptureDevice(const EnumCaptureDeviceCallback& callback)
{
	EnumCaptureDeviceCallback* new_callback = new EnumCaptureDeviceCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	enum_device_cb_list_[cb_id].reset(new_callback);

	auto cb = ToWeakCallback([this, cb_id]() {
		enum_device_cb_list_.erase(cb_id);
	});
	return cb;
}

void AudioManager::OnStartCaptureCallback(int code)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : start_capture_cb_list_)
	{
		(*(it.second))(capture_sid_, code);
	}

	if (code != nim_audio::kSuccess)
	{
		ClearCaptureId();
	}
}

void AudioManager::OnStopCaptureCallback(int rescode, const std::string& sid, const std::string& cid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration)
{
	if (sid.empty() || cid.empty())
		return;

	//如果这个停止的callback是当前正在录制的
	if (sid == AudioManager::GetInstance()->GetCaptureSid() && cid == AudioManager::GetInstance()->GetCaptureCid())
	{
		ClearCaptureId();
	}

	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : stop_capture_cb_list_)
	{
		(*(it.second))(rescode, sid, cid, file_path, file_ext, file_size, audio_duration);
	}
}

void AudioManager::OnCancelCaptureCallback(int code)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : cancel_capture_cb_list_)
	{
		(*(it.second))(capture_sid_, code);
	}

	ClearCaptureId();
}

void AudioManager::OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : enum_device_cb_list_)
	{
		(*(it.second))(rescode, device_list);
	}

}

std::string AudioManager::GetPlaySid()
{
	return play_sid_;
}

std::string AudioManager::GetPlayCid()
{
	return play_cid_;
}

void AudioManager::ClearPlayId()
{
	play_sid_.clear();
	play_cid_.clear();
}

std::string AudioManager::GetCaptureSid()
{
	return capture_sid_;
}

std::string AudioManager::GetCaptureCid()
{
	return capture_cid_;
}

void AudioManager::ClearCaptureId()
{
	capture_sid_.clear();
	capture_cid_.clear();
}

}
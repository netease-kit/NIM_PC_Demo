#include "audio_capture.h"
#include "module/audio/audio_manager.h"

using namespace ui;
namespace nim_comp
{
AudioCaptureView::AudioCaptureView()
{
	need_cancel_ = false;
}

void AudioCaptureView::InitControl(const std::string& session_id, OnCaptureCompeleteCallback callback)
{
	assert(!session_id.empty());
	session_id_ = session_id;
	compelete_callback_ = callback;

	GlobalManager::FillBox(this, L"session/aduio_capture_view.xml");

	this->AttachBubbledEvent(kEventClick, nbase::Bind(&AudioCaptureView::OnClicked, this, std::placeholders::_1));
	time_text_ = static_cast<Label*>(FindSubControl(L"time_text"));
	tip_text = static_cast<Label*>(FindSubControl(L"tip_text"));
	button_start_ = static_cast<Button*>(FindSubControl(L"btn_start_capture"));
	button_stop_ = static_cast<Button*>(FindSubControl(L"btn_send_audio"));

	AudioManager::GetInstance()->RegStartCapture(nbase::Bind(&AudioCaptureView::OnStartCaptureCallback, this, std::placeholders::_1, std::placeholders::_2));
	AudioManager::GetInstance()->RegStopCapture(nbase::Bind(&AudioCaptureView::OnStopCaptureCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
	AudioManager::GetInstance()->RegCancelCapture(nbase::Bind(&AudioCaptureView::OnCancelCaptureCallback, this, std::placeholders::_1, std::placeholders::_2));
	AudioManager::GetInstance()->RegEnumCaptureDevice(nbase::Bind(&AudioCaptureView::OnEnumCaptureDeviceCallback, this, std::placeholders::_1, std::placeholders::_2));
}

void AudioCaptureView::ShowControl(bool show)
{
	if (show)
	{
		if (this->IsVisible())
			return;

		need_cancel_ = false;
		this->SetVisible(true);
		button_start_->SetEnabled(true);
		button_stop_->SetEnabled(true);

		button_start_->SetVisible(true);
		button_stop_->SetVisible(false);

		AudioManager::GetInstance()->EnumCaptureDevice();
	}
	else
	{
		this->SetVisible(false);
	}
}

bool AudioCaptureView::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (name == L"btn_start_capture")
	{
		if (AudioManager::GetInstance()->IsCapturing())
		{
			button_start_->SetEnabled(false);
			button_stop_->SetEnabled(false);
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_RECORDING"));
			return true;
		}
		else if (AudioManager::GetInstance()->IsPlaying())
		{
			button_start_->SetEnabled(false);
			button_stop_->SetEnabled(false);
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_PLAYING"));
			return true;
		}
		bool aac = true;
		auto aac_option = dynamic_cast<ui::Option*>(FindSubControl(L"aac"));
		if (aac_option != nullptr)
			aac = aac_option->IsSelected();
		msg_uuid_ = QString::GetGUID();
		if (!AudioManager::GetInstance()->StartCapture(session_id_, msg_uuid_, aac ? nim_audio::AAC : nim_audio::AMR))
		{
			button_start_->SetEnabled(false);
			button_stop_->SetEnabled(false);
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_RECORDING"));
		}	
	}
	else if(name == L"btn_send_audio")
	{
		AudioManager::GetInstance()->StopCapture(session_id_);
	}
	else if (name == L"btn_calcel_capture")
	{
		need_cancel_ = true;;
		if (!AudioManager::GetInstance()->StopCapture(session_id_))
		{
			need_cancel_ = false;
			ShowControl(false);
			compelete_callback_(nim_audio::kFailed, session_id_, "", "", 0, 0);
		}	
	}

	return true;
}

void AudioCaptureView::OnStartCaptureCallback(const std::string& session_id, int code)
{
	if (session_id != session_id_)
		return;

	if (code == nim_audio::kSuccess)
	{
		button_start_->SetVisible(false);
		button_stop_->SetVisible(true);

		tick_ = 0;
		StdClosure cb = nbase::Bind(&AudioCaptureView::OnTick, this);
		auto weak_cb = timer_.ToWeakCallback(cb);
		nbase::ThreadManager::PostRepeatedTask(weak_cb, nbase::TimeDelta::FromSeconds(1));
	}
	else
	{
		time_text_->SetText(L"[00:00]");
		button_start_->SetEnabled(false);
		button_stop_->SetEnabled(false);

		MutiLanSupport* mls = MutiLanSupport::GetInstance();
		switch (code)
		{
		case nim_audio::kUninitError:
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_RECORD_DEVICE_INIT_FAIL"));
			break;
		case nim_audio::kClientPlaying:
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_PLAYING_CANT_RECORD"));
			break;
		case nim_audio::kClientCapturing:
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_RECORDING"));
			break;
		case nim_audio::kCaptureDeviceInitError:
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_CAPTURE_DEVICE_INIT_FAIL"));
			break;
		default:
			break;
		}
	}
}

void AudioCaptureView::OnStopCaptureCallback(int rescode, const std::string& sid, const std::string& cid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration)
{
	if (sid != session_id_ || cid != msg_uuid_)
		return;

	timer_.Cancel();
	ShowControl(false);

	if (need_cancel_)
	{
		need_cancel_ = false;
		AudioManager::GetInstance()->CancelCapture(session_id_, file_path);
		compelete_callback_(nim_audio::kFailed, sid, file_path, file_ext, file_size, audio_duration);
	}
	else
	{
		compelete_callback_(rescode, sid, file_path, file_ext, file_size, audio_duration);
	}
}

void AudioCaptureView::OnCancelCaptureCallback(const std::string& session_id, int code)
{
	if (session_id != session_id_)
		return;
}

void AudioCaptureView::OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list)
{
	// 如果正在本控件正在录制，就直接返回
	if (AudioManager::GetInstance()->GetCaptureSid() == session_id_)
		return;

	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (rescode != nim_audio::kSuccess || NULL == device_list)
	{
		time_text_->SetVisible(false);
		tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_DEVICE_NOT_FOUND"));
		button_start_->SetEnabled(false);
		button_stop_->SetEnabled(false);
	}
	else
	{
		if (button_start_->IsVisible() && button_start_->IsEnabled())
		{
			time_text_->SetVisible(true);
			time_text_->SetText(L"[00:00]");
			tip_text->SetText(mls->GetStringViaID(L"STRID_AUDIO_CAPTURE_CLICK_TO_RECORD"));
			button_start_->SetEnabled(true);
			button_stop_->SetEnabled(true);
		}
	}
}

void AudioCaptureView::OnTick()
{
	tick_++;
	int minutes = tick_ / 60;
	int seconds = tick_ % 60;
	std::wstring str = nbase::StringPrintf(L"[%02d:%02d]", minutes, seconds);
	time_text_->SetText(str);
}

}
#include "audio_callback.h"
#include "module/session/session_manager.h"
#include "gui/session/msg_record.h"
#include "module/audio/audio_manager.h"

namespace nim_comp
{
void UIPlayAudioCallback( int code, std::string sid, std::string cid )
{
	assert( !sid.empty() );
	assert( !cid.empty() );
	if(sid.empty() || cid.empty())
		return;

	if (code != nim_audio::kSuccess)
	{
		AudioManager::GetInstance()->ClearPlayId();
	}

	std::wstring wsid = nbase::UTF8ToUTF16(sid);
	if(wsid == MsgRecordForm::kClassName)
	{
		MsgRecordForm* f = dynamic_cast<MsgRecordForm*>(WindowsManager::GetInstance()->GetWindow(MsgRecordForm::kClassName, MsgRecordForm::kClassName));
		if(f)
			f->OnPlayAudioCallback(cid, code);
	}
	else
	{
		SessionBox* session_form = SessionManager::GetInstance()->FindSessionBox(sid);
		if(session_form)
			session_form->OnPlayAudioCallback(cid, code);
	}
}

void AudioCallback::OnPlayAudioCallback( int code, const char* file_path, const char* sid, const char* cid )
{
	QLOG_APP(L"OnPlayAudioCallback: sid={0} cid={1} code={2}") << sid << cid << code;
	Post2UI( nbase::Bind(&UIPlayAudioCallback, code, std::string(sid), std::string(cid)) );
}

void UIStopAudioCallback( int code, const std::string &sid, const std::string &cid )
{
	if(sid.empty() || cid.empty())
		return;

	//如果这个停止的callback是当前正在播放的
	if (sid == AudioManager::GetInstance()->GetPlaySid() && cid == AudioManager::GetInstance()->GetPlayCid())
	{
		AudioManager::GetInstance()->ClearPlayId();
	}

	std::wstring wsid = nbase::UTF8ToUTF16(sid);
	if(wsid == MsgRecordForm::kClassName)
	{
		MsgRecordForm* f = dynamic_cast<MsgRecordForm*>(WindowsManager::GetInstance()->GetWindow(MsgRecordForm::kClassName, MsgRecordForm::kClassName));
		if(f)
			f->OnStopAudioCallback(cid, code);
	}
	else
	{
		SessionBox* session_form = SessionManager::GetInstance()->FindSessionBox(sid);
		if(session_form)
			session_form->OnStopAudioCallback(cid, code);
	}
}

void AudioCallback::OnStopAudioCallback( int code, const char* file_path, const char* sid, const char* cid )
{
	QLOG_APP(L"OnStopAudioCallback: sid={0} cid={1} code={2}") <<sid <<cid <<code;
	Post2UI( nbase::Bind(&UIStopAudioCallback, code, std::string(sid), std::string(cid)) );
}

void AudioCallback::OnStartCaptureCallback(int code)
{
	QLOG_APP(L"OnStartCaptureCallback: code={0}") << code;
	Post2UI(nbase::Bind(&AudioManager::OnStartCaptureCallback, AudioManager::GetInstance(), code));
}

void AudioCallback::OnStopCaptureCallback(int rescode, const char* sid, const char* cid, const char* file_path, const char* file_ext, long file_size, int audio_duration)
{
	QLOG_APP(L"OnStopCaptureCallback: sid={0} cid={1} code={2} file={3}") << sid << cid << rescode << file_path;
	Post2UI(nbase::Bind(&AudioManager::OnStopCaptureCallback, AudioManager::GetInstance(), rescode, std::string(sid), std::string(cid), std::string(file_path), std::string(file_ext), file_size, audio_duration));
}

void AudioCallback::OnCancelCaptureCallback(int code)
{
	QLOG_APP(L"OnCancelCaptureCallback: code={0}") << code;
	Post2UI(nbase::Bind(&AudioManager::OnCancelCaptureCallback, AudioManager::GetInstance(), code));
}

void AudioCallback::OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list)
{
	QLOG_PRO(L"OnEnumCaptureDeviceCallback: code={0}") << rescode;
	Post2UI(nbase::Bind(&AudioManager::OnEnumCaptureDeviceCallback, AudioManager::GetInstance(), rescode, device_list));
}

}
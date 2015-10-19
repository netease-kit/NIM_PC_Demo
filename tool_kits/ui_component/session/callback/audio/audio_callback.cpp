#include "audio_callback.h"
#include "module/session/session_manager.h"
#include "gui/session/msg_record.h"

namespace nim_comp
{
std::string AudioCallback::play_sid_ = "";
std::string AudioCallback::play_cid_ = "";

void AudioCallback::SetPlaySid( const std::string &sid )
{
	play_sid_ = sid;
}

std::string AudioCallback::GetPlaySid()
{
	return play_sid_;
}

void AudioCallback::SetPlayCid( const std::string &cid )
{
	play_cid_ = cid;
}

std::string AudioCallback::GetPlayCid()
{
	return play_cid_;
}

void UIPlayAudioCallback( int code, std::string sid, std::string cid )
{
	assert( !sid.empty() );
	assert( !cid.empty() );
	if(sid.empty() || cid.empty())
		return;

	if (code != nim::kSuccess)
	{
		AudioCallback::SetPlaySid("");
		AudioCallback::SetPlayCid("");
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
		SessionForm* session_form = SessionManager::GetInstance()->Find(sid);
		if(session_form)
			session_form->OnPlayAudioCallback(cid, code);
	}
}

void AudioCallback::OnPlayAudioCallback( int code, const char* file_path, const char* sid, const char* cid )
{
	QLOG_PRO(L"OnPlayAudioCallback: sid={0} cid={1} code={2}") <<sid <<cid <<code;
	Post2UI( nbase::Bind(&UIPlayAudioCallback, code, std::string(sid), std::string(cid)) );
}

void UIStopAudioCallback( int code, const std::string &sid, const std::string &cid )
{
	if(sid.empty() || cid.empty())
		return;

	//如果这个停止的callback是当前正在播放的
	if(sid == AudioCallback::GetPlaySid() && cid == AudioCallback::GetPlayCid())
	{
		AudioCallback::SetPlaySid("");
		AudioCallback::SetPlayCid("");
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
		SessionForm* session_form = SessionManager::GetInstance()->Find(sid);
		if(session_form)
			session_form->OnStopAudioCallback(cid, code);
	}
}

void AudioCallback::OnStopAudioCallback( int code, const char* file_path, const char* sid, const char* cid )
{
	QLOG_APP(L"OnStopAudioCallback: sid={0} cid={1} code={2}") <<sid <<cid <<code;
	Post2UI( nbase::Bind(&UIStopAudioCallback, code, std::string(sid), std::string(cid)) );
}
}
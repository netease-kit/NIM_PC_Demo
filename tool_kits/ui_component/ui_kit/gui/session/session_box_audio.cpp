#include "session_box.h"

using namespace ui;

namespace nim_comp
{
void SessionBox::OnPlayAudioCallback( const std::string &cid, int code )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleAudio* item = dynamic_cast<MsgBubbleAudio*>(it->second);
		if(item)
		{
			item->OnPlayCallback(code);
		}
	}
	if (code != nim::kNIMResSuccess)
	{
		std::wstring tip = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_PLAY_AUDIO_ERROR").c_str(), code);
		ShowMsgBox(this->GetWindow()->GetHWND(), MsgboxCallback(), tip, false);
	}
}

void SessionBox::OnStopAudioCallback( const std::string &cid, int code )
{
	IdBubblePair::iterator it = id_bubble_pair_.find(cid);
	if(it != id_bubble_pair_.end())
	{
		MsgBubbleAudio* item = dynamic_cast<MsgBubbleAudio*>(it->second);
		if(item)
		{
			item->OnStopCallback(code);
		}
	}
}
}
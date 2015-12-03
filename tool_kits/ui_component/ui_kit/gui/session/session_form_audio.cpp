#include "session_form.h"

using namespace ui;

namespace nim_comp
{
void SessionForm::OnPlayAudioCallback( const std::string &cid, int code )
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
		std::wstring tip = nbase::StringPrintf(L"语音播放失败，错误码：%d", code);
		ShowMsgBox(m_hWnd, tip, MsgboxCallback(), L"提示", L"确定", L"");
	}
}

void SessionForm::OnStopAudioCallback( const std::string &cid, int code )
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
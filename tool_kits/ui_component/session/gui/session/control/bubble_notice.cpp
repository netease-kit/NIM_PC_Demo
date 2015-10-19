#include "bubble_notice.h"


using namespace ui;

namespace nim_comp
{
void MsgBubbleNotice::InitControl()
{
	notice_ = (Label*) this->FindSubControl(L"notice");

	this->AttachResize(nbase::Bind(&MsgBubbleNotice::OnResize, this, std::placeholders::_1));
}

void MsgBubbleNotice::InitInfo(const MsgData &msg)
{
	this->SetUTF8Name(msg.client_msg_id);

	std::wstring wstr;
	if (msg.msg_type == nim::kNIMMessageTypeCustom)
	{
		wstr = GetCustomMsg(msg.msg_attach);
	}
	else
	{
		GetNotifyMsg(msg.msg_attach, msg.from_account, msg.to_account, wstr);
	}
	notice_->SetText(wstr);

	OnResized();
}

bool MsgBubbleNotice::OnResize( ui::EventArgs* arg )
{
	OnResized();
	return true;
}

void MsgBubbleNotice::OnResized()
{
	UiRect rc = m_pWindow->GetPos(false);
	int width = rc.GetWidth() - 100;

	notice_->SetMaxWidth(width);
}
}
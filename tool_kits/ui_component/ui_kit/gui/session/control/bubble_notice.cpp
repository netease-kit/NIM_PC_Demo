#include "bubble_notice.h"


using namespace ui;

namespace nim_comp
{
void MsgBubbleNotice::InitControl()
{
	notice_ = (Label*) this->FindSubControl(L"notice");

	this->AttachResize(nbase::Bind(&MsgBubbleNotice::OnResize, this, std::placeholders::_1));
}

void MsgBubbleNotice::InitInfo(const nim::IMMessage &msg, const UTF8String& session_id)
{
	msg_ = msg;
	session_id_ = session_id;

	this->SetUTF8Name(msg.client_msg_id_);

	std::wstring wstr;
	if (msg.type_ == nim::kNIMMessageTypeCustom)
	{
		wstr = GetCustomMsg(msg.attach_);
	}
	else
	{
		GetNotifyMsg(msg.attach_, msg.sender_accid_, msg.receiver_accid_, wstr, session_id);
	}
	notice_->SetText(wstr);

	OnResized();
}

void MsgBubbleNotice::RefreshNotice()
{
	std::wstring wstr;
	if (msg_.type_ == nim::kNIMMessageTypeCustom)
		wstr = GetCustomMsg(msg_.attach_);
	else
		GetNotifyMsg(msg_.attach_, msg_.sender_accid_, msg_.receiver_accid_, wstr, session_id_);
	
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
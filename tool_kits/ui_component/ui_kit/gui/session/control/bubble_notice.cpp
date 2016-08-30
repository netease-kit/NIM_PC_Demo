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
	RefreshNotice();
}

void MsgBubbleNotice::RefreshNotice()
{
	if (is_custom_info_)
		return;

	std::wstring wstr;
	if (msg_.type_ == nim::kNIMMessageTypeCustom)
		wstr = GetCustomMsg(msg_.attach_);
	else if (msg_.type_ == nim::kNIMMessageTypeNotification)
		GetNotifyMsg(msg_.attach_, msg_.sender_accid_, msg_.receiver_accid_, wstr, session_id_);
	else if (msg_.type_ == nim::kNIMMessageTypeTips)
	{
		wstr = nbase::UTF8ToUTF16(msg_.content_);
		if (wstr.empty())
			wstr = L"空提醒消息";
	}
	
	notice_->SetText(wstr);
	OnResized();
}

void MsgBubbleNotice::InitCustomInfo(const std::wstring &show_notice, const UTF8String& session_id, const UTF8String& client_msg_id)
{
	is_custom_info_ = true;
	session_id_ = session_id;
	this->SetUTF8Name(client_msg_id);

	notice_->SetText(show_notice);
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
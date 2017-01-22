#include "bubble_notice.h"


using namespace ui;

namespace nim_comp
{
void MsgBubbleNotice::InitControl()
{
	notice_ = (Label*) this->FindSubControl(L"notice");

	this->AttachResize(nbase::Bind(&MsgBubbleNotice::OnResize, this, std::placeholders::_1));
}

void MsgBubbleNotice::InitInfo(const nim::IMMessage &msg, const UTF8String& session_id, bool is_custom_info)
{
	is_custom_info_ = is_custom_info;
	msg_ = msg;
	session_id_ = session_id;

	this->SetUTF8Name(msg.client_msg_id_);

	if (is_custom_info)
	{
		notice_->SetText(nbase::UTF8ToUTF16(msg_.content_));
		OnResized();
	}
	else
	{
		RefreshNotice();
	}
}

void MsgBubbleNotice::RefreshNotice()
{
	if (is_custom_info_)
		return;

	std::wstring wstr;
	if (msg_.type_ == nim::kNIMMessageTypeCustom)
		wstr = GetCustomMsg(msg_.sender_accid_, msg_.attach_);
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

nim::IMMessage MsgBubbleNotice::GetMsg()
{
	return msg_;
}

int64_t MsgBubbleNotice::GetMsgTimeTag()
{
	return msg_.timetag_;
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
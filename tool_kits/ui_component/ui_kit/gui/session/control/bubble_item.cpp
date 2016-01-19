#include "bubble_item.h"
#include "shared/ui/ui_menu.h"
#include "gui/main/control/session_item.h"
#include "gui/profile_form/profile_form.h"
#include "callback/session/session_callback.h"

using namespace ui;

namespace nim_comp
{
MsgBubbleItem::MsgBubbleItem()
{
	action_menu_ = true;
}

MsgBubbleItem::~MsgBubbleItem()
{

}

void MsgBubbleItem::InitControl(bool bubble_right)
{
	my_msg_ = bubble_right;
	this->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsgBubbleItem::OnClicked, this, std::placeholders::_1));

	msg_time_ = (Label*) this->FindSubControl(L"msg_time");
	msg_header_button_ = (Button*) this->FindSubControl(L"msg_header_button");
	sender_name_ = (Label*) this->FindSubControl(L"sender_name");

	bubble_box_ = (Box*) this->FindSubControl( kBubbleBox );

	status_sending_ = this->FindSubControl(L"status_sending");
	status_resend_ = (Button*) this->FindSubControl(L"status_resend");
	status_send_failed_ = this->FindSubControl(L"status_send_failed");

	status_loading_ = this->FindSubControl(L"status_loading");
	status_reload_ = (Button*) this->FindSubControl(L"status_reload");
	status_load_failed_ = this->FindSubControl(L"status_load_failed");
	play_status_ = this->FindSubControl(L"play_status");

	HideAllStatus(0);
}

void MsgBubbleItem::InitInfo(const nim::IMMessage &msg)
{
	msg_ = msg;

	this->SetUTF8Name(msg.client_msg_id_);

	SetShowHeader();

	SetMsgStatus(msg.status_);
}

void MsgBubbleItem::SetSessionId( const std::string &sid )
{
	sid_ = sid;
}

void MsgBubbleItem::SetSessionType(const nim::NIMSessionType &type)
{
	type_ = type;
}

void MsgBubbleItem::SetActionMenu( bool action )
{
	action_menu_ = action;
}

nim::IMMessage MsgBubbleItem::GetMsg()
{
	return msg_;
}

void MsgBubbleItem::SetShowTime(bool show)
{
	if(show)
	{
		std::wstring tm = GetMessageTime(msg_.timetag_, false);
		msg_time_->SetText(tm);
	}
	msg_time_->SetVisible(show);
}

void MsgBubbleItem::SetShowHeader()
{
	msg_header_button_->SetBkImage(UserService::GetInstance()->GetUserPhoto(msg_.sender_accid_));
}

void MsgBubbleItem::SetShowName(bool show, const std::string& from_nick)
{
	sender_name_->SetVisible(show);

	if (show)
		sender_name_->SetUTF8Text(from_nick);
}

void MsgBubbleItem::SetMsgStatus(nim::NIMMsgLogStatus status)
{
	HideAllStatus(1);
	switch(status)
	{
	case nim::kNIMMsgLogStatusSending:
		status_sending_->SetVisible(true);
		break;
	case nim::kNIMMsgLogStatusSendFailed:
	case nim::kNIMMsgLogStatusSendCancel:
		status_resend_->SetVisible(true);
		break;
	case nim::kNIMMsgLogStatusDeleted:
		status_send_failed_->SetVisible(true);
		break;
	default:
		break;
	}
}

void MsgBubbleItem::SetLoadStatus(MsgResLoadStatus status)
{
	HideAllStatus(2);
	switch(status)
	{
	case RS_LOADING:
		status_loading_->SetVisible(true);
		break;
	case RS_RELOAD:
		status_reload_->SetVisible(true);
		break;
	case RS_LOAD_NO:
		status_load_failed_->SetVisible(true);
		break;
	default:
		break;
	}
}

void MsgBubbleItem::SetMsgRead( bool read )
{

}

void MsgBubbleItem::SetPlayed(bool play)
{
	if (play_status_)
	{
		play_status_->SetVisible(!play);
	}
}

bool MsgBubbleItem::OnClicked(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"status_resend")
	{
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_RESEND, 0);
	}
	else if(name == L"status_reload")
	{
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_RELOAD, 0);
	}
	else if (name == L"msg_header_button")
	{
		ProfileForm::ShowProfileForm(msg_.sender_accid_);
	}
	return true;
}

void MsgBubbleItem::HideAllStatus(int type)
{
	if(type != 1)
	{
		status_loading_->SetVisible(false);
		status_reload_->SetVisible(false);
		status_load_failed_->SetVisible(false);
	}
	if(type != 2)
	{
		status_sending_->SetVisible(false);
		status_resend_->SetVisible(false);
		status_send_failed_->SetVisible(false);
	}
}

class MsgBubbleAudio;
void MsgBubbleItem::PopupMenu( bool copy )
{
	if(!action_menu_)
		return;

	POINT point;
	::GetCursorPos(&point);

	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"bubble_menu.xml");
	pMenu->Init(xml, _T("xml"), point);
	
	CMenuElementUI* cop = (CMenuElementUI*) pMenu->FindControl(L"copy");
	cop->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	cop->SetVisible(copy);

	CMenuElementUI* del = (CMenuElementUI*)pMenu->FindControl(L"delete");
	del->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	
	CMenuElementUI* transform = (CMenuElementUI*)pMenu->FindControl(L"transform");
	transform->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	transform->SetVisible(typeid(*this) == typeid(MsgBubbleAudio));

	pMenu->Show();
}

bool MsgBubbleItem::OnMenu( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"copy")
		OnMenuCopy();
	else if(name == L"delete")
		OnMenuDelete();
	else if (name == L"transform")
		OnMenuTransform();
	return false;
}

void MsgBubbleItem::OnMenuDelete()
{
	m_pWindow->SendNotify(this, ui::kEventNotify, BET_DELETE, 0);
}

void MsgBubbleItem::OnMenuTransform()
{
	m_pWindow->SendNotify(this, ui::kEventNotify, BET_TRANSFORM, 0);
}
}
#include "bubble_item.h"
#include "shared/ui/ui_menu.h"
#include "gui/main/control/session_item.h"
#include "gui/profile_form/profile_form.h"
#include "callback/session/session_callback.h"
#include "nim_cpp_team.h"
using namespace ui;

namespace nim_comp
{
MsgBubbleItem::MsgBubbleItem():
team_member_getter_(nullptr)
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
	status_read_count_ = (Button*) this->FindSubControl(L"status_read_count");

	status_loading_ = this->FindSubControl(L"status_loading");
	status_reload_ = (Button*) this->FindSubControl(L"status_reload");
	status_load_failed_ = this->FindSubControl(L"status_load_failed");
	play_status_ = this->FindSubControl(L"play_status");
	status_receipt_ = (Label*)this->FindSubControl(L"status_receipt");

	HideAllStatus(0);
}

void MsgBubbleItem::InitInfo(const nim::IMMessage &msg)
{
	msg_ = msg;

	this->SetUTF8Name(msg.client_msg_id_);

	SetShowHeader();

	//只显示最后一个回执
	if (msg.status_ != nim::kNIMMsgLogStatusReceipt)
		SetMsgStatus(msg.status_);

	if (!my_msg_ && msg_.session_type_ == nim::kNIMSessionTypeTeam)
	{
		msg_header_button_->SetContextMenuUsed(true);
		msg_header_button_->AttachMenu(nbase::Bind(&MsgBubbleItem::OnRightClick, this, std::placeholders::_1));
	}
}

void MsgBubbleItem::SetUnreadCount(int count)
{
	if (msg_.session_type_ == nim::kNIMSessionTypeP2P || !my_msg_)
		return;

	std::wstring txt;
	if (count == 0)
	{
		txt = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_UNREAD_ZERO");
		//status_read_count_->SetEnabled(false);
	}
	else
	{
		txt = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_UNREAD_COUNT");
		txt = nbase::StringPrintf(txt.c_str(), count);
		//status_read_count_->SetEnabled(true);
	}
	status_read_count_->SetText(txt);
	status_read_count_->SetVisible(true);
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

int64_t MsgBubbleItem::GetMsgTimeTag()
{
	return msg_.timetag_;
}

std::string MsgBubbleItem::GetSenderId()
{
	return msg_.sender_accid_;
}

nim::NIMMessageType MsgBubbleItem::GetMsgType()
{
	return msg_.type_;
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
	msg_header_button_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(msg_.sender_accid_, false));
}

void MsgBubbleItem::SetShowName(bool show, const std::string& from_nick)
{
	sender_name_->SetVisible(show);

	if (show)
		sender_name_->SetUTF8Text(from_nick);
}

void MsgBubbleItem::SetMsgStatus(nim::NIMMsgLogStatus status)
{
	HideAllStatus(0);
	msg_.status_ = status;
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
	case nim::kNIMMsgLogStatusReceipt:
		status_receipt_->SetVisible(true);
		break;
	case nim::kNIMMsgLogStatusRefused:
		//被对方拒绝，可能是被加入了黑名单，此时不设置消息的任何状态
		//HideAllStatus(0);
		break;
	default:
		break;
	}
}

void MsgBubbleItem::SetLoadStatus(MsgResLoadStatus status)
{
	HideAllStatus(0);
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

bool MsgBubbleItem::OnRightClick(ui::EventArgs* param)
{
	POINT point;
	::GetCursorPos(&point);

	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"cell_head_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* at_ta = (CMenuElementUI*)pMenu->FindControl(L"at_ta");
	at_ta->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	at_ta->SetVisible(true);

	pMenu->Show();

	return true;
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
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_SHOWPROFILE, 0);
	}
	else if (name == L"status_read_count")
	{
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_UNREAD_COUNT, 0);
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
		status_receipt_->SetVisible(false);
	}
}

class MsgBubbleAudio;
void MsgBubbleItem::PopupMenu(bool copy, bool recall, bool retweet/* = true*/)
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

	CMenuElementUI* rec = (CMenuElementUI*)pMenu->FindControl(L"recall");
	rec->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	rec->SetVisible(recall && IsShowRecallButton());

	CMenuElementUI* ret = (CMenuElementUI*)pMenu->FindControl(L"retweet");
	ret->AttachSelect(nbase::Bind(&MsgBubbleItem::OnMenu, this, std::placeholders::_1));
	ret->SetVisible(retweet);

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
	else if (name == L"retweet")
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_RETWEET, 0);
	else if (name == L"recall")
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_RECALL, 0);
	else if (name == L"at_ta")
		m_pWindow->SendNotify(this, ui::kEventNotify, BET_MENUATTA, 0);
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
bool MsgBubbleItem::IsShowRecallButton()
{
	bool ret = false;
	if (msg_.session_type_ == nim::kNIMSessionTypeP2P)
	{
		ret = my_msg_ && msg_.receiver_accid_ != LoginManager::GetInstance()->GetAccount() && msg_.status_ != nim::kNIMMsgLogStatusRefused;
	}
	else if (msg_.session_type_ == nim::kNIMSessionTypeTeam)
	{		
		if (my_msg_ && msg_.receiver_accid_ != LoginManager::GetInstance()->GetAccount())
			ret = true;
		else
		{
			if (team_member_getter_ != nullptr)
			{
				auto members = team_member_getter_();
				auto it = members.find(LoginManager::GetInstance()->GetAccount());
				if (it != members.end())
				{
					auto user_type = it->second.GetUserType();
					ret = (user_type == nim::kNIMTeamUserTypeCreator || user_type == nim::kNIMTeamUserTypeManager);
				}
			}
		}		
	}
	else
	{
		ret = false;
	}
	return ret;
}
}
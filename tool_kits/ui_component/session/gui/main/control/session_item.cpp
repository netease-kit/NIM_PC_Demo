#include "session_item.h"

#include "module/session/session_manager.h"
#include "callback/session/session_callback.h"

#include "shared/ui/ui_menu.h"
#include "shared/pin_yin_helper.h"
#include "gui/profile_form/profile_form.h"

using namespace ui;

namespace nim_comp
{
SessionItem::SessionItem()
{
	msg_time_ = 0;
	unread_count_ = 0;
}

SessionItem::~SessionItem()
{

}

void SessionItem::Init(const SessionMsgData &msg)
{
	this->AttachDoubleClick(nbase::Bind(&SessionItem::OnDbClicked, this, std::placeholders::_1));
	this->AttachMenu(nbase::Bind(&SessionItem::OnSessionItemMenu, this, std::placeholders::_1));

	label_name_ = (Label*) this->FindSubControl(L"label_name");
	label_msg_ = (Label*) this->FindSubControl(L"label_msg");
	label_time_ = (Label*) this->FindSubControl(L"label_time");
	box_unread_ = (Box*) this->FindSubControl(L"box_unread");
	label_unread_ = (Label*) this->FindSubControl(L"label_unread");

	id_ = msg.session_id;
	this->SetUTF8Name(id_);
	SetUTF8DataID(id_);
	
	type_ = msg.to_type;
	unread_count_ = msg.unread_count;
	
	OnGetUserInfoCallback cb = ToWeakCallback([this, msg](int ret, const std::list<UserInfo> &uinfos) {
		if (!ret || uinfos.empty()) return;
		UpdateInfo();
	});
	std::string id_to_get;
	if (msg.to_type == nim::kNIMSessionTypeP2P) // 如果是P2P聊天，先获取该用户的信息，再UpdateInfo
		id_to_get = id_;
	else
		id_to_get = msg.from_account;
	UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, id_to_get), cb);

	UpdateMsg(msg);
	UpdateUnread();
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&SessionItem::OnTeamNameChange, this, std::placeholders::_1)));
}

long long SessionItem::GetMsgTime()
{
	return msg_time_;
}

void SessionItem::UpdateInfo()
{
	if (type_ == nim::kNIMSessionTypeP2P)
	{
		UserInfo uinfo;
		UserService::GetInstance()->GetUserInfo(id_, uinfo);
		if (LoginManager::GetInstance()->IsEqual(id_))
		{
			label_name_->SetText(L"我的手机");
		} 
		else
		{
			label_name_->SetUTF8Text(uinfo.name);
		}
		std::wstring head_image = UserService::GetInstance()->GetUserPhoto(id_);
		ButtonBox* head_btn = (ButtonBox*)FindSubControl(L"head_image");
		head_btn->SetBkImage(head_image);
		head_btn->AttachClick(ToWeakCallback([this](ui::EventArgs* args){
			ProfileForm::ShowProfileForm(id_);
			return true;
		}));
	}
	else
	{
		std::wstring tname = TeamService::GetInstance()->GetTeamName(id_);
		label_name_->SetText(tname);
		FindSubControl(L"head_image")->SetBkImage(TeamService::GetInstance()->GetTeamPhoto(false));
	}
}

static void GetMsgContent(const SessionMsgData &msg, std::wstring &show_text)
{
	if (msg.msg_type == nim::kNIMMessageTypeText)
	{
		show_text = nbase::UTF8ToUTF16(msg.msg_body);
	}
	else if (msg.msg_type == nim::kNIMMessageTypeImage)
	{
		show_text = L"[图片]";
	}
	else if (msg.msg_type == nim::kNIMMessageTypeAudio)
	{
		show_text = L"[语音]";
	}
	else if (msg.msg_type == nim::kNIMMessageTypeVideo)
	{
		show_text = L"[视频]";
	}
	else if (msg.msg_type == nim::kNIMMessageTypeFile)
	{
		show_text = L"[文件]";
	}
	else if (msg.msg_type == nim::kNIMMessageTypeLocation)
	{
		show_text = L"[位置]";
	}
	else if (msg.msg_type == nim::kNIMMessageTypeNotification)
	{
		if (IsNetCallMsg((nim::NIMMessageType)msg.msg_type, msg.msg_attach))
		{
			GetNotifyMsg(msg.msg_attach, msg.from_account, "", show_text);
		}
		else
		{
			show_text = L"[通知消息]";
		}
	}
	else if (msg.msg_type == nim::kNIMMessageTypeCustom)
	{
		show_text = GetCustomMsg(msg.msg_attach);
	}
	else if (msg.msg_type == nim::kNIMMessageTypeUnknown)
	{
		show_text = L"[未定义消息]";
	}
	else
	{
		show_text = L"[未知消息]";
		std::string id = msg.session_id;
		QLOG_WAR(L"unknown msg: id_type={0}_{1} msg_type={2}") << id << msg.to_type << msg.msg_type;
	}
}

void SessionItem::UpdateMsg(const SessionMsgData &msg)
{
	msg_time_ = msg.msg_time;

	//if (msg.feature == nim::kNIMMessageFeatureLeaveMsg)
	//{
	//	if (msg.msg_type != nim::kNIMMessageTypeNotification)
	//		AddUnread();
	//}

	std::wstring show_text;
	if (msg.msg_status != nim::kNIMMsgLogStatusDeleted)
	{
		GetMsgContent(msg, show_text);

		if (type_ == nim::kNIMSessionTypeTeam) {
			UserInfo user_info;
			UserService::GetInstance()->GetUserInfo(msg.from_account, user_info);
			std::wstring nick_name = nbase::UTF8ToUTF16(user_info.name);
			if (!nick_name.empty())
			{
				show_text = nick_name + L":" + show_text;
			}
		}
		if (msg.msg_status == nim::kNIMMsgLogStatusSendFailed)
		{
			show_text = L"[失败]" + show_text;
		}
	}
	label_msg_->SetText(show_text);

	if (msg_time_ > 0 && msg.msg_status != nim::kNIMMsgLogStatusDeleted)
	{
		std::wstring str = GetMessageTime(msg_time_, true);
		label_time_->SetText(str);
	}
	else
	{
		label_time_->SetVisible(false);
	}
}
void SessionItem::ClearMsg()
{
	label_msg_->SetText(L"");
	label_time_->SetVisible(false);
	unread_count_;
	ResetUnread();
}

bool SessionItem::OnDbClicked(ui::EventArgs* arg)
{
	if(unread_count_ > 0)
	{
		ResetUnread();
	}
	SessionManager::GetInstance()->OpenSessionForm(id_, type_);
	return true;
}

bool SessionItem::OnSessionItemMenu( ui::EventArgs* arg )
{
	POINT point;
	::GetCursorPos(&point);
	PopupSessionItemMenu(point);
	return true;
}

void SessionItem::PopupSessionItemMenu( POINT point )
{
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"session_item_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* del_session_item = (CMenuElementUI*) pMenu->FindControl(L"del_session_item");
	del_session_item->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	CMenuElementUI* del_session_msg = (CMenuElementUI*) pMenu->FindControl(L"del_session_msg");
	del_session_msg->AttachSelect(nbase::Bind(&SessionItem::DelSessionItemMenuItemClick, this, std::placeholders::_1));

	pMenu->Show();
}

bool SessionItem::DelSessionItemMenuItemClick( ui::EventArgs* param )
{
	std::wstring name = param->pSender->GetName();
	if(name == L"del_session_item")
	{
		nim::Session::DeleteRecentSession(type_, id_, nbase::Bind(&SessionItem::DeleteRecentSessionCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		m_pWindow->SendNotify(this, ui::kEventNotify, SET_DELETE, 0);
	}
	else if(name == L"del_session_msg")
	{
		nim::MsgLog::BatchStatusDeleteAsync(id_, type_, nbase::Bind(&SessionItem::BatchStatusDeleteCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	return true;
}

int SessionItem::GetUnread()
{
	return unread_count_;
}

void SessionItem::SetUnread(int unread)
{
	unread_count_ = unread;
	UpdateUnread();
}

void SessionItem::AddUnread()
{
	unread_count_++;
	UpdateUnread();
}

void SessionItem::ResetUnread()
{
	if(unread_count_ > 0)
	{
		unread_count_ = 0;
		UpdateUnread();

		InvokeResetUnread(id_, type_);
	}
}

void SessionItem::UpdateUnread()
{
	if (unread_count_ > 0)
	{
		if (unread_count_ < 100) {
			label_unread_->SetText(nbase::StringPrintf(L"%d", unread_count_));
		}
		else {
			label_unread_->SetText(L"^ ^");
		}
		box_unread_->SetVisible(true);
	}
	else
	{
		box_unread_->SetVisible(false);
	}
}

void SessionItem::DeleteRecentSessionCb(nim::NIMResCode code, const std::string &result, int total_unread_counts)
{
	QLOG_APP(L"delete recent session, code={0} result={1} total_un_cn={2}") <<code <<result <<total_unread_counts;
}

void SessionItem::BatchStatusDeleteCb(nim::NIMResCode code, const std::string& id, nim::NIMSessionType type)
{
	QLOG_APP(L"batch delete msg, id={0} type={1} code={2}") <<id <<type <<code;
}

void SessionItem::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (team_info.id == id_)
	{
		label_name_->SetUTF8Text(team_info.name);
	}
}

void InvokeResetUnread(const std::string &id, nim::NIMSessionType type)
{
	nim::Session::SetUnreadCountZeroAsync(type, id, nim::Session::SetUnreadCountZeroCallback());
	nim::MsgLog::BatchStatusReadAsync(id, type, nim::MsgLog::BatchStatusReadCallback());
}
}
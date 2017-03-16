#include "event_item.h"
#include "module/session/session_manager.h"
#include "module/session/session_util.h"
#include "gui/main/team_event_form.h"
#include "gui/team_info/team_notify.h"
#include "tool_kits/shared/ui/ui_menu.h"
#include "callback/team/team_callback.h"

using namespace ui;
namespace nim_comp
{

TeamEventItem::TeamEventItem()
{

}

TeamEventItem::~TeamEventItem()
{

}

void TeamEventItem::InitCtrl()
{
	this->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&TeamEventItem::OnClicked, this, std::placeholders::_1));
	this->AttachMenu(nbase::Bind(&TeamEventItem::OnEventItemMenu, this, std::placeholders::_1));

	btn_head_ = (Button*) this->FindSubControl(L"btn_head");
	evt_team_ = (Label*) this->FindSubControl(L"evt_team");
	evt_time_ = (Label*) this->FindSubControl(L"evt_time");
	evt_tip_ = (Label*) this->FindSubControl(L"evt_tip");

	btn_ok_ = (Button*) this->FindSubControl(L"btn_ok");
	btn_no_ = (Button*) this->FindSubControl(L"btn_no");
	result_ = (Label*) this->FindSubControl(L"result");

	btn_ok_->SetVisible(false);
	btn_no_->SetVisible(false);
	result_->SetVisible(false);
}

bool TeamEventItem::InitInfo(const nim::SysMessage &json)
{
	msg_time_ = json.timetag_;
	msg_type_ = (int)json.type_;
	acc_id_ = json.sender_accid_;
	tid_ = json.receiver_accid_;
	msg_id_ = json.id_;
	msg_status_ = json.status_;

	btn_head_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(tid_, false));
	evt_team_->SetText(TeamService::GetInstance()->GetTeamName(tid_));
	evt_time_->SetText(GetMessageTime(msg_time_, false));

	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	std::wstring uname = UserService::GetInstance()->GetUserName(acc_id_);

	if (msg_type_ == nim::kNIMSysMsgTypeTeamApply)
	{
		evt_tip_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_MSG_CENTER_APPLY_JOIN_TEAM").c_str(), uname.c_str()));
		btn_ok_->SetVisible(true);
		btn_no_->SetVisible(true);
	}
	else if (msg_type_ == nim::kNIMSysMsgTypeTeamInvite)
	{
		evt_tip_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_MSG_CENTER_INVITE_YOU_JOIN_TEAM").c_str(), uname.c_str()));
		btn_ok_->SetVisible(true);
		btn_no_->SetVisible(true);
	}
	else if (msg_type_ == nim::kNIMSysMsgTypeTeamReject)
	{
		evt_tip_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_MSG_CENTER_REFUSE_YOU_JOIN_TEAM").c_str(), uname.c_str()));
	}
	else if (msg_type_ == nim::kNIMSysMsgTypeTeamInviteReject)
	{
		evt_tip_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_MSG_CENTER_REFUSE_JOIN_TEAM").c_str(), uname.c_str()));
	}
	else if (msg_type_ == nim::kNIMSysMsgTypeFriendAdd)
	{
		btn_head_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(acc_id_));
		evt_team_->SetText(uname);

		Json::Reader reader;
		Json::Value attach;
		reader.parse(json.attach_, attach);
		nim::NIMVerifyType verify_type = (nim::NIMVerifyType)attach["vt"].asInt();
		if (verify_type == nim::kNIMVerifyTypeAdd)
			evt_tip_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_ADD_YOU_FRIEND"));
		else if (verify_type == nim::kNIMVerifyTypeAsk)
		{
			std::wstring msg_content = nbase::UTF8ToUTF16(json.content_);
			evt_tip_->SetText(nbase::StringPrintf(mls->GetStringViaID(L"STRID_MSG_CENTER_APPLY_YOU_FRIEND").c_str(), msg_content.c_str()));
			evt_tip_->SetToolTipText(msg_content);
			if (UserService::GetInstance()->GetUserType(acc_id_) == nim::kNIMFriendFlagNotFriend)
			{
				btn_ok_->SetVisible(true);
				btn_no_->SetVisible(true);
			}
			else if (msg_status_ != nim::kNIMSysMsgStatusPass && msg_status_ != nim::kNIMSysMsgStatusDecline)
			{
				nim::SystemMsg::SetStatusAsync(msg_id_, nim::kNIMSysMsgStatusInvalid,
					nbase::Bind(&TeamEventItem::SetStatusCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				msg_status_ = nim::kNIMSysMsgStatusInvalid;
			}
		}
	}
	else
	{
		QLOG_ERR(L"error sysmsg, type={0}") << msg_type_;
		evt_tip_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_ERROR_MSG"));
	}

	if (msg_status_ == nim::kNIMSysMsgStatusPass || msg_status_ == nim::kNIMSysMsgStatusDecline || msg_status_ == nim::kNIMSysMsgStatusInvalid)
	{
		btn_ok_->SetVisible(false);
		btn_no_->SetVisible(false);
		if (msg_status_ == nim::kNIMSysMsgStatusPass)
			result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_AGREED"));
		else if (msg_status_ == nim::kNIMSysMsgStatusDecline)
			result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_REFUSED"));
		else
			result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_INVALID"));
		result_->SetVisible(true);
	}
	else if (msg_status_ == nim::kNIMSysMsgStatusDeleted)
	{
		assert(0);
	}

	return true;
}

__int64 TeamEventItem::GetMsgId()
{
	return msg_id_;
}

__int64 TeamEventItem::GetTime()
{
	return msg_time_;
}

void TeamEventItem::OnTeamEventCb(nim::NIMSysMsgStatus status)
{
	btn_ok_->SetVisible(false);
	btn_no_->SetVisible(false);
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (status == nim::kNIMSysMsgStatusPass)
		result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_AGREED"));
	else if (status == nim::kNIMSysMsgStatusDecline)
		result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_REFUSED"));
	else if (status == nim::kNIMSysMsgStatusInvalid)
		result_->SetText(mls->GetStringViaID(L"STRID_MSG_CENTER_INVALID"));
	result_->SetVisible(true);
}

void TeamEventItem::OnUserInfoChange(const nim::UserNameCard & info)
{
	if (info.GetAccId() == acc_id_)
	{
		std::wstring uname = UserService::GetInstance()->GetUserName(info.GetAccId());
		switch (msg_type_)
		{
		case nim::kNIMSysMsgTypeTeamApply:
		case nim::kNIMSysMsgTypeTeamInvite:
		case nim::kNIMSysMsgTypeTeamReject:
		case nim::kNIMSysMsgTypeTeamInviteReject:
		{
			std::wstring tip = evt_tip_->GetText();
			evt_tip_->SetText(uname + tip.substr(tip.find_last_of(L' ')));
			break;
		}
		case nim::kNIMSysMsgTypeFriendAdd:
		{
			btn_head_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(info.GetAccId()));
			evt_team_->SetText(uname);
			break;
		}
		default:
			break;
		}
	}
}

void TeamEventItem::OnUserPhotoReady(const std::string & accid, const std::wstring & photo_path)
{
	if (accid == acc_id_ && msg_type_ == nim::kNIMSysMsgTypeFriendAdd)
		btn_head_->SetBkImage(photo_path);
}

void TeamEventItem::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (team_info.GetTeamID() == tid_)
	{
		evt_team_->SetUTF8Text(team_info.GetName());
	}
}

void TeamEventItem::TeamEventCb(__int64 msg_id, const nim::TeamEvent& team_event)
{
	QLOG_APP(L"msg_id={0} notify_id={1} code={2}") << msg_id << team_event.notification_id_ << team_event.res_code_;

	nim::NIMSysMsgStatus status = nim::kNIMSysMsgStatusNone;
	if (team_event.notification_id_ == nim::kNIMNotificationIdTeamApplyPass) //5 同意xxx的入群申请
	{
		if (team_event.res_code_ == nim::kNIMResSuccess || team_event.res_code_ == nim::kNIMResTeamAlreadyIn)
			status = nim::kNIMSysMsgStatusPass;
		else
			status = nim::kNIMSysMsgStatusInvalid;
	}
	else if (team_event.notification_id_ == nim::kNIMNotificationIdLocalRejectApply) //2002 拒绝xxx的入群申请
	{
		if (team_event.res_code_ == nim::kNIMResSuccess)
			status = nim::kNIMSysMsgStatusDecline;
		else
			status = nim::kNIMSysMsgStatusInvalid;
	}
	else if (team_event.notification_id_ == nim::kNIMNotificationIdTeamInviteAccept) //9 同意xxx的入群邀请
	{
		if (team_event.res_code_ == nim::kNIMResSuccess)
			status = nim::kNIMSysMsgStatusPass;
		else
			status = nim::kNIMSysMsgStatusInvalid;
	}
	else if (team_event.notification_id_ == nim::kNIMNotificationIdLocalRejectInvite) //2003 拒绝xxx的入群邀请
	{
		if (team_event.res_code_ == nim::kNIMResSuccess)
			status = nim::kNIMSysMsgStatusDecline;
		else
			status = nim::kNIMSysMsgStatusInvalid;
	}
	if (status != nim::kNIMSysMsgStatusNone)
	{
		nim::SystemMsg::SetStatusAsync(msg_id, status,
			nbase::Bind(&TeamEventItem::SetStatusCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	TeamEventForm* f = dynamic_cast<TeamEventForm*>(WindowsManager::GetInstance()->GetWindow(TeamEventForm::kClassName, TeamEventForm::kClassName));
	if (f)
		f->OnTeamEventCb(msg_id, status);

	TeamNotifyForm* notify = (TeamNotifyForm*)(WindowsManager::GetInstance()->GetWindow(
		TeamNotifyForm::kClassName, nbase::UTF8ToUTF16(team_event.team_id_)));
	if (notify)
	{
		notify->Close();
	}

	if (team_event.notification_id_ == nim::kNIMNotificationIdTeamInviteAccept || team_event.notification_id_ == nim::kNIMNotificationIdTeamApplyPass)
	{
		TeamCallback::OnTeamEventCallback(team_event);
	}
}

void TeamEventItem::SetStatusCb(nim::NIMResCode code, __int64 msg_id, int unread)
{
	QLOG_APP(L"event SetStatusCb, msg_id={0} code={1} unread={2}") << msg_id << code << unread;
	UpdateSysmsgUnread(unread);
}

void TeamEventItem::DeleteCb(nim::NIMResCode code, __int64 msg_id, int unread)
{
	QLOG_APP(L"event DeleteCb, msg_id={0} code={1} unread={2}") << msg_id << code << unread;
	UpdateSysmsgUnread(unread);
}

bool TeamEventItem::OnEventItemMenu(ui::EventArgs* arg)
{
	POINT point;
	::GetCursorPos(&point);
	PopupEventItemMenu(point);
	return true;
}

void TeamEventItem::PopupEventItemMenu(POINT point)
{
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"event_item_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	CMenuElementUI* del_event_item = (CMenuElementUI*)pMenu->FindControl(L"del_event_item");
	del_event_item->AttachSelect(nbase::Bind(&TeamEventItem::DelEventItemMenuItemClick, this, std::placeholders::_1));

	pMenu->Show();
}

bool TeamEventItem::DelEventItemMenuItemClick(ui::EventArgs* param)
{
	nim::SystemMsg::DeleteAsync(msg_id_,
		nbase::Bind(&TeamEventItem::DeleteCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_pWindow->SendNotify(this, ui::kEventNotify, EOT_DELETE, 0);
	return false;
}

bool TeamEventItem::OnClicked(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"btn_ok")
	{
		btn_ok_->SetEnabled(false);
		btn_no_->SetEnabled(false);

		if (msg_type_ == nim::kNIMSysMsgTypeTeamApply)
			nim::Team::PassJoinApplyAsync(tid_, acc_id_,
			nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));
		else if (msg_type_ == nim::kNIMSysMsgTypeTeamInvite)
			nim::Team::AcceptInvitationAsync(tid_, acc_id_,
			nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));
		else if (msg_type_ == nim::kNIMSysMsgTypeFriendAdd)
		{
			nim::Friend::FriendOptCallback cb = ToWeakCallback([this](int res) {
				nim::SystemMsg::SetStatusAsync(msg_id_, nim::NIMSysMsgStatus::kNIMSysMsgStatusPass,
					nbase::Bind(&TeamEventItem::SetStatusCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			});
			nim::Friend::Request(acc_id_, nim::kNIMVerifyTypeAgree, "", cb);
		}
	}
	else if (name == L"btn_no")
	{
		btn_ok_->SetEnabled(false);
		btn_no_->SetEnabled(false);

		if (msg_type_ == nim::kNIMSysMsgTypeTeamApply)
			nim::Team::RejectJoinApplyAsync(tid_, acc_id_, "",
			nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));
		else if (msg_type_ == nim::kNIMSysMsgTypeTeamInvite)
			nim::Team::RejectInvitationAsync(tid_, acc_id_, "",
			nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));
		else if (msg_type_ == nim::kNIMSysMsgTypeFriendAdd)
		{
			nim::Friend::FriendOptCallback cb = ToWeakCallback([this](int res) {
				nim::SystemMsg::SetStatusAsync(msg_id_, nim::NIMSysMsgStatus::kNIMSysMsgStatusDecline,
					nbase::Bind(&TeamEventItem::SetStatusCb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			});
			nim::Friend::Request(acc_id_, nim::kNIMVerifyTypeReject, "", cb);
		}
	}
	return false;
}

}
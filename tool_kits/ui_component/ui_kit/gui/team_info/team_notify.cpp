#include "team_notify.h"
#include "module/login/login_manager.h"
#include "callback/team/team_callback.h"
#include "gui/main/control/event_item.h"


using namespace ui;

namespace nim_comp
{
const LPCTSTR TeamNotifyForm::kClassName = L"TeamNotifyForm";

TeamNotifyForm::TeamNotifyForm()
{

}

TeamNotifyForm::~TeamNotifyForm()
{

}

std::wstring TeamNotifyForm::GetSkinFolder()
{
	return L"team_info";
}

std::wstring TeamNotifyForm::GetSkinFile()
{
	return L"team_notify.xml";
}

std::wstring TeamNotifyForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring TeamNotifyForm::GetWindowId() const
{
	if( tid_.empty() )
	{
		return kClassName;
	}
	else
	{
		std::wstring wid = nbase::UTF8ToUTF16(tid_);
		return wid;
	}
}

UINT TeamNotifyForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT TeamNotifyForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void TeamNotifyForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&TeamNotifyForm::OnClicked, this, std::placeholders::_1));

	tnp_[0] = FindControl(L"page_ask_join");
	tnp_[1] = FindControl(L"page_invite");
	tnp_[2] = FindControl(L"page_reject");

	re_ask_join_ = (RichEdit*) FindControl(L"re_ask_join");
	re_invite_ = (RichEdit*) FindControl(L"re_invite");
	re_reject_ = (RichEdit*) FindControl(L"re_reject");

	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&TeamNotifyForm::OnTeamNameChange, this, std::placeholders::_1)));
}

void TeamNotifyForm::GotoPage( int t )
{
	for(int i = 0; i < 3; i++)
	{
		tnp_[i]->SetVisible(false);
	}
	tnp_[t]->SetVisible(true);
}

bool TeamNotifyForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"btn_ask_no")
	{
		QLOG_PRO(L"拒绝加入");
		nim::Team::RejectJoinApplyAsync(tid_, uid_, "", nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));

		this->Close();
	}
	else if(name == L"btn_ask_yes")
	{
		QLOG_PRO(L"同意加入");
		nim::Team::PassJoinApplyAsync(tid_, uid_, nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));

		this->Close();
	}
	else if(name == L"btn_ignore")
	{
		this->Close();
	}
	else if(name == L"btn_reject")
	{
		QLOG_PRO(L"拒绝邀请");
		nim::Team::RejectInvitationAsync(tid_, uid_, "", nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));

		this->Close();
	}
	else if(name == L"btn_accept")
	{
		QLOG_PRO(L"接受邀请");
		nim::Team::AcceptInvitationAsync(tid_, uid_, nbase::Bind(&TeamEventItem::TeamEventCb, msg_id_, std::placeholders::_1));

		this->Close();
	}
	else if(name == L"btn_ok")
	{
		this->Close();
	}
	return false;
}

void TeamNotifyForm::SetTid( const std::string &tid )
{
	tid_ = tid;
}

void TeamNotifyForm::SetAskJoinText()
{
	std::wstring user = UserService::GetInstance()->GetUserName(uid_);
	std::wstring team = TeamService::GetInstance()->GetTeamName(tid_);
	re_ask_join_->SetText( nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_APPLY_JOIN").c_str(), user.c_str(), team.c_str()) );
}

void TeamNotifyForm::OnAskJoin(const nim::SysMessage &json)
{
	msg_id_ = json.id_;
	type_ = TNP_ASK_JOIN;
	uid_ = json.sender_accid_;
	SetAskJoinText();
	GotoPage(0);
}

void TeamNotifyForm::SetInviteText()
{
	std::wstring team = TeamService::GetInstance()->GetTeamName(tid_);
	re_invite_->SetText(nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_ADMIN_INVITE_YOU").c_str(), team.c_str()));
}

void TeamNotifyForm::OnInviteYou(const nim::SysMessage &json)
{
	msg_id_ = json.id_;
	type_ = TNP_INVITE_YOU;
	uid_ = json.sender_accid_;
	nim::TeamInfo team_info;
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(json.attach_, values) && values.isObject())
	{
		if (values.isMember("attach"))
		{
			nim::Team::ParseTeamInfo(values[nim::kNIMNotificationKeyTeamInfo].toStyledString(), team_info);
			QLOG_APP(L"OnReceiveTeamInviteMsgWithAttach: {0}") << values["attach"].asString();
		}
		else
			nim::Team::ParseTeamInfo(json.attach_, team_info);
	}
	if (team_info.GetInviteMode() == nim::kNIMTeamInviteModeManager)
		re_invite_->SetText(nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_ADMIN_INVITE_YOU").c_str(), nbase::UTF8ToUTF16(team_info.GetName()).c_str()));
	else
		re_invite_->SetText(nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_INVITE_YOU").c_str(), nbase::UTF8ToUTF16(team_info.GetName()).c_str()));
	//SetInviteText();
	GotoPage(1);
}

void TeamNotifyForm::SetRejectInviteText()
{
	std::wstring user = UserService::GetInstance()->GetUserName(uid_);
	std::wstring team = TeamService::GetInstance()->GetTeamName(tid_);
	re_reject_->SetText(nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_REFUSE_INVITE").c_str(), user.c_str(), team.c_str()));
}

void TeamNotifyForm::OnRejectInvite(const nim::SysMessage &json)
{
	msg_id_ = json.id_;
	type_ = TNP_REJECT_INVITE;
	uid_ = json.sender_accid_;
	SetRejectInviteText();
	GotoPage(2);
}

void TeamNotifyForm::SetRejectJoinText()
{
	std::wstring team = TeamService::GetInstance()->GetTeamName(tid_);
	re_reject_->SetText(nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_TEAM_NOTIFY_REFUSE_APPLY").c_str(), team.c_str()));
}

void TeamNotifyForm::OnRejectJoin(const nim::SysMessage &json)
{
	msg_id_ = json.id_;
	type_ = TNP_REJECT_JOIN;
	uid_ = json.sender_accid_;
	SetRejectJoinText();
	GotoPage(2);
}

void TeamNotifyForm::RefreshText()
{
	if(type_ == TNP_ASK_JOIN)
		SetAskJoinText();
	//else if(type_ == TNP_INVITE_YOU)
		//SetInviteText();
	else if(type_ == TNP_REJECT_INVITE)
		SetRejectInviteText();
	else if(type_ == TNP_REJECT_JOIN)
		SetRejectJoinText();
}

void TeamNotifyForm::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (team_info.GetTeamID() == tid_)
	{
		RefreshText();
	}
}
}
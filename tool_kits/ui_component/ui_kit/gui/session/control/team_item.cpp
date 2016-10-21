#include "team_item.h"
#include "shared/ui/ui_menu.h"
#include "callback/team/team_callback.h"
#include "module/session/session_manager.h"

using namespace ui;

namespace nim_comp
{
void TeamItem::InitControl()
{
	this->AttachMenu(nbase::Bind(&TeamItem::OnItemMenu, this, std::placeholders::_1));

	member_icon_ = (Button*) this->FindSubControl(L"member_icon");
	member_name_ = (Label*) this->FindSubControl(L"member_name");

	icon_admin_ = this->FindSubControl(L"icon_admin");
	icon_mute_ = this->FindSubControl(L"icon_mute");
}

void TeamItem::InitInfo(const nim::TeamMemberProperty &info)
{
	team_id_ = info.GetTeamID();
	uid_ = info.GetAccountID();
	this->SetUTF8Name(info.GetAccountID());
	team_card_ = info.GetNick();
	SetMemberName(team_card_);
	member_icon_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(info.GetAccountID()));

	team_user_type_ = info.GetUserType();
	if (team_user_type_ == nim::kNIMTeamUserTypeCreator || team_user_type_ == nim::kNIMTeamUserTypeManager)
	{
		icon_admin_->SetBkImage(team_user_type_ == nim::kNIMTeamUserTypeCreator ? L"..\\public\\icon\\team_creator.png" : L"..\\public\\icon\\team_manager.png");
		icon_admin_->SetVisible(true);
	}
	else
		icon_admin_->SetVisible(false);

	is_mute_ = info.IsMute();
	if (info.IsMute())
	{
		icon_mute_->SetBkImage(L"..\\public\\icon\\mute.png");
		icon_mute_->SetVisible(true);
	}
}

void TeamItem::SetAdmin( bool admin )
{
	icon_admin_->SetBkImage(L"..\\public\\icon\\team_manager.png");
	icon_admin_->SetVisible(admin);
	if (admin) 
	{
		team_user_type_ = nim::kNIMTeamUserTypeManager;
	}
	else 
	{
		team_user_type_ = nim::kNIMTeamUserTypeNomal;
	}
}

void TeamItem::SetMute(bool mute)
{
	is_mute_ = mute;
	icon_mute_->SetBkImage(L"..\\public\\icon\\mute.png");
	icon_mute_->SetVisible(mute);
}

void TeamItem::SetOwner(bool is_owner)
{
	icon_admin_->SetBkImage(L"..\\public\\icon\\team_creator.png");
	icon_admin_->SetVisible(is_owner);
	if (is_owner) 
	{
		team_user_type_ = nim::kNIMTeamUserTypeCreator;
	}
	else 
	{
		team_user_type_ = nim::kNIMTeamUserTypeNomal;
	}
}

nim::NIMTeamUserType TeamItem::GetTeamUserType()
{
	return team_user_type_;
}

void TeamItem::SetMemberName(const std::string& team_card)
{
	if (NULL == member_name_ )
		return;

	if (!team_card.empty())
	{
		member_name_->SetUTF8Text(team_card);
		team_card_ = team_card;
	}
	else
		member_name_->SetText(UserService::GetInstance()->GetUserName(GetUTF8Name()));
}

std::string TeamItem::GetTeamCard() const
{
	return team_card_;
}

bool TeamItem::OnItemMenu(ui::EventArgs* arg)
{
	POINT point;
	::GetCursorPos(&point);
	PopupItemMenu(point);
	return true;
}

void TeamItem::PopupItemMenu(POINT point)
{
	CMenuWnd* pMenu = new CMenuWnd(NULL);
	STRINGorID xml(L"team_item_menu.xml");
	pMenu->Init(xml, _T("xml"), point);

	nim::NIMTeamUserType my_team_user_type = nim::kNIMTeamUserTypeNomal;
	SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(team_id_));
	if (session_box)
	{
		auto my_pro = session_box->GetTeamMemberInfo(LoginManager::GetInstance()->GetAccount());
		my_team_user_type = my_pro.GetUserType();
	}
	CMenuElementUI* mute_item = (CMenuElementUI*)pMenu->FindControl(L"mute");
	if (my_team_user_type == nim::kNIMTeamUserTypeNomal
		|| my_team_user_type == team_user_type_
		|| team_user_type_ == nim::kNIMTeamUserTypeCreator
		|| team_user_type_ == nim::kNIMTeamUserTypeApply
		|| team_user_type_ == nim::kNIMTeamUserTypeLocalWaitAccept)
		mute_item->SetEnabled(false);
	else
		mute_item->SetEnabled(true);

	if (is_mute_)
	{
		((Label*)(mute_item->FindSubControl(L"mute_text")))->SetText(L"解除禁言");
	}
	else
	{
		((Label*)(mute_item->FindSubControl(L"mute_text")))->SetText(L"禁言");
	}
	mute_item->AttachSelect(nbase::Bind(&TeamItem::TeamItemMenuItemClick, this, std::placeholders::_1));

	pMenu->Show();
}

bool TeamItem::TeamItemMenuItemClick(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"mute")
	{
		nim::Team::MuteMemberAsync(team_id_, uid_, !is_mute_, nbase::Bind(&TeamCallback::OnTeamEventCallback, std::placeholders::_1));
	}
	return true;
}

}
#include "team_item.h"


using namespace ui;

namespace nim_comp
{
void TeamItem::InitControl()
{
	member_icon_ = (Button*) this->FindSubControl(L"member_icon");
	member_name_ = (Label*) this->FindSubControl(L"member_name");

	icon_admin_ = this->FindSubControl(L"icon_admin");
}

void TeamItem::InitInfo(const nim::TeamMemberProperty &info)
{
	this->SetUTF8Name(info.GetAccountID());
	team_card_ = info.GetNick();

	nim::UserNameCard uinfo;
	UserService::GetInstance()->GetUserInfo(info.GetAccountID(), uinfo);
	SetMemberName(uinfo, team_card_);
	member_icon_->SetBkImage(UserService::GetInstance()->GetUserPhoto(info.GetAccountID()));

	team_user_type_ = info.GetUserType();
	if (team_user_type_ == nim::kNIMTeamUserTypeCreator || team_user_type_ == nim::kNIMTeamUserTypeManager)
	{
		icon_admin_->SetBkImage(team_user_type_ == nim::kNIMTeamUserTypeCreator ? L"..\\public\\icon\\team_creator.png" : L"..\\public\\icon\\team_manager.png");
		icon_admin_->SetVisible(true);
	}
	else
		icon_admin_->SetVisible(false);
}

void TeamItem::UpdateInfo(const std::string& team_card)
{
	nim::UserNameCard uinfo;
	SetMemberName(uinfo, team_card);
}

void TeamItem::OnUserInfoReady(const nim::UserNameCard& uinfo)
{
	if (GetUTF8Name() == member_name_->GetUTF8Text())
	{
		std::string team_card;
		SetMemberName(uinfo, team_card);
		member_icon_->SetBkImage(UserService::GetInstance()->GetUserPhoto(uinfo.GetAccId()));
	}
}

void TeamItem::SetAdmin( bool admin )
{
	icon_admin_->SetBkImage(L"..\\public\\icon\\team_manager.png");
	icon_admin_->SetVisible(admin);
	if (admin) {
		team_user_type_ = nim::kNIMTeamUserTypeManager;
	}
	else {
		team_user_type_ = nim::kNIMTeamUserTypeNomal;
	}
}

void TeamItem::SetOwner(bool is_owner)
{
	icon_admin_->SetBkImage(L"..\\public\\icon\\team_creator.png");
	icon_admin_->SetVisible(is_owner);
	if (is_owner) {
		team_user_type_ = nim::kNIMTeamUserTypeCreator;
	}
	else {
		team_user_type_ = nim::kNIMTeamUserTypeNomal;
	}
}

void TeamItem::SetTeamMember(const nim::TeamMemberProperty& team_member_info)
{
	this->SetUTF8Name(team_member_info.GetAccountID());

	nim::UserNameCard uinfo;
	UserService::GetInstance()->GetUserInfo(team_member_info.GetAccountID(), uinfo);

	SetMemberName(uinfo, team_member_info.GetNick());

	member_icon_->SetBkImage(UserService::GetInstance()->GetUserPhoto(uinfo.GetAccId()));

	icon_admin_->SetVisible(false);

}

nim::NIMTeamUserType TeamItem::GetTeamUserType()
{
	return team_user_type_;
}

void TeamItem::SetMemberName(const nim::UserNameCard& uinfo, const std::string& team_card)
{
	if (NULL == member_name_ )
		return;
	if (uinfo.GetAccId().empty() && team_card.empty())
		return;
	
	if (!team_card.empty())
	{
		member_name_->SetUTF8Text(team_card);
		team_card_ = team_card;
	}
	else if (team_card.empty() && !team_card_.empty())
	{
		member_name_->SetUTF8Text(team_card_);
	}
	if (team_card.empty() && team_card_.empty())
	{
		member_name_->SetText(UserService::GetInstance()->GetUserName(uinfo.GetAccId()));
	}

}

std::string TeamItem::GetTeamCard() const
{
	return team_card_;
}
}
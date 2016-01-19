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
	SetMemberName(team_card_);
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
}
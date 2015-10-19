#include "friend_item.h"

using namespace ui;


FriendItem::FriendItem()
{

}

FriendItem::~FriendItem()
{

}

void FriendItem::Init(bool is_team, const UserInfo &user_info)
{
	this->AttachDoubleClick(nbase::Bind(&FriendItem::OnDbClicked, this, std::placeholders::_1));

	contact_ = (Label*) this->FindSubControl(L"contact");

	is_team_ = is_team;
	id_ = user_info.account;

	SetUTF8Name(id_);
	SetUTF8DataID(id_);

	if(is_team)
		nick_name_ = nim_ui::TeamManager::GetInstance()->GetTeamName(id_);
	else
		nick_name_ = nim_ui::UserManager::GetInstance()->GetUserName(id_);
	contact_->SetText(nick_name_);

	auto head_ctrl = FindSubControl(L"head_image");
	if(is_team)
		head_ctrl->SetBkImage(nim_ui::TeamManager::GetInstance()->GetTeamPhoto(false));
	else
		head_ctrl->SetBkImage(nim_ui::UserManager::GetInstance()->GetUserPhoto(user_info.account));

	nick_name_ = nbase::MakeLowerString(nick_name_);
	nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
	nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));

	if(is_team_)
		unregister_cb.Add(nim_ui::TeamManager::GetInstance()->RegChangeTeamName(nbase::Bind(&FriendItem::OnTeamNameChange, this, std::placeholders::_1)));
}

UserInfo FriendItem::GetUserInfo()
{
	UserInfo uinfo;
	uinfo.account = id_;
	if(!is_team_)
	{
		nim_ui::UserManager::GetInstance()->GetUserInfo(id_, uinfo);
	}
	return uinfo;
}

bool FriendItem::OnDbClicked(ui::EventArgs* arg)
{
	nim_ui::SessionManager::GetInstance()->OpenSessionForm(id_, is_team_ ? nim::kNIMSessionTypeTeam : nim::kNIMSessionTypeP2P);
	return true;
}

void FriendItem::OnTeamNameChange(const nim::TeamInfo& user_info)
{
	if (user_info.id == id_)
	{
		contact_->SetUTF8Text(user_info.name);

		nick_name_ = nbase::UTF8ToUTF16(user_info.name);
		nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
		nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));
	}
}



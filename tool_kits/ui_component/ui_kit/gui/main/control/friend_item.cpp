#include "friend_item.h"
#include "module/session/session_manager.h"

using namespace ui;

namespace nim_comp
{
FriendItem::FriendItem()
{

}

FriendItem::~FriendItem()
{

}

void FriendItem::Init(bool is_team, const nim::UserNameCard &user_info)
{
	this->AttachDoubleClick(nbase::Bind(&FriendItem::OnDbClicked, this, std::placeholders::_1));

	contact_ = (Label*) this->FindSubControl(L"contact");

	is_team_ = is_team;
	id_ = user_info.GetAccId();

	SetUTF8Name(id_);
	SetUTF8DataID(id_);

	if(is_team)
		nick_name_ = TeamService::GetInstance()->GetTeamName(id_);
	else
		nick_name_ = UserService::GetInstance()->GetUserName(id_);
	contact_->SetText(nick_name_);

	auto head_ctrl = FindSubControl(L"head_image");
	if(is_team)
		head_ctrl->SetBkImage(TeamService::GetInstance()->GetTeamPhoto(false));
	else
		head_ctrl->SetBkImage(UserService::GetInstance()->GetUserPhoto(user_info.GetAccId()));

	nick_name_ = nbase::MakeLowerString(nick_name_);
	nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
	nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));

	if(is_team_)
		unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(nbase::Bind(&FriendItem::OnTeamNameChange, this, std::placeholders::_1)));
}

nim::UserNameCard FriendItem::GetUserInfo()
{
	nim::UserNameCard uinfo;
	uinfo.SetAccId(id_);
	if(!is_team_)
	{
		UserService::GetInstance()->GetUserInfo(id_, uinfo);
	}
	return uinfo;
}

bool FriendItem::OnDbClicked(ui::EventArgs* arg)
{
	SessionManager::GetInstance()->OpenSessionForm(id_, is_team_ ? nim::kNIMSessionTypeTeam : nim::kNIMSessionTypeP2P);
	return true;
}

void FriendItem::OnTeamNameChange(const nim::TeamInfo& user_info)
{
	if (user_info.GetTeamID() == id_)
	{
		contact_->SetUTF8Text(user_info.GetName());

		nick_name_ = nbase::UTF8ToUTF16(user_info.GetName());
		nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
		nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));
	}
}
}
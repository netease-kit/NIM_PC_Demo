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

void FriendItem::Init(FriendItemType type, const std::string &accid)
{
	this->AttachDoubleClick(nbase::Bind(&FriendItem::OnDbClicked, this, std::placeholders::_1));

	label_online_state_ = (Label*) this->FindSubControl(L"online_state");
	contact_ = (Label*) this->FindSubControl(L"contact");

	type_ = type;
	id_ = accid;

	SetUTF8Name(id_);
	SetUTF8DataID(id_);

	if (type_ != kFriendItemTypeRobot)
	{
		if (type_ == kFriendItemTypeTeam)
			nick_name_ = TeamService::GetInstance()->GetTeamName(id_);
		else
			nick_name_ = UserService::GetInstance()->GetUserName(id_);
	}
	else
	{
		nim::RobotInfo info;
		UserService::GetInstance()->GetRobotInfo(id_, info);
		nick_name_ = nbase::UTF8ToUTF16(info.GetName());
	}
	contact_->SetText(nick_name_);

	auto head_ctrl = FindSubControl(L"head_image");
	if (type_ == kFriendItemTypeTeam)
		head_ctrl->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(id_, false));
	else
		head_ctrl->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(accid, type_ == kFriendItemTypeRobot));

	nick_name_ = nbase::MakeLowerString(nick_name_);
	nick_name_full_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name_));
	nick_name_simple_spell_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name_));

	if (SubscribeEventManager::GetInstance()->IsEnabled())
	{
		if (type_ == kFriendItemTypeP2P)
		{
			EventDataEx data;
			SubscribeEventManager::GetInstance()->GetEventData(nim::kNIMEventTypeOnlineState, accid, data);
			SetOnlineState(data);
		}
		else if (type_ == kFriendItemTypeRobot)
		{
			EventDataEx data;
			data.online_client_.online_client_type_.insert(nim::kNIMClientTypeDefault);
			SetOnlineState(data);
		}
	}
}

bool FriendItem::Match(const UTF8String& search_key)
{
	std::wstring ws_search_key = nbase::UTF8ToUTF16(search_key);
	ws_search_key = nbase::MakeLowerString(ws_search_key);
	if (nick_name_.find(ws_search_key) != std::wstring::npos
		|| nick_name_full_spell_.find(search_key) != UTF8String::npos
		|| nick_name_simple_spell_.find(search_key) != UTF8String::npos)
	{
		return true;
	}
	return false;
}

void FriendItem::SetOnlineState(const EventDataEx& data)
{
	label_online_state_->SetText(OnlineStateEventHelper::GetOnlineState(data.online_client_, data.multi_config_, true));
}

bool FriendItem::OnDbClicked(ui::EventArgs* arg)
{
	SessionManager::GetInstance()->OpenSessionBox(id_, type_ == kFriendItemTypeTeam ? nim::kNIMSessionTypeTeam : nim::kNIMSessionTypeP2P);
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
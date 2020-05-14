#include "at_list_form.h"
#include "util/windows_manager.h"
#include "module/login/login_manager.h"
#include "module/runtime_data/runtime_data_manager.h"
using namespace ui;

namespace nim_comp
{
const LPCTSTR AtlistForm::kClassName = L"AtlistForm";

AtlistForm::AtlistForm(std::string session_id, nim::NIMSessionType session_type, OnSelectAtItem cb)
{
	session_id_ = session_id;
	session_type_ = session_type;
	callback_select_ = cb;
	team_members_container_ = nullptr;

	ASSERT(!session_id_.empty());
}

AtlistForm::~AtlistForm()
{

}

std::wstring AtlistForm::GetSkinFolder()
{
    return L"session";
}

std::wstring AtlistForm::GetSkinFile()
{
    return L"at_list_form.xml";
}

std::wstring AtlistForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring AtlistForm::GetWindowId() const
{
	return nbase::UTF8ToUTF16(session_id_);
}

void AtlistForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(kEventReturn, nbase::Bind(&AtlistForm::OnSelectItem, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(kEventClick, nbase::Bind(&AtlistForm::OnSelectItem, this, std::placeholders::_1));

	team_members_container_ = (ui::VirtualListBox *)FindControl(L"team_list_box");
	int kRoomMemberItemHeight = 40;
	team_members_container_->SetElementHeight(ui::DpiManager::GetInstance()->ScaleInt(kRoomMemberItemHeight));
	team_members_container_->SetDataProvider(this);
	team_members_container_->InitElement(30);

	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(nbase::Bind(&AtlistForm::OnUserInfoChange, this, std::placeholders::_1)));
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(nbase::Bind(&AtlistForm::OnFriendInfoChange, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(nbase::Bind(&AtlistForm::OnUserPhotoChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

	if (session_type_ == nim::kNIMSessionTypeTeam)
	{
		unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamMember(nbase::Bind(&AtlistForm::OnTeamCardChange, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegAddTeamMember(nbase::Bind(&AtlistForm::OnTeamMemberAdd, this, std::placeholders::_1, std::placeholders::_2)));
		unregister_cb.Add(TeamService::GetInstance()->RegRemoveTeamMember(nbase::Bind(&AtlistForm::OnTeamMemberRemove, this, std::placeholders::_1, std::placeholders::_2)));
	}
}

void AtlistForm::InitTeamMembers(const std::map<std::string, std::shared_ptr<nim::TeamMemberProperty>>& team_member_info_list)
{
	team_member_info_list_ = team_member_info_list;
	team_member_info_list_.erase(LoginManager::GetInstance()->GetAccount());
	RefrashShowListData();
	team_members_container_->RemoveAll();
	team_members_container_->Refresh();
	team_members_container_->SetVisible(team_member_info_list_.size() > 0);
}

void AtlistForm::RemoveListItem(const std::string& uid)
{
	std::wstring wstr_uid = nbase::UTF8ToUTF16(uid);
	if (wstr_uid.empty())
		return;

	// 同时删除最近5个发言人里的子项
	AtListItem* five_item = FindInLastFiveSender(wstr_uid);
	if (five_item != NULL)
	{
		team_members_container_->Remove(five_item);
	}

	AtListItem* item = (AtListItem*)team_members_container_->FindSubControl(wstr_uid);
	if (item != NULL)
	{
		team_members_container_->Remove(item);
	}

	int member_count = team_members_container_->GetCount();
	if (member_count == 0)
	{
		team_members_container_->SetVisible(false);
		FindControl(L"team_member")->SetVisible(false);
	}
}

void AtlistForm::CheckListItemHeadIcon(const std::string& uid, const std::wstring& photo_path)
{
	std::wstring wuid = nbase::UTF8ToUTF16(uid);

	// 同时更新最近5个发言人里的子项的头像
	AtListItem* five_item = FindInLastFiveSender(wuid);
	if (five_item != NULL)
	{
		five_item->SetHeadImage(photo_path);
	}

	AtListItem *item = (AtListItem*)FindControl(wuid);
	if (item != NULL)
	{
		item->SetHeadImage(photo_path);
	}
}

nim_comp::AtListItem* AtlistForm::CreateAtListItem(const std::string& uid, bool is_last_five)
{
	AtListItem* item = new AtListItem(is_last_five);
	GlobalManager::FillBoxWithCache(item, L"session\\at_list\\at_list_item.xml");

	item->InitControls();
	item->SetUserID(uid);
	item->SetSessionId(session_id_);

	std::wstring show_name;

	std::wstring alias = UserService::GetInstance()->GetFriendAlias(uid);
	if (!alias.empty())
	{
		show_name = alias;
		item->SetAliasName(alias);
	}

	if (session_type_ == nim::kNIMSessionTypeTeam)
	{
		std::wstring card_name = GetTeamCardName(uid);
		if (!card_name.empty())
		{
			if (show_name.empty())
				show_name = card_name;
			item->SetTeamCardName(card_name);
		}
	}

	std::wstring nick = UserService::GetInstance()->GetUserName(uid, false);
	if (!nick.empty())
	{
		if (show_name.empty())
			show_name = nick;
		item->SetNickName(nick);
	}

	item->SetShowName(show_name);

	return item;
}

bool AtlistForm::OnSelectItem(EventArgs *param)
{
	//GetDataID中保存了控件的名字，GetName是控件的uid
	if (param->pSender->GetDataID() == L"at_list_item")
	{
		//判断不应该是自己
		AtListItem *item = static_cast<AtListItem*>(param->pSender);
		if (item != NULL)
		{
			if (callback_select_)
			{
				std::string id = item->GetUserID();
				StdClosure closure = nbase::Bind(callback_select_, id/*item->GetUserID()*/);
				nbase::ThreadManager::PostTask(closure);
			}
		}
		this->ShowWindow(false, false);
	}
	return true;

}

void AtlistForm::RemoveLastFiveSender()
{
	for (size_t i = 0; i < uid_last_five_.size(); ++i)
	{
		Control *item = team_members_container_->GetItemAt(0);
		if (item != NULL)
		{
			team_members_container_->RemoveAt(0);
		}
	}
	uid_last_five_.clear();
}

AtListItem* AtlistForm::FindInLastFiveSender(const std::wstring& uid)
{
	if (!uid_last_five_.empty())
	{
		for (size_t i = 0; i < uid_last_five_.size(); ++i)
		{
			Control* item = team_members_container_->GetItemAt(i);
			if (item != NULL)
			{
				if (item->GetDataID() == uid)
				{
					return (AtListItem*)item;
					break;
				}
			}
		}
	}

	return NULL;
}

std::wstring AtlistForm::GetTeamCardName(const std::string& uid)
{
	auto i = team_member_info_list_.find(uid);
	if (i != team_member_info_list_.end())
	{
		return nbase::UTF8ToUTF16(i->second->GetNick());
	}

	return L"";
}

std::wstring AtlistForm::GetShowName(const std::string& uid)
{	
	std::wstring alias = UserService::GetInstance()->GetFriendAlias(uid);
	if (!alias.empty())
		return alias;

	std::wstring nick = UserService::GetInstance()->GetUserName(uid, false);

	std::wstring card_name = GetTeamCardName(uid);
	if (!card_name.empty())
		return card_name;

	return nick;
}

LRESULT AtlistForm::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//不允许关闭@列表，只能隐藏。只有对应的会话窗体关闭时才能直接销毁@列表窗口
	ShowWindow(false, false);
	bHandled = true;
	return 0;
}

bool AtlistForm::Match(const std::wstring& search_key, bool match_visible)
{
	std::wstring low_search_key = nbase::MakeLowerString(search_key);
	if (low_search_key.empty())
		return false;
	team_member_match_list_.clear();
	for (auto it : team_member_sort_list_)
	{
		if (it->Match(nbase::UTF16ToUTF8(search_key)))
			team_member_match_list_.emplace_back(it);
	}
	team_members_container_->Refresh();
	return team_member_match_list_.size() > 0;
	

	// 匹配时删除之前的最近5个发言人
	RemoveLastFiveSender();

	std::string utf8_low_search_key = nbase::UTF16ToUTF8(low_search_key);

	int count = team_members_container_->GetCount();
	int visible_count = 0;

	for (int i = uid_last_five_.size(); i < count; i++)
	{
		AtListItem *item = static_cast<AtListItem*>(team_members_container_->GetItemAt(i));
		if (item == NULL)
			 continue;
		
		if (match_visible && !item->IsVisible())
			continue;
	
		if (item->Match(utf8_low_search_key))
		{
			visible_count++;
			item->SetVisible(true);
		}			
		else
			item->SetVisible(false);
	}

	if (visible_count == 0)
		return false;
	else
		return true;
}

void AtlistForm::ShowMemberItems(std::list<std::string> &last_five)
{
	UpdateLastFive(last_five);
	RefrashShowListData();
	team_members_container_->Refresh();
	team_members_container_->SetVisible(team_member_sort_list_.size() > 0);
}
void AtlistForm::ShowWindow(bool bShow, bool bTakeFocus)
{
	WindowEx::ShowWindow(bShow, bTakeFocus);
	auto task = ToWeakCallback([this]() {
		team_member_match_list_.clear();
		if (team_members_container_ != nullptr)
			team_members_container_->Refresh();
	});
	if (!bShow)
	{
		Post2UI(task);
	}
		
	RunTimeDataManager::GetInstance()->SetAttingSomeOne(bShow);
}
void AtlistForm::SetShowPos(POINT pt)
{
	//第一个默认选中
	int count = team_members_container_->GetCount();
	if (count == 0)
	{
		this->ShowWindow(false, false);
		return;
	}

	for (int i = 0; i < count; i++)
	{
		Control* control = team_members_container_->GetItemAt(i);
		if (control->IsVisible())
		{
			AtListItem *item = static_cast<AtListItem*>(control);
			item->Selected(true, false);
			break;
		}
	}

	ui::UiRect rc(pt.x, pt.y, 0, 0);
	this->SetPos(rc, false, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);	

	//强制刷新一下，否则列表项的选中状态会有问题
	::InvalidateRect(m_hWnd,NULL, FALSE);
	RunTimeDataManager::GetInstance()->SetAttingSomeOne(true);
}

bool AtlistForm::HandleMouseWheel(bool is_up)
{
	if (::IsWindowVisible(m_hWnd))
	{
		if (is_up)
		{
			team_members_container_->SelectItem(team_members_container_->FindSelectable(team_members_container_->GetCurSel() - 1, false), false);
		} 
		else
		{
			team_members_container_->SelectItem(team_members_container_->FindSelectable(team_members_container_->GetCurSel() + 1, true), false);
		}

		return true;
	}
	return false;
}

bool AtlistForm::HandleKeyDown()
{
	if (::IsWindowVisible(m_hWnd))
	{
		team_members_container_->SelectItem(team_members_container_->FindSelectable(team_members_container_->GetCurSel() + 1, true), false);
		return true;
	}
	return false;
}

bool AtlistForm::HandleKeyUp()
{
	if (::IsWindowVisible(m_hWnd))
	{
		team_members_container_->SelectItem(team_members_container_->FindSelectable(team_members_container_->GetCurSel() - 1, false), false);
		return true;
	}
	return false;
}

bool AtlistForm::HandleKeyEnter()
{
	if (::IsWindowVisible(m_hWnd))
	{
		Control *select_item = nullptr;
		select_item = team_members_container_->GetItemAt(team_members_container_->GetCurSel());
		if (NULL != select_item)
		{
			select_item->Activate();
		}
		return true;
	}
	return false;
}

void AtlistForm::CloseForm()
{
	::DestroyWindow(m_hWnd);
}

ui::Control* AtlistForm::CreateElement()
{
	AtListItem* item = new AtListItem();
	GlobalManager::FillBoxWithCache(item, L"session\\at_list\\at_list_item.xml");
	return item;	
}

void AtlistForm::FillElement(ui::Control* control, int index)
{
	std::vector<std::shared_ptr<ItemMatcher>>* team_member_list = nullptr;
	if (team_member_match_list_.size() > 0)
		team_member_list = &team_member_match_list_;
	else
		team_member_list = &team_member_sort_list_;
	AtListItem* item = (AtListItem *)control;
	std::string uid = team_member_list->at(index)->uid_;
	bool is_last_five = uid_last_five_for_fined_.find(uid) != uid_last_five_for_fined_.end();
	item->InitControls();
	item->SetUserID(uid, is_last_five);
	item->SetSessionId(session_id_);

	std::wstring show_name;
	std::wstring alias = UserService::GetInstance()->GetFriendAlias(uid);
	if (!alias.empty())
	{
		show_name = alias;
		item->SetAliasName(alias);
	}

	if (session_type_ == nim::kNIMSessionTypeTeam)
	{
		std::wstring card_name = GetTeamCardName(uid);
		if (!card_name.empty())
		{
			if (show_name.empty())
				show_name = card_name;
			item->SetTeamCardName(card_name);
		}
	}

	std::wstring nick = UserService::GetInstance()->GetUserName(uid, false);
	if (!nick.empty())
	{
		if (show_name.empty())
			show_name = nick;
		item->SetNickName(nick);
	}
	std::wstring photo = PhotoService::GetInstance()->GetUserPhoto(uid);
	item->SetHeadImage(photo);
	item->SetShowName(show_name);
}

int AtlistForm::GetElementtCount()
{
	std::vector<std::shared_ptr<ItemMatcher>>* team_member_list = nullptr;
	if (team_member_match_list_.size() > 0)
		team_member_list = &team_member_match_list_;
	else
		team_member_list = &team_member_sort_list_;
	return team_member_list->size();
}

void AtlistForm::OnUserPhotoChange(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	if (type == kTeam)
		return;

	if (team_member_info_list_.find(accid) != team_member_info_list_.end())
	{
		CheckListItemHeadIcon(accid, photo_path);
		return;
	}
}

void AtlistForm::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto info : uinfos)
	{
		std::wstring wuid = nbase::UTF8ToUTF16(info.GetAccId());
		AtListItem *item = (AtListItem *)FindControl(wuid.c_str());
		if (item == NULL)
			return;

		// 同时更新最近5个发言人里的子项的昵称
		AtListItem* five_item = FindInLastFiveSender(wuid);

		if (team_member_info_list_.find(info.GetAccId()) != team_member_info_list_.end())
		{		
			std::wstring nick = nbase::UTF8ToUTF16(info.GetName());
			item->SetNickName(nick);
			if (five_item != NULL)
				five_item->SetNickName(nick);

			if (LoginManager::GetInstance()->IsEqual(info.GetAccId()))
			{
				std::wstring card_name = GetTeamCardName(info.GetAccId());
				if (!card_name.empty())
				{
					item->SetShowName(card_name);
					if (five_item != NULL)
						five_item->SetShowName(card_name);
				}
				else
				{
					item->SetShowName(nick);
					if (five_item != NULL)
						five_item->SetShowName(nick);
				}
			}
			else
			{
				std::wstring final_name = GetShowName(info.GetAccId());
				item->SetShowName(final_name);
				if (five_item != NULL)
					five_item->SetShowName(final_name);
			}
		}
	}
}

void AtlistForm::OnFriendInfoChange(FriendChangeType change_type, const std::string& accid)
{
	std::wstring wuid = nbase::UTF8ToUTF16(accid);
	AtListItem *item = (AtListItem *)FindControl(wuid.c_str());
	if (item == NULL)
		return;

	// 同时更新最近5个发言人里的子项的备注名
	AtListItem* five_item = FindInLastFiveSender(wuid);

	if (team_member_info_list_.find(accid) != team_member_info_list_.end())
	{
		std::wstring alias = UserService::GetInstance()->GetFriendAlias(accid);
		if (alias.empty())
			return;

		std::wstring final_name = GetShowName(accid);
		item->SetShowName(final_name);
		item->SetAliasName(alias);
		if (five_item != NULL)
		{
			five_item->SetShowName(final_name);
			five_item->SetAliasName(alias);
		}
	}
}

void AtlistForm::OnTeamCardChange(const std::string& tid_uid, const std::string& team_card)
{
	size_t splitter = tid_uid.find_first_of('#');
	std::string tid = tid_uid.substr(0, splitter), uid = tid_uid.substr(splitter + 1);

	if (tid == session_id_)
	{
		auto i = team_member_info_list_.find(uid);
		if (i != team_member_info_list_.end())
		{
			i->second->SetNick(team_card);
		}

		std::wstring wuid = nbase::UTF8ToUTF16(uid);
		AtListItem *item = (AtListItem *)FindControl(wuid.c_str());
		if (item == NULL)
			return;

		// 同时更新最近5个发言人里的子项的群名片
		std::wstring wteam_card = nbase::UTF8ToUTF16(team_card);
		AtListItem* five_item = FindInLastFiveSender(wuid);	
		item->SetTeamCardName(wteam_card);
		if (five_item != NULL)
		{
			five_item->SetTeamCardName(wteam_card);
		}

		if (LoginManager::GetInstance()->IsEqual(uid))
		{		
			item->SetShowName(wteam_card);
			if (five_item != NULL)
				five_item->SetShowName(wteam_card);
		}
		else
		{
			std::wstring final_name = GetShowName(uid);
			item->SetShowName(final_name);
			if (five_item != NULL)
				five_item->SetShowName(final_name);
		}
	}
}

void AtlistForm::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	if (tid == session_id_)
	{
		std::string uid = team_member_info.GetAccountID();

		auto i = team_member_info_list_.find(uid);
		if (i != team_member_info_list_.end()) //已添加
			return;

		team_member_info_list_[uid] = std::make_shared<nim::TeamMemberProperty>( team_member_info);
		RefrashShowListData();
		team_members_container_->Refresh();
	}
}

void AtlistForm::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
{
	if (tid == session_id_)
	{
		auto i = team_member_info_list_.find(uid);
		if (i == team_member_info_list_.end()) //已从群中删除
			return;

		team_member_info_list_.erase(uid);
		RemoveListItem(uid);
	}
}
void AtlistForm::RefrashShowListData()
{
	auto init_match_fun = [this](const std::shared_ptr< ItemMatcher>& item,const std::shared_ptr<nim::TeamMemberProperty>& member_info) {
		auto uid = member_info->GetAccountID();
		item->uid_ = uid;
		std::wstring alias = UserService::GetInstance()->GetFriendAlias(uid);
		if (!alias.empty())
			item->SetAliasName(alias);
		std::wstring nick = UserService::GetInstance()->GetUserName(uid, false);
		if (!nick.empty())
			item->SetNickName(nick);
		std::wstring card_name = nbase::UTF8ToUTF16(member_info->GetNick());
		if (!card_name.empty())
			item->SetTeamCardName(card_name);
	};
	team_member_sort_list_.clear();
	team_member_sort_list_.reserve(team_member_info_list_.size());	
	for (auto it = uid_last_five_.rbegin(); it != uid_last_five_.rend(); it++)
	{
		auto member_info_it = team_member_info_list_.find(*it);
		if (member_info_it != team_member_info_list_.end())
		{
			auto item = std::make_shared< ItemMatcher>();
			init_match_fun(item, member_info_it->second);			
			team_member_sort_list_.emplace_back(item);
		}			
	}
	for (auto it : team_member_info_list_)
	{
		if (uid_last_five_for_fined_.find(it.first) == uid_last_five_for_fined_.end())
		{
			auto item = std::make_shared< ItemMatcher>();
			init_match_fun(item, it.second);
			team_member_sort_list_.emplace_back(item);
		}
	}
}
void AtlistForm::UpdateLastFive(const std::list<std::string>& last_five)
{
	uid_last_five_.clear();
	uid_last_five_for_fined_.clear();
	for (auto it : last_five)
	{
		if (team_member_info_list_.find(it) != team_member_info_list_.end())
			uid_last_five_.emplace_back(it);
	}
	for (auto it : uid_last_five_)
	{
		uid_last_five_for_fined_[it] = it;
	}
}
}
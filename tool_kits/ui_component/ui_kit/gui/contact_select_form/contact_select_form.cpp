#include "contact_select_form.h"
#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "shared/tool.h"
#include "module/login/login_manager.h"
#include "module/service/mute_black_service.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
void ContactSelectForm::AddGroup(ui::TreeNode* tree_node)
{
	tree_node->SetEnabled(false);
	contract_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node->SetVisible(false);
	TreeNode* tree_node2 = new TreeNode();

	ContactTileListUI* tile_layout = new ContactTileListUI;
	ui::GlobalManager::FillBox(tile_layout, L"contact_select/contact_select_tile_list.xml");

	RECT rect = { 5, 5, 5, 5 };
	tree_node2->GetLayout()->SetPadding(rect);
	tree_node2->Add(tile_layout);
	tree_node2->SetFixedWidth(DUI_LENGTH_AUTO);
	tree_node2->SetFixedHeight(DUI_LENGTH_AUTO);

	tree_node->AddChildNode(tree_node2);
	tree_node_ver_.push_back(tile_layout);
}

void ContactSelectForm::AddFriendListItem(const std::string& accid, bool is_enable)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(accid);
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ContactTileListUI* tile_layout;
	if (!ws_spell.empty())
	{
		tile_layout = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tile_layout = GetGroup(GT_COMMON_NUMBER);
	}

	ListContainerElement* contain = (ListContainerElement*)AddListItemInGroup(accid, false, tile_layout);
	if (contain)
	{
		//contain->AttachSelect(notify);
		CheckBox* ckb = (CheckBox*)contain->FindSubControl(L"checkbox");
		if (!is_enable)
		{
			ckb->Selected(true);
			contain->SetEnabled(false);
		}
	}
}

void ContactSelectForm::RemoveFriendListItem(const std::string& accid)
{
	wstring ws_show_name = UserService::GetInstance()->GetUserName(accid);
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ContactTileListUI* tile_layout;
	if (!ws_spell.empty())
	{
		tile_layout = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tile_layout = GetGroup(GT_COMMON_NUMBER);
	}
	RemoveListItemInGroup(accid, tile_layout);

	//从已选列表和搜索列表中删除
	auto selected_item = selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (selected_item != NULL)
		selected_user_list_->Remove(selected_item);
	auto search_item = search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (search_item != NULL)
		search_result_list_->Remove(search_item);
}

void ContactSelectForm::AddTeamListItem(const std::string& teamid, bool is_enable)
{
	ContactTileListUI* tile_layout = GetGroup(GT_TEAM);

	ListContainerElement* contain = (ListContainerElement*)AddListItemInGroup(teamid, true, tile_layout);
	if (contain)
	{
		CheckBox* check = (CheckBox*)contain->FindSubControl(L"checkbox");
		if (!is_enable)
		{
			check->Selected(true);
			contain->SetEnabled(false);
		}
	}
}

void ContactSelectForm::RemoveTeamListItem(const std::string& teamid)
{
	ContactTileListUI* tile_layout = GetGroup(GT_TEAM);
	RemoveListItemInGroup(teamid, tile_layout);

	//从已选列表和搜索列表中删除
	auto selected_item = selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(teamid));
	if (selected_item != NULL)
		selected_user_list_->Remove(selected_item);
	auto search_item = search_result_list_->FindSubControl(nbase::UTF8ToUTF16(teamid));
	if (search_item != NULL)
		search_result_list_->Remove(search_item);
}

ui::Box* ContactSelectForm::AddListItemInGroup(const std::string& accid, bool is_team, ContactTileListUI* tile_layout)
{
	if (tile_layout->GetCount() == 0)
	{
		((TreeNode*)tile_layout->GetParent())->GetParentNode()->SetVisible(true);
	}

	ContactListItemUI* container_element = CreateListItem(accid, is_team);
	int index = 0;
	for (index = 0; index < tile_layout->GetCount(); index++)
	{
		ContactListItemUI* temp = (ContactListItemUI*)tile_layout->GetItemAt(index);
		if (container_element->GetDataID() == temp->GetDataID())
		{
			//ASSERT(FALSE);
			return NULL;
		}
		if (*((ListItemUserData*)container_element) < *((ListItemUserData*)temp))
		{
			tile_layout->AddAt(container_element, index);
			break;
		}
	}

	if (index == tile_layout->GetCount())
	{
		tile_layout->AddAt(container_element, index);
	}

	return container_element;
}

bool ContactSelectForm::RemoveListItemInGroup(const std::string& accid, ContactTileListUI* tile_layout)
{
	bool ret = false;
	int index = 0;
	for (index = 0; index < tile_layout->GetCount(); index++)
	{
		ContactListItemUI* temp = (ContactListItemUI*)tile_layout->GetItemAt(index);
		if (accid == temp->GetUTF8DataID())
		{
			OnCheckBox(accid, temp->IsTeam(), false); //同时从已选择列表中删除这个帐号
			tile_layout->RemoveAt(index);
			ret = true;
			break;
		}
	}

	if (tile_layout->GetCount() == 0)
	{
		((TreeNode*)tile_layout->GetParent())->GetParentNode()->SetVisible(false);
	}

	return ret;
}

ContactListItemUI* ContactSelectForm::CreateListItem(const std::string& accid, bool is_team)
{
	ContactListItemUI* container_element = new ContactListItemUI();
	container_element->Init(accid, is_team);
	
	if (std::find(exclude_ids_.begin(), exclude_ids_.end(), accid) != exclude_ids_.end())
	{
		((ui::CheckBox*)container_element->FindSubControl(L"checkbox"))->Selected(true);
		container_element->SetEnabled(false);
	}

	return container_element;
}

SelectedContactItemUI* ContactSelectForm::CreateSelectedListItem(const std::string& accid, bool is_team)
{
	SelectedContactItemUI* selected_item = new SelectedContactItemUI();
	selected_item->Init(accid, is_team);

	Button* btn_delete = (Button*)selected_item->FindSubControl(L"delete");
	btn_delete->AttachClick(nbase::Bind(&ContactSelectForm::OnBtnDeleteClick, this, accid, std::placeholders::_1));

	return selected_item;
}

ContactTileListUI* ContactSelectForm::GetGroup(GroupType groupType, wchar_t letter)
{
	return tree_node_ver_[ListItemUtil::GetGroup(groupType, letter)];
}

void ContactSelectForm::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == accid)
		return;

	if (change_type == kChangeTypeAdd)
	{
		if (UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal
			&& !MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			AddFriendListItem(accid, true);
		}
	}
	else if (change_type == kChangeTypeDelete)
	{
		RemoveFriendListItem(accid);
	}
}

void ContactSelectForm::OnSetBlackCallback(const std::string& id, bool black)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	if (current_user_id == id)
		return;

	if (black)
		RemoveFriendListItem(id);
	else
		AddFriendListItem(id, true);
}

void ContactSelectForm::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	UserService* user_service = UserService::GetInstance();
	for (auto info : uinfos)
	{
		std::string accid = info.GetAccId();
		if (user_service->GetUserType(accid) == nim::kNIMFriendFlagNormal && !MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			//好友列表项
			ContactListItemUI* friend_item = dynamic_cast<ContactListItemUI*>(contract_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			if(!friend_item)
				continue;

			//搜索列表项
			ContactListItemUI* search_item = NULL;
			if(search_result_list_->IsVisible())
				search_item = dynamic_cast<ContactListItemUI*>(search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));

			//选中列表项
			SelectedContactItemUI* select_item = dynamic_cast<SelectedContactItemUI*>(selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			
			if (info.ExistValue(nim::kUserNameCardKeyName))
			{
				bool enabled = friend_item->IsEnabled();
				bool checked = ((CheckBox*)friend_item->FindSubControl(L"checkbox"))->IsSelected();
				bool is_team = friend_item->IsTeam();

				Box* tile_box = friend_item->GetParent();
				tile_box->Remove(friend_item); //删除
				if (tile_box->GetCount() == 0) //该字母下没有item了，隐藏bar
					dynamic_cast<TreeNode*>(tile_box->GetParent())->GetParentNode()->SetVisible(false);

				AddFriendListItem(accid, enabled); //重新添加
				friend_item = dynamic_cast<ContactListItemUI*>(contract_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
				((CheckBox*)friend_item->FindSubControl(L"checkbox"))->Selected(checked);
				
				if (search_item)
				{
					UTF8String search_key = search_edit_->GetUTF8Text();
					if (friend_item->Match(search_key)) //名字变了但依然符合搜索条件
						search_item->Init(accid, is_team);
					else
						search_result_list_->Remove(search_item);
				}

				if (select_item)
					((Label*)select_item->FindSubControl(L"show_name"))->SetText(user_service->GetUserName(accid));
			}
			
			if(info.ExistValue(nim::kUserNameCardKeyIconUrl))
			{
				friend_item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(accid));

				if(search_item)
					search_item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(accid));
			}
		}
	}
}

void ContactSelectForm::OnUserPhotoReady(PhotoType type, const std::string & accid, const std::wstring & photo_path)
{
	if (type == kUser)
	{
		if (UserService::GetInstance()->GetUserType(accid) != nim::kNIMFriendFlagNormal || MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			return;
		}
	}

	ContactListItemUI* friend_item = dynamic_cast<ContactListItemUI*>(contract_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
	if (friend_item)
		friend_item->FindSubControl(L"head_image")->SetBkImage(photo_path);

	if (search_result_list_->IsVisible())
	{
		ContactListItemUI* search_item = dynamic_cast<ContactListItemUI*>(search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
		if (search_item)
			search_item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}

void ContactSelectForm::OnAddTeam(const std::string& teamid, const std::string& tname, nim::NIMTeamType type)
{
	OnRemoveTeam(teamid);
	AddTeamListItem(teamid, true);
}

void ContactSelectForm::OnRemoveTeam(const std::string& teamid)
{
	RemoveTeamListItem(teamid);
}

void ContactSelectForm::OnTeamNameChanged(const nim::TeamInfo& team_info)
{
	std::string accid = team_info.GetTeamID();

	//群组列表项
	ContactListItemUI* team_item = dynamic_cast<ContactListItemUI*>(contract_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
	if (!team_item)
		return;

	//搜索列表项
	ContactListItemUI* search_item = NULL;
	if (search_result_list_->IsVisible())
		search_item = dynamic_cast<ContactListItemUI*>(search_result_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));

	//选中列表项
	SelectedContactItemUI* select_item = dynamic_cast<SelectedContactItemUI*>(selected_user_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));

	if (team_info.ExistValue(nim::kTeamInfoKeyName))
	{
		bool enabled = team_item->IsEnabled();
		bool checked = ((CheckBox*)team_item->FindSubControl(L"checkbox"))->IsSelected();
		bool is_team = team_item->IsTeam();

		Box* tile_box = team_item->GetParent();
		tile_box->Remove(team_item); //删除
		if (tile_box->GetCount() == 0) //该字母下没有item了，隐藏bar
			dynamic_cast<TreeNode*>(tile_box->GetParent())->GetParentNode()->SetVisible(false);

		AddTeamListItem(accid, enabled); //重新添加
		team_item = dynamic_cast<ContactListItemUI*>(contract_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
		((CheckBox*)team_item->FindSubControl(L"checkbox"))->Selected(checked);

		if (search_item)
		{
			UTF8String search_key = search_edit_->GetUTF8Text();
			if (team_item->Match(search_key)) //名字变了但依然符合搜索条件
				search_item->Init(accid, is_team);
			else
				search_result_list_->Remove(search_item);
		}

		if (select_item)
			((Label*)select_item->FindSubControl(L"show_name"))->SetText(TeamService::GetInstance()->GetTeamName(accid));
	}

	if (team_info.ExistValue(nim::kTeamInfoKeyIcon))
	{
		team_item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(accid, false));

		if (search_item)
			search_item->FindSubControl(L"head_image")->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(accid, false));
	}


	RemoveTeamListItem(team_info.GetTeamID());
	AddTeamListItem(team_info.GetTeamID(), true);
}

}
#include "gui/main/friend_list_item_manager.h"
#include "shared/tool.h"
#include "gui/main/control/friend_item.h"

#include "gui/profile_form/profile_form.h"


FriendListItemManager::FriendListItemManager(ui::TreeView* friend_list) :
	friend_list_(friend_list)
{
	auto friend_list_change_cb = nbase::Bind(&FriendListItemManager::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegFriendListChange(friend_list_change_cb));
	auto user_info_change_cb = nbase::Bind(&FriendListItemManager::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserInfoChange(user_info_change_cb));
	auto set_black_cb = nbase::Bind(&FriendListItemManager::OnBlackListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(nim_ui::MuteBlackManager::GetInstance()->RegSyncSetBlackCallback(set_black_cb));
	auto user_photo_ready_cb = nbase::Bind(&FriendListItemManager::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(nim_ui::UserManager::GetInstance()->RegUserPhotoReady(user_photo_ready_cb));


	ui::TreeNode* tree_node = ListItemUtil::CreateFirstLetterListItem(L"*");
	friend_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"Èº×é");
	friend_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"#");
	friend_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	for (int i = 0; i < 26; i++)
	{
		wchar_t letter = L'A' + i;
		wstring letter_str;
		letter_str += letter;
		tree_node = ListItemUtil::CreateFirstLetterListItem(letter_str);
		friend_list_->GetRootNode()->AddChildNode(tree_node);
		tree_node_ver_.push_back(tree_node);
		tree_node->SetVisible(false);
		tree_node->SetEnabled(false);
	}
}

FriendListItemManager::~FriendListItemManager()
{

}

void FriendListItemManager::OnGetFriendList(const std::list<UserInfo> &user) 
{
	for (std::size_t i = 2; i < friend_list_->GetRootNode()->GetChildNodeCount(); i++)
	{
		friend_list_->GetRootNode()->GetChildNode(i)->RemoveAllChildNode();
	}
	
	UTF8String current_user_id = nim_ui::LoginManager::GetInstance()->GetAccount();
	for(auto it = user.begin(); it != user.end(); it++) {
		if (it->account != current_user_id
			&& nim_ui::UserManager::GetInstance()->GetUserType(it->account) == UT_FRIEND
			&& !nim_ui::MuteBlackManager::GetInstance()->IsInBlackList(it->account))
		{
			AddListItem(*it);
		}
	}
}

void FriendListItemManager::OnFriendListChange(UserChangeType change_type, const UserInfo& user)
{
	if (change_type == kChangeTypeAdd)
	{
		if (user.account != nim_ui::LoginManager::GetInstance()->GetAccount() 
			&& nim_ui::UserManager::GetInstance()->GetUserType(user.account) == UT_FRIEND 
			&& !nim_ui::MuteBlackManager::GetInstance()->IsInBlackList(user.account))
		{
			AddListItem(user);
		}
	}
	else if (change_type == kChangeTypeDelete)
	{
		DeleteListItem(user);
	}
}

void FriendListItemManager::OnUserInfoChange(const std::list<UserInfo> &uinfos)
{
	for (auto info : uinfos)
	{
		if (nim_ui::UserManager::GetInstance()->GetUserType(info.account) == UT_FRIEND)
		{
			FriendItem* item = dynamic_cast<FriendItem*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(info.account)));
			if (item != NULL)
				item->Init(false, info);
		}
	}
}

void FriendListItemManager::OnBlackListChange(const std::string& id, bool is_black)
{
	UserInfo user;
	nim_ui::UserManager* user_service = nim_ui::UserManager::GetInstance();
	user_service->GetUserInfo(id, user);
	if (is_black)
	{
		DeleteListItem(user);
	}
	else if (user_service->GetUserType(id) == UT_FRIEND)
	{
		AddListItem(user);
	}
}

void FriendListItemManager::OnUserPhotoReady(const std::string& accid, const std::wstring &photo_path)
{
	if (nim_ui::UserManager::GetInstance()->GetUserType(accid) == UT_FRIEND)
	{
		FriendItem* item = (FriendItem*)friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item == NULL)
			return;
		item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}

ui::TreeNode* FriendListItemManager::GetGroup(GroupType groupType, wchar_t letter)
{
	return tree_node_ver_[ListItemUtil::GetGroup(groupType, letter)];
}

void FriendListItemManager::AddListItem(const UserInfo& all_info)
{
	if (friend_list_->FindSubControl(nbase::UTF8ToUTF16(all_info.account)) != NULL)
		return;

	wstring ws_show_name = nbase::UTF8ToUTF16(all_info.name);
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ui::TreeNode* tree_node;
	if (!ws_spell.empty())
	{
		tree_node = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tree_node = tree_node_ver_[2];
	}

	AddListItemInGroup(all_info, tree_node);
}

void FriendListItemManager::AddListItemInGroup(const UserInfo& all_info, ui::TreeNode* tree_node)
{
	if (tree_node->GetChildNodeCount() == 0)
	{
		tree_node->SetVisible(true);
	}

	FriendItem* item = new FriendItem;
	ui::GlobalManager::FillBoxWithCache( item, L"main/friend_item.xml" );
	item->SetUTF8Name(all_info.account);
	item->Init(false, all_info);
	ui::ButtonBox* head_image = (ui::ButtonBox*)(item->FindSubControl(L"head_image"));
	head_image->AttachClick(nbase::Bind(&FriendListItemManager::OnHeadImageClick, this, all_info.account, std::placeholders::_1));
	FriendItem* container_element = item;
	std::size_t index = 0;
	for (index = 0; index < tree_node->GetChildNodeCount(); index++)
	{
		FriendItem* temp = (FriendItem*)tree_node->GetChildNode(index);
		if (*container_element == *temp)
		{
			//delete item;
			//ASSERT(FALSE);
			return;
		}
		if (*container_element < *temp)
		{
			tree_node->AddChildNodeAt(container_element, index);
			break;
		}
	}

	if (index == tree_node->GetChildNodeCount())
	{
		tree_node->AddChildNodeAt(container_element, index);
	}
}

void FriendListItemManager::DeleteListItem(const UserInfo& all_info)
{
	wstring ws_show_name = nbase::UTF8ToUTF16(all_info.name);
	string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ui::TreeNode* tree_node;
	if (!ws_spell.empty())
	{
		tree_node = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tree_node = tree_node_ver_[2];
	}

	DeleteListItemInGroup(all_info, tree_node);
}

void FriendListItemManager::DeleteListItemInGroup(const UserInfo& all_info, ui::TreeNode* tree_node)
{
	FriendItem* item = new FriendItem;
	ui::GlobalManager::FillBoxWithCache(item, L"main/friend_item.xml");
	item->Init(false, all_info);
	ui::ButtonBox* head_image = (ui::ButtonBox*)(item->FindSubControl(L"head_image"));
	head_image->AttachClick(nbase::Bind(&FriendListItemManager::OnHeadImageClick, this, all_info.account, std::placeholders::_1));
	FriendItem* container_element = item;
	std::size_t index = 0;
	for (index = 0; index < tree_node->GetChildNodeCount(); index++)
	{
		FriendItem* temp = (FriendItem*)tree_node->GetChildNode(index);
		if (*container_element == *temp)
		{
			tree_node->RemoveChildNodeAt(index);
			break;
		}
	}

	if (tree_node->GetChildNodeCount() == 0)
	{
		tree_node->SetVisible(false);
	}
}

bool FriendListItemManager::OnHeadImageClick(const std::string& uid, ui::EventArgs*)
{
	nim_comp::ProfileForm::ShowProfileForm(uid);
	return true;
}
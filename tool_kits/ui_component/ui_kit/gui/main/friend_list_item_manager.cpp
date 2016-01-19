#include "gui/main/friend_list_item_manager.h"
#include "shared/tool.h"
#include "gui/main/control/friend_item.h"
#include "gui/profile_form/profile_form.h"
#include "module/login/login_manager.h"

namespace nim_comp
{
FriendListItemManager::FriendListItemManager(ui::TreeView* friend_list) :
	friend_list_(friend_list)
{
	auto friend_list_change_cb = nbase::Bind(&FriendListItemManager::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));
	auto user_info_change_cb = nbase::Bind(&FriendListItemManager::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));
	auto set_black_cb = nbase::Bind(&FriendListItemManager::OnBlackListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(set_black_cb));
	auto user_photo_ready_cb = nbase::Bind(&FriendListItemManager::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(user_photo_ready_cb));


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

void FriendListItemManager::OnGetFriendList(const std::list<nim::UserNameCard> &user)
{
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	for (auto it = user.begin(); it != user.end(); it++) {
		if (it->GetAccId() != current_user_id
			&& UserService::GetInstance()->GetUserType(it->GetAccId()) == nim::kNIMFriendFlagNormal
			&& !MuteBlackService::GetInstance()->IsInBlackList(it->GetAccId()))
		{
			AddListItem(it->GetAccId());
		}
	}
}

void FriendListItemManager::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	if (change_type == kChangeTypeAdd)
	{
		if (accid != LoginManager::GetInstance()->GetAccount()
			&& UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal
			&& !MuteBlackService::GetInstance()->IsInBlackList(accid))
		{
			AddListItem(accid);
		}
	}
	else if (change_type == kChangeTypeDelete)
	{
		DeleteListItem(accid);
	}
}

void FriendListItemManager::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
{
	for (auto info : uinfos)
	{
		std::string accid = info.GetAccId();
		if (UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal)
		{
			FriendItem* item = dynamic_cast<FriendItem*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
			if (item != NULL)
			{
				if (info.ExistValue(nim::kUserNameCardKeyName))
				{
					//nim::UserNameCard all_info;
					//UserService::GetInstance()->GetUserInfo(info.GetAccId(), all_info);
					DeleteListItem(accid);
					AddListItem(accid);
				}
				else
					item->Init(false, accid);
			}
			else if(accid != LoginManager::GetInstance()->GetAccount()
				&& !MuteBlackService::GetInstance()->IsInBlackList(accid))
				AddListItem(accid);
		}
	}
}

void FriendListItemManager::OnBlackListChange(const std::string& id, bool is_black)
{
	if (is_black)
		DeleteListItem(id);
	else if (UserService::GetInstance()->GetUserType(id) == nim::kNIMFriendFlagNormal)
		AddListItem(id);
}

void FriendListItemManager::OnUserPhotoReady(const std::string& accid, const std::wstring &photo_path)
{
	if (UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal)
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

void FriendListItemManager::AddListItem(const std::string& accid)
{
	if (friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)) != NULL)
		return;

	std::wstring ws_show_name = UserService::GetInstance()->GetUserName(accid);
	std::string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	std::wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ui::TreeNode* tree_node;
	if (!ws_spell.empty())
	{
		tree_node = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tree_node = tree_node_ver_[2];
	}

	AddListItemInGroup(accid, tree_node);
}

void FriendListItemManager::AddListItemInGroup(const std::string& accid, ui::TreeNode* tree_node)
{
	if (tree_node->GetChildNodeCount() == 0)
	{
		tree_node->SetVisible(true);
	}

	FriendItem* item = new FriendItem;
	ui::GlobalManager::FillBoxWithCache( item, L"main/friend_item.xml" );
	item->SetUTF8Name(accid);
	item->Init(false, accid);
	ui::ButtonBox* head_image = (ui::ButtonBox*)(item->FindSubControl(L"head_image"));
	head_image->AttachClick(nbase::Bind(&FriendListItemManager::OnHeadImageClick, this, accid, std::placeholders::_1));
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

void FriendListItemManager::DeleteListItem(const std::string& accid)
{
	FriendItem* item = (FriendItem*)friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
	if (!item) return;
	std::wstring ws_show_name = ((ui::Label*)item->FindSubControl(L"contact"))->GetText();
	std::string spell = PinYinHelper::GetInstance()->ConvertToFullSpell(ws_show_name);
	std::wstring ws_spell = nbase::UTF8ToUTF16(spell);
	ui::TreeNode* tree_node;
	if (!ws_spell.empty())
	{
		tree_node = GetGroup(GT_COMMON_FRIEND, ws_spell[0]);
	}
	else
	{
		tree_node = tree_node_ver_[2];
	}

	DeleteListItemInGroup(accid, tree_node);
}

void FriendListItemManager::DeleteListItemInGroup(const std::string& accid, ui::TreeNode* tree_node)
{
	for (size_t index = 0; index < tree_node->GetChildNodeCount(); index++)
	{
		
		FriendItem* temp = (FriendItem*)tree_node->GetChildNode(index);
		if (accid == temp->GetId())
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
	ProfileForm::ShowProfileForm(uid);
	return true;
}
}
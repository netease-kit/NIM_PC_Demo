#include "gui/main/friend_list.h"
#include "gui/add_friend/add_friend_wnd.h"
#include "gui/black_list/black_list_wnd.h"
#include "gui/main/control/friend_item.h"
#include "gui/profile_form/profile_form.h"
#include "module/login/login_manager.h"
#include "shared/tool.h"

namespace nim_comp
{
FriendList::FriendList(ui::TreeView* friend_list) :
	friend_list_(friend_list),
	pos_tip_(nullptr)
{
	auto friend_list_change_cb = nbase::Bind(&FriendList::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));
	auto user_info_change_cb = nbase::Bind(&FriendList::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(user_info_change_cb));
	auto set_black_cb = nbase::Bind(&FriendList::OnBlackListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetBlackCallback(set_black_cb));
	auto user_photo_ready_cb = nbase::Bind(&FriendList::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(user_photo_ready_cb));

	friend_list->AttachScrollChange(nbase::Bind(&FriendList::OnScrollChange, this, std::placeholders::_1));
	friend_list->AttachResize(nbase::Bind(&FriendList::OnScrollChange, this, std::placeholders::_1));

	//添加好友listitem
	ui::TreeNode* add_friend_item = (ui::TreeNode*)ui::GlobalManager::CreateBox(L"main/main_add_friend_list_item.xml");
	friend_list_->GetRootNode()->AddChildNodeAt(add_friend_item, 0);
	add_friend_item->AttachClick([](ui::EventArgs* param)
	{
		WindowsManager::GetInstance()->SingletonShow<AddFriendWindow>(AddFriendWindow::kClassName);

		return true;
	});

	ui::TreeNode* black_list_item = (ui::TreeNode*)ui::GlobalManager::CreateBox(L"main/main_black_list_list_item.xml");
	friend_list_->GetRootNode()->AddChildNodeAt(black_list_item, 1);
	black_list_item->AttachClick([](ui::EventArgs* param)
	{
		WindowsManager::GetInstance()->SingletonShow<BlackListWindow>(BlackListWindow::kClassName);
		return true;
	});

	//添加分组
	ui::TreeNode* tree_node = ListItemUtil::CreateFirstLetterListItem(L"*");
	friend_list_->GetRootNode()->AddChildNode(tree_node);
	tree_node_ver_.push_back(tree_node);
	tree_node->SetVisible(false);
	tree_node->SetEnabled(false);

	tree_node = ListItemUtil::CreateFirstLetterListItem(L"群组");
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

FriendList::~FriendList()
{

}

void FriendList::OnGetFriendList(const std::list<nim::UserNameCard> &user)
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

void FriendList::AddListItem(const std::string& accid)
{
	if (NULL != FindFriendItem(accid))
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

void FriendList::AddListItemInGroup(const std::string& accid, ui::TreeNode* tree_node)
{
	if (tree_node->GetChildNodeCount() == 0)
	{
		tree_node->SetVisible(true);
	}

	FriendItem* item = new FriendItem;
	ui::GlobalManager::FillBoxWithCache(item, L"main/friend_item.xml");
	item->SetUTF8Name(accid);
	item->Init(false, accid);
	ui::ButtonBox* head_image = (ui::ButtonBox*)(item->FindSubControl(L"head_image"));
	head_image->AttachClick(nbase::Bind(&FriendList::OnHeadImageClick, this, accid, std::placeholders::_1));
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

void FriendList::DeleteListItem(const std::string& accid)
{
	FriendItem *item = static_cast<FriendItem*>(FindFriendItem(accid));
	if (!item)
		return;

	ui::Label* contact = ((ui::Label*)item->FindSubControl(L"contact"));
	if (!contact)
	{
		ASSERT(contact);
		return;
	}

	std::wstring ws_show_name = contact->GetText();
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

void FriendList::DeleteListItemInGroup(const std::string& accid, ui::TreeNode* tree_node)
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

ui::TreeNode* FriendList::FindFriendItem(const std::string& accid)
{
	ui::TreeNode* item = NULL;

	// 这里要考虑特殊情况，某些accid是A到Z，而FriendList中的分组控件GroupTitle的name也是A到Z
	// 所以直接用FindSubControl搜索可能会GroupTitle。对于这种情况就直接遍历所有分组去查找
	char id = accid.at(0);
	if (accid.length() == 1 && ((id >= 'a' && id <= 'z') || (id >= 'A' && id <= 'Z')))
	{
		std::wstring waccid = nbase::UTF8ToUTF16(accid);

		for (size_t i = 2; i < tree_node_ver_.size(); i++)
		{
			ui::TreeNode *group_node = tree_node_ver_.at(i);

			for (size_t j = 0; j < group_node->GetChildNodeCount(); j++)
			{
				ui::TreeNode *friend_node = group_node->GetChildNode(j);
				std::wstring name = friend_node->GetName();
				if (name == waccid)
				{
					item = friend_node;
					break;
				}
			}
		}
	}
	else
	{
		item = static_cast<ui::TreeNode*>(friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
	}

	return item;
}

ui::TreeNode* FriendList::GetGroup(GroupType groupType, wchar_t letter)
{
	return tree_node_ver_[ListItemUtil::GetGroup(groupType, letter)];
}

void FriendList::ScrollToLetter(char letter)
{
	if (letter >= 'a' && letter <= 'z')
	{
		letter = letter + 'A' - 'a';
	}
	UTF8String utf8_letter;
	utf8_letter.push_back(letter);
	for (int index = 0; index < friend_list_->GetCount(); index++)
	{
		ui::TreeNode* list_item = (ui::TreeNode*)friend_list_->GetItemAt(index);
		if (list_item->GetUTF8Name() == utf8_letter)
		{
			if (list_item->IsVisible())
			{
				list_item->SetExpand(true);
				RECT list_item_pos = list_item->GetPos();
				RECT list_pos = friend_list_->GetPaddingPos();
				friend_list_->SetScrollPos(ui::CSize(0, list_item_pos.top - list_pos.top));
			}
			break;
		}
	}
}

bool FriendList::OnScrollChange(ui::EventArgs* param)
{
	ui::TreeNode* top_control = dynamic_cast<ui::TreeNode*>(friend_list_->GetTopItem());
	if (!top_control) {
		//ASSERT(FALSE);
		return false;
	}
	GroupTitle* group_title;
	if (typeid(*top_control) != typeid(GroupTitle)) {
		group_title = dynamic_cast<GroupTitle*>(top_control->GetParentNode());
	}
	else {
		group_title = dynamic_cast<GroupTitle*>(top_control);
	}

	if (group_title && !group_title->GetName().empty()) {
		if (pos_tip_ && pos_tip_->GetName() == group_title->GetName()) {
			pos_tip_->SetAlpha(255);
		}
		else {
			friend_list_->GetRootNode()->RemoveChildNode(pos_tip_);
			pos_tip_ = ListItemUtil::CreateFirstLetterListItem(group_title->GetName());
			pos_tip_->SetFixedHeight(40);
			pos_tip_->SetFloat(true);
			pos_tip_->SetMargin(ui::UiRect(0, -14, 0, 0));
			friend_list_->GetRootNode()->AddChildNode(pos_tip_);
			pos_tip_->GetLayout()->SetPadding(ui::UiRect(0, 14, 0, 0));
			pos_tip_->SetMouseEnabled(false);
			pos_tip_->SetMouseChildEnabled(false);
			pos_tip_->FindSubControl(L"top_splitLine")->SetAlpha(0);
		}

		int index = top_control->GetIndex();
		for (int i = index + 1; i < friend_list_->GetCount(); i++) {
			auto control = friend_list_->GetItemAt(i);
			if (control->IsVisible() && !control->IsFloat() && dynamic_cast<GroupTitle*>(control)) {
				int deta = pos_tip_->GetFixedHeight() - control->GetPos().top
					+ friend_list_->GetLayout()->GetInternalPos().top - 14 + friend_list_->GetScrollPos().cy;
				if (deta >= 0) {
					pos_tip_->SetRenderOffset(ui::CPoint(0, deta));
				}
				else {
					pos_tip_->SetRenderOffset(ui::CPoint(0, 0));
				}
				return false;
			}
		}

	}
	else {
		if (pos_tip_) {
			pos_tip_->SetAlpha(0);
		}
	}

	return true;
}

bool FriendList::OnHeadImageClick(const std::string& uid, ui::EventArgs*)
{
	ProfileForm::ShowProfileForm(uid);
	return true;
}

void FriendList::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
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

void FriendList::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos)
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
					if (item->GetNickName() == nbase::UTF8ToUTF16(info.GetName()) )
					{
						item->Init(false, accid);
					}
					else
					{
						DeleteListItem(accid);
						AddListItem(accid);
					}	
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

void FriendList::OnBlackListChange(const std::string& id, bool is_black)
{
	if (is_black)
		DeleteListItem(id);
	else if (UserService::GetInstance()->GetUserType(id) == nim::kNIMFriendFlagNormal)
		AddListItem(id);
}

void FriendList::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	if (type == kUser && UserService::GetInstance()->GetUserType(accid) == nim::kNIMFriendFlagNormal)
	{
		FriendItem* item = (FriendItem*)friend_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item == NULL)
			return;
		item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}
}
#include "friend_list.h"
//#include "util/windows_manager.h"
#include "shared/pin_yin_helper.h"
#include "gui/add_friend/add_friend_wnd.h"
#include "gui/black_list/black_list_wnd.h"

using namespace ui;
using namespace std;

namespace nim_comp
{
FriendList::FriendList(ui::TreeView* friend_list):
	friend_list_(friend_list),
	nim_list_base_(friend_list),
	pos_tip_(nullptr)
{
	friend_list->AttachScrollChange(nbase::Bind(&FriendList::OnScrollChange, this, std::placeholders::_1));
	friend_list->AttachResize(nbase::Bind(&FriendList::OnScrollChange, this, std::placeholders::_1));

	//Ìí¼ÓºÃÓÑlistitem
	TreeNode* add_friend_item = (TreeNode*)GlobalManager::CreateBox(L"main/main_add_friend_list_item.xml");
	friend_list_->GetRootNode()->AddChildNodeAt(add_friend_item, 0);
	add_friend_item->AttachClick([](ui::EventArgs* param)
	{
		WindowsManager::GetInstance()->SingletonShow<AddFriendWindow>(AddFriendWindow::kClassName);

		return true;
	});

	TreeNode* black_list_item = (TreeNode*)GlobalManager::CreateBox(L"main/main_black_list_list_item.xml");
	friend_list_->GetRootNode()->AddChildNodeAt(black_list_item, 1);
	black_list_item->AttachClick([](ui::EventArgs* param)
	{
		WindowsManager::GetInstance()->SingletonShow<BlackListWindow>(BlackListWindow::kClassName);
		return true;
	});
}


FriendList::~FriendList()
{

}

bool FriendList::OnScrollChange(ui::EventArgs* param)
{
	TreeNode* top_control = dynamic_cast<TreeNode*>(friend_list_->GetTopItem());
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
			pos_tip_->SetMargin(UiRect(0,-14,0,0));
			friend_list_->GetRootNode()->AddChildNode(pos_tip_);
			pos_tip_->GetLayout()->SetPadding(UiRect(0,14,0,0));
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
					pos_tip_->SetRenderOffset(CPoint(0, deta));
				}
				else {
					pos_tip_->SetRenderOffset(CPoint(0, 0));
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
		TreeNode* list_item = (TreeNode*)friend_list_->GetItemAt(index);
		if (list_item->GetUTF8Name() == utf8_letter)
		{
			if(list_item->IsVisible())
			{
				list_item->SetExpand(true);
				RECT list_item_pos = list_item->GetPos();
				RECT list_pos = friend_list_->GetPaddingPos();
				friend_list_->SetScrollPos(CSize(0, list_item_pos.top - list_pos.top));
			}
			break;
		}
	}
}
}
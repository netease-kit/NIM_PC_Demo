#include "main_form.h"
#include "gui/main/friend_list.h"
#include "gui/main/group_list.h"


using namespace ui;

bool MainForm::SearchEditChange(ui::EventArgs* param) 
{
	UTF8String search_key = search_edit_->GetUTF8Text();
	bool has_serch_key = !search_key.empty();
	btn_clear_input_->SetVisible(has_serch_key);
	search_result_list_->SetVisible(has_serch_key);
	FindControl(L"no_search_result_tip")->SetVisible(has_serch_key);
	if (has_serch_key)
	{
		std::list<UTF8String> searched_ids;
		search_result_list_->RemoveAll();

		for (int i = 0; i < friend_list_->GetFriendList()->GetCount(); i++)
		{
			FriendItem* friend_item = dynamic_cast<FriendItem*>(friend_list_->GetFriendList()->GetItemAt(i));
			if (friend_item && friend_item->Match(search_key))
			{
				FriendItem* search_res_friend_item = new FriendItem;
				GlobalManager::FillBoxWithCache( search_res_friend_item, L"main/friend_item.xml" );
				search_res_friend_item->Init(friend_item->GetIsTeam(), friend_item->GetUserInfo());
				if (std::find(searched_ids.begin(), searched_ids.end(), friend_item->GetUserInfo().account) == searched_ids.end())
				{
					search_result_list_->Add(search_res_friend_item);
				}
			}
		}
		for (int i = 0; i < group_list_->GetGroupList()->GetCount(); i++)
		{
			FriendItem* friend_item = dynamic_cast<FriendItem*>(group_list_->GetGroupList()->GetItemAt(i));
			if (friend_item && friend_item->Match(search_key))
			{
				FriendItem* search_res_friend_item = new FriendItem;
				GlobalManager::FillBoxWithCache(search_res_friend_item, L"main/friend_item.xml");
				search_res_friend_item->Init(friend_item->GetIsTeam(), friend_item->GetUserInfo());
				if (std::find(searched_ids.begin(), searched_ids.end(), friend_item->GetUserInfo().account) == searched_ids.end())
				{
					search_result_list_->Add(search_res_friend_item);
				}
			}
		}

		FindControl(L"no_search_result_tip")->SetVisible(search_result_list_->GetCount() == 0);
	}
	return true;
}

bool MainForm::OnClearInputBtnClicked(ui::EventArgs* param)
{
	search_edit_->SetText(L"");
	return true;
}

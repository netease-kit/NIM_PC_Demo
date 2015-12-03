#pragma once

#include "nim_ui_dll.h"
#include "gui/main/control/friend_item.h"
#include "gui/main/friend_list.h"
#include "gui/main/group_list.h"

namespace nim_ui
{

/** @class ContactsListManager
  * @brief 提供联系人列表控制接口（好友列表和群列表）
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/10/10
  */
class NIM_UI_DLL_API ContactsListManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(ContactsListManager);
public:
	ContactsListManager();
	~ContactsListManager();
public:
	bool AttachFriendListBox(ui::TreeView *list_box);
	bool AttachGroupListBox(ui::TreeView *list_box);

	void InvokeGetAllUserInfo();
	void FillSearchResultList(ui::ListBox* search_result_list, const UTF8String& search_key);

private:
	void OnGetAllFriendInfo(const std::list<nim::UserNameCard> &uinfos);
	
private:
	std::unique_ptr<nim_comp::FriendList> friend_list_;
	std::unique_ptr<nim_comp::GroupList> group_list_;
};

}
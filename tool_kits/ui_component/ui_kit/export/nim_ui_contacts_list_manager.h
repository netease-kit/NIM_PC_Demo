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
	/**
	* 把传入的树控件作为好友列表控件，控制树控件来展示好友列表
	* @param[in] list_box 作为好友列表的树控件指针(当控件被销毁时应该传入nullprt)
	* @return void	无返回值
	*/
	void AttachFriendListBox(ui::TreeView *list_box);

	/**
	* 把传入的树控件作为群组列表控件，控制树控件来展示群组列表
	* @param[in] list_box 作为群组列表的树控件指针(当控件被销毁时应该传入nullprt)
	* @return void	无返回值
	*/
	void AttachGroupListBox(ui::TreeView *list_box);

	/**
	* 重新获取所有好友信息并填充好友列表
	* @return void	无返回值
	*/
	void InvokeGetAllUserInfo();

	/**
	* 把符合关键词的好友或者群组填充到列表控件中
	* @param[in] search_result_list 用来填充结果的控件
	* @param[in] search_key 匹配关键字
	* @return void	无返回值
	*/
	void FillSearchResultList(ui::ListBox* search_result_list, const UTF8String& search_key);

private:
	/**
	* 获取到所有好友信息的回调
	* @param[in] uinfos 好友信息
	* @return void	无返回值
	*/
	void OnGetAllFriendInfo(const std::list<nim::UserNameCard> &uinfos);
	
private:
	ContactsListManager(){};
	~ContactsListManager(){};

	std::unique_ptr<nim_comp::FriendList> friend_list_;
	std::unique_ptr<nim_comp::GroupList> group_list_;
};

}
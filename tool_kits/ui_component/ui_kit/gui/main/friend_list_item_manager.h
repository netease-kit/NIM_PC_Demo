#ifndef NIM_WIN_GUI_LIST_UTIL_NIM_LIST_BASE_H_
#define NIM_WIN_GUI_LIST_UTIL_NIM_LIST_BASE_H_

#include "shared/list_item_util.h"

namespace nim_comp
{
/** @class FriendListItemManager
  * @brief 负责处理一些与好友列表项相关的事件
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendListItemManager : public nbase::SupportWeakCallback
{
public:

	FriendListItemManager(ui::TreeView* friend_list);
	virtual ~FriendListItemManager();

public:
	void OnGetFriendList(const std::list<nim::UserNameCard> &user);

private:
	void AddListItem(const nim::UserNameCard& user_info);
	void DeleteListItem(const nim::UserNameCard& user_info);
	void OnFriendListChange(FriendChangeType change_type, const nim::UserNameCard& user);
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnBlackListChange(const std::string& id, bool is_black);
	void OnUserPhotoReady(const std::string& accid, const std::wstring &photo_path);
	ui::TreeNode* GetGroup(GroupType groupType, wchar_t letter = L'');
	void AddListItemInGroup(const nim::UserNameCard& all_info, ui::TreeNode* tree_node);
	void DeleteListItemInGroup(const nim::UserNameCard& all_info, ui::TreeNode* tree_node);
	bool OnHeadImageClick(const std::string& uid, ui::EventArgs*);

	ui::TreeView* friend_list_;
	vector<ui::TreeNode*> tree_node_ver_;
	AutoUnregister unregister_cb;
};
}
#endif // NIM_WIN_GUI_LIST_UTIL_NIM_LIST_BASE_H_
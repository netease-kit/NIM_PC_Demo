#pragma once

#include "shared/list_item_util.h"

namespace nim_comp
{
/** @class GroupList
  * @brief 用于查询和保存群列表
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class GroupList : public nbase::SupportWeakCallback
{
public:

	GroupList(ui::TreeView* group_list);
	virtual ~GroupList();
	ui::TreeView* GetGroupList() {
		return group_list_;
	}
private:
	void QueryAllMyTeamsInfoAsync(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status);
	void OnQueryAllMyTeams(int team_count, const std::list<nim::TeamInfo>& team_info_list);
	void AddListItem(const nim::TeamInfo& user_info);
	void AddListItemInGroup(const nim::TeamInfo& all_info, ui::TreeNode* tree_node);
	void OnAddTeam(const std::string& tid, const std::string& tname, nim::NIMTeamType type);
	void OnTeamNameChanged(const nim::TeamInfo& team_info);
	void OnRemoveTeam(const std::string& tid);

	ui::TreeView* group_list_;
	vector<ui::TreeNode*> tree_node_ver_;

	AutoUnregister unregister_cb;
};
}
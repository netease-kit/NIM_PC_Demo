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
	/**
	* 构造函数
	* @param[in] group_list 作为分组列表的TreeView控件指针
	*/
	GroupList(ui::TreeView* group_list);
	virtual ~GroupList();

	/**
	* 获取作为分组列表的TreeView控件指针
	* @return ui::TreeView*	作为分组列表的TreeView控件指针
	*/
	ui::TreeView* GetGroupList() { return group_list_; }
private:

	/**
	* 添加一个群组列表项
	* @param[in] team_info 群组信息
	* @return void	无返回值
	*/
	void AddListItem(const nim::TeamInfo& team_info);

	/**
	* 添加一个群组列表项到指定的分组内
	* @param[in] accid 用户id
	* @param[in] tree_node 分组控件指针
	* @return void	无返回值
	*/
	void AddListItemInGroup(const nim::TeamInfo& team_info, ui::TreeNode* tree_node);

	/**
	* 响应查询所有群信息的回调函数
	* @param[in] team_count 群组总数
	* @param[in] team_info_list 群组信息列表
	* @return void 无返回值
	*/
	void OnQueryAllMyTeams(int team_count, const std::list<nim::TeamInfo>& team_info_list);

	/**
	* 响应添加群的回调函数
	* @param[in] tid 群组id
	* @param[in] tname 群组名字
	* @param[in] type 群组类型
	* @return void 无返回值
	*/
	void OnAddTeam(const std::string& tid, const std::string& tname, nim::NIMTeamType type);
	
	/**
	* 响应移除群的回调函数
	* @param[in] tid 群组id
	* @return void 无返回值
	*/
	void OnRemoveTeam(const std::string& tid);

	/**
	* 响应群名称改变的回调函数
	* @param[in] team_info 群组信息
	* @return void 无返回值
	*/
	void OnTeamNameChanged(const nim::TeamInfo& team_info);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] accid 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	bool OnReturnEventsClick(ui::EventArgs* param);

	/**
	* 处理好友控件头像单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnHeadImageClick(const std::string& uid, ui::EventArgs*);

private:
	ui::TreeView* group_list_;
	vector<ui::TreeNode*> tree_node_ver_;

	AutoUnregister unregister_cb;
};
}
#pragma once
#include "shared/list_item_util.h"
#include "module/service/photo_service.h"
#include "module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{
/** @class FriendListItemManager
  * @brief 负责处理一些与好友列表项相关的事件
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendList : public nbase::SupportWeakCallback
{
public:
	/**
	* 构造函数
	* @param[in] friend_list 作为好友列表的TreeView控件指针
	*/
	FriendList(ui::TreeView* friend_list);
	virtual ~FriendList();

	/**
	* 获取作为好友列表的TreeView控件指针
	* @return ui::TreeView*	作为好友列表的TreeView控件指针
	*/
	ui::TreeView* GetFriendList() { return friend_list_; }

	/**
	* 响应获取好友列表的回调函数
	* @param[in] uinfos 好友名片列表
	* @return void	无返回值
	*/
	void OnGetFriendList(const std::list<nim::UserNameCard> &user);

	/**
	* 响应获取机器人列表的回调函数
	* @param[in] robots 机器人信息列表
	* @return void	无返回值
	*/
	void OnGetRobotList(const nim::RobotInfos &robots);


private:
	/**
	* 添加一个好友列表项
	* @param[in] accid 用户id
	* @param[in] type 类型
	* @return void	无返回值
	*/
	void AddListItem(const std::string& accid, FriendItemType type);

	/**
	* 添加一个好友列表项到指定的分组内
	* @param[in] accid 用户id
	* @param[in] type 类型
	* @param[in] tree_node 分组控件指针
	* @return void	无返回值
	*/
	void AddListItemInGroup(const std::string& accid, FriendItemType type, ui::TreeNode* tree_node);

	/**
	* 删除一个好友列表项
	* @param[in] accid 用户id
	* @param[in] type 类型
	* @return void	无返回值
	*/
	void DeleteListItem(const std::string& accid, FriendItemType type);

	/**
	* 从指定的分组内移除一个好友列表项
	* @param[in] accid 用户id
	* @param[in] type 类型
	* @param[in] tree_node 分组控件指针
	* @return void	无返回值
	*/
	void DeleteListItemInGroup(const std::string& accid, FriendItemType type, ui::TreeNode* tree_node);

	/**
	* 查找一个好友列表项
	* @param[in] accid 用户id
	* @return ui::TreeNode* 好友列表项控件指针
	*/
	ui::TreeNode* FindFriendItem(const std::string& accid);

	/**
	* 根据分组类型和分组标签获取分组控件
	* @param[in] groupType 分组类型
	* @param[in] letter 分组标签
	* @return ui::TreeNode*	分组控件
	*/
	ui::TreeNode* GetGroup(GroupType groupType, wchar_t letter = L'\0');

	/**
	* 控件范围滚动到指定的分组标签
	* @param[in] letter 分组标签，A到Z
	* @return void	无返回值
	*/
	void ScrollToLetter(char letter);

private:
	/**
	* 处理好友列表控件滚动消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnScrollChange(ui::EventArgs* param);

	/**
	* 处理好友控件头像单击消息
	* @param[in] msg 消息的相关信息
	* @param[in] type 类型
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnHeadImageClick(const std::string& uid, FriendItemType type, ui::EventArgs*);

private:

	/**
	* 响应机器人列表列表改变的回调函数
	* @param[in] rescode 错误码
	* @param[in] type 类型
	* @param[in] robots 机器人列表
	* @return void 无返回值
	*/
	void OnRobotChange(nim::NIMResCode rescode, nim::NIMRobotInfoChangeType type, const nim::RobotInfos& robots);

	/**
	* 响应用户列表改变的回调函数
	* @param[in] change_type 好友变化类型
	* @param[in] accid 用户id
	* @return void 无返回值
	*/
	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);

	/**
	* 响应用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 响应黑名单改变的回调函数
	* @param[in] id 用户id
	* @param[in] is_black 是否加入黑名单
	* @return void 无返回值
	*/
	void OnBlackListChange(const std::string& id, bool is_black);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] accid 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	/**
	* 响应接收事件的回调函数
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[in] data 事件信息
	* @return void 无返回值
	*/
	void OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data);

	bool OnReturnEventsClick(ui::EventArgs* param);

private:
	ui::Label* tip_letter_;
	ui::TreeNode* pos_tip_;
	ui::TreeView* friend_list_;
	vector<ui::TreeNode*> tree_node_ver_;
	AutoUnregister unregister_cb;
};
}

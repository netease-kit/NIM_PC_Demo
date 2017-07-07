#pragma once

#include "shared/list_item_util.h"
#include "shared/pin_yin_helper.h"
#include "module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{
/** @class FriendItem
  * @brief 好友列表项的UI类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendItem : public ui::TreeNode
{
public:
	FriendItem();
	virtual ~FriendItem();

	/**
	* 初始化控件
	* @param[in] type 类型
	* @param[in] accid 用户id或者群组id
	* @return void	无返回值
	*/
	virtual void Init(FriendItemType type, const std::string &accid);

	/**
	* 查询类型
	* @return FriendItemType
	*/
	FriendItemType GetFriendItemType() const { return type_; }

	/**
	* 获取用户id或者群组id
	* @return string 用户id或者群组id
	*/
	std::string GetId()	{ return id_; }

	/**
	* 获取用户名或者群组名
	* @return string 用户id或者群组id
	*/
	std::wstring GetNickName() { return nick_name_; }

	/**
	* 用昵称、用户名等信息匹配搜索关键字
	* @param[in] search_key 关键字
	* @return bool true 匹配成功，false 匹配失败
	*/
	bool Match(const UTF8String& search_key);

	/**
	* 设置好友在线状态
	* @param[in] EventDataEx 事件数据
	* @return void	无返回值
	*/
	void SetOnlineState(const EventDataEx& data);

	/**
	* 与另一个好友项控件进行比较，根据昵称判断大小
	* @param[in] elment2 好友项控件
	* @return bool true 昵称在另一个控件之前，false 昵称在另一个控件之后
	*/
	bool operator < (const FriendItem& elment2) const
	{
		static DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRCP);
		return ::CompareString(lcid, 0, this->nick_name_.c_str(), (int)this->nick_name_.length(), elment2.nick_name_.c_str(),(int)elment2.nick_name_.length()) == 1;
	}

	/**
	* 判断与另一个好友项控件id是否一样
	* @param[in] elment2 好友项控件
	* @return bool true 一样，false 不一样
	*/
	bool operator == (const FriendItem& elment2) const 
	{
		return this->id_ == elment2.id_;
	}

	/**
	* 处理好友控件双击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnDbClicked(ui::EventArgs* arg);

	/**
	* 响应群名称改变的回调函数
	* @param[in] team_info 群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

private:
	ui::Label*		label_online_state_;
	ui::Label*		contact_;

	FriendItemType	type_;
	std::string		id_;

	std::wstring	nick_name_;
	UTF8String		nick_name_full_spell_;
	UTF8String		nick_name_simple_spell_;

	AutoUnregister unregister_cb;
};
}
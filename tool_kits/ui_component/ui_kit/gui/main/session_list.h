#pragma once
#include "control/session_item.h"
#include "gui/main/multispot_form.h"
#include "module/service/photo_service.h"

namespace nim_comp
{

typedef std::function<void(int unread_count)> OnUnreadCountChangeCallback;

/** @class SessionList
  * @brief 用于操作和维护最近会话列表
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/14
  */
class SessionList : public nbase::SupportWeakCallback
{
public:
	/**
	* 构造函数
	* @param[in] list_box 作为会话列表的列表控件指针
	* @return void	无返回值
	*/
	SessionList(ui::ListBox* session_list);
	~SessionList(){};

	/**
	* 按每个会话最后一条消息的时间排序会话的位置
	* @param[in] msg 被排序的会话数据
	* @return int 在绘画列表中的位置
	*/
	int AdjustMsg(const nim::SessionData &msg);

	/**
	* 添加一个会话控件到会话列表
	* @param[in] msg 会话数据
	* @return SessionItem* 被添加的会话控件的指针
	*/
	SessionItem* AddSessionItem(const nim::SessionData &msg);

	/**
	* 从会话列表查找会话控件
	* @param[in] session_id 会话id
	* @return SessionItem* 会话控件的指针
	*/
	SessionItem* GetSessionItem(const std::string &session_id);

	/**
	* 移除某个会话控件
	* @param[in] item 会话控件指针
	* @return void 无返回值
	*/
	void DeleteSessionItem(SessionItem* item);

	/**
	* 移除所有会话控件
	* @return void 无返回值
	*/
	void RemoveAllSessionItem();

	/**
	* 注册未读消息(会话消息、系统消息、自定义消息)总数改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegUnreadCountChange(const OnUnreadCountChangeCallback& callback);

	/**
	* 通知所有注册过未读消息总数改变事件的回调函数，当前的未读消息总数
	* @return void	无返回值
	*/
	void InvokeUnreadCountChange();

	/**
	* 为某个会话控件增加一条未读消息
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void AddUnreadCount(const std::string &id);

	/**
	* 为某个会话控件重置未读消息数为0
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void ResetUnreadCount(const std::string &id);

	/**
	* 改变界面上系统未读消息总数的显示
	* @param[in] count 未读消息总数
	* @return void 无返回值
	*/
	void UISysmsgUnread(int count);

	/**
	* 改变界面上自定义未读消息总数的显示
	* @param[in] add 增加或者减少未读自定义消息总数
	* @return void 无返回值
	*/
	void UICustomSysmsgUnread(bool add);

	/**
	* 更新包含用户信息的会话控件的显示
	* @param[in] user_info 用户信息
	* @return void 无返回值
	*/
	void UpdateSessionInfo(const nim::UserNameCard& user_info);

	/**
	* 移动端登录发生变化时的回调
	* @param[in] online 是否在线
	* @param[in] clients 其他客户端信息
	* @return void	无返回值
	*/
	void OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients);

	/**
	* 把移动端踢下线的结果回调
	* @param[in] client_ids 被踢的设备id
	* @return void	无返回值
	*/
	void OnMultispotKickout(const std::list<std::string> &client_ids);

	/**
	* 更新会话列表控件中多端信息的界面显示
	* @return void	无返回值
	*/
	void UpdateMultispotUI();

private:
	/**
	* 注册给sdk的回调函数，会话消息变化（如有新消息、删除会话记录）时执行
	* @param[in] rescode 返回码
	* @param[in] data 会话消息的具体信息和内容
	* @param[in] total_unread_counts 未读消息条数
	* @return void 无返回值
	*/
	void OnSessionChangeCallback(nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts);

	/**
	* 用户信息改变的回调函数
	* @param[in] uinfos 用户信息列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);

	/**
	* 用户头像改变的回调函数
	* @param[in] type 头像类型
	* @param[in] accid 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path);

	/**
	* 群组头像改变的回调函数
	* @param[in] team_info 群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

	/**
	* 响应接收事件的回调函数
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[in] data 事件信息
	* @return void 无返回值
	*/
	void OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data);

	/**
	* 响应机器人信息改变的回调函数
	* @param[in] rescode 错误码
	* @param[in] type 类型
	* @param[in] robots 机器人列表
	* @return void 无返回值
	*/
	void OnRobotChange(nim::NIMResCode rescode, nim::NIMRobotInfoChangeType type, const nim::RobotInfos& robots);

private:
	/**
	* 响应会话控件的通知事件
	* @param[in] param 发送事件的控件的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnItemNotify(ui::EventArgs* msg);

	/**
	* 响应消息中心按钮的单击事件
	* @param[in] param 发送事件的控件的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnEventsClick(ui::EventArgs* param);

	/**
	* 响应多端同步按钮的单击事件
	* @param[in] param 发送事件的控件的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMultispotClick(ui::EventArgs* param);

	/**
	* 响应用户列表改变的回调函数
	* @param[in] change_type 好友变化类型
	* @param[in] accid 用户id
	* @return void 无返回值
	*/
	void OnFriendListChange(FriendChangeType change_type, const std::string& accid);

	void OnNotifyChangeCallback(std::string id, bool mute);
	void OnTeamNotificationModeChangeCallback(const std::string &tid, const int64_t bits);

	bool OnReturnEventsClick(ui::EventArgs* param);
	
private:
	ui::ListBox*	session_list_;

	ui::Box*		box_unread_sysmsg_;
	ui::Label*		label_unread_sysmsg_;

	int sys_msg_unread_;
	int custom_msg_unread_;

	AutoUnregister unregister_cb;
	std::map<nim::NIMClientType, nim::OtherClientPres> map_multispot_infos_;
	std::map<int, std::unique_ptr<OnUnreadCountChangeCallback>> unread_count_change_cb_list_;
};
}
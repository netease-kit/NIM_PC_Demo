#pragma once
#include "module/session/session_util.h"
#include "module/login/login_manager.h"
#include "module/service/user_service.h"
#include "module/subscribe_event/subscribe_event_manager.h"

enum SessionEventType
{
	SET_DELETE		//删除会话项
};

namespace nim_comp
{
/** @class SessionItem
  * @brief 会话列表项的UI类，当有新的会话消息或用户操作时，负责作出界面上的改变
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class SessionItem : public ui::ListContainerElement
{
public:
	SessionItem();
	virtual ~SessionItem();

	virtual void InitCtrl();

	/**
	* 初始化该会话列表项的显示
	* @param[in] msg 消息内容和信息
	* @return void	无返回值
	*/
	void InitMsg(const nim::SessionData &msg);

	/**
	* 用昵称、用户名等信息匹配搜索关键字
	* @param[in] search_key 关键字
	* @return bool true 匹配成功，false 匹配失败
	*/
	bool Match(const UTF8String& search_key) { return false; }

	/**
	* 是否为群组项
	* @return bool true 是，false 否
	*/
	bool GetIsTeam() { return msg_.type_ == nim::kNIMSessionTypeTeam; }

	/**
	* 初始化头像和用户名/群名
	* @return void	无返回值
	*/
	void InitUserProfile();

	/**
	* 初始化机器人信息
	* @return void	无返回值
	*/
	void InitRobotProfile();


	/**
	* 设置好友在线状态
	* @param[in] EventDataEx 事件数据
	* @return void	无返回值
	*/
	void SetOnlineState(const EventDataEx& data);

	/**
	* 更新会话列表项的显示内容
	* @param[in] id 会话id
	* @return void	无返回值
	*/
	void UpdateMsgContent(const std::string& id = "");

	/**
	* 清理会话列表想的显示内容
	* @return void	无返回值
	*/
	void ClearMsg();

	/**
	* 获取消息时间戳
	* @return long long 时间戳
	*/
	long long GetMsgTime();

	/**
	* 获取会话消息数据
	* @return nim::SessionData 消息内容
	*/
	nim::SessionData& GetSessionData() { return msg_; }
	
	/**
	* 获取本会话项未读数
	* @return int 未读数
	*/
	int GetUnread();

	/**
	* 设置本会话项未读数
	* @param[in] unread 未读数
	* @return void 无返回值
	*/
	void SetUnread(int unread);

	/**
	* 增加一条本会话项未读数
	* @return void 无返回值
	*/
	void AddUnread();

	/**
	* 重置本会话项未读数
	* @return void 无返回值
	*/
	void ResetUnread();

	/**
	* 删除最近会话项的回调函数
	* @param[in] code 错误码
	* @param[in] result 会话数据
	* @param[in] total_unread_counts 总未读消息数
	* @return void	无返回值
	*/
	static void DeleteRecentSessionCb(nim::NIMResCode code, const nim::SessionData &result, int total_unread_counts);

	/**
	* 批量设置会话项未读消息为已读的回调函数
	* @param[in] res_code 错误码
	* @param[in] uid 会话id
	* @param[in] to_type 会话类型
	* @return void	无返回值
	*/
	static void BatchStatusDeleteCb(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type);

	void SetMute(bool mute);
private:
	/**
	* 显示或隐藏@我消息提示
	* @param[in] show 显示或隐藏
	* @return void 无返回值
	*/
	void ShowAtmeTip(bool show);

	/**
	* 更新控件界面上显示的未读数
	* @return void 无返回值
	*/
	void UpdateUnread();

	/**
	* 弹出菜单
	* @param[in] point 显示菜单的坐标
	* @return void	无返回值
	*/
	void PopupSessionItemMenu(POINT point);

	/**
	* 处理菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool DelSessionItemMenuItemClick(ui::EventArgs* param);

	/**
	* 处理控件双击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnDbClicked(ui::EventArgs* arg);

	/**
	* 处理弹出菜单消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSessionItemMenu(ui::EventArgs* arg);

	/**
	* 处理头像按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnHeadImageClicked(bool is_robot, ui::EventArgs* arg);

private:
	ui::ButtonBox*	head_image_;
	ui::Label*		label_name_;
	ui::Label*		label_msg_;
	ui::Label*		label_online_state_;
	ui::Label*		label_atme_;
	ui::Label*		label_time_;
	ui::Box*		box_unread_;
	ui::Label*		label_unread_;

	nim::SessionData msg_;
	std::set<std::string> relate_ids;
};

/**
* 清除某个会话的未读数
* @param[in] id 会话id
* @param[in] type 会话类型
* @return void	无返回值
*/
void InvokeResetUnread(const std::string &id, nim::NIMSessionType type);
}
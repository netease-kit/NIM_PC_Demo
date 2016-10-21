#pragma once


enum EventOpType
{
	EOT_DELETE
};

namespace nim_comp
{
/** @class TeamEventItem
  * @brief 消息中心系统消息列表项UI类，当有新的系统消息或用户操作时，负责做出界面上的改变
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TeamEventItem : public ui::ListContainerElement
{
public:
	TeamEventItem();
	virtual ~TeamEventItem();

	/**
	* 初始化控件
	* @return void	无返回值
	*/
	void InitCtrl();

	/**
	* 初始化控件界面效果
	* @param[in] json 系统消息和自定义通知数据
	* @return void	无返回值
	*/
	bool InitInfo(const nim::SysMessage &json);

	/**
	* 获取消息id
	* @return __int64 消息id
	*/
	__int64 GetMsgId();

	/**
	* 获取消息时间戳
	* @return __int64 消息时间戳
	*/
	__int64 GetTime();

	/**
	* 响应系统消息事件的回调函数
	* @param[in] status 系统消息状态
	* @return void 无返回值
	*/
	void OnTeamEventCb(nim::NIMSysMsgStatus status);

	/**
	* 响应用户信息改变的回调函数
	* @param[in] info 用户信息
	* @return void 无返回值
	*/
	void OnUserInfoChange(const nim::UserNameCard &info);

	/**
	* 响应用户头像改变的回调函数
	* @param[in] account 用户id
	* @param[in] photo_path 头像路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(const std::string& accid, const std::wstring& photo_path);

	/**
	* 响应群名称改变的回调函数
	* @param[in] team_info 群信息
	* @return void 无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);

	/**
	* 响应同意入群、接收邀请回调函数
	* @param[in] msg_id 消息id
	* @param[in] team_event 群组事件通知
	* @return void 无返回值
	*/
	static void TeamEventCb(__int64 msg_id, const nim::TeamEvent& team_event);

	/**
	* 响应设置系统消息状态回调函数
	* @param[in] code 错误码
	* @param[in] msg_id 消息id
	* @param[in] unread 未读的系统消息数
	* @return void 无返回值
	*/
	static void SetStatusCb(nim::NIMResCode code, __int64 msg_id, int unread);

	/**
	* 响应删除系统消息回调函数
	* @param[in] code 错误码
	* @param[in] msg_id 消息id
	* @param[in] unread 未读的系统消息数
	* @return void 无返回值
	*/
	static void DeleteCb(nim::NIMResCode code, __int64 msg_id, int unread);
private:
	/**
	* 弹出菜单
	* @param[in] point 显示菜单的坐标
	* @return void	无返回值
	*/
	void PopupEventItemMenu(POINT point);

	/**
	* 处理菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool DelEventItemMenuItemClick(ui::EventArgs* param);
private:
	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* arg);

	/**
	* 处理弹出菜单消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnEventItemMenu(ui::EventArgs* arg);
	
private:
	ui::Button*		btn_head_;
	ui::Label*		evt_team_;
	ui::Label*		evt_time_;
	ui::Label*		evt_tip_;

	ui::Button*		btn_ok_;
	ui::Button*		btn_no_;
	ui::Label*		result_;

	__int64		msg_time_;
	int			msg_type_;
	std::string	acc_id_;
	std::string	tid_;
	__int64		msg_id_;
	int			msg_status_;

	AutoUnregister unregister_cb;
};
}
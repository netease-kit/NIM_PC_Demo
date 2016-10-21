#pragma once

namespace nim_comp
{
/** @class CustomMsgBubble
  * @brief 消息中心“自定义消息”列表项UI类，当有新的自定义消息时，负责做出界面上的改变
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class CustomMsgBubble : public ui::ListContainerElement
{
public:
	/**
	* 初始化控件
	* @param[in] msg 会话消息
	* @return void	无返回值
	*/
	void InitControl(const nim::IMMessage &msg);

private:
	/**
	* 初始化控件界面效果
	* @param[in] msg 会话消息
	* @return void	无返回值
	*/
	void InitInfo(const nim::IMMessage &msg);

	/**
	* 设置显示在界面的消息内容
	* @param[in] str 消息内容
	* @return void	无返回值
	*/
	void SetMsgText(const std::wstring &str);

public:
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

private:
	ui::RichEdit*	text_;
	ui::Label*		time_;
	ui::Label*		name_;
	ui::Button*		head_;

private:
	std::string		sender_id_;
	std::string		receiver_id_;
	nim::NIMSessionType session_type_;
};
}
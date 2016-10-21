#pragma once

namespace nim_comp
{
/** @class MergeItem
  * @brief 会话窗口左侧的合并列表合并项
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/8/29
  */
class MergeItem : public ui::ListContainerElement
{
public:
	MergeItem();

	/** 
	* 初始化控件内部指针	
	* @param[in] session_id 会话id
	* @param[in] is_team 是否为群聊
	* @return void 无返回值
	*/
	void InitControl(std::string session_id, bool is_team);

	/**
	* 设置显示的会话标题
	* @param[in] name 标题
	* @return void	无返回值
	*/
	void SetTitle(std::wstring name);

	/**
	* 设置显示的会话头像
	* @param[in] icon 头像路径
	* @return void	无返回值
	*/
	void SetIcon(std::wstring icon);

	/**
	* 设置本会话项未读数
	* @param[in] unread 未读消息数
	* @return void 无返回值
	*/
	void SetUnread(int unread);

private:

	/**
	* 处理鼠标移入消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMouseEnter(ui::EventArgs* msg);

	/**
	* 处理鼠标移出消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMouseLeave(ui::EventArgs* msg);

	/**
	* 处理控件的菜单消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnItemMenu(ui::EventArgs* msg);

	/**
	* 弹出菜单
	* @param[in] point 弹出坐标
	* @return void	无返回值
	*/
	void PopupItemMenu(POINT point);

	/**
	* 处理菜单项被单击的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool MergeItemMenuItemClick(ui::EventArgs* msg);

private:
	ui::Control		*session_icon_;
	ui::Label		*session_name_;
	ui::Button		*button_close_;
	ui::Label		*label_unread_;

	int				unread_count_;
	std::wstring	session_id_;
};
}
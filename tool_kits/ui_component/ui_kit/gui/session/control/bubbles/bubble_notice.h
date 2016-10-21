#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleNotice
  * @brief 会话窗体中聊天框内的系统通知消息项（包括入群出群通知等）
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleNotice : public ui::ListContainerElement
{
public:
	/**
	* 初始化控件内部指针
	* @return void 无返回值
	*/
	void InitControl();

	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @param[in] session_id 会话id
	* @return void 无返回值
	*/
	void InitInfo(const nim::IMMessage &msg, const UTF8String& session_id);

	/**
	* 更新控件的界面显示效果
	* @return void	无返回值
	*/
	void RefreshNotice();

	/**
	* 设置自定义通知消息的内容
	* @param[in] show_notice 通知消息内容
	* @param[in] session_id 会话id
	* @param[in] client_msg_id 消息uuid
	* @return void	无返回值
	*/
	void InitCustomInfo(const std::wstring &show_notice, const UTF8String& session_id, const UTF8String& client_msg_id);

private:

	/**
	* 响应此消息项的大小改变消息
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnResize(ui::EventArgs* arg);

	/** 
	* 调整此消息项的大小		
	* @return void 无返回值
	*/
	void OnResized();
private:
	ui::Label*	notice_;

	nim::IMMessage	msg_;
	std::string		session_id_;

	bool is_custom_info_ = false;
};
}
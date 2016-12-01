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
	* @param[in] is_custom_info 是否自定义消息通知内容
	* @return void 无返回值
	*/
	void InitInfo(const nim::IMMessage &msg, const UTF8String& session_id, bool is_custom_info = false);

	/**
	* 更新控件的界面显示效果
	* @return void	无返回值
	*/
	void RefreshNotice();

	/**
	* 获取此消息项所包含的消息内容
	* @return MsgData 消息信息结构体
	*/
	nim::IMMessage GetMsg();

	/**
	* 获取消息时间戳
	* @return string 用户id
	*/
	int64_t GetMsgTimeTag();

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
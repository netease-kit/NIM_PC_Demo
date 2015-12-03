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
	void InitControl();
	void InitInfo(const nim::IMMessage &msg, const UTF8String& session_id);
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
	ui::Label*  notice_;
};
}
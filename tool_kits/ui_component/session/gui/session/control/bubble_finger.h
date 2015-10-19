#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleFinger
  * @brief 会话窗体中聊天框内的石头剪刀布消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleFinger : public MsgBubbleItem
{
public:
	virtual void InitControl(bool bubble_right);
	virtual void InitInfo(const MsgData &msg);

	/**
	* 响应此消息项的右击消息，弹出菜单
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);
private:
	ui::Box*		msg_finger_;
	ui::Control*	finger_;
};

}
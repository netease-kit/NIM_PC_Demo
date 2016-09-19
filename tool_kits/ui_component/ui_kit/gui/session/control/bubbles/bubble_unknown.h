#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleUnknown
  * @brief 会话窗体中聊天框内的未知消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleUnknown : public MsgBubbleItem
{
public:
	virtual void InitControl(bool bubble_right);
	virtual void InitInfo(const nim::IMMessage &msg);
private:
	ui::HBox*	msg_unknown_;

	ui::Label*  unknown_tip_;
};
}
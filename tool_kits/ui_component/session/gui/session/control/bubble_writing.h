#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleWriting
  * @brief 会话窗体中聊天框内的表示对方正在输入状态的消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleWriting : public MsgBubbleItem
{
public:
	virtual void InitControl(bool bubble_right);
	virtual void InitInfo(const MsgData &msg);
private:
	ui::Box* cell_;
};

}
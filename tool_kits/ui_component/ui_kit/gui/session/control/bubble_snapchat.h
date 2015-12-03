#pragma once
#include "bubble_image.h"

namespace nim_comp
{
/** @class MsgBubbleSnapChat
  * @brief 会话窗体中聊天框内的阅后即焚消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleSnapChat : public MsgBubbleImage
{
public:
	void InitInfo(const nim::IMMessage &msg);
	virtual bool OnClicked(ui::EventArgs* arg) override;

private:
	std::wstring src_image_path_;
};
}
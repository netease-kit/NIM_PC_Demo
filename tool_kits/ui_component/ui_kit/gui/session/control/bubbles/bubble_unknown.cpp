#include "bubble_unknown.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleUnknown::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	msg_unknown_ = new HBox;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_unknown_, L"session/unknown_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_unknown_, L"session/unknown_left.xml");
	bubble_box_->Add(msg_unknown_);

	unknown_tip_ = (Label*) msg_unknown_->FindSubControl(L"unknown_tip");
}

void MsgBubbleUnknown::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);
	if (msg.type_ == nim::kNIMMessageTypeCustom)
		unknown_tip_->SetText(L"自定义消息");
}
}
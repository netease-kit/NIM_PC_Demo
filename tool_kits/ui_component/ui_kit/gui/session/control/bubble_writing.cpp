#include "bubble_writing.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleWriting::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	cell_ = new Box;
	GlobalManager::FillBoxWithCache(cell_, L"session/cell_writing.xml");
	bubble_box_->Add(cell_);
}

void MsgBubbleWriting::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);
}
}
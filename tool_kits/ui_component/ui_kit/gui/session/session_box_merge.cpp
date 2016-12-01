#include "session_box.h"
#include "session_form.h"

using namespace ui;

namespace nim_comp
{
TaskbarTabItem* SessionBox::GetTaskbarItem()
{
	return taskbar_item_;
}

void SessionBox::SetInternVisible(bool bVisible /*= true*/)
{
	Control::SetInternVisible(bVisible);
	if (m_items.empty()) return;
	for (auto it = m_items.begin(); it != m_items.end(); it++) {
		(*it)->SetInternVisible(bVisible);
	}
}

void SessionBox::Invalidate() const
{
	__super::Invalidate();

	if (taskbar_item_)
		taskbar_item_->InvalidateTab();
}

void SessionBox::SetPos(UiRect rc)
{
	__super::SetPos(rc);

	if (taskbar_item_)
		taskbar_item_->InvalidateTab();
}
}
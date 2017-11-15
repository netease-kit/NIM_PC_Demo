#include "custom_button_box.h"

namespace nim_comp
{
	void CustomButtonBox::HandleMessage(ui::EventArgs& event)
	{
		if (!IsMouseEnabled() && event.Type > ui::kEventMouseBegin && event.Type < ui::kEventMouseEnd) {
			if (m_pParent != NULL) m_pParent->HandleMessageTemplate(event);
			else __super::HandleMessage(event);
			return;
		}

		if (event.Type == ui::kEventMouseEnter) {
			if (event.pSender != this && m_pWindow) {
				if (!IsChild(this, m_pWindow->GetNewHover())) {
					return;
				}
			}
			if (m_items.empty()) return;
			for (auto it = m_items.begin(); it != m_items.end(); it++) {
				// 控制子控件显示状态
				// InternVisible状态应由子控件自己控制
				(*it)->SetVisible(false);
			}
		}
		else if (event.Type == ui::kEventMouseLeave) {
			if (event.pSender != this && m_pWindow) {
				if (IsChild(this, m_pWindow->GetNewHover())) {
					return;
				}
			}
			if (m_items.empty()) return;
			for (auto it = m_items.begin(); it != m_items.end(); it++) {
				// 控制子控件显示状态
				// InternVisible状态应由子控件自己控制
				(*it)->SetVisible(true);
			}
		}
		__super::HandleMessage(event);
	}

}
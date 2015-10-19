#ifndef UI_CORE_VBOX_H_
#define UI_CORE_VBOX_H_

#pragma once

namespace ui
{
	class UILIB_API VLayout : public Layout
	{
	public:
		VLayout();
		virtual CSize ArrangeChild(const std::vector<Control*>& m_items, UiRect rc) override;
		virtual CSize AjustSizeByChild(const std::vector<Control*>& m_items, CSize szAvailable) override;
	};

	class UILIB_API VBox : public Box
	{
	public:
		VBox();
	};
}
#endif // UI_CORE_VBOX_H_

#ifndef UI_CORE_TABBOX_H_
#define UI_CORE_TABBOX_H_

#pragma once

namespace ui
{
	class UILIB_API TabBox : public Box
	{
	public:
		TabBox(Layout* pLayout = new Layout());

		virtual bool Add(Control* pControl) override;
		virtual bool AddAt(Control* pControl, std::size_t iIndex) override;
		virtual bool Remove(Control* pControl) override;
		virtual void RemoveAll() override;
		int GetCurSel() const;
		bool SelectItem(int iIndex);
		bool SelectItem(Control* pControl);
		bool SelectItem(const std::wstring& pControlName);
		virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

		void SetFadeSwitch(bool bFadeSwitch);
		bool IsFadeSwitch() 
		{
			return m_bFadeSwith;
		}
	
	protected:
		void ShowTabItem(std::size_t it);
		void HideTabItem(std::size_t it);

	protected:
		int m_iCurSel;
		bool m_bFadeSwith;
	};
}
#endif // UI_CORE_TABBOX_H_

#ifndef UI_CONTROL_COMBO_H_
#define UI_CONTROL_COMBO_H_

#pragma once

namespace ui 
{

class CComboWnd;
class UILIB_API Combo : public Box
{
    friend class CComboWnd;
public:
    Combo();

	virtual bool Add(Control* pControl) override;
	virtual bool Remove(Control* pControl) override;
	virtual bool RemoveAt(std::size_t iIndex) override;
	virtual void RemoveAll() override;

	virtual void Activate() override;
	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;
	virtual void PaintText(HDC hDC) override;

    std::wstring GetText() const;
	ListBox* GetListBox() { return m_pLayout.get(); }
    std::wstring GetDropBoxAttributeList();
    void SetDropBoxAttributeList(const std::wstring& pstrList);
    CSize GetDropBoxSize() const;
    void SetDropBoxSize(CSize szDropBox);

	bool SelectItem(int iIndex);
	Control* GetItemAt(int iIndex);
	int GetCurSel() const { return m_iCurSel; }
	virtual int GetCount() const { return m_pLayout->GetCount(); }
    
	void AttachSelect(const EventCallback& callback) { m_pLayout->AttachSelect(callback); }

private:
	bool OnSelectItem(EventArgs* args);

protected:
    CComboWnd *m_pWindow;
	std::unique_ptr<ListBox> m_pLayout;
    int m_iCurSel;  
	ControlStateType m_uButtonState;
	CSize m_szDropBox;
	std::wstring m_sDropBoxAttributes;
};

} // namespace ui

#endif // UI_CONTROL_COMBO_H_

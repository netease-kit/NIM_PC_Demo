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

    void DoInit();

    std::wstring GetText() const;
	ListBox* GetListBox()
	{
		return m_pLayout.get();
	}
    std::wstring GetDropBoxAttributeList();
    void SetDropBoxAttributeList(const std::wstring& pstrList);
    CSize GetDropBoxSize() const;
    void SetDropBoxSize(CSize szDropBox);

    int GetCurSel() const;
    bool SelectItem(int iIndex);
	Control* GetItemAt(int iIndex);
	virtual int GetCount() const;

	virtual bool Add(Control* pControl) override;
	virtual bool Remove(Control* pControl) override;
	virtual bool RemoveAt(std::size_t iIndex) override;
	virtual void RemoveAll() override;

    virtual void Activate() override;
    virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;
    virtual void PaintText(HDC hDC) override;

	void AttachSelect(const EventCallback& callback)
	{
		m_pLayout->AttachSelect(callback);
	}

private:
	bool Combo::OnSelectItem(EventArgs* args);

protected:
    CComboWnd* m_pWindow;
	std::unique_ptr<ListBox> m_pLayout;
    int m_iCurSel;
    CSize m_szDropBox;
	ControlStateType m_uButtonState;
	std::wstring m_sDropBoxAttributes;
};

} // namespace ui

#endif // UI_CONTROL_COMBO_H_

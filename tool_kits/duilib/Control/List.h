#ifndef UI_CONTROL_LIST_H_
#define UI_CONTROL_LIST_H_

#pragma once
#include "Label.h"
#include "Box/VBox.h"
#include "Box/HBox.h"
#include "Button.h"
#include "CheckBox.h"
#include "Option.h"

namespace ui {
/////////////////////////////////////////////////////////////////////////////////////
//

typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API IListOwner
{
public:
    virtual int GetCurSel() const = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTrigger = true) = 0;
	virtual void HandleMessageTemplate(EventArgs& event) = 0;
	virtual void EnsureVisible(const UiRect& rcItem) = 0;
	virtual void StopScroll() {}
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API ListBox : public ScrollableBox, public IListOwner
{
public:
    ListBox(Layout* pLayout = new VLayout);

	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

	virtual void HandleMessage(EventArgs& event) override;
	
	virtual void HandleMessageTemplate(EventArgs& event) override;

    int GetCurSel() const;
	virtual bool ButtonDown(EventArgs& msg) override;
	virtual void StopScroll() override;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTrigger = true);
	virtual bool ScrollItemToTop(const std::wstring& itemName);
	virtual Control* GetTopItem();
	void EnsureVisible(const UiRect& rcItem);
	bool SetItemIndex(Control* pControl, std::size_t iIndex);

	void Previous(); //选中上一项
	void Next(); //选中下一项
	void ActiveItem(); //触发选中项的 双击 事件

    bool Add(Control* pControl);
    bool AddAt(Control* pControl, int iIndex);
    bool Remove(Control* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

	BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);
	static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
	int __cdecl ItemComareFunc(const void *item1, const void *item2);

	bool GetScrollSelect();
	void SetScrollSelect(bool bScrollSelect);

	void AttachSelect(const EventCallback& callback)
	{
		OnEvent[kEventSelect] += callback;
	}

private:
	bool m_bScrollSelect;
    int m_iCurSel;
	PULVCompareFunc m_pCompareFunc;
	UINT_PTR m_compareData;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API ListContainerElement : public OptionTemplate<Box>
{
public:
    ListContainerElement();

    int GetIndex() const;
    void SetIndex(int iIndex);

    IListOwner* GetOwner();
    void SetOwner(IListOwner* pOwner);
    void SetVisible(bool bVisible = true);
	void Selected(bool bSelect, bool trigger) override;
	void InvokeDoubleClickEvent();

    virtual void HandleMessage(EventArgs& event) override;

	void AttachDoubleClick(const EventCallback& callback)
	{
		OnEvent[kEventMouseDoubleClick] += callback;
	}

	void AttachReturn(const EventCallback& callback)
	{
		OnEvent[kEventReturn] += callback;
	}

protected:
    int m_iIndex;
    IListOwner* m_pOwner;
};


} // namespace ui

#endif // UI_CONTROL_LIST_H_

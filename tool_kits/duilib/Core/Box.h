#ifndef UI_CORE_BOX_H_
#define UI_CORE_BOX_H_

#pragma once

namespace ui 
{

class Box;
class UILIB_API Layout
{
public:
	Layout();
	virtual ~Layout() {}

	void SetOwner(Box* pOwner);

	static CSize SetFloatPos(Control* pControl, UiRect rcContainer);

	virtual bool SetAttribute(const std::wstring& strName, const std::wstring& strValue);
	virtual CSize ArrangeChild(const std::vector<Control*>& items, UiRect rc);
	virtual CSize AjustSizeByChild(const std::vector<Control*>& items, CSize szAvailable);

	virtual UiRect GetPadding() const;
	virtual void SetPadding(UiRect rcPadding); // 设置内边距，相当于设置客户区
	virtual int GetChildMargin() const;
	virtual void SetChildMargin(int iMargin);
	UiRect GetInternalPos() const;

protected:
	UiRect m_rcPadding;
	int m_iChildMargin;
	Box *m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//
class ScrollBar;

class UILIB_API Box : public Control
{
public:
    Box(Layout* pLayout = new Layout());
    virtual ~Box();

public:
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true) override;
	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;
	virtual void SetPos(UiRect rc) override;
	virtual void HandleMessageTemplate(EventArgs& msg) override;
	virtual void Paint(HDC hDC, const UiRect& rcPaint) override;
	virtual void SetVisible(bool bVisible = true) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	virtual void SetEnabled(bool bEnabled) override;
	virtual CSize EstimateSize(CSize szAvailable) override;
	virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint()) override;
	Control* FindSubControl(const std::wstring& pstrSubControlName);
	virtual int FindSelectable(int iIndex, bool bForward = true) const;

	virtual Control* GetItemAt(std::size_t iIndex) const;
	virtual int GetItemIndex(Control* pControl) const;
	virtual bool SetItemIndex(Control* pControl, std::size_t iIndex);
	virtual int GetCount() const;
	virtual bool Add(Control* pControl);
	virtual bool AddAt(Control* pControl, std::size_t iIndex);
	virtual bool Remove(Control* pControl);
	virtual bool RemoveAt(std::size_t iIndex);
	virtual void RemoveAll();
	void SwapChild(Control* pChild1, Control* pChild2);
	void ResetChildIndex(Control* pChild, std::size_t iIndex);

    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);
	virtual Layout* GetLayout() const;
	virtual void RetSetLayout(Layout* pLayout);
	virtual	UiRect GetPaddingPos() const;

	virtual void InvokeLoadImageCache() override;
	virtual void UnLoadImageCache() override;
	virtual void ClearImageCache() override;

	void AttachBubbledEvent(EventType eventType, const EventCallback& callback)	{ OnBubbledEvent[eventType] += callback; }

private:
	friend WindowBuilder;
	void AttachXmlBubbledEvent(EventType eventType, const EventCallback& callback) { OnXmlBubbledEvent[eventType] += callback; }

	EventMap OnXmlBubbledEvent;

protected:
	std::unique_ptr<Layout> m_pLayout;
	bool m_bAutoDestroy;
	bool m_bDelayedDestroy;
	bool m_bMouseChildEnabled;
	std::vector<Control*> m_items;
	EventMap OnBubbledEvent;
};

class UILIB_API ScrollableBox : public Box
{
public:
	ScrollableBox(Layout* pLayout);

	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;
	virtual void SetPos(UiRect rc) override;
	virtual void HandleMessage(EventArgs& event) override;
	virtual bool MouseEnter(EventArgs& msg) override;
	virtual bool MouseLeave(EventArgs& msg) override;
	virtual void Paint(HDC hDC, const UiRect& rcPaint) override;
	virtual void SetMouseEnabled(bool bEnable = true) override;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit) override;
	virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint()) override;

	virtual CSize GetScrollPos() const;
	virtual CSize GetScrollRange() const;
	virtual void SetScrollPos(CSize szPos);
	virtual void SetScrollPosY(int y);
	virtual void LineUp(int detaValue = DUI_NOSET_VALUE);
	virtual void LineDown(int detaValue = DUI_NOSET_VALUE);
	virtual void PageUp();
	virtual void PageDown();
	virtual void HomeUp();
	virtual void EndDown(bool arrange = true, bool withAnimation = true);
	virtual void LineLeft();
	virtual void LineRight();
	virtual void PageLeft();
	virtual void PageRight();
	virtual void HomeLeft();
	virtual void EndRight();
	virtual void TouchUp(int detaValue);
	virtual void TouchDown(int detaValue);
	virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual ScrollBar* GetVerticalScrollBar() const;
	virtual ScrollBar* GetHorizontalScrollBar() const;
	virtual void ProcessVScrollBar(UiRect rc, int cyRequired);
	virtual void ProcessHScrollBar(UiRect rc, int cxRequired);
	bool IsVScrollBarValid() const;
	bool IsHScrollBarValid() const;

	void ReomveLastItemAnimation();
	void PlayRenderOffsetYAnimation(int nRenderY);

	bool IsAtEnd() const;
	bool IsHoldEnd() const;
	void SetHoldEnd(bool bHoldEnd);

	int GetVerScrollUnitPixels() const;
	void SetVerScrollUnitPixels(int nUnitPixels);
	bool GetScrollBarFloat() const;
	void SetScrollBarFloat(bool bScrollBarFloat);
	UiRect GetScrollBarPadding() const;
	void SetScrollBarPadding(UiRect rcScrollBarPadding);
	bool GetDefaultDisplayScrollbar() const;
	void SetDefaultDisplayScrollbar(bool bDefaultDisplay);

	virtual void ClearImageCache() override;

	void AttachScrollChange(const EventCallback& callback) { OnEvent[kEventScrollChange] += callback; }

private:
	void LoadImageCache(bool bFromTopLeft);
	void SetPosInternally(UiRect rc);

protected:
	std::unique_ptr<ScrollBar> m_pVerticalScrollBar;
	std::unique_ptr<ScrollBar> m_pHorizontalScrollBar;

	int m_nVerScrollUnitPixels;
	bool m_bScrollProcess; // 防止SetPos循环调用
	bool m_bHoldEnd;
	bool m_bScrollBarFloat;
	bool m_bDefaultDisplayScrollbar;
	UiRect m_rcScrollBarPadding;

	CPoint m_ptLastTouchPos;
	AnimationPlayer m_scrollAnimation;
	AnimationPlayer m_renderOffsetYAnimation;
};

} // namespace ui

#endif // UI_CORE_BOX_H_

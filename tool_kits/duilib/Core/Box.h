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

	Layout& Copy(const Layout& container);

	void SetOwner(Box* pOwner);

	static CSize SetFloatPos(Control* pControl, UiRect containerRect);

	virtual bool SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);
	virtual CSize ArrangeChild(const std::vector<Control*>& m_items, UiRect rc);
	virtual CSize AjustSizeByChild(const std::vector<Control*>& m_items, CSize szAvailable);

	virtual UiRect GetPadding() const;
	virtual void SetPadding(UiRect rcPadding); // 设置内边距，相当于设置客户区
	virtual int GetChildMargin() const;
	virtual void SetChildMargin(int iMargin);
	UiRect GetInternalPos() const;

protected:
	UiRect m_rcPadding;
	int m_iChildMargin;
	Box* m_pOwner;
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
	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;
	virtual void SetPos(UiRect rc) override;
	virtual	UiRect GetPaddingPos() const;
	virtual void Paint(HDC hDC, const UiRect& rcPaint) override;

	virtual Control* GetItemAt(std::size_t iIndex) const;
	virtual int GetItemIndex(Control* pControl) const;
	virtual bool SetItemIndex(Control* pControl, std::size_t iIndex);
	virtual int GetCount() const;
	virtual bool Add(Control* pControl);
	virtual bool AddAt(Control* pControl, std::size_t iIndex);
	virtual bool Remove(Control* pControl);
	virtual bool RemoveAt(std::size_t iIndex);
	virtual void RemoveAll();
	void SwapChild(Control* child1, Control* child2);
	void ResetChildIndex(Control* child, std::size_t newIndex);

    virtual void SetVisible(bool bVisible = true) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	virtual void SetEnabled(bool bEnabled) override;
	virtual CSize EstimateSize(CSize szAvailable) override;
    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);
	virtual Layout* GetLayout() const;
	virtual void RetSetLayout(Layout* pLayout);

    virtual int FindSelectable(int iIndex, bool bForward = true) const;
    virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true) override;
    virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint()) override;
	Control* FindSubControl(const std::wstring& pstrSubControlName);

	virtual void HandleMessageTemplate(EventArgs& msg) override;

	virtual void InvokeLoadImageCache() override;
	virtual void UnLoadImageCache() override;

public:
	void AttachBubbledEvent(EventType eventType, const EventCallback& callback)
	{
		OnBubbledEvent[eventType] += callback;
	}

private:
	friend WindowBuilder;

	void AttachXmlBubbledEvent(EventType eventType, const EventCallback& callback)
	{
		OnXmlBubbledEvent[eventType] += callback;
	}

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
	void ReomveLastItemAnimation();
	bool IsAtEnd() const;
	void PlayRenderOffsetYAnimation(int renderY);
	virtual void LineLeft();
	virtual void LineRight();
	virtual void PageLeft();
	virtual void PageRight();
	virtual void HomeLeft();
	virtual void EndRight();
	virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual ScrollBar* GetVerticalScrollBar() const;
	virtual ScrollBar* GetHorizontalScrollBar() const;
	bool IsVScrollBarValid() const;
	bool IsHScrollBarValid() const;
	virtual void ProcessVScrollBar(UiRect rc, int cyRequired);
	virtual void ProcessHScrollBar(UiRect rc, int cxRequired);

	virtual void SetMouseEnabled(bool bEnable = true) override;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit) override;
	virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint()) override;

	int GetVerScrollUnitPixels() const
	{
		return nVerScrollUnitPixels;
	}
	void SetVerScrollUnitPixels(int verScrollUnitPixels)
	{
		nVerScrollUnitPixels = verScrollUnitPixels;
	}

	bool GetScrollBarFloat() const
	{
		return m_bScrollBarFloat;
	}
	void SetScrollBarFloat(bool bScrollBarFloat)
	{
		m_bScrollBarFloat = bScrollBarFloat;
	}

	UiRect GetScrollBarPadding() const
	{
		return m_rcScrollBarPadding;
	}
	void SetScrollBarPadding(UiRect rcScrollBarPadding)
	{
		m_rcScrollBarPadding = rcScrollBarPadding;
	}

	bool GetDefaultDisplayScrollbar() const
	{
		return m_bDefaultDisplayScrollbar;
	}
	void SetDefaultDisplayScrollbar(bool bDefaultDisplay) 
	{
		m_bDefaultDisplayScrollbar = bDefaultDisplay;
	}

	bool GetHoldEnd() const
	{
		return m_bHoldEnd;
	}
	void SetHoldEnd(bool bHoldEnd) 
	{
		m_bHoldEnd = bHoldEnd;
	}


	void AttachScrollChange(const EventCallback& callback)
	{
		OnEvent[kEventScrollChange] += callback;
	}

private:
	void LoadImageCache(bool bFromTopLeft);
	void SetPosInternally(UiRect rc);

protected:
	std::unique_ptr<ScrollBar> m_pVerticalScrollBar;
	std::unique_ptr<ScrollBar> m_pHorizontalScrollBar;
	bool m_bScrollProcess; // 防止SetPos循环调用
	int nVerScrollUnitPixels;
	bool m_bScrollBarFloat;
	UiRect m_rcScrollBarPadding;
	bool m_bDefaultDisplayScrollbar;
	bool m_bHoldEnd;
	AnimationPlayer m_scrollAnimation;
	AnimationPlayer m_renderOffsetYAnimation;
};

} // namespace ui

#endif // UI_CORE_BOX_H_

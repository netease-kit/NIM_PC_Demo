#ifndef UI_CORE_CONTROL_H_
#define UI_CORE_CONTROL_H_

#pragma once


namespace ui {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef Control* (CALLBACK* FINDCONTROLPROC)(Control*, LPVOID);

class UILIB_API UserDataBase
{
public:
	virtual ~UserDataBase()
	{

	}
};

class UILIB_API Control : public PlaceHolder
{
public:
    Control();
    virtual ~Control();

public:
    virtual CursorType GetCursorType() const;
	void SetCursorType(CursorType flag);

    virtual void Activate();
	virtual bool IsActivatable() const;

    // 图形相关
	std::wstring GetBkColor() const;
	void SetBkColor(const std::wstring& dwColor);
	std::wstring GetStateColor(ControlStateType stateType);
	void SetStateColor(ControlStateType stateType, const std::wstring& dwColor);

    std::wstring GetBkImage() const;
	std::string GetUTF8BkImage() const;
    void SetBkImage(const std::wstring& pStrImage);
	void SetUTF8BkImage(const std::string& pStrImage);

	std::wstring GetStateImage(ControlStateType stateType);
	void SetStateImage(ControlStateType stateType, const std::wstring& pStrImage);

	std::wstring GetForeStateImage(ControlStateType stateType);
	void SetForeStateImage(ControlStateType stateType, const std::wstring& pStrImage);

	ControlStateType GetState() const;
	void SetState(ControlStateType pStrState);
	virtual Image* GetEstimateImage();

    CSize GetBorderRound() const;
    void SetBorderRound(CSize cxyRound);
	void GetImage(Image& duiImage) const;
	bool DrawImage(HDC hDC, Image& duiImage, const std::wstring& pStrModify = L"", int fade = DUI_NOSET_VALUE);

	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	std::wstring GetBorderColor() const;
	void SetBorderColor(const std::wstring& dwBorderColor);

	void SetBorderSize(UiRect rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);

    // 位置相关
    virtual	UiRect GetPos(bool bContainShadow = true) const override;
    virtual void SetPos(UiRect rc) override;
    virtual UiRect GetMargin() const;
    virtual void SetMargin(UiRect rcMargin);

    // 鼠标提示
    virtual std::wstring GetToolTipText() const;
	virtual std::string GetUTF8ToolTipText() const;
	virtual void SetToolTipText(const std::wstring& pstrText);
	virtual void SetUTF8ToolTipText(const std::string& pstrText);
	virtual void SetToolTipTextId(const std::wstring& strTextId);
	virtual void SetUTF8ToolTipTextId(const std::string& strTextId);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void) const;	// 多行ToolTip单行最长宽度

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户数据
    virtual std::wstring GetDataID() const;					        //辅助函数，供用户使用
	virtual std::string GetUTF8DataID() const;				
    virtual void SetDataID(const std::wstring& pstrText);					    //辅助函数，供用户使用
	virtual void SetUTF8DataID(const std::string& pstrText);	

	virtual UserDataBase* GetUserDataBase() const;					//辅助函数，供用户使用
	virtual void SetUserDataBase(UserDataBase* userDataBase);	//辅助函数，供用户使用

    // 一些重要的属性
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些ui拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
	virtual bool IsMouseFocused() const
	{
		return m_bMouseFocused;
	}
	virtual void SetMouseFocused(bool mouseFocused)
	{
		m_bMouseFocused = mouseFocused;
	}

    virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint());

	void HandleMessageTemplate(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0, TCHAR tChar = 0, CPoint mousePos = CPoint());
    virtual void HandleMessageTemplate(EventArgs& msg);
    virtual void HandleMessage(EventArgs& msg);
	virtual bool MouseEnter(EventArgs& msg);
	virtual bool MouseLeave(EventArgs& msg);
	virtual bool ButtonDown(EventArgs& msg);
	virtual bool ButtonUp(EventArgs& msg);
    virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);
	void SetClass(const std::wstring& pstrClass);
    void ApplyAttributeList(const std::wstring& strList);
	bool OnApplyAttributeList(const std::wstring& receiver, const std::wstring& strList, EventArgs* eventArgs);

	virtual CSize EstimateSize(CSize szAvailable);
	virtual CSize EstimateText(CSize szAvailable, bool& reEstimateSize);

    void AlphaPaint(HDC hDC, const UiRect& rcPaint);
	virtual void Paint(HDC hDC, const UiRect& rcPaint);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
	virtual void PaintStatusColor(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
	virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

	void SetNoFocus(); //控件永远不要焦点，与KillFocus不一样

	void SetAlpha(int alpha);
	int GetAlpha() const
	{
		return m_nAlpha;
	}

	void SetHotAlpha(int nHotAlpha);
	int GetHotAlpha() const
	{
		return m_nHotAlpha;
	}

	bool IsAlpha() const
	{
		return m_nAlpha != 255;
	}

	CPoint GetRenderOffset() const
	{
		return m_renderOffset;
	}
	void SetRenderOffset(CPoint renderOffset) 
	{
		m_renderOffset = renderOffset;
		Invalidate();
	}
	void SetRenderOffsetX(int renderOffsetX) {
		m_renderOffset.x = renderOffsetX;
		Invalidate();
	}
	void SetRenderOffsetY(int renderOffsetY) {
		m_renderOffset.y = renderOffsetY;
		Invalidate();
	}

	bool IsPointInWithScrollOffset(const CPoint& point) const;

	void GifPlay();
	void StopGifPlay(GifStopType type = kGifStopCurrent);
	void StartGifPlayForUI(GifStopType type = kGifStopFirst);
	void StopGifPlayForUI(GifStopType type = kGifStopCurrent);
	virtual void SetVisible_(bool bVisible);

	AnimationManager& GetAnimationManager()
	{
		return m_animationManager;
	}

	virtual void InvokeLoadImageCache();
	virtual void UnLoadImageCache();

	void AttachAllEvents(const EventCallback& callback)
	{
		OnEvent[kEventAll] += callback;
	}

	void AttachMouseEnter(const EventCallback& callback)
	{
		OnEvent[kEventMouseEnter] += callback;
	}

	void AttachMouseLeave(const EventCallback& callback)
	{
		OnEvent[kEventMouseLeave] += callback;
	}

	void AttachMouseHover(const EventCallback& callback)
	{
		OnEvent[kEventMouseHover] += callback;
	}

	void AttachButtonDown(const EventCallback& callback)
	{
		OnEvent[kEventMouseButtonDown] += callback;
	}

	void AttachButtonUp(const EventCallback& callback)
	{
		OnEvent[kEventMouseButtonUp] += callback;
	}

	void AttachSetFocus(const EventCallback& callback)
	{
		OnEvent[kEventSetFocus] += callback;
	}

	void AttachKillFocus(const EventCallback& callback)
	{
		OnEvent[kEventKillFocus] += callback;
	}

	void AttachMenu(const EventCallback& callback)
	{
		OnEvent[kEventMouseMenu] += callback;
	}

	void AttachResize(const EventCallback& callback)
	{
		OnEvent[kEventResize] += callback;
	}

	void DetachEvent(EventType type);

protected:
	friend WindowBuilder;

	void AttachXmlEvent(EventType eventType, const EventCallback& callback)
	{
		OnXmlEvent[eventType] += callback;
	}

protected:
	EventMap OnXmlEvent;
	EventMap OnEvent;
	std::unique_ptr<UserDataBase> m_sUserDataBase;
	bool m_bMenuUsed;
	bool m_bEnabled;
	bool m_bMouseEnabled;
	bool m_bKeyboardEnabled;
	bool m_bFocused;
	bool m_bMouseFocused;
	bool m_bSetPos; // 防止SetPos循环调用
	bool m_bNoFocus; //控件不需要焦点
	bool m_bClip;
	bool m_bGifPlay;//UI的开关
	CSize m_szEstimateSize;
	CPoint m_renderOffset;
	CSize m_cxyBorderRound;
	UiRect m_rcMargin;
	UiRect m_rcPaint;
	UiRect m_rcBorderSize;
	CursorType m_cursorType; //影响控件的鼠标形状
	ControlStateType m_uButtonState;
	int m_nBorderSize;
	int m_nTooltipWidth;
	int m_nAlpha;
	int m_nHotAlpha;
	std::wstring m_sToolTipText;
	std::wstring m_sToolTipTextId;
	std::wstring m_sUserData;
	std::wstring m_dwBkColor;
	StateColorMap m_colorMap;
	std::wstring m_dwBorderColor;
	nbase::WeakCallbackFlag m_gifWeakFlag;
	AnimationManager m_animationManager;
	StateImageMap m_imageMap;
	Image m_diBkImage;
	nbase::WeakCallbackFlag m_LoadBkImageWeakFlag;
};

} // namespace ui

#endif // UI_CORE_CONTROL_H_

#ifndef UI_CORE_CONTROL_H_
#define UI_CORE_CONTROL_H_

#pragma once

namespace ui 
{

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

    // 图形相关
	std::wstring GetBkColor() const;
	void SetBkColor(const std::wstring& strColor);
	std::wstring GetStateColor(ControlStateType stateType);
	void SetStateColor(ControlStateType stateType, const std::wstring& strColor);

    std::wstring GetBkImage() const;
	std::string GetUTF8BkImage() const;
    void SetBkImage(const std::wstring& strImage);
	void SetUTF8BkImage(const std::string& strImage);
	std::wstring GetStateImage(ControlStateType stateType);
	void SetStateImage(ControlStateType stateType, const std::wstring& strImage);
	std::wstring GetForeStateImage(ControlStateType stateType);
	void SetForeStateImage(ControlStateType stateType, const std::wstring& strImage);
	ControlStateType GetState() const;
	void SetState(ControlStateType pStrState);
	virtual Image* GetEstimateImage();

	//边框相关
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	std::wstring GetBorderColor() const;
	void SetBorderColor(const std::wstring& strBorderColor);

	void SetBorderSize(UiRect rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);

	CSize GetBorderRound() const;
	void SetBorderRound(CSize cxyRound);

    // 鼠标相关
	virtual CursorType GetCursorType() const;
	void SetCursorType(CursorType flag);
    virtual std::wstring GetToolTipText() const;
	virtual std::string GetUTF8ToolTipText() const;
	virtual void SetToolTipText(const std::wstring& strText);
	virtual void SetUTF8ToolTipText(const std::string& strText);
	virtual void SetToolTipTextId(const std::wstring& strTextId);
	virtual void SetUTF8ToolTipTextId(const std::string& strTextId);
	virtual void SetToolTipWidth(int nWidth);
	virtual int GetToolTipWidth(void) const;	// 多行ToolTip单行最长宽度

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户数据，辅助函数，供用户使用
    virtual std::wstring GetDataID() const;
	virtual std::string GetUTF8DataID() const;				
    virtual void SetDataID(const std::wstring& strText);
	virtual void SetUTF8DataID(const std::string& strText);	

	virtual UserDataBase* GetUserDataBase() const;
	virtual void SetUserDataBase(UserDataBase* pUserDataBase);

    // 一些重要的属性
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些ui拥有窗口句柄，需要重写此函数
	virtual void SetVisible_(bool bVisible);
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
	void SetNoFocus(); //控件永远不要焦点，与KillFocus不一样
	virtual bool IsMouseFocused() const { return m_bMouseFocused;}
	virtual void SetMouseFocused(bool bMouseFocused) { m_bMouseFocused = bMouseFocused; }
	virtual bool IsActivatable() const;
	virtual void Activate();

	// 控件搜索
    virtual Control* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos = CPoint());

	// 位置相关
	virtual	UiRect GetPos(bool bContainShadow = true) const override;
	virtual void SetPos(UiRect rc) override;
	virtual UiRect GetMargin() const;
	virtual void SetMargin(UiRect rcMargin, bool bNeedDpiScale = true);

	virtual CSize EstimateSize(CSize szAvailable);
	virtual CSize EstimateText(CSize szAvailable, bool& bReEstimateSize);

	bool IsPointInWithScrollOffset(const CPoint& point) const;

	// 消息处理
	void HandleMessageTemplate(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0, TCHAR tChar = 0, CPoint mousePos = CPoint());
    virtual void HandleMessageTemplate(EventArgs& msg);
    virtual void HandleMessage(EventArgs& msg);
	virtual bool MouseEnter(EventArgs& msg);
	virtual bool MouseLeave(EventArgs& msg);
	virtual bool ButtonDown(EventArgs& msg);
	virtual bool ButtonUp(EventArgs& msg);

	// 属性设置
    virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue);
	void SetClass(const std::wstring& strClass);
    void ApplyAttributeList(const std::wstring& strList);
	bool OnApplyAttributeList(const std::wstring& strReceiver, const std::wstring& strList, EventArgs* eventArgs);

	// 绘制操作
	void GetImage(Image& duiImage) const;
	bool DrawImage(HDC hDC, Image& duiImage, const std::wstring& strModify = L"", int nFade = DUI_NOSET_VALUE);

    void AlphaPaint(HDC hDC, const UiRect& rcPaint);
	virtual void Paint(HDC hDC, const UiRect& rcPaint);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
	virtual void PaintStatusColor(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
	virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

	void SetAlpha(int alpha);
	int GetAlpha() const { return m_nAlpha;	}
	bool IsAlpha() const { return m_nAlpha != 255; }

	void SetHotAlpha(int nHotAlpha);
	int GetHotAlpha() const { return m_nHotAlpha; }

	CPoint GetRenderOffset() const { return m_renderOffset;	}
	void SetRenderOffset(CPoint renderOffset);
	void SetRenderOffsetX(int renderOffsetX);
	void SetRenderOffsetY(int renderOffsetY);

	// Gif图片
	void GifPlay();
	void StopGifPlay(GifStopType type = kGifStopCurrent);
	void StartGifPlayForUI(GifStopType type = kGifStopFirst);
	void StopGifPlayForUI(GifStopType type = kGifStopCurrent);

	// 动画管理
	AnimationManager& GetAnimationManager()	{ return m_animationManager; }

	// 图片缓存
	virtual void InvokeLoadImageCache();
	virtual void UnLoadImageCache();
	virtual void ClearImageCache();

	void AttachAllEvents(const EventCallback& callback)	{ OnEvent[kEventAll] += callback; }
	void AttachMouseEnter(const EventCallback& callback) { OnEvent[kEventMouseEnter] += callback; }
	void AttachMouseLeave(const EventCallback& callback) { OnEvent[kEventMouseLeave] += callback; }
	void AttachMouseHover(const EventCallback& callback) { OnEvent[kEventMouseHover] += callback; }
	void AttachButtonDown(const EventCallback& callback) { OnEvent[kEventMouseButtonDown] += callback; }
	void AttachButtonUp(const EventCallback& callback) { OnEvent[kEventMouseButtonUp] += callback; }
	void AttachSetFocus(const EventCallback& callback) { OnEvent[kEventSetFocus] += callback; }
	void AttachKillFocus(const EventCallback& callback) { OnEvent[kEventKillFocus] += callback; }
	void AttachMenu(const EventCallback& callback) { OnEvent[kEventMouseMenu] += callback; }
	void AttachResize(const EventCallback& callback) { OnEvent[kEventResize] += callback; }
	void DetachEvent(EventType type);

protected:
	friend WindowBuilder;
	void AttachXmlEvent(EventType eventType, const EventCallback& callback) { OnXmlEvent[eventType] += callback; }

protected:
	EventMap OnXmlEvent;
	EventMap OnEvent;
	std::unique_ptr<UserDataBase> m_pUserDataBase;
	bool m_bMenuUsed;
	bool m_bEnabled;
	bool m_bMouseEnabled;
	bool m_bKeyboardEnabled;
	bool m_bFocused;
	bool m_bMouseFocused;
	bool m_bSetPos;		// 防止SetPos循环调用
	bool m_bNoFocus;	//控件不需要焦点
	bool m_bClip;
	bool m_bGifPlay;
	int m_nBorderSize;
	int m_nTooltipWidth;
	int m_nAlpha;
	int m_nHotAlpha;
	CSize m_szEstimateSize;
	CPoint m_renderOffset;
	CSize m_cxyBorderRound;
	UiRect m_rcMargin;
	UiRect m_rcPaint;
	UiRect m_rcBorderSize;
	CursorType m_cursorType;	//影响控件的鼠标形状
	ControlStateType m_uButtonState;
	std::wstring m_sToolTipText;
	std::wstring m_sToolTipTextId;
	std::wstring m_sUserData;
	std::wstring m_strBkColor;
	StateColorMap m_colorMap;
	Image m_bkImage;
	StateImageMap m_imageMap;
	std::wstring m_strBorderColor;
	nbase::WeakCallbackFlag m_gifWeakFlag;
	AnimationManager m_animationManager;
	nbase::WeakCallbackFlag m_loadBkImageWeakFlag;
};

} // namespace ui

#endif // UI_CORE_CONTROL_H_

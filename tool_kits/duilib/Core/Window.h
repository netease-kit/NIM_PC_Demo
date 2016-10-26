#ifndef UI_CORE_WINDOW_H_
#define UI_CORE_WINDOW_H_


#pragma once


namespace ui {


/////////////////////////////////////////////////////////////////////////////////////
//
class Box;

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020


/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct tagTFontInfo
{
	HFONT hFont;
	std::wstring sFontName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;
} TFontInfo;

// MessageFilter interface
class IUIMessageFilter
{
public:
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API Window : public virtual nbase::SupportWeakCallback
{
public:
    Window();
	~Window();

    HWND GetHWND() const;

    bool RegisterWindowClass();
    bool RegisterSuperclass();

	virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, bool isLayeredWindow = true, const UiRect& rc = UiRect(0, 0, 0, 0));
    HWND Subclass(HWND hWnd);
    void Unsubclass();
    void ShowWindow(bool bShow = true, bool bTakeFocus = true);
    UINT ShowModal();
    virtual void Close(UINT nRet = IDOK);
    void CenterWindow();	// 居中，支持扩展屏幕
    void SetIcon(UINT nRes);

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);


public:
	std::wstring GetWindowResourcePath();
	void SetWindowResourcePath(const std::wstring& strPath);

	void Init(HWND hWnd);
	void SetArrange(bool bArrange);
	void Invalidate(const UiRect& rcItem);

	HDC GetPaintDC() const;
	HWND GetTooltipWindow() const;

	//阴影相关部分
	void SetShadowAttached(bool bShadowAttached);
	UiRect GetShadowLength() const;

	UiRect GetPos(bool bContainShadow = false) const;	//bContainShadow为false表示返回值不包含阴影
	void SetPos(const UiRect& rc, UINT uFlags, HWND hWndInsertAfter = NULL, bool bContainShadow = false);	//bContainShadow为false表示rc不包含阴影
	CSize GetMinInfo(bool bContainShadow = false) const;	//bContainShadow为false表示返回值不包含阴影，
	void SetMinInfo(int cx, int cy, bool bContainShadow = false);	//bContainShadow为false表示cx cy不包含阴影
	CSize GetMaxInfo(bool bContainShadow = false) const;	//bContainShadow为false表示返回值不包含阴影，
	void SetMaxInfo(int cx, int cy, bool bContainShadow = false);	//bContainShadow为false表示cx cy不包含阴影
	CSize GetInitSize(bool bContainShadow = false) const;	//bContainShadow为false表示返回值不包含阴影，
	void SetInitSize(int cx, int cy, bool bContainShadow = false);	//bContainShadow为false表示cx cy不包含阴影

	POINT GetMousePos() const;
	UiRect GetSizeBox();
	void SetSizeBox(const UiRect& rcSizeBox);
	UiRect GetCaptionRect() const;
	void SetCaptionRect(UiRect& rcCaption);
	CSize GetRoundCorner() const;
	void SetRoundCorner(int cx, int cy);
	UiRect GetMaximizeInfo() const;
	void SetMaximizeInfo(UiRect& rcMaximize);
	UiRect GetCustomShadowRect() const;
	void SetCustomShadowRect(UiRect& rc);

	TFontInfo* GetDefaultFontInfo();

	void AddClass(const std::wstring& strClassName, const std::wstring& strControlAttrList);
	const std::map<std::wstring, std::wstring>* GetClassMap();
	std::wstring GetClassAttributes(const std::wstring& strClassName) const;
	bool RemoveClass(const std::wstring& strClassName);
	void RemoveAllClass();

	bool AttachDialog(Box* pRoot);
	bool InitControls(Control* pControl, Box* pParent = NULL);
	void ReapObjects(Control* pControl);

	bool AddOptionGroup(const std::wstring& strGroupName, Control* pControl);
	std::vector<Control*>* GetOptionGroup(const std::wstring& strGroupName);
	void RemoveOptionGroup(const std::wstring& strGroupName, Control* pControl);
	void RemoveAllOptionGroups();

	Control* GetFocus() const;
	void SetFocus(Control* pControl);
	void SetFocusNeeded(Control* pControl);

	//bool SetNextTabControl(bool bForward = true);

	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured() const;
  
	bool SendNotify(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0);
	bool SendNotify(Control* pControl, EventType msg, WPARAM wParam = 0, LPARAM lParam = 0);

	bool AddPreMessageFilter(IUIMessageFilter* pFilter);
	bool RemovePreMessageFilter(IUIMessageFilter* pFilter);

	bool AddMessageFilter(IUIMessageFilter* pFilter);
	bool RemoveMessageFilter(IUIMessageFilter* pFilter);

	void AddDelayedCleanup(Control* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

	Control* GetRoot() const;
	Control* FindControl(POINT pt) const;
	Control* FindControl(const std::wstring& strName) const;
	Control* FindSubControlByPoint(Control* pParent, POINT pt) const;
	Control* FindSubControlByName(Control* pParent, const std::wstring& strName) const;
	Control* FindSubControlByClass(Control* pParent, const type_info& typeinfo, int iIndex = 0);
	std::vector<Control*>* FindSubControlsByClass(Control* pParent, const type_info& typeinfo);
	std::vector<Control*>* GetSubControlsByClass();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT DoHandlMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled);
	void Paint();
	virtual void OnInitLayout();
	LRESULT CallWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

	void KillFocus();

	HBITMAP GetBackgroundBitmap()
	{
		return m_hbmpBackground;
	}

	LPBYTE GetBackgroundBits() const
	{
		return  m_pBmpBackgroundBits;
	}
	
	Control* GetNewHover()
	{
		return m_pNewHover;
	}

	POINT GetLastMousePos() const
	{
		return m_ptLastMousePos;
	}

	bool GetCanvasTransparent() const
	{
		return m_bIsCanvasTransparent;
	}
	bool SelectCanvasTransparent(bool bCanvasTransparent)
	{
		bool oldValue = m_bIsCanvasTransparent;
		m_bIsCanvasTransparent = bCanvasTransparent;
		return oldValue;
	}

	double GetHeightPercent() const
	{
		return m_heightPercent;
	}
	void SetHeightPercent(double heightPercent)
	{
		m_heightPercent = heightPercent;
	}

	void SetTextId(const std::wstring& textId)
	{
		::SetWindowText(m_hWnd, MutiLanSupport::GetInstance()->GetStringViaID(textId).c_str());
	}

	void AttachWindowClose(const EventCallback& callback)
	{
		OnEvent[kEventWindowClose] += callback;
	}

protected:
	virtual std::wstring GetWindowClassName() const 
	{
		ASSERT(FALSE);
		return L"";
	}
	virtual std::wstring GetSuperClassName() const;
	virtual UINT GetClassStyle() const;

	virtual void OnFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static Control* CALLBACK __FindControlFromNameHash(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlFromCount(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlFromPoint(Control* pThis, LPVOID pData);
	//static Control* CALLBACK __FindControlFromTab(Control* pThis, LPVOID pData);
	//static Control* CALLBACK __FindControlFromShortcut(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlFromUpdate(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlFromName(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlFromClass(Control* pThis, LPVOID pData);
	static Control* CALLBACK __FindControlsFromClass(Control* pThis, LPVOID pData);

private:
	void SetAlpha(int alpha);
	void SetRenderOffset(CPoint renderOffset);
	void SetRenderOffsetX(int renderOffsetX);
	void SetRenderOffsetY(int renderOffsetY);

protected:
	HWND m_hWnd;
	Box* m_pRoot;
	EventMap OnEvent;

protected:
	WNDPROC m_OldWndProc;
	bool m_bSubclassed;

	int m_nAlpha;
	CPoint m_renderOffset;
	HDC m_hDcPaint;
	HDC m_hDcBackground;
	HBITMAP m_hbmpBackground;
	LPBYTE m_pBmpBackgroundBits;
	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;

	Control* m_pFocus;
	Control* m_pNewHover;
	Control* m_pEventHover;
	Control* m_pEventClick;
	Control* m_pEventKey;

	CPoint m_ptLastMousePos;
	CSize m_szMinWindow;
	CSize m_szMaxWindow;
	CSize m_szInitWindowSize;
	UiRect m_rcMaximizeInfo;
	UiRect m_rcSizeBox;
	UiRect m_rcCustomShadow;
	CSize m_szRoundCorner;
	UiRect m_rcCaption;
	UINT m_uTimerID;
	bool m_bFirstLayout;
	bool m_bIsArranged;
	bool m_bFocusNeeded;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	bool m_bIsLayeredWindow;
	bool m_bIsCanvasTransparent;

	std::vector<IUIMessageFilter*> m_aPreMessageFilters;
	std::vector<IUIMessageFilter*> m_aMessageFilters;
	std::vector<Control*> m_aDelayedCleanup;
	std::vector<Control*> m_aFoundControls;
	std::map<std::wstring, Control*> m_mNameHash;
	std::map<std::wstring, std::vector<Control*>> m_mOptionGroup;

	TFontInfo m_DefaultFontInfo;
	std::map<std::wstring, std::wstring> m_DefaultAttrHash;
	std::wstring m_pStrWindowResourcePath;			//每个窗口的资源路径,等于GetSkinFolder()
	std::vector<ITranslateAccelerator*> m_aTranslateAccelerator;
	double m_heightPercent;
	nbase::WeakCallbackFlag m_closeFlag;
	

protected:
	class Shadow
	{
	public:
		Shadow();
		
		void SetShadowAttached(bool bShadowAttached)
		{
			m_bShadowAttached = bShadowAttached;
		}
		bool IsShadowAttached() const
		{
			return m_bShadowAttached;
		}

		UiRect GetShadowLength() const;

		Box* AttachShadow(Box* pRoot);
		void SetFocus(bool bFocused);
		void MaximizedOrRestored(bool isMaximized);

	private:
		UiRect m_rcShadowLength;
		bool m_bShadowAttached;
		Box* m_pRoot;
	};

	Shadow m_shadow;

};

} // namespace ui

#endif // UI_CORE_WINDOW_H_

#ifndef UI_CORE_WINDOW_H_
#define UI_CORE_WINDOW_H_

#pragma once

namespace ui 
{

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
	virtual std::wstring GetWindowClassName() const;
	virtual std::wstring GetSuperClassName() const;
	virtual UINT GetClassStyle() const;
	HWND Subclass(HWND hWnd);
	void Unsubclass();

	virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, bool isLayeredWindow = true, const UiRect& rc = UiRect(0, 0, 0, 0));
	virtual void Close(UINT nRet = IDOK);

    void ShowWindow(bool bShow = true, bool bTakeFocus = true);
    UINT ShowModal();
    void CenterWindow();	// 居中，支持扩展屏幕
    void SetIcon(UINT nRes);

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	void AttachWindowClose(const EventCallback& callback);

protected:
	virtual void OnFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	void Init(HWND hWnd);
	bool AttachDialog(Box* pRoot);
	bool InitControls(Control* pControl, Box* pParent = NULL);
	void ReapObjects(Control* pControl);

	// 资源相关部分
	std::wstring GetWindowResourcePath();
	void SetWindowResourcePath(const std::wstring& strPath);

	TFontInfo* GetDefaultFontInfo();

	void AddClass(const std::wstring& strClassName, const std::wstring& strControlAttrList);
	const std::map<std::wstring, std::wstring>* GetClassMap();
	std::wstring GetClassAttributes(const std::wstring& strClassName) const;
	bool RemoveClass(const std::wstring& strClassName);
	void RemoveAllClass();

	bool AddOptionGroup(const std::wstring& strGroupName, Control* pControl);
	std::vector<Control*>* GetOptionGroup(const std::wstring& strGroupName);
	void RemoveOptionGroup(const std::wstring& strGroupName, Control* pControl);
	void RemoveAllOptionGroups();

	void ClearImageCache();

	// 窗口属性
	POINT GetMousePos() const;
	UiRect GetSizeBox();
	void SetSizeBox(const UiRect& rcSizeBox);
	UiRect GetCaptionRect() const;
	void SetCaptionRect(UiRect& rcCaption);
	CSize GetRoundCorner() const;
	void SetRoundCorner(int cx, int cy);
	UiRect GetMaximizeInfo() const;
	void SetMaximizeInfo(UiRect& rcMaximize);
	UiRect GetAlphaFixCorner() const;
	void SetAlphaFixCorner(UiRect& rc);
	double GetHeightPercent() const;
	void SetHeightPercent(double heightPercent);
	void SetTextId(const std::wstring& strTextId);

	// 阴影相关部分
	void SetShadowAttached(bool bShadowAttached);
	std::wstring GetShadowImage() const;
	void SetShadowImage(const std::wstring &strImage);
	UiRect GetShadowCorner() const;
	void SetShadowCorner(const UiRect rect);

	// bContainShadow为false表示返回值不包含阴影
	UiRect GetPos(bool bContainShadow = false) const;
	CSize GetMinInfo(bool bContainShadow = false) const;
	CSize GetMaxInfo(bool bContainShadow = false) const;
	CSize GetInitSize(bool bContainShadow = false) const;
	// bContainShadow为false表示rc不包含阴影
	// bNeedDpiScale为false表示不需要把rc根据dpi自动调整
	void SetPos(const UiRect& rc, bool bNeedDpiScale, UINT uFlags, HWND hWndInsertAfter = NULL, bool bContainShadow = false);
	// bContainShadow为false表示cx cy不包含阴影
	void SetMinInfo(int cx, int cy, bool bContainShadow = false);
	void SetMaxInfo(int cx, int cy, bool bContainShadow = false);
	void SetInitSize(int cx, int cy, bool bContainShadow = false, bool bNeedDpiScale = true);

	// 窗口内部消息处理
	bool AddPreMessageFilter(IUIMessageFilter* pFilter);
	bool RemovePreMessageFilter(IUIMessageFilter* pFilter);

	bool AddMessageFilter(IUIMessageFilter* pFilter);
	bool RemoveMessageFilter(IUIMessageFilter* pFilter);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

	bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT DoHandlMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled);
	LRESULT CallWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 焦点相关
	Control* GetFocus() const;
	void SetFocus(Control* pControl);
	void SetFocusNeeded(Control* pControl);
	void KillFocus();

	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured() const;

	Control* GetNewHover();
	POINT GetLastMousePos() const;
	HWND GetTooltipWindow() const;

	//bool SetNextTabControl(bool bForward = true);

	// 控件相关
	Control* GetRoot() const;
	void SetArrange(bool bArrange);
	void AddDelayedCleanup(Control* pControl);
	Control* FindControl(POINT pt) const;
	Control* FindControl(const std::wstring& strName) const;
	Control* FindSubControlByPoint(Control* pParent, POINT pt) const;
	Control* FindSubControlByName(Control* pParent, const std::wstring& strName) const;
	Control* FindSubControlByClass(Control* pParent, const type_info& typeinfo, int iIndex = 0);
	std::vector<Control*>* FindSubControlsByClass(Control* pParent, const type_info& typeinfo);
	std::vector<Control*>* GetSubControlsByClass();

	bool SendNotify(EventType eventType, WPARAM wParam = 0, LPARAM lParam = 0);
	bool SendNotify(Control* pControl, EventType msg, WPARAM wParam = 0, LPARAM lParam = 0);

	// 绘制相关
	HDC GetPaintDC() const;
	IRenderContext* GetRenderContext() const;
	void Invalidate(const UiRect& rcItem);
	void Paint();
	void SetAlpha(int nAlpha);

	bool IsRenderTransparent() const;
	bool SetRenderTransparent(bool bCanvasTransparent);

	virtual void OnInitLayout();

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
	void SetRenderOffset(CPoint renderOffset);
	void SetRenderOffsetX(int renderOffsetX);
	void SetRenderOffsetY(int renderOffsetY);

protected:
	HWND m_hWnd;
	WNDPROC m_OldWndProc;
	bool m_bSubclassed;

	Box* m_pRoot;
	EventMap OnEvent;

protected:
	CSize m_szMinWindow;
	CSize m_szMaxWindow;
	CSize m_szInitWindowSize;
	UiRect m_rcMaximizeInfo;
	UiRect m_rcSizeBox;
	UiRect m_rcAlphaFix;
	CSize m_szRoundCorner;
	UiRect m_rcCaption;
	double m_heightPercent;

	HDC m_hDcPaint;
	std::unique_ptr<IRenderContext> m_renderContext;
	bool m_bIsLayeredWindow;
	int m_nAlpha;
	CPoint m_renderOffset;
	bool m_bFirstLayout;

	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;

	Control* m_pFocus;
	Control* m_pNewHover;
	Control* m_pEventHover;
	Control* m_pEventClick;
	Control* m_pEventKey;
	CPoint m_ptLastMousePos;

	Control* m_pEventTouch;
	CPoint m_ptLastTouchPos;

	UINT m_uTimerID;
	bool m_bIsArranged;
	bool m_bFocusNeeded;
	bool m_bMouseTracking;
	bool m_bMouseCapture;

	std::wstring m_strWindowResourcePath; //每个窗口的资源路径,等于GetSkinFolder()
	TFontInfo m_defaultFontInfo;
	std::map<std::wstring, std::wstring> m_defaultAttrHash;
	std::map<std::wstring, std::vector<Control*>> m_mOptionGroup;

	std::vector<IUIMessageFilter*> m_aPreMessageFilters;
	std::vector<IUIMessageFilter*> m_aMessageFilters;
	std::vector<ITranslateAccelerator*> m_aTranslateAccelerator;

	std::vector<Control*> m_aDelayedCleanup;
	std::vector<Control*> m_aFoundControls;
	std::map<std::wstring, Control*> m_mNameHash;

	nbase::WeakCallbackFlag m_closeFlag;
	
	Shadow m_shadow;
};

} // namespace ui

#endif // UI_CORE_WINDOW_H_

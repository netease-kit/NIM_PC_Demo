/** @file cef_control.h
 * @brief 封装Cef浏览器对象为duilib控件
 * @copyright (c) 2016, NetEase Inc. All rights reserved
 * @author Redrain
 * @date 2016/7/19
 */
#pragma once
#include "include/cef_base.h"
#include "cef_control_event.h"
#include "browser_handler.h"
#include "memory_dc.h"

namespace ui 
{

class CefControl :public Control, public IUIMessageFilter, public nim_cef::BrowserHandler::HandlerDelegate
{	
public:
	CefControl(void);
	~CefControl(void);	

	virtual void Init() override;
	virtual void SetPos(UiRect rc) override;
	virtual void HandleMessage(EventArgs& event) override;
	virtual void SetVisible(bool bVisible = true) override;
	virtual void SetInternVisible(bool bVisible = true) override;
	virtual void Paint(IRenderContext* pRender, const UiRect& rcPaint) override;

	// 处理窗体消息，转发到Cef浏览器对象
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

protected:
	// 转换窗体消息并转发到Cef浏览器对象
	LRESULT SendButtonDownEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendButtonDoubleDownEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendButtonUpEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendMouseMoveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendMouseWheelEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendMouseLeaveEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT SendCaptureLostEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	
	static bool IsKeyDown(WPARAM wparam);
	static int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam);
	static int GetCefMouseModifiers(WPARAM wparam);

public:
	// 控件对外的控制接口
	void LoadURL(const CefString& url);
	void LoadString(const CefString& stringW, const CefString& url);
	void GoBack();
	void GoForward();
	bool CanGoBack();
	bool CanGoForward();
	void Refresh();
	void StopLoad();
	bool IsLoading();
	void ExecJavaScript(const CefString& js);

	CefString GetURL();
	std::string GetUTF8URL();
	CefString GetMainURL(const CefString& url);

public:
	// 事件处理接口回调函数，当浏览器发生这些事件后会调用回调函数来通知
	void AttachBeforeContextMenu(const OnBeforeMenuEvent& callback){ cb_before_menu_ = callback; }
	void AttachMenuCommand(const OnMenuCommandEvent& callback){ cb_menu_command_ = callback; }
	void AttachTitleChange(const OnTitleChangeEvent& callback){ cb_title_change_ = callback; }
	void AttachUrlChange(const OnUrlChangeEvent& callback){ cb_url_change_ = callback; }
	void AttachMainURLChange(OnMainURLChengeEvent cb){ cb_main_url_change_ = cb; }
	void AttachBeforeNavigate(const OnBeforeBrowseEvent& callback){ cb_before_navigate_ = callback; }
	void AttachLinkClick(const OnLinkClickEvent& callback){ cb_link_click_ = callback; }
	void AttachLoadingStateChange(const OnLoadingStateChangeEvent& callback){ cb_loadstate_change_ = callback; }
	void AttachLoadStart(const OnLoadStartEvent& callback){ cb_load_start_ = callback; }
	void AttachLoadEnd(const OnLoadEndEvent& callback){ cb_load_end_ = callback; }
	void AttachLoadError(const OnLoadErrorEvent& callback){ cb_load_error_ = callback; }
	void AttachJsCallback(const OnJsCallbackEvent& callback);

private:
	// 处理BrowserHandler的HandlerDelegate委托接口

	// 当浏览器渲染数据变化时，会触发此接口，此时把渲染数据保存到内存dc
	// 并且通知窗体刷新控件，在控件的Paint函数里把内存dc的位图画到窗体上
	// 由此实现离屏渲染数据画到窗体上
	virtual void OnPaint(CefRefPtr<CefBrowser> browser,
		CefRenderHandler::PaintElementType type,
		const CefRenderHandler::RectList& dirtyRects,
		const std::string* buffer,
		int width,
		int height) OVERRIDE;

	virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;

	virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;

	virtual void UpdateWindowPos() OVERRIDE;

	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;

	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		CefContextMenuHandler::EventFlags event_flags) OVERRIDE;

	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;

	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;

	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) OVERRIDE;

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame) OVERRIDE;

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefLoadHandler::ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;

	// 在非UI线程中被调用
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		CefLifeSpanHandler::WindowOpenDisposition target_disposition,
		bool user_gesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) OVERRIDE;

	// 在非UI线程中被调用
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect) OVERRIDE;

	// 在非UI线程中被调用
	virtual CefRequestHandler::ReturnValue OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefRequestCallback> callback) OVERRIDE;

	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, CefRequestHandler::TerminationStatus status) OVERRIDE;

	virtual void OnJsCallback(const CefString& fun_name, const CefString& param) OVERRIDE;

private:

	CefRefPtr<nim_cef::BrowserHandler> browser_handler_ = nullptr;
	MemoryDC			dc_cef_;		// 内存dc,把cef离屏渲染的数据保存到dc中
	MemoryDC			dc_cef_popup_;	// 内存dc,把cef的popup窗口的离屏渲染数据保存到dc中
	CefRect				rect_popup_;	// 当网页的组合框一类的控件弹出时，记录弹出的位置
	CefString			url_;

	OnBeforeMenuEvent	cb_before_menu_ = nullptr;
	OnMenuCommandEvent	cb_menu_command_ = nullptr;
	OnTitleChangeEvent	cb_title_change_ = nullptr;
	OnBeforeBrowseEvent	cb_before_navigate_ = nullptr;
	OnUrlChangeEvent	cb_url_change_ = nullptr;
	OnMainURLChengeEvent cb_main_url_change_ = nullptr;
	OnLinkClickEvent	cb_link_click_ = nullptr;
	OnLoadingStateChangeEvent	cb_loadstate_change_ = nullptr;
	OnLoadStartEvent	cb_load_start_ = nullptr;
	OnLoadEndEvent		cb_load_end_ = nullptr;
	OnLoadErrorEvent	cb_load_error_ = nullptr;
	OnJsCallbackEvent	cb_js_callback_ = nullptr;
	int					js_callback_thread_id_ = -1; // 保存调用AttachJsCallback函数的代码所属线程，以后触发Js回调时把回调转到那个线程
};
}
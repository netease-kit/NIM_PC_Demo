#pragma once
#include "cef/cef_module/cef_control/cef_control.h"
#include "cef/cef_module/js_bridge/cef_js_bridge.h"
#include "module/cef/cef_global_methods.h"

class CefTip : public ui::Box
{
public:
	CefTip();
	~CefTip() { };	
public:
	static ui::Control* CreateControl(const std::wstring& pstrClass);
	virtual void DoInit() override;
private:	
	bool OnClicked(ui::EventArgs* arg);
	bool OnReturn(ui::EventArgs* arg);

	void ShowMessage(const std::string&params, nim_cef::ReportResultFunction callback);
	void OnBeforeMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);
	bool OnMenuCommand(CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags);
	void OnTitleChange(const std::wstring& title);
	void OnUrlChange(const std::wstring& url);
	bool OnLinkClick(const std::wstring& url);
	CefRequestHandler::ReturnValue OnBeforeNavigate(CefRefPtr<CefRequest> request, bool is_redirect);
	void OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward);
	void OnLoadStart();
	void OnLoadEnd(int httpStatusCode);
	void OnLoadError(CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl);
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	void OnBeforeClose(CefRefPtr<CefBrowser> browser);
	void OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);
	void ShowDevView();
	void OnDevToolVisibleChange(bool visible);
private:
	ui::Label		*lbl_title_;

	ui::RichEdit	*edit_url_;
	ui::RichEdit	*edit_js_;
	ui::CefControl	*cef_control_;
	ui::CefControl	*cef_control_dev_;

	std::shared_ptr<nim_comp::CefGlobalFunctions> global_functions_;
};
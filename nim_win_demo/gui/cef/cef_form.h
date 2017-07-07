/** @file cef_form.h
* @brief ÀëÆÁÄ£Ê½Cefä¯ÀÀÆ÷¿Ø¼þ²âÊÔ´°¿Ú
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2016/7/19
*/
#pragma once
#include "util/window_ex.h"
#include "shared/auto_unregister.h"

#include "cef/cef_module/cef_control.h"

class CefForm : public nim_comp::WindowEx
{
public:
	CefForm();
	~CefForm() { };
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnWndSizeMax(bool max);
	bool OnClicked(ui::EventArgs* arg);
	bool OnReturn(ui::EventArgs* arg);

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
	void OnJsCallback(const CefString& fun_name, const CefString& param);

public:
	static const LPCTSTR kClassName;

private:
	ui::Label		*lbl_title_;
	ui::Button		*btn_max_restore_;

	ui::RichEdit	*edit_url_;
	ui::RichEdit	*edit_js_;
	ui::CefControl	*cef_control_;
};
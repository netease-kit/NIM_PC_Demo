#pragma once
#include "include/cef_browser.h"
#include "cef_control/cef_control.h"

class CefChatroomList : public ui::Box
{
public:
	CefChatroomList();
	~CefChatroomList();

	virtual void DoInit() override;
	static ui::Control* CreateControl(const std::wstring& pstrClass);

	void InvokChatroomList();

private:
	void ShowChatroomById(const std::string& params, nim_cef::ReportResultFunction callback);

private:
	/**
	 * 在 browser 创建回调该方法，此时可以在 native 端注册提供 js 调用的方法了。
	 */
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	/**
	 * 右键菜单
	 */
	void OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);
	/**
	 * 页面加载完成
	 */
	void OnLoadEnd(int httpStatusCode);

private:
	ui::CefControl*	cef_control_;
};
#pragma once
#include "cef/cef_module/cef_control/cef_control.h"
#include "gui/profile_form/head_modify_form.h"

namespace nim_comp
{

using namespace ui;

class ProfileMine : public WindowEx
{
public:
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual Control* CreateControl(const std::wstring& pstrClass) override;

	virtual void InitWindow() override;

	// duilib
private:
	bool OnClicked(ui::EventArgs* arg);

	// cef
private:
	/**
	 * 在页面加载完毕后回调该方法，此时页面 register 的方法均可以调用了。
	 */
	void OnLoadEnd(int httpStatusCode);
	/**
	 * 在 browser 创建回调该方法，此时可以在 native 端注册提供 js 调用的方法了。
	 */
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	/**
	 * 右键菜单
	 */
	void OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);
	void OnBeforeClose(CefRefPtr<CefBrowser> browser);
	void OnMultiportPushConfigChange(int rescode, bool switch_on);
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnMiscUInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnModifyHeaderComplete(const std::string& id, const std::string &url);
	void UpdateUInfoHeaderCallback(int res);

private:
	void SetUserInfo();
	void SetMultiportPushStatus();
	void SetWebRTCStatus();

	void UpdateUserInfo(const std::string& params, nim_cef::ReportResultFunction callback);
	void UpdateMultiportPushStatus(const std::string& params, nim_cef::ReportResultFunction callback);
	void UpdateWebRTCStatus(const std::string& params, nim_cef::ReportResultFunction callback);
	void UpdateHeadImage(const std::string& params, nim_cef::ReportResultFunction callback);

public:
	static const LPCTSTR kClassName;

private:
	AutoUnregister		unregister_cb;
	ui::CefControl*		cef_control_;
	nim::UserNameCard	info_;
};

}

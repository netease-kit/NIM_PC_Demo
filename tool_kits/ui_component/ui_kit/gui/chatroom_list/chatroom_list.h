#pragma once
#include "cef/cef_module/cef_control/cef_control.h"
#include "cef/cef_module/cef_control/cef_native_control.h"

namespace nim_comp
{

using namespace ui;

class ChatroomList : public WindowEx
{
public:
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual Control* CreateControl(const std::wstring& pstrClass) override;

	virtual void InitWindow() override;

private:
	bool OnClicked(ui::EventArgs* arg);

private:
	void OnLoadEnd(int httpStatusCode);
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);

private:

public:
	static const LPCTSTR kClassName;

private:
	ui::CefNativeControl*		cef_control_;
};

}

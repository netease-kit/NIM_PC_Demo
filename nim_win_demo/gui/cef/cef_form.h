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
class CefTip;
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
	
	
	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void OnWndSizeMax(bool max);
	bool OnClicked(ui::EventArgs* arg);
public:
	static const LPCTSTR kClassName;

private:
	ui::Button	*btn_max_restore_;
	CefTip* cef_tip_;
};
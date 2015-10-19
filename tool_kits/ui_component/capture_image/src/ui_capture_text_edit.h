#ifndef _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#define _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#pragma once

#include "duilib/UIlib.h"

class CCaptureTextEdit : public ui::RichEdit
{
public:
	CCaptureTextEdit(const ui::UiRect &rect);
	~CCaptureTextEdit();
	
	virtual void HandleMessage(ui::EventArgs& event);
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 
	ui::UiRect GetCaptureTextEditRect(){ return rc_valid_; };
	void SetDotPenColor(DWORD color) { color_pen_ = color; };
private:
	void DrawRect(HDC hdc);
private:
	ui::UiRect rc_valid_;		// 有效位置(窗口绘制的位置)
	HBRUSH			 white_brush_;  // 用于绘制白底背景
	DWORD			 color_pen_;	// 虚框的颜色
};
#endif  // _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
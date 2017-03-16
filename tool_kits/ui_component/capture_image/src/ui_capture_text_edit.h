#ifndef _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#define _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
#pragma once

#include "duilib/UIlib.h"

class CCaptureTextEdit : public ui::RichEdit
{
public:
	CCaptureTextEdit(const ui::UiRect &rect);
	~CCaptureTextEdit();
	
	void InitInfos();
	virtual void HandleMessage(ui::EventArgs& event);
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 
	ui::UiRect GetCaptureTextEditRect(){ return rc_valid_; }
	void SetDotPenColor(DWORD color){ color_pen_ = color; }
	void SetTextFinishedCallback(const StdClosure& cb) { text_finished_cb_ = cb; }

private:
	void DrawRect(HDC hdc);

private:
	ui::UiRect		rc_valid_;			// 有效位置(窗口绘制的位置)
	DWORD			color_pen_;			// 虚框的颜色
	StdClosure		text_finished_cb_;	// 文字写完时需要执行的外部回调
};
#endif  // _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_TEXT_EDIT_H_
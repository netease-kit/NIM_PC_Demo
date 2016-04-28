/*
 *
 *	Author       huangjun
 *	Email        hzhuangjun2013@corp.netease.com
 *	Date         2013-12-03
 *	Copyright    Hangzhou, Netease Inc.
 *	Brief        头像选择控件
 *
 */

#ifndef _NIM_GUI_CAPTURE_FORM_UI_HEAD_SELECT_CTRL_H_
#define _NIM_GUI_CAPTURE_FORM_UI_HEAD_SELECT_CTRL_H_
#pragma once

namespace nim_comp
{
class HeadSelectControl : public ui::Control
{
public:
	HeadSelectControl();
	~HeadSelectControl();
	
	virtual void HandleMessage(ui::EventArgs& event);
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 
	int CheckCursor(POINT point);
	void GetRealSelectRect(RECT &select_rect); // 得到实际的选区（虚线区域）
	int GetHalfBoxWidth();
	bool CheckValid();

private:
	void DrawBox(HDC hdc,const ui::UiRect &rect);
	void DrawRect(Gdiplus::Graphics &gp, const ui::UiRect &rect);
	void DrawCircle(Gdiplus::Graphics &gp, const ui::UiRect &rect);
	void DrawMask(Gdiplus::Graphics &gp, const ui::UiRect &rect);

public:
	enum
	{
		SIZE_NULL = 0,
		SIZE_NW,
		SIZE_NE,
		SIZE_SW,
		SIZE_SE,
		SIZE_ALL
	};

private:
	Gdiplus::Pen		dash_pen_;		// 用于绘制虚线框的画笔
	HBRUSH				null_brush_;    // 空画刷
	HPEN				box_pen_;		// 用于绘制四个BOX
	HBRUSH				white_brush_;   // 白色画刷
	Gdiplus::Pen		circle_pen_;    // 用于绘制圆形选区
	Gdiplus::SolidBrush mask_brush_;    // 用于绘制半透明蒙版
	int					width_;         // 绘制区域的宽度
	int					height_;		// 绘制区域的高度
	int					box_width_;     // 四边方块的宽度(同高度)
	ui::UiRect    box_rect_NW_;
	ui::UiRect    box_rect_NE_;
	ui::UiRect    box_rect_SW_;
	ui::UiRect    box_rect_SE_;
	ui::UiRect    box_rect_ALL_;
};
}
#endif  // _NIM_GUI_CAPTURE_FORM_UI_HEAD_SELECT_CTRL_H_
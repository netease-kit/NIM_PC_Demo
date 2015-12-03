#include "ui_head_select_ctrl.h"

namespace nim_comp
{
HeadSelectControl::HeadSelectControl()
: circle_pen_(Gdiplus::Color::MakeARGB(255, 0x2c, 0xcf, 0x9e), 2.0f),
  mask_brush_(Gdiplus::Color::MakeARGB(60, 0x2c, 0xcf, 0x9e)),
  dash_pen_(Gdiplus::Color::MakeARGB(255, 0x2c, 0xcf, 0x9e), 1.0f)
{
	width_ = 0;
	box_width_ = 8;
	Gdiplus::REAL dash_info[2] = {10.0f, 4.0f}; // line width， space width
	dash_pen_.SetDashPattern(dash_info, 2);
	box_pen_ = CreatePen(PS_SOLID, 1 , RGB(0x61, 0x61, 0x61)); 
	LOGBRUSH lb = {BS_NULL};
	null_brush_ = CreateBrushIndirect(&lb);
	white_brush_ = CreateSolidBrush(RGB(255, 255, 255));
}

HeadSelectControl::~HeadSelectControl()
{
	// 释放GDI句柄
	DeleteObject(box_pen_);
	DeleteObject(null_brush_);
	DeleteObject(white_brush_);
}

void HeadSelectControl::HandleMessage( ui::EventArgs& event )
{
	return ui::Control::HandleMessage(event); 
}

void HeadSelectControl::Paint( HDC hDC, const ui::UiRect& rcPaint )
{
	// 先绘制背景图片
	__super::Paint(hDC, rcPaint);  
	Gdiplus::Graphics gp(hDC);

	// 保证宽度和高度相等
	ui::UiRect paint_rect = m_rcItem;
	width_ = paint_rect.right - paint_rect.left;
	height_ = paint_rect.bottom - paint_rect.top;
	paint_rect.Normalize();
	if(width_ > height_)
		paint_rect.right = paint_rect.left + height_;
	else
		paint_rect.bottom = paint_rect.top + width_;

	// 绘制虚线矩形
	ui::UiRect rect = paint_rect;
	rect.Deflate(box_width_/2 -1, box_width_/2 -1);
	DrawRect(gp, rect);
	
	// 填充半透明蒙版
	rect = paint_rect;
	rect.Deflate(box_width_/2, box_width_/2);
	DrawMask(gp, rect);

	// 绘制四个Box
	rect = paint_rect;
	rect.right = rect.left + box_width_;
	rect.bottom = rect.top + box_width_;
	box_rect_NW_ = rect;
	DrawBox(hDC, rect);
	rect = paint_rect;
	rect.left = rect.right - box_width_;
	rect.bottom = rect.top + box_width_;
	box_rect_NE_= rect;
	DrawBox(hDC, rect);
	rect = paint_rect;
	rect.right = rect.left + box_width_;
	rect.top = rect.bottom - box_width_;
	box_rect_SW_ = rect;
	DrawBox(hDC, rect);
	rect = paint_rect;
	rect.left = rect.right - box_width_;
	rect.top = rect.bottom - box_width_;
	box_rect_SE_ = rect;
	DrawBox(hDC, rect);

	// 绘制圆形
	rect = paint_rect;
	rect.Deflate(box_width_/2, box_width_/2);
	box_rect_ALL_ = rect;
	DrawCircle(gp, rect);
}

void HeadSelectControl::DrawBox( HDC hdc,const ui::UiRect &rect )
{
	HPEN old_pen = (HPEN)SelectObject(hdc, box_pen_);
	HBRUSH old_brush = (HBRUSH)SelectObject(hdc, white_brush_);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(hdc, old_pen);
	SelectObject(hdc, old_brush);
}

void HeadSelectControl::DrawRect( Gdiplus::Graphics &gp, const ui::UiRect &rect )
{
	using namespace Gdiplus;
	gp.DrawRectangle(&dash_pen_, rect.left, rect.top, rect.GetWidth(), rect.GetHeight());
}

void HeadSelectControl::DrawCircle( Gdiplus::Graphics &gp, const ui::UiRect &rect )
{
	using namespace Gdiplus;
	gp.SetSmoothingMode(SmoothingModeAntiAlias);
#if (GDIPVER >= 0x0110)
	gp.SetSmoothingMode(SmoothingModeAntiAlias8x8);
#endif
	gp.SetCompositingMode(CompositingModeSourceOver);
	gp.SetInterpolationMode(InterpolationModeHighQuality);
	gp.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	Rect ellipse_rect(rect.left, rect.top, rect.GetWidth(), rect.GetHeight());
	gp.DrawEllipse(&circle_pen_, ellipse_rect);
}

void HeadSelectControl::DrawMask( Gdiplus::Graphics &gp, const ui::UiRect &rect )
{
	using namespace Gdiplus;
	Rect fill_rect(rect.left, rect.top, rect.GetWidth() + 1, rect.GetHeight() + 1);
	gp.FillRectangle(&mask_brush_, fill_rect);
}

int HeadSelectControl::CheckCursor( POINT point )
{
	if(::PtInRect(&box_rect_SE_, point))
	{
		return SIZE_SE;
	}
	else if(::PtInRect(&box_rect_NE_, point))
	{
		return SIZE_NE;
	}
	else if(::PtInRect(&box_rect_NW_, point))
	{
		return SIZE_NW;
	}
	else if(::PtInRect(&box_rect_SW_, point))
	{
		return SIZE_SW;
	}
	else if(::PtInRect(&box_rect_ALL_, point))
	{
		return SIZE_ALL;
	}
	else
	{
		return SIZE_NULL;
	}
}

void HeadSelectControl::GetRealSelectRect( RECT &select_rect )
{
	ui::UiRect rect = select_rect;
	rect.Deflate(box_width_/2 -1, box_width_/2 -1); // 获取虚线的区域
	select_rect = rect;
}

int HeadSelectControl::GetHalfBoxWidth()
{
	return box_width_/2 - 1;
}

bool HeadSelectControl::CheckValid()
{
	return (width_ != 0) ;
}
}
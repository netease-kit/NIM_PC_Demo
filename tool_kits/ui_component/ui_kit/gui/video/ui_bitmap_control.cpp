#include "ui_bitmap_control.h"
#include <sys/timeb.h>

#define TIMER_FOR_CHECK_BITMAP	163
namespace ui
{

CBitmapControl::CBitmapControl(void)
{
	auto_size_ = false;
	need_paint_ = false;
	auto_paint_ = true;
}

CBitmapControl::~CBitmapControl(void)
{
	Clear();
}

void CBitmapControl::Paint(HDC hDC, const UiRect& rcPaint)
{
	try
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
			return;
		need_paint_ = false;
		Control::Paint(hDC, rcPaint);
		//paint hbitmap
		if (cur_paint_bitmap_info_.bitmap_)
		{
			int item_w = m_rcItem.right-m_rcItem.left;
			int item_h = m_rcItem.bottom-m_rcItem.top;
			int item_x = m_rcItem.left;
			int item_y = m_rcItem.top;
			int source_w = cur_paint_bitmap_info_.width_;
			int source_h = cur_paint_bitmap_info_.height_;
			if (source_w > 0 && source_h > 0)
			{
				int paint_w = source_w;
				int paint_h = source_h;
				//等比
				if (item_h * source_w > item_w * source_h)
				{
					paint_w = item_w;
					paint_h = paint_w * source_h / source_w;
				} 
				else
				{
					paint_h = item_h;
					paint_w = paint_h * source_w / source_h;
				}
				//居中
				item_x += (item_w - paint_w) / 2;
				item_y += (item_h - paint_h) / 2;


				RenderEngine::GdiDrawImage(hDC, false, m_rcPaint, cur_paint_bitmap_info_.bitmap_, false,
					UiRect(item_x, item_y, item_x + paint_w, item_y + paint_h), UiRect(0, 0, source_w, source_h), UiRect(0, 0, 0, 0));
				//HDC hCloneDC = ::CreateCompatibleDC(hDC);
				//HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, cur_paint_bitmap_info_.bitmap_);
				//int stretchBltMode = ::SetStretchBltMode(hDC, HALFTONE);
				//::StretchBlt(hDC, item_x, item_y, paint_w, paint_h, hCloneDC, \
				//	0, 0, source_w, source_h, SRCCOPY);
				//::SetStretchBltMode(hDC, stretchBltMode);
				//::SelectObject(hCloneDC, hOldBitmap);
				//::DeleteDC(hCloneDC);
			}
		}
		//绘制子控件
		for( auto it = m_items.begin(); it != m_items.end(); it++ )
		{
			Control* pControl = *it;
			if( !pControl->IsVisible() ) continue;
			UiRect controlPos = pControl->GetPos();
			if( !::IntersectRect( &m_rcPaint, &rcPaint, &controlPos ) ) continue;
			pControl->AlphaPaint( hDC, rcPaint );
		}
	}
	catch (...)
	{
		throw "CBitmapControl::DoPaint";
	}
}

void CBitmapControl::Refresh(HWND hWnd, BYTE* data, uint32_t size, uint32_t width, uint32_t height, bool reversal)
{
	uint32_t data_size = width * height * 4;
	if (size < data_size)
	{
		return;
	}
	if (cur_paint_bitmap_info_.pBitmapData_ == NULL || cur_paint_bitmap_info_.bitmap_ == NULL\
		|| cur_paint_bitmap_info_.width_ != width\
		|| cur_paint_bitmap_info_.height_ != height)
	{
		cur_paint_bitmap_info_.ClearData();
		cur_paint_bitmap_info_.ClearHBitmap();
		cur_paint_bitmap_info_.width_ = width;
		cur_paint_bitmap_info_.height_ = height;
		HDC	hdc = ::GetDC(hWnd);
		HDC hMemDC = CreateCompatibleDC(hdc);
		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = size;
		cur_paint_bitmap_info_.bitmap_ = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, 
			(void**)&cur_paint_bitmap_info_.pBitmapData_, NULL, 0);
		DeleteDC(hMemDC);
		::ReleaseDC( hWnd, hdc );
	}
	if (reversal)
	{
		Reversal_Pic((uint32_t*)data, (uint32_t*)cur_paint_bitmap_info_.pBitmapData_, width, height);
	}
	else
	{
		memcpy(cur_paint_bitmap_info_.pBitmapData_, data, data_size);
	}
	if (auto_size_)
	{
		int item_w = GetMaxWidth();
		int item_h = GetMaxHeight();
		if (item_w > 0 && item_h > 0 && width > 0 && height > 0)
		{
			int paint_w = width;
			int paint_h = height;
			//等比
			if (item_h * width > item_w * height)
			{
				paint_w = item_w;
				paint_h = paint_w * height / width;
			} 
			else
			{
				paint_h = item_h;
				paint_w = paint_h * width / height;
			}
			SetFixedHeight(paint_h);
			SetFixedWidth(paint_w);
		}
	}
	if (auto_paint_)
	{
		Invalidate();
	}
	need_paint_ = true;
}
//清理失效数据
void CBitmapControl::Clear()
{
	cur_paint_bitmap_info_.ClearData();
	cur_paint_bitmap_info_.ClearHBitmap();
	if (auto_paint_)
	{
		Invalidate();
	}
	need_paint_ = true;
}
bool CBitmapControl::Reversal_Pic( uint32_t* src, uint32_t* dest, int width, int height )
{
	if (width > 0 && height > 0 && src != NULL && dest != NULL)
	{
		int point_src = 0;
		for (int y = 0; y < height; y++)
		{
			int point_dest = point_src + width - 1;
			for (int x = 0; x < width; x++)
			{
				dest[point_dest] = src[point_src];
				point_src++;
				point_dest--;
			}
		}
		return true;
	}
	return false;
}
}



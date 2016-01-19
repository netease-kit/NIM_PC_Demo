// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: gaoqi <gaoqi@yixin.im>
// Date: 2014/3/10

//支持贴hbitmap的图片

#ifndef _UIBITMAPCONTROL_H_
#define _UIBITMAPCONTROL_H_
#pragma once

#include <map>

namespace ui
{
class CBitmapControl : public ui::Box
{

	struct BitmapDrawInfo
	{
		uint32_t width_;
		uint32_t height_;
		HBITMAP	bitmap_;
		BYTE* pData_;
		BYTE* pBitmapData_;
		BitmapDrawInfo()
		{
			width_ = 0;
			height_ = 0;
			bitmap_ = NULL;
			pData_ = NULL;
			pBitmapData_ = NULL;
		}

		~BitmapDrawInfo()
		{
			ClearHBitmap();
			ClearData();
		}
		void ClearHBitmap()
		{
			pBitmapData_ = NULL;
			if (bitmap_)
			{
				DeleteObject(bitmap_);
			}
		}
		void ClearData()
		{
			if (pData_)
			{
				delete[] pData_;
			}
		}
	};
public:
	CBitmapControl(void);
	~CBitmapControl(void);

	void Paint(HDC hDC, const UiRect& rcPaint) override;
	void Refresh(HWND hWnd, BYTE* data, uint32_t size, uint32_t width, uint32_t height, bool reversal);
	//清理数据
	void Clear();
	void SetAutoSize(bool auto_size ){auto_size_ = auto_size;}
	bool IsNeedPaint(){ return need_paint_; }
	void SetAutoPaint(bool auto_paint) { auto_paint_ = auto_paint; }
private:
	bool Reversal_Pic( uint32_t* src, uint32_t* dest, int width, int height );
protected:
	BitmapDrawInfo cur_paint_bitmap_info_;
	bool auto_size_;
	bool need_paint_;
	bool auto_paint_;
};

}

#endif // _UIBITMAPCONTROL_H_

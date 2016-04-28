// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: gaoqi
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
public:
	CBitmapControl(void);
	~CBitmapControl(void);

	void Paint(HDC hDC, const UiRect& rcPaint) override;
	//清理数据
	void Clear();

	bool Refresh(Window* wnd, bool captrue = true, bool mirror = false);

	void SetAutoSize(bool auto_size){ auto_size_ = auto_size; }
protected:
	bool auto_size_;
	int64_t timestamp_;
	std::string data_;
	int width_;
	int height_;
	Window* parent_wnd_;
};

}

#endif // _UIBITMAPCONTROL_H_

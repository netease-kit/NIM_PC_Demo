// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: gaoqi
// Date: 2014/3/10

//支持贴hbitmap的图片

#ifndef _UIBITMAPCONTROL_H_
#define _UIBITMAPCONTROL_H_
#pragma once

#include "module/video/video_frame_mng.h"
#include <map>

namespace ui
{
class CBitmapControl : public ui::Box
{
public:
	CBitmapControl(nim_comp::VideoFrameMng* video_frame_mng);
	~CBitmapControl(void);

	void SetVideoFrameMng(nim_comp::VideoFrameMng* video_frame_mng){ video_frame_mng_ = video_frame_mng; }
	void SetAccount(std::string account){ account_ = account; }
	std::string GetAccount() { return account_; }
	void Paint(HDC hDC, const UiRect& rcPaint) override;
	//清理数据
	void Clear();

	bool Refresh(Window* wnd, bool captrue = true, bool mirror = false);

	void SetAutoSize(bool auto_size){ auto_size_ = auto_size; }

	bool IsRefreshTimeout();

	float GetPicWHPercent() { return height_ > 0 ? width_*1.0 / height_ : 0; }

protected:
	std::string account_;
	bool auto_size_;
	int64_t timestamp_;
	std::string data_;
	int width_;
	int height_;
	Window* parent_wnd_;
	nim_comp::VideoFrameMng* video_frame_mng_;
};

}

#endif // _UIBITMAPCONTROL_H_

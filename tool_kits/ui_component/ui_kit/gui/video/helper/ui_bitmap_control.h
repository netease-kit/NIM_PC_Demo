#ifndef _UIBITMAPCONTROL_H_
#define _UIBITMAPCONTROL_H_
#pragma once

#include "module/video/video_frame_mng.h"
#include <map>

namespace ui
{
/** @class BitmapControl
  * @brief 支持贴hbitmap的图片
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author gaoqi
  * @date 2014/3/10
  */
class BitmapControl : public ui::Box
{
public:
	/**
	* 构造函数
	* @param[in] video_frame_mng 视频帧管理器
	* @return void	无返回值
	*/
	BitmapControl(nim_comp::VideoFrameMng* video_frame_mng);
	~BitmapControl(void);

	/**
	* 设置视频帧管理器
	* @param[in] video_frame_mng 视频帧管理器
	* @return void	无返回值
	*/
	void SetVideoFrameMng(nim_comp::VideoFrameMng* video_frame_mng){ video_frame_mng_ = video_frame_mng; }

	/**
	* 设置帐号
	* @param[in] account 帐号
	* @return void	无返回值
	*/
	void SetAccount(std::string account){ account_ = account; }

	/**
	* 获取帐号
	* @return std::strin 帐号
	*/
	std::string GetAccount() { return account_; }

	/**
	* 重写父类绘制函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 可绘制区域
	* @return void	无返回值
	*/
	void Paint(IRenderContext* pRender, const UiRect& rcPaint) override;

	/**
	* 清理数据
	* @return void	无返回值
	*/
	void Clear();

	/**
	* 刷新图像
	* @param[in] wnd 父窗口
	* @param[in] captrue 是否为录制的图像
	* @param[in] mirror 是否翻转图像
	* @return bool true 成功，false 失败
	*/
	bool Refresh(Window* wnd, bool captrue = true, bool mirror = false);

	/**
	* 设置是否自动调整大小
	* @param[in] auto_size 是否自动调整大小
	* @return void	无返回值
	*/
	void SetAutoSize(bool auto_size){ auto_size_ = auto_size; }

	/**
	* 是否已经刷新超时
	* @return bool true 是，false 否
	*/
	bool IsRefreshTimeout();

	/**
	* 获取图像宽高比
	* @return float 比例
	*/
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

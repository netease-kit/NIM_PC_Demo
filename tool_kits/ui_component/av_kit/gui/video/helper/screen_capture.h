#pragma once
#include "module/video/video_frame_mng.h"

namespace nim_comp
{
struct CaptureWindowInfo 
{
	HWND id;
	std::wstring title;
};
typedef std::vector<CaptureWindowInfo> CaptureWindowInfoList;

/** @class ScreenCapture
  * @brief 屏幕录制控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class ScreenCapture : public virtual nbase::SupportWeakCallback
{
public:
	/**
	* 枚举所有可录制的窗口的列表
	* @param[out] windows 获取窗口列表
	* @return bool true 枚举成功，false 枚举失败
	*/
	static bool GetCaptureWindowList(CaptureWindowInfoList* windows);

	/**
	* 获取系统DPI设置值
	* @return int DPI值
	*/
	static int GetDesktopScale();

	ScreenCapture();
	~ScreenCapture();

	/**
	* 开始录制
	* @param[in] id 被录制的窗口的句柄
	* @param[in] 
	* @return void	无返回值
	*/
	void StartCapture(HWND id, int time_ms=60);

	/**
	* 设置录制范围
	* @param[in] cut 是否录制窗口中的一部分
	* @param[in] rect 录制范围
	* @return void	无返回值
	*/
	void SetCutMode(bool cut, RECT rect);

	/**
	* 截取一帧位图
	* @return void	无返回值
	*/
	void CustomFrame();

	/**
	* 获取视频帧管理器
	* @return VideoFrameMng* 视频帧管理器
	*/
	VideoFrameMng* GetFrame() { return &frame_; }
private:
	/**
	* 初始化内存位图
	* @param[in] width 位图宽度
	* @param[in] height 位图高度
	* @return void	无返回值
	*/
	void InitHBitmap(int width, int height);
private:
	HBITMAP capture_bitmap_;
	HWND	capture_hwnd_;
	char* capture_data_;
	int capture_width_;
	int capture_height_;
	RECT cut_rect_;
	bool cut_screen_;
	VideoFrameMng frame_;

};
}
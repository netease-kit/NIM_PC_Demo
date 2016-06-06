#pragma once
#include "module/video/video_frame_mng.h"

namespace nim_comp
{
struct CaptureWindowInfo {
	HWND id;
	std::wstring title;
};
typedef std::vector<CaptureWindowInfo> CaptureWindowInfoList;

class ScreenCapture : public virtual nbase::SupportWeakCallback
{
public:
	static bool GetCaptureWindowList(CaptureWindowInfoList* windows);
	static int GetDesktopScale();
	ScreenCapture();
	~ScreenCapture();

	void StartCapture(HWND id, int time_ms=60);
	void SetCutMode(bool cut, RECT rect);
	void CustomFrame();
	VideoFrameMng* GetFrame() { return &frame_; }
private:
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
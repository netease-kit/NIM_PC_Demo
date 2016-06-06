#pragma once
#include "./helper/screen_capture.h"
#include "./helper/yuv_image.h"

namespace nim_comp
{
#define kScreenDefWidth		1280
#define kScreenDefHeight	720
#define kCameraDefWidth		640
#define kCameraDefHeight	480
enum CaptureType
{
	kCaptureTypeCamera = 0,
	kCaptureTypeScreen,
	kCaptureTypeScreenAndCamera,
};
//½ØÍ¼ÇøÓò
enum
{
	NOTRACK = 0,
	SIZEALL = 1 << 3,
	SIZELEFT = 1 << 4,
	SIZERIGHT = 1 << 5,
	SIZETOP = 1 << 6,
	SIZEBOTTOM = 1 << 7,
	SIZENE = 1 << 8,
	SIZENW = 1 << 9,
	SIZESE = 1 << 10,
	SIZESW = 1 << 11,
};

class ScreenCaptureTool : public WindowEx
{
public:
	ScreenCaptureTool();
	~ScreenCaptureTool();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;

	virtual void InitWindow() override;

	//´°¿ÚÊÂ¼þ
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	bool Notify(ui::EventArgs* msg);
	bool OnClicked(ui::EventArgs* msg);

	void SetVideoMng(VideoFrameMng* mng) { video_frame_mng_ = mng; }

	void StartCapture(CaptureType type, HWND id, int time_ms);
	void SetCaptureType(CaptureType type);
	void SetCaptureWnd(HWND id, bool cut);
	void SetTextLogo(std::wstring text);

private:
	void SetHitTestCursor(int hit_type);
	int CheckPtType(POINT pt);
	void ResizeTrackRect(POINT& pt);
	void StopCapture();
	void MakeFrame();
	void CopyI420Data(int type, std::string& dest, const std::string& src, int width, int height, ImageYuvDataType image_type = kYuvDataTypeNormal, POINT pt = { 0, 0 });

public:
	static const LPTSTR kClassName;

private:
	nbase::NLock lock_;
	nbase::WeakCallbackFlag capture_timer_;
	int width_;
	int height_;
	RECT track_rect_;
	//½ØÍ¼×´Ì¬
	int track_state_;
	ui::Box* track_box_;
	ui::Control* track_NW_;
	ui::Control* track_T_;
	ui::Control* track_NE_;
	ui::Control* track_L_;
	ui::Control* track_R_;
	ui::Control* track_SW_;
	ui::Control* track_B_;
	ui::Control* track_SE_;

	//½ØÍ¼ÇøÓò
	int screen_width_;
	int screen_height_;
	int screen_x_;
	int screen_y_;

	POINT point_offset_;

	ScreenCapture capture_;
	std::vector<YuvImage> yuv_image_list_;
	CaptureType capture_type_;
	nim_comp::VideoFrameMng* video_frame_mng_;
};
}
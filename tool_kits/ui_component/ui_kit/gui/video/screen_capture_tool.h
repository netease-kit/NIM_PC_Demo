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
//截图区域
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

/** @class ScreenCaptureTool
  * @brief 录屏窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class ScreenCaptureTool : public WindowEx
{
public:
	ScreenCaptureTool();
	~ScreenCaptureTool();

	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 拦截并处理窗口关闭消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否需要继续传递消息，true则不传递
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;

	/**
	* 拦截并处理鼠标左键按下
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否需要继续传递消息，true则不传递
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	/**
	* 拦截并处理鼠标松开消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否需要继续传递消息，true则不传递
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	/**
	* 拦截并处理鼠标移动消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否需要继续传递消息，true则不传递
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 设置视频帧管理器
	* @param[in] mng 视频帧管理器指针
	* @return void	无返回值
	*/
	void SetVideoMng(VideoFrameMng* mng) { video_frame_mng_ = mng; }

	/**
	* 开始录制
	* @param[in] type 录制类型
	* @param[in] id 录制目标窗口的句柄
	* @param[in] time_ms 录制间隔时间
	* @return void	无返回值
	*/
	void StartCapture(CaptureType type, HWND id, int time_ms);

	/**
	* 设置录制类型
	* @param[in] type 录制类型
	* @return void	无返回值
	*/
	void SetCaptureType(CaptureType type);

	/**
	* 设置录制目标窗口
	* @param[in] id 所属窗口的句柄
	* @param[in] cut 是否录制目标窗口中的一部分
	* @return void	无返回值
	*/
	void SetCaptureWnd(HWND id, bool cut);

	/**
	* 增加logo文字
	* @param[in] text logo文字
	* @return void	无返回值
	*/
	void SetTextLogo(std::wstring text);

private:
	/**
	* 根据命中测试结果设置光标
	* @param[in] hit_type 命中测试结果
	* @return void	无返回值
	*/
	void SetHitTestCursor(int hit_type);

	/**
	* 根据鼠标坐标判断命中测试结果
	* @param[in] pt 鼠标坐标
	* @return int 命中测试结果
	*/
	int CheckPtType(POINT pt);

	/**
	* 调整选区大小
	* @param[in] pt 调整的偏移量
	* @return void	无返回值
	*/
	void ResizeTrackRect(POINT& pt);

	/**
	* 停止录制
	* @return void	无返回值
	*/
	void StopCapture();

	/**
	* 截取一帧图像
	* @return void	无返回值
	*/
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
	//截图状态
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

	//截图区域
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
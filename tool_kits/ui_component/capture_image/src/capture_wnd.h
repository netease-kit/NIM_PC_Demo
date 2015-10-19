#ifndef NIM_WIN_GUI_CAPTURE_WND_H_
#define NIM_WIN_GUI_CAPTURE_WND_H_

#include "capture_def.h"
#include "ui_capture_edit.h"
#include "ui_color_palette.h"

#define DeTrackState(n) ((n) & 0x03)
#define DeSizeState(n) ((n) & 0xFFFFFFFC)

class CCaptureEdit;
class CaptureWindow : public ui::WindowImplBase
{
public:
	CaptureWindow(const std::wstring& user_data_dir);
	virtual ~CaptureWindow();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	//virtual std::wstring GetWindowId(void) const;
	virtual std::wstring GetWindowClassName() const override { return _T("CaptureWindow"); };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void InitWindow() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	bool Notify(ui::EventArgs* msg);

	//窗口事件
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	std::wstring GetImagePath(){return track_image_path_;}

	void SetSendInfo(std::wstring send_info); // 设置提示文本

	static BOOL SaveBitmapToFile(HBITMAP hBitmap, const std::wstring &file_path);

private:
	int GetTrackState() { return track_state_; }

	//绘制
	void DrawTrack(RECT track_rect);

	//显示工具条
	void ShowToolBar(bool edit = false);

	//显示截图大小信息
	void ShowSizeTip();

	//保存截取的区域
	BOOL SaveTrackImage(bool save_to_clipboard = true);

	//收藏截取的区域

	// 初始化位图
	BOOL InitBitmaps();

	void NormalizeRect(RECT& rect);

	void SetTrackState(int track_state) { track_state_ = track_state; } // 设置Tracker的状态

	RECT ClipTrackRect(RECT rect);//裁剪在客户区的可截图区域

	RECT GetToolBarRect();//工具条区域
	RECT GetToolBar2Rect();//工具条区域

	RECT GetSizeTipRect();//大小信息提示区域

	RECT GetDrawTrackRect(int hit_test);//弹力框元素区域

	// 对Tracker进行HitTest
	int TrackerHitTest(RECT rect, POINT point);

	// 当在调节点（线）上调节的时候，修正位置
	void AdjustOffset(RECT& rcTracker, POINT point_dest, POINT point_offset, int hit_test);

	// 根据HitTest结果取得正确的光标名
	void SetHitTestCursor(int hit_test);

	// 设置当前截图编辑的状态
	void SetCaptureEditMode(EditMode mode);
	EditMode GetCaptureEditMode() { return this->capture_edit_mode_; }
	void ResetCaptureEditMode(); // 重置编辑状态，清除上次编辑绘制

	// 保存截图到用户指定的地址
	void Save();

	void SaveTrackImageDlgCallback(BOOL ret, std::wstring file_path);

private:
	enum
	{
		NOTRACK = 0,
		TRACKING = 1,
		TRACKED = 3,
		SIZEALL			= 1<<3,
		SIZELEFT		= 1<<4,
		SIZERIGHT		= 1<<5,
		SIZETOP			= 1<<6,
		SIZEBOTTOM		= 1<<7,
		SIZENE			= 1<<8,
		SIZENW			= 1<<9,
		SIZESE			= 1<<10,
		SIZESW			= 1<<11,
		SIZELEFT_LINE	= 1<<12,
		SIZERIGHT_LINE	= 1<<13,
		SIZETOP_LINE	= 1<<14,
		SIZEBOTTOM_LINE	= 1<<15
	};
	RECT track_rect_;
	//截图状态
	int track_state_;

	//截图区域
	int screen_width_;
	int screen_height_;
	int screen_x_;
	int screen_y_;

	POINT point_offset_;

	//背景图
	HBITMAP bitmap_bg_;
	std::wstring track_image_path_;
	std::wstring send_info_;

	int toolbar_min_width_;
	int toolbar_min_width2_;

	ui::Box* toolbar_;
	// 截图编辑状态
	EditMode capture_edit_mode_;
	CCaptureEdit *capture_edit_;
	std::vector<std::wstring> vec_option_names_;
	CColorPalette* color_palette_;
	bool is_show_color_palette_;
	int32_t desktop_scale_;//百分制
	std::wstring user_data_dir_;
};

#endif // NIM_WIN_GUI_CAPTURE_WND_H_

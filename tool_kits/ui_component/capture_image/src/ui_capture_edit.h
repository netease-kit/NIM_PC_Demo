#ifndef _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_EDIT_H_
#define _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_EDIT_H_
#pragma once

#include "capture_def.h"
#include "draw_unit.h"
#include "ui_capture_text_edit.h"
#include "ui_color_palette.h"

class CCaptureEdit : public ui::Control
{
public:
	CCaptureEdit();
	~CCaptureEdit();
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override;
	
public:
	// 鼠标操作绘图
	void SetTrackRect(RECT track_rect){ track_rect_ = track_rect; };
	void SetEditMode(EditMode mode){ edit_mode_ = mode; };
	void SetColorPalette(CColorPalette* color_palette) { color_palette_ = color_palette; };
	BOOL OnLButtonDownInCaptureEditMode(POINT point, RECT tool_bar_rect); // 编辑时鼠标按下
	void OnMouseMoveInCaptureEditMode(POINT point, RECT tool_bar_rect); // 编辑时鼠标拖动
	void OnLButtonUpInCaptureEditMode(POINT point); // 编辑时鼠标弹起
	void DrawExtraUnits(HDC hdc); // 控件重绘、保存截图时调用
	void OnTextEditFinished(); // 当文本编辑结束时
	void Undo(); // 撤销
	void Redo(); // 重做
	void ReleaseAllDrawUnits(); // 释放所有绘制单元

protected:
	// 绘制相关
	void OnDrawUnitStart(const ui::CPoint &point);
	void OnDrawUnitProcess(const ui::CPoint &point);
	void OnDrawUnitEnd(const ui::CPoint &point);
	void CreateDrawUnit(const ui::CPoint& point);
	void SetEditModeCursor(const ui::CPoint& point);
	void SetNormalCursor();
	void CalcValidEndPoint(ui::CPoint& pt_end);
	void SubmitDrawUnit();
	void ClearHistoryVector();
	void PaintContent();
	//获取遮罩
	void GetExtraUnitsBimap(HDC hdc, HBITMAP& bitmap, DrawUnit::BlurType blur_type = DrawUnit::BLUR_TYPE_NONE, int range = 0);
	void ReleaseHBitmap(HBITMAP& bitmap);
	bool CheckBlurBitmap(HDC hdc, DrawUnit* draw_units_);
	//获取指针图片对象
	void CheckCursor(HCURSOR &cursor, std::wstring image_name);

private:
	std::vector<DrawUnit*>			vec_draw_units_;			// 当前绘制单元列表
	std::vector<DrawUnit*>			vec_history_draw_units_;	// 撤销/回滚绘制单元列表
	DrawUnit*						current_draw_unit_;         // 当前正在绘制的单元
	BOOL							is_begin_draw_;				// 是否开始绘制
	ui::UiRect				track_rect_;				// 当前截图的区域（也是编辑的可行域）
	EditMode						edit_mode_;					// 当前编辑的模式（确定绘制的图形）
	TCHAR*							str_cursor_;				// 当前编辑操作要显示的鼠标指针
	CCaptureTextEdit*				text_edit_;					// 文本编辑指针
	CColorPalette*					color_palette_;				// 调色板指针
	HBITMAP							extra_bitmap_;
	HBITMAP							blur_bitmap_;
	int								blur_range_;
	DrawUnit::BlurType				blur_type_;
	HCURSOR							cursor_blur_big_;
	HCURSOR							cursor_blur_mid_;
	HCURSOR							cursor_blur_small_;
};
#endif  // _NIM_GUI_CAPTURE_FORM_UI_CAPTURE_EDIT_H_
// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: gaoqi
// Date: 2015/7/7

//支持白板绘制

#ifndef _UIBOARDCONTROL_H_
#define _UIBOARDCONTROL_H_
#pragma once

#include <map>

namespace ui
{
enum DrawOpType
{
	DrawOpStart = 1,
	DrawOpMove,
	DrawOpEnd,
	DrawOpUndo,
	DrawOpPktId,
	DrawOpClear,
	DrawOpClearCb,
};
enum ShowOpType
{
	ShowOpAll = 0,
	ShowOpMine,
	ShowOpOther,
};
struct DrawOpInfo
{
	int draw_op_type_;
	float x_;
	float y_;
	DrawOpInfo()
	{
		draw_op_type_ = -1;
		x_ = 0;
		y_ = 0;
	}
};

typedef std::function<void(DrawOpInfo info)> DrawOpCallback;

class DrawBaseTool;
class BoardControl : public ui::Box
{
public:
	BoardControl(void);
	~BoardControl(void);

	void Paint(HDC hDC, const UiRect& rcPaint) override;

	void SetDrawCb(DrawOpCallback cb) { draw_op_cb_ = cb; }

	BOOL OnLButtonDown(POINT point); // 编辑时鼠标按下
	void OnMouseMove(POINT point); // 编辑时鼠标拖动
	void OnLButtonUp(POINT point); // 编辑时鼠标弹起
	void DrawExtraUnits(HDC hdc); // 控件重绘、保存截图时调用
	bool MyUndo(); // 撤销
	bool OtherUndo(); // 撤销
	void Redo(); // 重做
	void ReleaseAllDrawUnits(ShowOpType type = ShowOpAll); // 释放所有绘制单元
	void PaintContent();

	void OnOtherDrawInfos(std::list<DrawOpInfo> info_list);

	void SetShowType(ShowOpType type);
	void ReShowMine(const std::list<DrawOpInfo>& info_lists, int play_pos);
	void ReShowOther(const std::list<DrawOpInfo>& info_lists, int play_pos);
	void AddDrawInfo(std::vector<DrawBaseTool*>& vec_units, const std::list<DrawOpInfo>& info_lists, int play_pos, bool mine);

	void PlayDrawInfo(DrawOpInfo info, bool mine);

protected:
	// 绘制相关
	void OnDrawUnitStart(const ui::CPoint &point);
	void OnDrawUnitProcess(const ui::CPoint &point);
	void OnDrawUnitEnd(const ui::CPoint &point);
	void CreateMyDrawUnit(const ui::CPoint& point);
	void CreateOtherDrawUnit(const ui::CPoint& point);

	void SubmitDrawUnit();
	void ClearHistoryVector();
	//获取遮罩
	void CreareHBitmap(HDC hdc, HBITMAP& bitmap);
	void ReleaseHBitmap(HBITMAP& bitmap);

private:
	DrawOpCallback		draw_op_cb_;
	std::vector<DrawBaseTool*>			vec_my_draw_units_;			// 当前绘制单元列表
	std::vector<DrawBaseTool*>			vec_other_draw_units_;		// 当前对方绘制单元列表
	std::vector<DrawBaseTool*>			vec_other_waiting_draw_units_;		// 当前对方绘制单元列表等待列表
	std::vector<DrawBaseTool*>			vec_history_draw_units_;	// 撤销/回滚绘制单元列表
	DrawBaseTool*						current_my_draw_unit_;         // 当前正在绘制的单元
	DrawBaseTool*						current_other_draw_unit_;         // 当前正在绘制的单元
	BOOL								is_begin_draw_;				// 是否开始绘制
	HBITMAP								extra_bitmap_;
	ShowOpType							show_type_;

};

//画笔
//基本画笔
class DrawBaseTool
{
public:
	DrawBaseTool(int x, int y, const RECT& rc_valid);
	virtual ~DrawBaseTool();
	void Render(HDC hdc, HBITMAP bitmap=nullptr, bool is_continue = false);  // 绘制接口
	void SetLineWidth(int line_width)	{ line_width_ = line_width; }
	void SetColor(COLORREF dw_color)	{ color_ = dw_color; }
	ui::CPoint	GetStartPoint()		{ return ui::CPoint(left_, top_); }
	ui::UiRect GetWindowRect()	{ return ui::UiRect(left_, top_, right_, bottom_); }
	bool NeedSaveDraw() { return need_save_draw_; }
	virtual bool SetEndPoint(int x, int y)	{ right_ = x; bottom_ = y;  return true; }
	virtual void ResetDrawPos(){}

protected:
	virtual	void RenderSelf(HDC hdc, HBITMAP bitmap, bool is_continue) = 0;
	void MeasurePoint(int &x, int &y);	// 确保要绘制的点在截图区域范围内 

protected:
	int left_, top_, right_, bottom_;	//起始坐标，终点坐标
	int line_width_;					//线粗
	COLORREF color_;					//颜色
	ui::UiRect rc_valid_;			//合法范围
	bool need_save_draw_;
};
//简单画笔
class DrawSinglePen : public DrawBaseTool
{
public:
	DrawSinglePen(int x, int y, const RECT& rc_valid)
		:DrawBaseTool(x, y, rc_valid),
		draw_pos_(0)
	{
		m_vtPoints.push_back(ui::CPoint(x, y));
	}
	virtual	bool SetEndPoint(int x, int y) override;
	virtual void ResetDrawPos() override { draw_pos_ = 0; }
protected:
	virtual	void RenderSelf(HDC hdc, HBITMAP bitmap, bool is_continue);
private:
	void DrawSingleLine(HDC hdc, const ui::CPoint &pt_start, const ui::CPoint &pt_end);
	void DrawSingleLine(Gdiplus::Graphics &graphics, Gdiplus::Brush &brush, Gdiplus::Pen &pen, const ui::CPoint &pt_start, const ui::CPoint &pt_end);
	Gdiplus::Rect GetRectByPt(const ui::CPoint& pt);
private:
	std::vector<ui::CPoint>	m_vtPoints;
	int draw_pos_;	//绘制度（对于复杂的一步操作设置完成度，不需要每次都重新画）
};

}

#endif // _UIBITMAPCONTROL_H_

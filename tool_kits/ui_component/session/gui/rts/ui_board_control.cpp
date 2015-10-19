#include "ui_board_control.h"

namespace ui
{
#define kdwDefaultColor		0						// 默认颜色
#define kdwRedColor			RGB(0xe6,0x00,0x13)		// 默认颜色
#define kdwGreenColor		RGB(0x06,0xe6,0x13)		// 默认颜色
#define kdwDefaultBgColor	RGB(0xff, 0xff, 0xff)	// 默认背景颜色
#define	knDefaultLineWidth	2						// 默认线粗

BoardControl::BoardControl(void)
{
	is_begin_draw_ = FALSE;
	current_my_draw_unit_ = NULL;
	current_other_draw_unit_ = NULL;
	extra_bitmap_ = NULL;
	draw_op_cb_ = nullptr;
	show_type_ = ShowOpAll;
}

BoardControl::~BoardControl(void)
{
	ReleaseAllDrawUnits();
}

void BoardControl::Paint(HDC hDC, const UiRect& rcPaint)
{
	try
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
			return;
		Control::Paint(hDC, rcPaint);
		//paint custom
		DrawExtraUnits(hDC);

		//绘制子控件
		for (auto it = m_items.begin(); it != m_items.end(); it++)
		{
			Control* pControl = *it;
			if (!pControl->IsVisible()) continue;
			UiRect controlPos = pControl->GetPos();
			if (!::IntersectRect(&m_rcPaint, &rcPaint, &controlPos)) continue;
			pControl->AlphaPaint(hDC, rcPaint);
		}
	}
	catch (...)
	{
		throw "BoardControl::DoPaint";
	}
}
BOOL BoardControl::OnLButtonDown(POINT point)
{
	if (::PtInRect(&m_rcItem, point))
	{
		point.x -= m_rcItem.left;
		point.y -= m_rcItem.top;
		OnDrawUnitStart(point);
		return TRUE;
	}
	return FALSE;
}

void BoardControl::OnMouseMove(POINT point)
{
	if (is_begin_draw_)
	{
		point.x -= m_rcItem.left;
		point.y -= m_rcItem.top;
		OnDrawUnitProcess(point);
	}
	//SetEditModeCursor(point);
}

void BoardControl::OnLButtonUp(POINT point)
{
	if (is_begin_draw_)
	{
		point.x -= m_rcItem.left;
		point.y -= m_rcItem.top;
		OnDrawUnitEnd(point);
	}
}

void BoardControl::DrawExtraUnits(HDC hdc)
{
	bool redraw = false;
	if (!extra_bitmap_)
	{
		redraw = true;
		CreareHBitmap(hdc, extra_bitmap_);
	}
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap2 = (HBITMAP)SelectObject(hMemDC, extra_bitmap_);
	if (redraw)
	{
		ui::UiRect rc(0, 0, m_rcItem.GetWidth(), m_rcItem.GetHeight());
		HBRUSH current_brush = CreateSolidBrush(kdwDefaultBgColor);
		::FillRect(hMemDC, &rc, current_brush);
		DeleteObject(current_brush);
		if (show_type_ == ShowOpAll || show_type_ == ShowOpOther)
		{
			for (size_t i = 0; i < vec_other_draw_units_.size(); i++)
			{
				vec_other_draw_units_[i]->Render(hMemDC);
			}
		}
		if (show_type_ == ShowOpAll || show_type_ == ShowOpMine)
		{
			for (size_t i = 0; i < vec_my_draw_units_.size(); i++)
			{
				vec_my_draw_units_[i]->Render(hMemDC);
			}
		}
	}
	if (show_type_ == ShowOpAll || show_type_ == ShowOpOther)
	{
		for (auto it : vec_other_waiting_draw_units_)
		{
			it->Render(hMemDC);
			vec_other_draw_units_.push_back(it);
		}
		vec_other_waiting_draw_units_.clear();
		if (current_other_draw_unit_)
		{
			current_other_draw_unit_->Render(hMemDC, nullptr, true);
		}
	}

	if (show_type_ == ShowOpAll || show_type_ == ShowOpMine)
	{
		if (current_my_draw_unit_)
		{
			current_my_draw_unit_->Render(hMemDC, nullptr, true);
		}
	}
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hdc, m_rcItem.left, m_rcItem.top, m_rcItem.GetWidth(), m_rcItem.GetHeight(), hMemDC, 0, 0, SRCCOPY);
	//BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
	SelectObject(hMemDC, hOldBitmap2);
	DeleteDC(hMemDC);
}

bool BoardControl::MyUndo()
{
	if (!vec_my_draw_units_.empty())
	{
		ReleaseHBitmap(extra_bitmap_);
		// 弹出队尾
		auto it = vec_my_draw_units_.back();
		vec_my_draw_units_.pop_back();
		// 加入到历史中
		vec_history_draw_units_.push_back(it);
		// 重绘
		PaintContent();
		if (draw_op_cb_)
		{
			DrawOpInfo info;
			info.draw_op_type_ = DrawOpUndo;
			info.x_ = 0;
			info.y_ = 0;
			draw_op_cb_(info);
		}
		return true;
	}
	return false;
}
bool BoardControl::OtherUndo()
{
	if (!vec_other_draw_units_.empty() || !vec_other_waiting_draw_units_.empty())
	{
		ReleaseHBitmap(extra_bitmap_);
		// 弹出队尾
		DrawBaseTool* unit = nullptr;
		if (!vec_other_waiting_draw_units_.empty())
		{
			unit = vec_other_waiting_draw_units_.back();
			vec_other_waiting_draw_units_.pop_back();

		} 
		else
		{
			unit = vec_other_draw_units_.back();
			vec_other_draw_units_.pop_back();
		}
		if (current_my_draw_unit_)
		{
			current_my_draw_unit_->ResetDrawPos();
		}
		if (current_other_draw_unit_)
		{
			current_other_draw_unit_->ResetDrawPos();
		}
		if (unit)
		{
			delete unit;
		}
		// 重绘
		PaintContent();
		return true;
	}
	return false;
}

void BoardControl::Redo()
{
	if (!vec_history_draw_units_.empty())
	{
		ReleaseHBitmap(extra_bitmap_);
		// 弹出历史队尾
		auto it = vec_history_draw_units_.back();
		vec_history_draw_units_.pop_back();
		// 还原到原队尾
		vec_my_draw_units_.push_back(it);
		// 重绘
		PaintContent();
	}
}

void BoardControl::ReleaseAllDrawUnits(ShowOpType type)
{
	if (type == ShowOpAll || type == ShowOpMine)
	{
		for (size_t i = 0; i < vec_my_draw_units_.size(); i++)
		{
			delete vec_my_draw_units_[i];
		}
		vec_my_draw_units_.clear();

		for (size_t i = 0; i < vec_history_draw_units_.size(); i++)
		{
			delete vec_history_draw_units_[i];
		}
		vec_history_draw_units_.clear();

		if (current_my_draw_unit_)
		{
			delete current_my_draw_unit_;
			current_my_draw_unit_ = nullptr;
		}
	}
	if (type == ShowOpAll || type == ShowOpOther)
	{
		for (size_t i = 0; i < vec_other_draw_units_.size(); i++)
		{
			delete vec_other_draw_units_[i];
		}
		vec_other_draw_units_.clear();
		for (size_t i = 0; i < vec_other_waiting_draw_units_.size(); i++)
		{
			delete vec_other_waiting_draw_units_[i];
		}
		vec_other_waiting_draw_units_.clear();
		if (current_other_draw_unit_)
		{
			delete current_other_draw_unit_;
			current_other_draw_unit_ = nullptr;
		}
	}
	ReleaseHBitmap(extra_bitmap_);
}
void BoardControl::OnOtherDrawInfos(std::list<DrawOpInfo> info_list)
{
	bool redraw = false;
	for (auto& it : info_list)
	{
		POINT pt;
		pt.x = (LONG)(it.x_ * m_rcItem.GetWidth());
		pt.y = (LONG)(it.y_ * m_rcItem.GetHeight());
		switch (it.draw_op_type_)
		{
		case DrawOpStart:
			if (current_other_draw_unit_)
			{
				vec_other_waiting_draw_units_.push_back(current_other_draw_unit_);
				current_other_draw_unit_ = nullptr;
			}
			CreateOtherDrawUnit(pt);
			redraw = true;
			break;
		case DrawOpMove:
			if (current_other_draw_unit_ && current_other_draw_unit_->SetEndPoint(pt.x, pt.y))
			{
				redraw = true;
			}
			break;
		case DrawOpEnd:
			if (current_other_draw_unit_)
			{
				current_other_draw_unit_->SetEndPoint(pt.x, pt.y);
				vec_other_waiting_draw_units_.push_back(current_other_draw_unit_);
				current_other_draw_unit_ = nullptr;
				redraw = true;
			}
			break;
		case DrawOpUndo:
			OtherUndo();
			redraw = true;
			break;
		case DrawOpClear:
			ReleaseAllDrawUnits(ShowOpAll);
			if (draw_op_cb_)
			{
				DrawOpInfo info;
				info.draw_op_type_ = DrawOpClearCb;
				draw_op_cb_(info);
			}
			redraw = true;
			break;
		case DrawOpClearCb:
			ReleaseAllDrawUnits(ShowOpOther);
			redraw = true;
			break;
		default:
			break;
		}
	}
	if (redraw)
	{
		PaintContent();
	}
}
void BoardControl::SetShowType(ShowOpType type)
{
	if (show_type_ != type)
	{
		show_type_ = type;
		ReleaseHBitmap(extra_bitmap_);
		PaintContent();
	}
}
void BoardControl::ReShowMine(const std::list<DrawOpInfo>& info_lists, int play_pos)
{
	ReleaseAllDrawUnits(ShowOpMine);
	AddDrawInfo(vec_my_draw_units_, info_lists, play_pos, true);
	PaintContent();
}
void BoardControl::ReShowOther(const std::list<DrawOpInfo>& info_lists, int play_pos)
{
	ReleaseAllDrawUnits(ShowOpOther);
	AddDrawInfo(vec_other_draw_units_, info_lists, play_pos, false);
	PaintContent();
}
void BoardControl::AddDrawInfo(std::vector<DrawBaseTool*>& vec_units, const std::list<DrawOpInfo>& info_lists, int play_pos, bool mine)
{
	DrawBaseTool* draw_tool_temp = nullptr;
	int num = 0;
	for (auto& info : info_lists)
	{
		num++;
		if (num > play_pos)
		{
			break;
		}
		POINT pt;
		pt.x = (LONG)(info.x_ * m_rcItem.GetWidth());
		pt.y = (LONG)(info.y_ * m_rcItem.GetHeight());
		switch (info.draw_op_type_)
		{
		case DrawOpStart:
		{
			if (draw_tool_temp)
			{
				draw_tool_temp->SetColor(kdwDefaultColor);
				vec_units.push_back(draw_tool_temp);
				draw_tool_temp = nullptr;
			}
			ui::UiRect rc(0, 0, m_rcItem.GetWidth(), m_rcItem.GetHeight());
			draw_tool_temp = new DrawSinglePen(pt.x, pt.y, rc);
			if (mine)
			{
				draw_tool_temp->SetColor(kdwRedColor);
			}
			else
			{
				draw_tool_temp->SetColor(kdwGreenColor);
			}
			//redraw = true;
		}
		break;
		case DrawOpMove:
			if (draw_tool_temp && draw_tool_temp->SetEndPoint(pt.x, pt.y))
			{
				//redraw = true;
			}
			else
			{
				int i = 0;
				i++;
			}
			break;
		case DrawOpEnd:
			if (draw_tool_temp)
			{
				draw_tool_temp->SetColor(kdwDefaultColor);
				draw_tool_temp->SetEndPoint(pt.x, pt.y);
				vec_units.push_back(draw_tool_temp);
				draw_tool_temp = nullptr;
				//redraw = true;
			}
			break;
		case DrawOpUndo:
			if (vec_units.size() > 0)
			{
				DrawBaseTool* tool = vec_units.back();
				vec_units.pop_back();
				if (tool)
				{
					delete tool;
				}
			}
			//redraw = true;
			break;
		case DrawOpClear:
		case DrawOpClearCb:
			if (draw_tool_temp)
			{
				delete draw_tool_temp;
				draw_tool_temp = nullptr;
			}
			while (vec_units.size() > 0)
			{
				DrawBaseTool* tool = vec_units.back();
				vec_units.pop_back();
				if (tool)
				{
					delete tool;
				}
			}
			break;
		default:
			break;
		}
	}
	if (draw_tool_temp)
	{
		vec_units.push_back(draw_tool_temp);
		draw_tool_temp = nullptr;
		//redraw = true;
	}
}
void BoardControl::PlayDrawInfo(DrawOpInfo info, bool mine)
{
	bool redraw = false;
	POINT pt;
	pt.x = (LONG)(info.x_ * m_rcItem.GetWidth());
	pt.y = (LONG)(info.y_ * m_rcItem.GetHeight());
	DrawBaseTool** current_draw_unit = nullptr;
	std::vector<DrawBaseTool*>* vec_draw_units = nullptr;
	if (mine)
	{
		current_draw_unit = &current_my_draw_unit_;
		vec_draw_units = &vec_my_draw_units_;
	} 
	else
	{
		current_draw_unit = &current_other_draw_unit_;
		vec_draw_units = &vec_other_draw_units_;
	}
	switch (info.draw_op_type_)
	{
	case DrawOpStart:
		if (*current_draw_unit)
		{
			vec_draw_units->push_back(*current_draw_unit);
			*current_draw_unit = nullptr;
		}
		if (mine)
		{
			CreateMyDrawUnit(pt);
		} 
		else
		{
			CreateOtherDrawUnit(pt);
		}
		redraw = true;
		break;
	case DrawOpMove:
		if (*current_draw_unit && (*current_draw_unit)->SetEndPoint(pt.x, pt.y))
		{
			redraw = true;
		}
		break;
	case DrawOpEnd:
		if (*current_draw_unit)
		{
			(*current_draw_unit)->SetEndPoint(pt.x, pt.y);
			vec_draw_units->push_back(*current_draw_unit);
			*current_draw_unit = nullptr;
			redraw = true;
		}
		break;
	case DrawOpUndo:
		if (mine)
		{
			MyUndo();
		} 
		else
		{
			OtherUndo();
		}
		redraw = true;
		break;
	case DrawOpClear:
	case DrawOpClearCb:
		if (mine)
		{
			ReleaseAllDrawUnits(ShowOpMine);
		}
		else
		{
			ReleaseAllDrawUnits(ShowOpOther);
		}
		redraw = true;
		break;
	default:
		break;
	}
	if (redraw)
	{
		PaintContent();
	}
}

void BoardControl::OnDrawUnitStart(const ui::CPoint &point)
{
	is_begin_draw_ = TRUE;
	CreateMyDrawUnit(point);
	if (draw_op_cb_)
	{
		DrawOpInfo info;
		info.draw_op_type_ = DrawOpStart;
		info.x_ = (float)(point.x * 1.0 / m_rcItem.GetWidth());
		info.y_ = (float)(point.y * 1.0 / m_rcItem.GetHeight());
		draw_op_cb_(info);
	}
}

void BoardControl::OnDrawUnitProcess(const ui::CPoint &point)
{
	if (current_my_draw_unit_ && current_my_draw_unit_->SetEndPoint(point.x, point.y))
	{
		PaintContent(); // 鼠标移动过程中，需要立即重绘
		if (draw_op_cb_)
		{
			DrawOpInfo info;
			info.draw_op_type_ = DrawOpMove;
			info.x_ = (float)(point.x * 1.0 / m_rcItem.GetWidth());
			info.y_ = (float)(point.y * 1.0 / m_rcItem.GetHeight());
			draw_op_cb_(info);
		}
	}
}

void BoardControl::OnDrawUnitEnd(const ui::CPoint &point)
{
	if (!current_my_draw_unit_)
	{
		return;
	}
	current_my_draw_unit_->SetEndPoint(point.x, point.y);

	SubmitDrawUnit();
	if (draw_op_cb_)
	{
		DrawOpInfo info;
		info.draw_op_type_ = DrawOpEnd;
		info.x_ = (float)(point.x * 1.0 / m_rcItem.GetWidth());
		info.y_ = (float)(point.y * 1.0 / m_rcItem.GetHeight());
		draw_op_cb_(info);
	}
}

void BoardControl::CreateMyDrawUnit(const ui::CPoint& point)
{
	delete current_my_draw_unit_;
	ui::UiRect rc(0, 0, m_rcItem.GetWidth(), m_rcItem.GetHeight());
	current_my_draw_unit_ = new DrawSinglePen(point.x, point.y, rc);
}
void BoardControl::CreateOtherDrawUnit(const ui::CPoint& point)
{
	delete current_other_draw_unit_;
	ui::UiRect rc(0, 0, m_rcItem.GetWidth(), m_rcItem.GetHeight());
	current_other_draw_unit_ = new DrawSinglePen(point.x, point.y, rc);
	//current_other_draw_unit_->SetColor(kdwRedColor);
}
void BoardControl::SubmitDrawUnit()
{
	vec_my_draw_units_.push_back(current_my_draw_unit_);
	ClearHistoryVector();
	current_my_draw_unit_ = NULL; //置空,vector自己管理内存
	is_begin_draw_ = FALSE;
	PaintContent();
}

void BoardControl::ClearHistoryVector()
{
	if (vec_history_draw_units_.empty())
	{
		return;
	}
	for (size_t i = 0; i < vec_history_draw_units_.size(); i++)
	{
		delete vec_history_draw_units_[i];
	}
	vec_history_draw_units_.clear();
}

void BoardControl::PaintContent()
{
	this->Invalidate();
}
void BoardControl::CreareHBitmap(HDC hdc, HBITMAP& bitmap)
{
	ReleaseHBitmap(bitmap);

	int width = m_rcItem.GetWidth();
	int height = m_rcItem.GetHeight();
	UINT data_size = width * height * 4;
	BITMAPINFO bmi;
	::ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = data_size;
	long* pBitmapData;
	bitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,
		(void**)&pBitmapData, NULL, 0);
}
void BoardControl::ReleaseHBitmap(HBITMAP& bitmap)
{
	if (bitmap)
	{
		DeleteObject(bitmap);
		bitmap = NULL;
	}
}


//画笔
//基本画笔
DrawBaseTool::DrawBaseTool(int x, int y, const RECT& rc_valid)
	:line_width_(knDefaultLineWidth), color_(kdwDefaultColor), rc_valid_(rc_valid),
	left_(x), top_(y), right_(x), bottom_(y), need_save_draw_(false)
{

}
DrawBaseTool::~DrawBaseTool()
{
}

void DrawBaseTool::Render(HDC hdc, HBITMAP bitmap, bool is_continue)
{
	// 创建公用PEN，各个DrawUnit的Pen的公用的，但是Brush各个DrawUnit不同
	HPEN current_pen = CreatePen(PS_SOLID, line_width_, color_);
	HPEN old_pen = (HPEN)SelectObject(hdc, current_pen);
	RenderSelf(hdc, bitmap, is_continue); // 子类处理
	SelectObject(hdc, old_pen);
	DeleteObject(current_pen);
}

void DrawBaseTool::MeasurePoint(int &x, int &y)
{
	x = (x < rc_valid_.left ? rc_valid_.left : x);
	x = (x > rc_valid_.right ? rc_valid_.right : x);
	y = (y < rc_valid_.top ? rc_valid_.top : y);
	y = (y > rc_valid_.bottom ? rc_valid_.bottom : y);
}

//简单画笔------------------------------
// 重写，在MouseMove过程中，收集所有经过的点
bool DrawSinglePen::SetEndPoint(int x, int y)
{
	ui::CPoint point = m_vtPoints.back();
	if (x != point.x || y != point.y)
	{
		m_vtPoints.push_back(ui::CPoint(x, y));
		return true;
	}
	return false;
}

void DrawSinglePen::RenderSelf(HDC hdc, HBITMAP bitmap, bool is_continue)
{
	// 绘制所有MouseMove经过的点
	UINT draw_pos = is_continue ? draw_pos_ : 0;
	need_save_draw_ = false;

	if (0)
	{
		if (draw_pos < m_vtPoints.size())
		{
			Gdiplus::Graphics graphics(hdc);
			Gdiplus::Color gdiPlusColor(color_|0xff000000);
			Gdiplus::SolidBrush brush(gdiPlusColor);
			Gdiplus::Pen pen(&brush, (Gdiplus::REAL)line_width_);
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			if (draw_pos == 0)
			{
				graphics.FillEllipse(&brush, GetRectByPt(m_vtPoints[0]));
				draw_pos = 1;
				need_save_draw_ = true;
			}
			for (size_t i = draw_pos; i < m_vtPoints.size(); i++)
			{
				DrawSingleLine(graphics, brush, pen, m_vtPoints[i - 1], m_vtPoints[i]);
				need_save_draw_ = true;
			}
		}
	}
	else
	{
		HBRUSH current_brush = CreateSolidBrush(color_);
		HBRUSH old_brush = (HBRUSH)SelectObject(hdc, current_brush);
		if (draw_pos == 0)
		{
			if (m_vtPoints.size() == 1)
			{
				DrawSingleLine(hdc, m_vtPoints[0], m_vtPoints[0]);
			}
			draw_pos = 1;
		}
		for (size_t i = draw_pos; i < m_vtPoints.size(); i++)
		{
			DrawSingleLine(hdc, m_vtPoints[i - 1], m_vtPoints[i]);
			need_save_draw_ = true;
		}
		SelectObject(hdc, old_brush);
		DeleteObject(current_brush);
	}
	draw_pos_ = m_vtPoints.size();
}

void DrawSinglePen::DrawSingleLine(HDC hdc, const ui::CPoint& pt_start, const ui::CPoint& pt_end)
{
	int start_x = pt_start.x;
	int start_y = pt_start.y;
	int end_x = pt_end.x;
	int end_y = pt_end.y;

	//MeasurePoint(start_x, start_y);
	//MeasurePoint(end_x, end_y);

	MoveToEx(hdc, start_x, start_y, NULL);
	LineTo(hdc, end_x, end_y);
}
void DrawSinglePen::DrawSingleLine(Gdiplus::Graphics &graphics, Gdiplus::Brush &brush, Gdiplus::Pen &pen, const ui::CPoint &pt_start, const ui::CPoint &pt_end)
{
	int start_x = pt_start.x;
	int start_y = pt_start.y;
	int end_x = pt_end.x;
	int end_y = pt_end.y;

	MeasurePoint(start_x, start_y);
	MeasurePoint(end_x, end_y);

	graphics.DrawLine(&pen, Gdiplus::Point(pt_start.x, pt_start.y), Gdiplus::Point(pt_end.x, pt_end.y));
	graphics.FillEllipse(&brush, GetRectByPt(pt_end));
}
Gdiplus::Rect DrawSinglePen::GetRectByPt(const ui::CPoint& pt)
{
	int x = pt.x;
	int y = pt.y;
	MeasurePoint(x, y);
	return Gdiplus::Rect(x - line_width_ / 2, y - line_width_ / 2, line_width_, line_width_);
}

}



#include "ui_capture_edit.h"
#include "image_effects/mosaic.h"
#include "image_effects/gauss_blur.h"
#include "image_effects/sse_check.h"


CCaptureEdit::CCaptureEdit()
{
	is_begin_draw_ = FALSE;
	current_draw_unit_ = NULL;
	text_edit_ = NULL;
	color_palette_ = NULL;
	extra_bitmap_ = NULL;
	blur_bitmap_ = NULL;
	blur_range_ = 0;
	blur_type_ = DrawUnit::BLUR_TYPE_NONE;
	cursor_blur_big_ = NULL;
	cursor_blur_mid_ = NULL;
	cursor_blur_small_ = NULL;
}

CCaptureEdit::~CCaptureEdit()
{
	ReleaseAllDrawUnits();
}

void CCaptureEdit::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	Control::Paint(pRender, rcPaint);
	DrawExtraUnits(pRender->GetDC());
}

BOOL CCaptureEdit::OnLButtonDownInCaptureEditMode( POINT point, RECT tool_bar_rect)
{
	if (::PtInRect(&track_rect_, point))
	{
		// 当在某绘制单元状态下，工具条也在截图区域内，在工具条上点击，不做处理
		if(::PtInRect(&tool_bar_rect, point))
		{
			SetNormalCursor();
			return FALSE;
		}

		// 当前是文本模式
		if (edit_mode_ == EM_TEXT && text_edit_ != NULL)
		{
			if (m_pWindow->FindSubControlByPoint(text_edit_->GetParent(), point) == text_edit_)
				return FALSE; //点击在text_edit_上
			else //没有点击在text_edit_上，先提交text_edit_
				OnTextEditFinished(); 
		}

		OnDrawUnitStart(point);
		return TRUE;
	}
	return FALSE;
}

void CCaptureEdit::OnMouseMoveInCaptureEditMode( POINT point, RECT tool_bar_rect)
{
	if (is_begin_draw_)
	{
		OnDrawUnitProcess(point);
	}
	else
	{
		// 不在绘制状态下，如果鼠标进入了工具条，那么鼠标正常手势
		if(::PtInRect(&tool_bar_rect, point))
		{
			SetNormalCursor();
			return;
		}
	}
	SetEditModeCursor(point);
}

void CCaptureEdit::OnLButtonUpInCaptureEditMode( POINT point)
{
	if (is_begin_draw_)
	{
		OnDrawUnitEnd(point);
	}
}

void CCaptureEdit::OnDrawUnitStart( const ui::CPoint &point)
{
	is_begin_draw_ = TRUE;
	CreateDrawUnit(point);
}

void CCaptureEdit::OnDrawUnitProcess( const ui::CPoint &point )
{
	ui::CPoint pt_valid = point;
	CalcValidEndPoint(pt_valid); // 限制绘图终点在截图框内
	current_draw_unit_->SetEndPoint(pt_valid.x, pt_valid.y);
	PaintContent(); // 鼠标移动过程中，需要立即重绘
}

void CCaptureEdit::OnDrawUnitEnd( const ui::CPoint &point)
{
	ReleaseHBitmap(extra_bitmap_);
	ReleaseHBitmap(blur_bitmap_);
	ui::CPoint pt_end = point;
	ui::CPoint pt_start = current_draw_unit_->GetStartPoint();
	CalcValidEndPoint(pt_end);
	current_draw_unit_->SetEndPoint(pt_end.x, pt_end.y);

	if (edit_mode_ == EM_TEXT)	//文本需要额外处理
	{
		//判断下区域大小 如果太小就不画
		ui::UiRect rcRect(pt_start.x,pt_start.y,pt_end.x,pt_end.y);
		rcRect.Normalize();
		if (rcRect.GetWidth() > 5 && rcRect.GetHeight() > 5)
		{
			text_edit_ = new CCaptureTextEdit(rcRect); //创建一个RichEdit来输入文字
			text_edit_->SetFont(3); //添加到容器前必须设置一个font
			ui::Box* parent_hor = static_cast<ui::Box*>(this->GetParent());
			parent_hor->Add(text_edit_); // 加入到父容器（float型）
			text_edit_->InitInfos(); //设置一些固定的属性
			text_edit_->SetVisible(true);
			text_edit_->SetFocus(); // 让编辑框得到输入焦点
			text_edit_->SetTextFinishedCallback(nbase::Bind(&CCaptureEdit::OnTextEditFinished, this));

			if(color_palette_)
			{
				DWORD text_color = color_palette_->GetSelectedColorRGB();
				std::wstring color_name = color_palette_->GetSelectedColorName();
				int font_index = color_palette_->GetSelectedFontIndex();
				text_edit_->SetDotPenColor(text_color); //画笔的颜色
				text_edit_->SetTextColor(color_name); //文字的颜色
				text_edit_->SetFont(font_index);
			}
		}
		delete current_draw_unit_;
		current_draw_unit_ = NULL;
		is_begin_draw_ = FALSE; // 不要再画了
		PaintContent();
	}
	else
	{
		SubmitDrawUnit();
	}
}

void CCaptureEdit::CreateDrawUnit( const ui::CPoint& point)
{
	delete current_draw_unit_;
	switch (edit_mode_)
	{
	case EM_TEXT: //这个DrawUnitText只帮助记录绘制框范围，不绘制文字，鼠标一释放就被销毁
		current_draw_unit_ = new DrawUnitText(point.x, point.y, track_rect_, FALSE);
		break;
	case EM_RECTANGLE:
		current_draw_unit_ = new DrawUnitRectangle(point.x, point.y, track_rect_);
		break;
	case EM_BRUSH:
		current_draw_unit_ = new DrawUnitBrush(point.x, point.y, track_rect_);
		break;
	case EM_ELLIPSE:
		current_draw_unit_ = new DrawUnitEllipse(point.x, point.y, track_rect_);
		break;
	case EM_ARROW:
		current_draw_unit_ = new DrawUnitArrow(point.x, point.y, track_rect_);
		break;
	case EM_LINE:
		current_draw_unit_ = new DrawUnitLine(point.x, point.y, track_rect_);
		break;
	case EM_MOSAIC:
		current_draw_unit_ = new DrawUnitBrush(point.x, point.y, track_rect_);
		current_draw_unit_->SetBlur(DrawUnit::BLUR_TYPE_MOSAIC, color_palette_ ? color_palette_->GetRangePos() : 1);
		break;
	case EM_GAUSS:
		current_draw_unit_ = new DrawUnitBrush(point.x, point.y, track_rect_);
		current_draw_unit_->SetBlur(DrawUnit::BLUR_TYPE_GAUSS, color_palette_ ? color_palette_->GetRangePos() : 1);
		break;
	default:
		break;
	}
	assert(current_draw_unit_);

	// 从调色板获取用户选择的颜色和线宽
	if(color_palette_)
	{
		current_draw_unit_->SetColor(color_palette_->GetSelectedColorRGB());
		if (current_draw_unit_->GetBlurRange() > 0)
		{
			current_draw_unit_->SetLineWidth(color_palette_->GetSelectedBrushWidth2());
		}
		else
		{
			current_draw_unit_->SetLineWidth(color_palette_->GetSelectedBrushWidth());
		}
	}
}

void CCaptureEdit::SetEditModeCursor( const ui::CPoint& point )
{
	ui::CPoint pt_mouse = point;
	HCURSOR hCursor = NULL;
	if (::PtInRect(&track_rect_, pt_mouse))
	{
		if(edit_mode_ == EM_TEXT && text_edit_!= NULL)
		{
			// 当鼠标进入文本编辑框内时
			RECT edit_rect = text_edit_->GetCaptureTextEditRect();
			if(::PtInRect(&edit_rect, pt_mouse))
			{
				str_cursor_ = IDC_IBEAM;  // I字光标
			}
			else
			{
				str_cursor_ = IDC_CROSS; // 十字光标
			}
		}
		else if (edit_mode_ == EM_GAUSS || edit_mode_ == EM_MOSAIC)
		{
			int brush_w = 20;
			if (color_palette_)
			{
				brush_w = color_palette_->GetSelectedBrushWidth2();
			}
			if (brush_w == 30)
			{
				CheckCursor(cursor_blur_big_, L"blur_cur_big.png");
				hCursor = cursor_blur_big_;
			}
			else if (brush_w == 10)
			{
				CheckCursor(cursor_blur_small_, L"blur_cur_small.png");
				hCursor = cursor_blur_small_;
			} 
			else
			{
				CheckCursor(cursor_blur_mid_, L"blur_cur_mid.png");
				hCursor = cursor_blur_mid_;
			}
			if (hCursor)
			{
				SetCursor(hCursor);
				return;
			} 
			else
			{
				str_cursor_ = IDC_CROSS;  // 十字光标
			}
		}
		else
		{
			str_cursor_ = IDC_CROSS;  // 十字光标
		}
	}
	else
	{
		str_cursor_ = IDC_ARROW;
	}
	hCursor = LoadCursor(NULL, str_cursor_);
	SetCursor(hCursor);
}

void CCaptureEdit::SetNormalCursor()
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CCaptureEdit::CalcValidEndPoint( ui::CPoint& pt_end )
{
	ui::UiRect rc_tracker = track_rect_;

	//如果不在矩形内
	if (!::PtInRect(&rc_tracker, pt_end))
	{
		pt_end.y = pt_end.y >= rc_tracker.bottom ? rc_tracker.bottom : pt_end.y;
		pt_end.y = pt_end.y <= rc_tracker.top ? rc_tracker.top : pt_end.y;
		pt_end.x = pt_end.x >= rc_tracker.right ? rc_tracker.right : pt_end.x;
		pt_end.x = pt_end.x <= rc_tracker.left ? rc_tracker.left : pt_end.x;
	}
}

void CCaptureEdit::SubmitDrawUnit()
{
	vec_draw_units_.push_back(current_draw_unit_);
	ClearHistoryVector();
	current_draw_unit_ = NULL; //置空,vector自己管理内存
	is_begin_draw_ = FALSE;
	PaintContent();
}

void CCaptureEdit::ClearHistoryVector()
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

void CCaptureEdit::PaintContent()
{
	Invalidate();
}

void CCaptureEdit::DrawExtraUnits( HDC hdc )
{
	if (extra_bitmap_)
	{
		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap2 = (HBITMAP)SelectObject(hMemDC, extra_bitmap_);
		// 把屏幕设备描述表拷贝到内存设备描述表中
		BitBlt(hdc, track_rect_.left, track_rect_.top, track_rect_.GetWidth() + 1, track_rect_.GetHeight() + 1, hMemDC, track_rect_.left, track_rect_.top, SRCCOPY);
		//BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
		SelectObject(hMemDC, hOldBitmap2);
		DeleteDC(hMemDC);
	} 
	else
	{
		for (size_t i = 0; i < vec_draw_units_.size(); i++)
		{
			CheckBlurBitmap(hdc, vec_draw_units_[i]);
			vec_draw_units_[i]->Render(hdc, blur_bitmap_);
		}
		if (vec_draw_units_.size() > 0)
		{
			GetExtraUnitsBimap(hdc, extra_bitmap_);
		}
	}

	if (current_draw_unit_)
	{
		bool bcontinue = !CheckBlurBitmap(hdc, current_draw_unit_);
		current_draw_unit_->Render(hdc, blur_bitmap_, bcontinue);
		if (current_draw_unit_->NeedSaveDraw())
		{
			GetExtraUnitsBimap(hdc, extra_bitmap_);
		}
	}
}

void CCaptureEdit::OnTextEditFinished()
{
	if(text_edit_ == NULL)
		return;

	// TODO:如果当前鼠标不在设置窗口的范围内才产生文字
	// 获取用户输入的文本
	std::wstring text = text_edit_->GetText();
	if (!text.empty())
	{
		bool is_begin_draw_ = TRUE;
		ui::UiRect edit_rect = text_edit_->GetCaptureTextEditRect();
		DWORD text_color = kdwDefaultColor;
		int font_index = 3;
		if (color_palette_)
		{
			text_color = color_palette_->GetSelectedColorRGB();
			font_index = color_palette_->GetSelectedFontIndex();
		}
		current_draw_unit_ = new DrawUnitText(edit_rect.left, edit_rect.top, track_rect_, TRUE); // 绘制文本
		current_draw_unit_->SetEndPoint(edit_rect.right, edit_rect.bottom);
		current_draw_unit_->SetColor(text_color);
		DrawUnitText* pDrawUnitText = (DrawUnitText*)current_draw_unit_;
		pDrawUnitText->SetText(text);
		pDrawUnitText->SetFontIndex(font_index);
		
		ReleaseHBitmap(extra_bitmap_);
		ReleaseHBitmap(blur_bitmap_);
		SubmitDrawUnit(); //提交当前绘制单元到绘制列表中
	}

	// 释放text_edit
	ui::Box* parent_hor = static_cast<ui::Box*>(this->GetParent());
	if(parent_hor)
		parent_hor->Remove(text_edit_); // 移除，会自动delete

	text_edit_ = NULL;
	this->SetFocus();
}

void CCaptureEdit::Undo()
{
	if (!vec_draw_units_.empty())
	{
		ReleaseHBitmap(extra_bitmap_);
		ReleaseHBitmap(blur_bitmap_);
		// 弹出队尾
		DrawUnit* last_draw_unit = vec_draw_units_.back();
		vec_draw_units_.pop_back();
		// 加入到历史中
		vec_history_draw_units_.push_back(last_draw_unit);
		// 重绘
		PaintContent();
	}
}

void CCaptureEdit::Redo()
{
	if (!vec_history_draw_units_.empty())
	{
		ReleaseHBitmap(extra_bitmap_);
		ReleaseHBitmap(blur_bitmap_);
		// 弹出历史队尾
		DrawUnit* pDrawUnit = vec_history_draw_units_.back();
		vec_history_draw_units_.pop_back();
		// 还原到原队尾
		vec_draw_units_.push_back(pDrawUnit);
		// 重绘
		PaintContent();
	}
}

void CCaptureEdit::ReleaseAllDrawUnits()
{
	if (text_edit_ != NULL)
	{
		text_edit_->GetParent()->Remove(text_edit_);
		text_edit_ = NULL;
	}

	for (size_t i = 0; i < vec_draw_units_.size(); i++)
	{
		delete vec_draw_units_[i];
	}
	vec_draw_units_.clear();

	for (size_t i = 0; i < vec_history_draw_units_.size(); i++)
	{
		delete vec_history_draw_units_[i];
	}
	vec_history_draw_units_.clear();

	current_draw_unit_ = NULL;

	edit_mode_ = EM_NONE;

	ReleaseHBitmap(extra_bitmap_);
	ReleaseHBitmap(blur_bitmap_);
}
//获取遮罩
void CCaptureEdit::GetExtraUnitsBimap(HDC hdc, HBITMAP& bitmap, DrawUnit::BlurType blur_type, int range)
{
	ReleaseHBitmap(bitmap);
//#ifdef _DEBUG
//	timeb time_now;
//	ftime(&time_now); // 秒数
//	uint64_t timestamp_cur = time_now.time * 1000 + time_now.millitm;
//#endif
	int width = track_rect_.right + 1;
	int height = track_rect_.bottom + 1;
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
	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hOldBitmap2 = (HBITMAP)SelectObject(hMemDC, bitmap);
	// 把屏幕设备描述表拷贝到内存设备描述表中
	BitBlt(hMemDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
	SelectObject(hMemDC, hOldBitmap2);
	DeleteDC(hMemDC);
	if (range > 0)
	{
		switch (blur_type)
		{
		case DrawUnit::BLUR_TYPE_MOSAIC:
			ImageEffects::MosaicTrans(width, height,
				track_rect_.left, track_rect_.top, track_rect_.right, track_rect_.bottom,
				pBitmapData, range * 3, range * 3, range>=10);
			break;
		case DrawUnit::BLUR_TYPE_GAUSS:
		{
			bool sse_check = sse_check::SimdSSELevel() >= sse_check::SIMD_SSE_2;
			ImageEffects::GaussianBlur(4, sse_check, false, 0, 0, width, height, width * 4, (float)range, (unsigned int*)pBitmapData, (unsigned int*)pBitmapData);
		}
			break;
		}
	}
//#ifdef _DEBUG
//	ftime(&time_now); // 秒数
//	uint64_t timestamp_cur2 = time_now.time * 1000 + time_now.millitm - timestamp_cur;
//	printf("---check  %d\n", timestamp_cur2);
//#endif
}
void CCaptureEdit::ReleaseHBitmap(HBITMAP& bitmap)
{
	if (bitmap)
	{
		DeleteObject(bitmap);
		bitmap = NULL;
	}
}
bool CCaptureEdit::CheckBlurBitmap(HDC hdc, DrawUnit* draw_units_)
{
	int blur_range = draw_units_->GetBlurRange();
	DrawUnit::BlurType blur_type = draw_units_->GetBlurType();
	if (!blur_bitmap_ || blur_range != blur_range_ || blur_type != blur_type_)
	{
		blur_range_ = blur_range;
		blur_type_ = blur_type;
		if (blur_type != DrawUnit::BLUR_TYPE_NONE)
		{
			GetExtraUnitsBimap(hdc, blur_bitmap_, blur_type, blur_range);
			return true;
		}
	}
	return false;
}
//获取指针图片对象
void CCaptureEdit::CheckCursor(HCURSOR &cursor, std::wstring image_name)
{
	if (cursor == NULL)
	{
		std::wstring image_path = ui::GlobalManager::GetResourcePath() + L"capture_image/" + image_name;
		Gdiplus::Bitmap bmp(image_path.c_str());
		bmp.GetHICON(&cursor);
	}
}

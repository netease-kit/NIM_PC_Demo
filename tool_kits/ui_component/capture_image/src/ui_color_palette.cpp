#include "ui_color_palette.h"
#include "base/util/string_util.h"
#include "base/util/string_number_conversions.h"

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////// 颜色块 /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CColorBlock::CColorBlock()
{
	fill_color_brush_ = NULL;
	LOGBRUSH lb = { BS_NULL };
	select_rect_brush_ = CreateBrushIndirect(&lb);
	select_rect_pen_ = CreatePen(PS_SOLID, 2, 0xffffff);
	color_ = 0;
}

CColorBlock::~CColorBlock()
{
	::DeleteObject(fill_color_brush_);
	fill_color_brush_ = NULL;
	::DeleteObject(select_rect_brush_);
	select_rect_brush_ = NULL;
	::DeleteObject(select_rect_pen_);
	select_rect_pen_ = NULL;
}

void CColorBlock::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	__super::Paint(pRender, rcPaint);
	// 绘制外框
	DrawColorBlock(pRender->GetDC(), m_rcItem);
}

void CColorBlock::DrawColorBlock( HDC hdc, const ui::UiRect &rect )
{
	if(fill_color_brush_)
	{
		ui::UiRect block_rect = rect;
		block_rect.Deflate(1, 1);
		::FillRect(hdc, &block_rect, fill_color_brush_);
		if (select_rect_pen_ && m_bSelected)
		{
			HPEN old_pen = (HPEN)SelectObject(hdc, select_rect_pen_);
			HBRUSH old_brush = (HBRUSH)SelectObject(hdc, select_rect_brush_);
			::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			SelectObject(hdc, old_brush);
			SelectObject(hdc, old_pen);
		}
	}	
}

void CColorBlock::SetAttribute( const std::wstring& pstrName, const std::wstring& pstrValue )
{
	if(pstrName == _T("color"))
	{
		color_name_ = pstrValue;
		color_ = ui::GlobalManager::GetTextColor(pstrValue);
		color_ = (0xff & color_) << 16 | (0xff00 & color_) | (0xff0000 & color_) >> 16;
		fill_color_brush_ = CreateSolidBrush(color_);
	}
	else __super::SetAttribute(pstrName, pstrValue);
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////// 画刷板 /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CBrushBlock::CBrushBlock()
{
	brush_width_ = 0;
	brush_width2_ = 0;
	brush_paint_width_ = 0;
	brush_.reset(NULL);
}

CBrushBlock::~CBrushBlock()
{

}

void CBrushBlock::SetAttribute( const std::wstring& pstrName, const std::wstring& pstrValue )
{
	if(pstrName == _T("brush_width"))
	{
		brush_width_ = _ttoi(pstrValue.c_str());
	}
	else if (pstrName == _T("brush_width2"))
	{
		brush_width2_ = _ttoi(pstrValue.c_str());
	}
	else if (pstrName == _T("brush_paint_width"))
	{
		brush_paint_width_ = _ttoi(pstrValue.c_str());
	}
	else __super::SetAttribute(pstrName, pstrValue);
}

void CBrushBlock::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	__super::Paint(pRender, rcPaint);
	int width = m_rcItem.right - m_rcItem.left;
	int offset = (width - brush_paint_width_) / 2;
	RECT rect = { m_rcItem.left + offset, m_rcItem.top + offset, rect.left + brush_paint_width_, rect.top + brush_paint_width_ };
	DrawCircleBlock(pRender->GetDC(), rect);
}

void CBrushBlock::HandleMessage( ui::EventArgs& event )
{
	ui::Option::HandleMessage(event);
}

void CBrushBlock::DrawCircleBlock(HDC hdc, const ui::UiRect &rect)
{
	if(brush_.get() == NULL)
		brush_.reset(new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(0xff, 0xd8, 0xe1, 0xeb)));

	if(brush_.get())
	{
		using namespace Gdiplus;
		Graphics graphic(hdc);
		graphic.SetSmoothingMode(SmoothingModeAntiAlias);
#if (GDIPVER >= 0x0110)
		graphic.SetSmoothingMode(SmoothingModeAntiAlias8x8);
#endif
		graphic.SetCompositingMode(CompositingModeSourceOver);
		graphic.SetInterpolationMode(InterpolationModeHighQuality);
		graphic.SetPixelOffsetMode(PixelOffsetModeHighQuality);

		Rect fill_rect(rect.left, rect.top, rect.GetWidth(), rect.GetHeight());
		graphic.FillEllipse(brush_.get(), fill_rect);
	}
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////// 调色板 /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#define MIN_BRUSH_WIDTH 1
#define MIN_BRUSH_WIDTH2 10

CColorPalette::CColorPalette()
{
	color_palette_ = NULL;
	selected_color_ = NULL;
	selected_brush_width_ = MIN_BRUSH_WIDTH; // 默认2px
	selected_brush_width2_ = MIN_BRUSH_WIDTH2;
	SetFixedWidth(DUI_LENGTH_AUTO);
	SetFixedHeight(DUI_LENGTH_AUTO);
	color_ = 0;
	font_size_combo_ = NULL;
}

CColorPalette::~CColorPalette()
{

}

ui::Box* CColorPalette::CreateColorPaletteUI(ui::CreateControlCallback callback, bool visible /*=true*/)
{
	color_palette_ = ui::GlobalManager::CreateBox(L"capture_image/color_palette.xml", callback);
	color_palette_->SetVisible(visible);
	this->Add(color_palette_);
	return this;
}
void CColorPalette::ShowRange(bool show_brush_width, bool show_font_size, bool show_range, bool show_color)
{
	PTR_VOID(color_palette_);

	ui::Box* brush_width_box = (ui::Box*)color_palette_->FindSubControl(L"brush_width_box");
	if (brush_width_box)
		brush_width_box->SetVisible(show_brush_width);

	ui::Box* font_size_box = (ui::Box*)color_palette_->FindSubControl(L"font_size_box");
	if (font_size_box)
		font_size_box->SetVisible(show_font_size);

	ui::Box* range_box = (ui::Box*)color_palette_->FindSubControl(L"range_box");
	if (range_box)
		range_box->SetVisible(show_range);

	ui::Box* color_block = (ui::Box*)color_palette_->FindSubControl(L"color_block");
	if (color_block)
		color_block->SetVisible(show_color);
}

void CColorPalette::InitChildControls()
{
	if(m_pWindow)
	{
		selected_color_ = static_cast<CColorBlock*>(m_pWindow->FindControl(L"select_color"));
		std::wstring ctrl_name;
		CColorBlock* color_block_ctrl = NULL;
		CBrushBlock* brush_ctrl = NULL;
		for(int i =1; i <= 9; i++)
		{
			nbase::StringPrintf(ctrl_name, L"color%d", i);
			color_block_ctrl = static_cast<CColorBlock*>(m_pWindow->FindControl(ctrl_name.c_str()));
			if(color_block_ctrl)
			{
				color_block_ctrl->AttachSelect(nbase::Bind(&CColorPalette::OnClick, this, std::placeholders::_1)); // 添加点击响应
				if (i==1)
				{
					color_ = color_block_ctrl->GetColor();
					color_name_ = color_block_ctrl->GetColorName();
					color_block_ctrl->Selected(true);
				}
			}
		}
		for(int i = 1; i <=3; i++)
		{
			nbase::StringPrintf(ctrl_name, L"brush%d", i);
			brush_ctrl = static_cast<CBrushBlock*>(m_pWindow->FindControl(ctrl_name.c_str()));
			if(brush_ctrl)
			{
				brush_ctrl->AttachSelect(nbase::Bind(&CColorPalette::OnSelect, this, std::placeholders::_1)); // 单选响应
			}
		}
		ui::Slider* range = static_cast<ui::Slider*>(m_pWindow->FindControl(L"range"));
		if (range)
		{
			range->SetValue(5);
		}

		font_size_combo_ = static_cast<ui::Combo*>(m_pWindow->FindControl(L"font_size_combo"));
		if (font_size_combo_)
		{
			int font_num = font_size_combo_->GetCount();
			if (font_num > 3)
				font_size_combo_->SelectItem(3);

			for (int i = 0; i < font_num; i++)
			{
				ui::ListContainerElement* item = (ui::ListContainerElement*)font_size_combo_->GetItemAt(i);
				if (item)
				{
					int index = 3;
					nbase::StringToInt(item->GetUTF8Name(), &index); //0_font_index, 1_font_index ...
					ui::TFontInfo* tfont_info = ui::GlobalManager::GetTFontInfo(index);
					if (tfont_info)
						item->SetText(nbase::IntToString16(tfont_info->iSize)); //显示字号
				}
			}
		}
	}
}

bool CColorPalette::OnSelect( ui::EventArgs *msg )
{
	std::wstring sender_name = msg->pSender->GetName();

	if(sender_name.find(L"brush") == 0)
	{
		// 获取画刷的宽度
		selected_brush_width_ = ((CBrushBlock*)msg->pSender)->GetBrushWidth();
		selected_brush_width2_ = ((CBrushBlock*)msg->pSender)->GetBrushWidth2();
	}

	return true;
}

bool CColorPalette::OnClick(ui::EventArgs* param)
{
	std::wstring sender_name = param->pSender->GetName();
	if(sender_name.find(L"color") == 0)
	{
		// 获取颜色名称
		color_ = ((CColorBlock*)param->pSender)->GetColor();
		color_name_ = ((CColorBlock*)param->pSender)->GetColorName();
	}

	return true;
}
DWORD CColorPalette::GetSelectedColorRGB()
{
	return color_;
}

std::wstring CColorPalette::GetSelectedColorName()
{
	return color_name_;
}

int CColorPalette::GetSelectedBrushWidth()
{
	return (selected_brush_width_ >= MIN_BRUSH_WIDTH)? selected_brush_width_ : MIN_BRUSH_WIDTH;
}
int CColorPalette::GetSelectedBrushWidth2()
{
	return (selected_brush_width2_ >= MIN_BRUSH_WIDTH2) ? selected_brush_width2_ : MIN_BRUSH_WIDTH2;
}
int CColorPalette::GetRangePos()
{
	ui::Slider* range = static_cast<ui::Slider*>(m_pWindow->FindControl(L"range"));
	if (range)
	{
		return (int)range->GetValue();
	}
	return 1;
}
int CColorPalette::GetToolbarHeight()
{
	if (color_palette_)
	{
		return color_palette_->GetFixedHeight();
	}
	return 0;
}

int CColorPalette::GetSelectedFontIndex()
{
	int font_index = 3; //默认为16号字
	if (font_size_combo_)
	{
		ui::ListContainerElement* item = (ui::ListContainerElement*)font_size_combo_->GetItemAt(font_size_combo_->GetCurSel());
		if (item)
			nbase::StringToInt(item->GetUTF8Name(), &font_index);
	}
	return font_index;
}

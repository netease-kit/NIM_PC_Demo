#include "ui_capture_text_edit.h"
#include "capture_def.h"

CCaptureTextEdit::CCaptureTextEdit(const ui::UiRect &rect)
{
	rc_valid_ = rect;
	text_finished_cb_ = nullptr;
}

CCaptureTextEdit::~CCaptureTextEdit()
{
}

void CCaptureTextEdit::InitInfos()
{
	SetFloat(true);
	SetAttribute(L"multiline", L"true");
	SetAttribute(L"returnmsgwantctrl", L"true");
	SetAttribute(L"align", L"left");

	SetFixedHeight(rc_valid_.GetHeight());
	SetFixedWidth(rc_valid_.GetWidth());
	ui::Box* parent = static_cast<ui::Box*>(this->GetParent());
	ASSERT(parent);
	if (parent)
	{
		RECT margin = { rc_valid_.left - parent->GetPos().left, rc_valid_.top - parent->GetPos().top, 0, 0 };
		SetMargin(margin);
	}
}

void CCaptureTextEdit::HandleMessage( ui::EventArgs& event )
{
	// 当编辑框失去焦点的时候
	if(event.Type == ui::kEventKillFocus)
	{
		if (text_finished_cb_)
			text_finished_cb_();
	}
	return ui::RichEdit::HandleMessage(event); 
}

void CCaptureTextEdit::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	DrawRect(pRender->GetDC()); // 绘制矩形虚线
	return __super::Paint(pRender, rcPaint);
}

void CCaptureTextEdit::DrawRect( HDC hdc )
{
	// 使得文本背景透明，与窗口背景一样
	int old_bk_mode = SetBkMode(hdc, TRANSPARENT);
	// 虚线画笔
	HPEN dot_pen = CreatePen(PS_DOT, 1, color_pen_);
	HPEN old_pen = (HPEN)SelectObject(hdc, dot_pen);
	// 画刷
	LOGBRUSH lb = { BS_NULL };
	HBRUSH current_brush = CreateBrushIndirect(&lb);
	HBRUSH old_brush = (HBRUSH)SelectObject(hdc, current_brush);
	// 绘制虚线框
	ui::UiRect rcEdit = rc_valid_; // 要绘制的区域 
	//InflateRect(&rcEdit, knEditOffset, knEditOffset); // 向外扩展3px
	Rectangle(hdc, rcEdit.left, rcEdit.top, rcEdit.right, rcEdit.bottom);
	// 还原
	SelectObject(hdc, old_pen);
	SelectObject(hdc, old_brush);
	SetBkMode(hdc, old_bk_mode);
	// 释放
	DeleteObject(current_brush);
	DeleteObject(dot_pen);
}


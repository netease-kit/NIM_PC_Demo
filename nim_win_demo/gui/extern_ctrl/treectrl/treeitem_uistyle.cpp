#include "stdafx.h"
#include "treeitem_uistyle.h"
#include "TreeView.h"
#include "treeitem.h"
int ITreeItemUIStyle::GetLevel() const
{
	if (doc_item_ != nullptr)
		return doc_item_->GetLevel();
	return 0;
}
void ITreeItemUIStyle::Fill(const std::shared_ptr<NulTreeItem>& doc_item) {
	OnEvent.clear();
	OnXmlEvent.clear();
	OnBubbledEvent.clear();
	doc_item_ = doc_item;
	Selected(doc_item_->GetUI()->GetSelected(), false);
	OnFill();
};
void ITreeItemUIStyle::SetPos(ui::UiRect rc, bool recalc_child)
{
	Control::SetPos(rc);
	if (recalc_child)
	{
		rc.left += m_pLayout->GetPadding().left;
		rc.top += m_pLayout->GetPadding().top;
		rc.right -= m_pLayout->GetPadding().right;
		rc.bottom -= m_pLayout->GetPadding().bottom;
		int offset_x = 0;
		if (virtual_parent_->ShowTreeLine())
			offset_x += virtual_parent_->GetLevelSpace() / 2;
		if (virtual_parent_->ShowExpandButton())
			offset_x += virtual_parent_->GetExpandButtonWidth() + 2;
		if (virtual_parent_->ShowCheckbox())
			offset_x += virtual_parent_->GetCheckboxButtonWidth() + 2;
		offset_x += (GetLevel()) * virtual_parent_->GetLevelSpace();
		rc.left += offset_x;
		if (m_items.size() == 0) {
			requiredSize_.cx = offset_x;
			requiredSize_.cy = 0;
		}
		else {
			requiredSize_ = m_pLayout->ArrangeChild(m_items, rc);
			requiredSize_.cx += offset_x;
		}
	}	
}
void ITreeItemUIStyle::DoPaint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	ui::Control::Paint(pRender, rcPaint);
	if (doc_item_ != nullptr)
	{
		auto&& item_data = doc_item_->GetUI();
		if (item_data != nullptr)
			item_data->Paint(pRender, GetPos());
	}	
	for (auto it = m_items.begin(); it != m_items.end(); it++) {
		Control* pControl = *it;
		if (!pControl->IsVisible()) continue;
		pControl->AlphaPaint(pRender, rcPaint);
	}
}
bool ITreeItemUIStyle::IsPointInWithScrollOffset(const ui::CPoint& point) const
{
	//ui::CPoint scrollOffset = GetScrollOffset();
	//ui::CPoint newPoint = point;
	//newPoint.Offset(scrollOffset);
	return m_rcItem.IsPointIn(point);
}
void ITreeItemUIStyle::Activate()
{
	ui::ButtonTemplate<ui::Box>::Activate();
}
void ITreeItemUIStyle::HandleMessage(ui::EventArgs& event)
{
	SetWindow(GetWindow(), virtual_parent_->GetComponent(), false);	
	OnHandleMessage(event);	
	SetWindow(GetWindow(), nullptr, false);
}
void ITreeItemUIStyle::HandleMessageTemplate(ui::EventArgs& event)
{	
	SetWindow(GetWindow(), virtual_parent_->GetComponent(), false);
	
	OnHandleMessageTemplate(event);
	SetWindow(GetWindow(), nullptr, false);
}
void ITreeItemUIStyle::OnHandleMessage(ui::EventArgs& event)
{
	if (doc_item_ != nullptr)
	{
		auto&& item_data = doc_item_->GetUI();

		if (item_data != nullptr)
		{
			if (item_data->HandleMessage(event))
				return;
		}
	}	
	if (!IsMouseEnabled() && event.Type > ui::kEventMouseBegin && event.Type < ui::kEventMouseEnd) {
		if (m_pOwner != NULL) m_pOwner->HandleMessageTemplate(event);
		else Box::HandleMessage(event);
		return;
	}
	else if (event.Type == ui::kEventInternalDoubleClick) {
		if (IsActivatable()) {
			InvokeDoubleClickEvent();
		}
		return;
	}
	else if (event.Type == ui::kEventKeyDown && IsEnabled()) {
		if (event.chKey == VK_RETURN) {
			if (IsActivatable()) {
				if (m_pWindow != NULL) m_pWindow->SendNotify(this, ui::kEventReturn);
			}
			return;
		}
	}
	else if (event.Type == ui::kEventInternalMenu && IsEnabled()) {
		m_pWindow->SendNotify(this, ui::kEventMouseMenu);
		Invalidate();
		return;
	}
	ui::ListContainerElement::HandleMessage(event);
}
void ITreeItemUIStyle::OnHandleMessageTemplate(ui::EventArgs& event)
{
	ui::ListContainerElement::HandleMessageTemplate(event);
}
void ITreeItemUIStyle::Selected(bool bSelect, bool trigger)
{
	//ui::ListContainerElement::Selected(bSelect, trigger);
	ui::OptionTemplate<Box>::Selected(bSelect, trigger);
}
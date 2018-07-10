#include "stdafx.h"
#include "treeview.h"
#include "treeitem.h"
#include "treeitem_uistyle.h"
#define MAX_STYLE_COUNT (200)
std::shared_ptr<ITreeItemUIStyle> TreeComponent::GetUIStyle(const std::shared_ptr<NulTreeItem>& item)
{
	bool need_fill = false;
	return GetUIStyle(item, need_fill);
}
std::shared_ptr<ITreeItemUIStyle> TreeComponent::GetUIStyle(const std::shared_ptr<NulTreeItem>& item, bool& need_fill)
{
	need_fill = true;
	TreeItemIDType id = item->GetItemID();
	std::shared_ptr<ITreeItemUIStyle> ret = nullptr;
	auto& style_group = catched_style_[item->GetIUIStyleName()];
	auto it_style = style_group.find(id);
	if (it_style != style_group.end())
	{
		need_fill = false;
		ret = it_style->second;
	}		
	if (ret == nullptr)
	{
		auto it = std::find_if(style_group.begin(), style_group.end(), [&](const std::pair<TreeItemIDType, std::shared_ptr<ITreeItemUIStyle>> &item) {
			return visible_item_list_.find(item.first) == visible_item_list_.end();
		});
		if (it == style_group.end())
		{
			std::function<std::shared_ptr<ITreeItemUIStyle>()> style_crator = nullptr;
			auto it = style_factory_.find(item->GetIUIStyleName());
			if (it != style_factory_.end())
				style_crator = it->second;
			if (style_crator != nullptr)
			{
				style_group.emplace(std::make_pair(id, style_crator()));
				it_style = style_group.find(id);
				ret = it_style->second;
			}
		}
		else
		{
			ret = it->second;
			style_group.erase(it);
			style_group.emplace(std::make_pair(id, ret));
		}
	//	


	//	if (style_group.size() < MAX_STYLE_COUNT)
	//	{
	//		std::function<std::shared_ptr<ITreeItemUIStyle>()> style_crator = nullptr;
	//		auto it = style_factory_.find(item->GetIUIStyleName());
	//		if (it != style_factory_.end())
	//			style_crator = it->second;
	//		if (style_crator != nullptr)
	//		{
	//			style_group.emplace(std::make_pair(id, style_crator()));
	//			it_style = style_group.find(id);
	//			ret = it_style->second;
	//		}
	//	}
	//	else
	//	{	
	//		auto it = std::find_if(style_group.begin(), style_group.end(), [&](const std::pair<TreeItemIDType, std::shared_ptr<ITreeItemUIStyle>> &item) {
	//			return visible_item_list_.find(item.first) == visible_item_list_.end();
	//		});
	//		ret = it->second;
	//		style_group.erase(it);
	//		style_group.emplace(std::make_pair(id, ret));			
	//	}
	}
	return ret;
}
TreeComponent::TreeComponent() : 
	ui::ListBox(new ui::VLayout), 
	doc_(new TreeDoc),
	show_tree_line_(true), 
	show_check_box_(true),
	show_expand_button_(true),
	expand_button_width_(14),
	checkbox_button_width_(14),
	level_space_widht_(14),
	selected_item_(nullptr)
{  
	doc_->AttachRemoveItem(std::bind(&TreeComponent::OnRemoveItemCallback, this, std::placeholders::_1));
};
TreeComponent::~TreeComponent() 
{
	GetWindow()->RemoveControlFromPointFinder(this);
};
void TreeComponent::SetWindow(ui::Window* pManager, ui::Box* pParent, bool bInit/* = true*/)
{
	ui::ListBox::SetWindow(pManager, pParent, bInit);	
	if (bInit)
	{
		pManager->AddControlFromPointFinder(this);
		EnableScrollBar(true, true);
		GetVerticalScrollBar()->SetAutoHideScroll(false);
		GetHorizontalScrollBar()->SetAutoHideScroll(false);
		SetExpandImage(L"../public/button/ico_expand_normal.png", L"../public/button/ico_expand.png");
		SetCheckImage(L"../public/checkbox/check.png", L"../public/checkbox/check_no2.png", L"../public/checkbox/part_check.png");
	}
}
void TreeComponent::DoInit()
{
	//if (show_tree_line_)
	//{
	//	auto padding = GetPaddingPos();
	//	padding.left += 5;
	//	m_pLayout->SetPadding(padding);
	//}
}
ui::Control* TreeComponent::FindControlFromPoint(const ui::CPoint& pt)
{
	POINT pt_temp = { pt.x,pt.y };
	ui::Control* ret = GetWindow()->GetRoot()->FindControl([](Control* pThis, LPVOID pData) {
		LPPOINT pPoint = static_cast<LPPOINT>(pData);
		ui::UiRect pos = pThis->GetPos();
		return ::PtInRect(&pos, *pPoint) ? pThis : NULL;
	}, &pt_temp, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
	if (ret == this &&  IsVisible())
	{
		for (auto& it : visible_item_list_)
		{
			auto ui_style = std::get<3>(it.second);
			if(ui_style == nullptr)
				continue;
			ui::UiRect rc(ui_style->GetPos());
			if (rc.IsPointIn(pt))
			{				
				auto ctrl = ui_style->FindControl([](Control* pThis, LPVOID pData) {
					LPPOINT pPoint = static_cast<LPPOINT>(pData);
					ui::UiRect pos = pThis->GetPos();
					return ::PtInRect(&pos, *pPoint) ? pThis : NULL;
				}, &pt_temp, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
				if (ctrl != nullptr)
				{
					ret = ctrl;
				}
				else
				{
					ret = ui_style.get();
				}
			}
		}
	}	
	return ret;
}
void TreeComponent::Update(bool recalc_visible)
{
	if (recalc_visible)
	{
		draw_timer_.Cancel();
		auto task = [this]() {
			auto rc = GetPos();
			auto old_required_size = required_size_;
			required_size_ = ResetVisibleList(rc);
			rc.left += m_pLayout->GetPadding().left;
			rc.top += m_pLayout->GetPadding().top;
			rc.right -= m_pLayout->GetPadding().right;
			rc.bottom -= m_pLayout->GetPadding().bottom;
			bool reupdate = false;
			if (GetVerticalScrollBar()->GetScrollPos() + rc.GetHeight() > required_size_.cy &&  required_size_.cy >  rc.GetHeight())
			{
				reupdate = true;
				GetVerticalScrollBar()->SetScrollPos(required_size_.cy - rc.GetHeight());
			}
			if (GetHorizontalScrollBar()->GetScrollPos() + rc.GetWidth() > required_size_.cx &&  required_size_.cx >  rc.GetWidth())
			{
				reupdate = true;
				GetHorizontalScrollBar()->SetScrollPos(required_size_.cx - rc.GetWidth());
			}
			GetVerticalScrollBar()->SetScrollRange(required_size_.cy - rc.GetHeight());
			GetHorizontalScrollBar()->SetScrollRange(required_size_.cx - rc.GetWidth());
			if (required_size_.cx > rc.GetWidth())
				GetHorizontalScrollBar()->SetVisible(true);
			if (required_size_.cy > rc.GetHeight())
				GetVerticalScrollBar()->SetVisible(true);		
			if (reupdate)
				Update(true);
		};		
		//nbase::ThreadManager::PostTask(ToWeakCallback(task));
		//nbase::ThreadManager::PostDelayedTask(ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(30));
		task();
	}
	else
	{
		Invalidate();
	}
}
ui::Box* TreeComponent::GetComponent()
{
	return this;
}
ui::CSize TreeComponent::GetScrollPos() const
{
	return ui::ListBox::GetScrollPos();
}
ui::CSize TreeComponent::GetScrollPos_i() const
{
	return ui::ListBox::GetScrollPos();
}
ui::CSize TreeComponent::GetUIStyleSize(std::string style_name)
{
	auto it = style_size_list_.find(style_name);
	if (it != style_size_list_.end())
	{
		return it->second;
	}
	else
	{
		std::function<std::shared_ptr<ITreeItemUIStyle>()> style_crator = nullptr;
		auto it = style_factory_.find(style_name);
		if (it != style_factory_.end())
			style_crator = it->second;
		if (style_crator != nullptr)
		{
			auto ui_temp = style_crator();
			style_size_list_.emplace(std::make_pair(style_name, ui::CSize(ui_temp->GetWidth(), ui_temp->GetHeight())));
			return GetUIStyleSize(style_name);
		}
		else
		{
			return ui::CSize(0, 0);
		}
	}
}
ui::CSize TreeComponent::ResetVisibleList(const ui::UiRect& rc_component)
{	
	ui::UiRect rc(rc_component);
	rc.left += m_pLayout->GetPadding().left;
	rc.top += m_pLayout->GetPadding().top;
	rc.right -= m_pLayout->GetPadding().right;
	rc.bottom -= m_pLayout->GetPadding().bottom;
	ui::CSize required_size(0, 0);
	visible_item_list_.clear();
	if (doc_ != nullptr)
	{
		auto&& scroll_pos = GetScrollPos_i();
		auto&& root = doc_->GetRootItem();
		auto&& child_item_list = root->GetSubItemList();
		ui::UiRect rcdraw(rc.left - scroll_pos.cx,rc.top - scroll_pos.cy, rc.left - scroll_pos.cx, rc.top - scroll_pos.cy);
		for (auto& child_it : child_item_list)
		{
			auto&& componet_doc_item = std::dynamic_pointer_cast<NulTreeItem>(child_it);
			int componet_doc_item_height = componet_doc_item->GetUI()->GetWholeHeight();
			required_size.cy += componet_doc_item_height;
			if (!((rcdraw.bottom + componet_doc_item_height) < rc.top || rcdraw.top >rc.bottom))
			{
				auto&& size = ResetVisibleList(scroll_pos, rc, rcdraw, child_it);
				if (required_size.cx < size.cx)
					required_size.cx = size.cx;
				rcdraw.top = rcdraw.bottom;
			}	
			else
			{
				rcdraw.Offset(0, componet_doc_item_height);
			}
		}
	}
	return required_size;
}
ui::CSize TreeComponent::ResetVisibleList(const ui::CSize& scrollpos, const ui::UiRect& rc_component, ui::UiRect& rc, TreeDocItemPtr doc_item)
{
	ui::CSize ret(0, 0);
	auto&& componet_doc_item = std::dynamic_pointer_cast<NulTreeItem>(doc_item);
	ret = GetUIStyleSize(componet_doc_item->GetIUIStyleName());
	bool auto_width = ret.cx == DUI_LENGTH_AUTO;
	if (ret.cx == DUI_LENGTH_STRETCH || ret.cx == DUI_LENGTH_AUTO)
		ret.cx = rc_component.GetWidth() + scrollpos.cx;
	rc.top = rc.bottom;
	rc.right = rc.left + ret.cx;
	rc.bottom = rc.top + ret.cy;	
	if (!(rc.bottom < rc_component.top  || rc.top >rc_component.bottom ))
	{
		bool need_fill = true;
		auto&& ui = GetUIStyle(componet_doc_item, need_fill);
		if(need_fill)
			ui->Fill(componet_doc_item);
		ui->SetPos(rc);
		if (auto_width)
		{
			rc.right = rc.left + ui->GetRequiredSize().cx;
			ui->SetPos(rc, false);
			if (ret.cx < rc.GetWidth())
				ret.cx = rc.GetWidth();
		}
		visible_item_list_.emplace(std::make_pair(componet_doc_item->GetItemID(), std::make_tuple(auto_width,rc, componet_doc_item,ui)));
	}
	if (componet_doc_item->GetUI()->IsExpand())
	{
		auto&& child_item = doc_item->GetSubItemList();
		int componet_doc_child_height = 0;
		std::shared_ptr<NulTreeItem> componet_doc_child = nullptr;
		ui::CSize child_size;
		for (auto& child_it : child_item)
		{
			componet_doc_child = std::move(std::dynamic_pointer_cast<NulTreeItem>(child_it));
			componet_doc_child_height = componet_doc_child->GetUI()->GetWholeHeight();
			ret.cy += componet_doc_child_height;
			if (!((rc.bottom + componet_doc_child_height) < rc_component.top || rc.top > rc_component.bottom))
			{
				child_size = std::move(ResetVisibleList(scrollpos, rc_component, rc, child_it));
				if (ret.cx < child_size.cx)
					ret.cx = child_size.cx;
				rc.top = rc.bottom;
			}
			else
			{
				rc.Offset(0, componet_doc_child_height);
			}
		}
	}
	return ret;
}
void TreeComponent::SetPos(ui::UiRect rc)
{	
	if (rc.Equal(m_rcItem))
		return;
	ui::ListBox::SetPos(rc);	
	Update(true);
}
ui::CSize TreeComponent::CalcRequiredSize(const ui::UiRect& rc)
{
	auto ret(required_size_);
	return ret;
}
void TreeComponent::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	ui::UiRect rcTemp;
	if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return;

	ui::Control::Paint(pRender, rcPaint);
	auto padding_r = m_pLayout->GetPadding().right;
	for (auto& it : visible_item_list_)
	{
		auto ui = std::get<3>(it.second);
		if (ui != nullptr)
		{
			auto pos = ui->GetPos();
			if (pos.right >= (rcPaint.right - padding_r))
			{
				pos.right = rcPaint.right - padding_r;
			}
			ui::UiRect rc(pos);
			ui->DoPaint(pRender, rc);
		}
	}
	for (auto it = m_items.begin(); it != m_items.end(); it++) {
		Control* pControl = *it;
		if (!pControl->IsVisible()) continue;
		if (pControl->IsFloat()) {
			pControl->AlphaPaint(pRender, rcPaint);
		}
		else {
			ui::CSize scrollPos = GetScrollPos();
			ui::UiRect rcNewPaint = rcPaint;
			rcNewPaint.Offset(scrollPos.cx, scrollPos.cy);
			rcNewPaint.Offset(GetRenderOffset().x, GetRenderOffset().y);

			ui::CPoint ptOffset(scrollPos.cx, scrollPos.cy);
			ui::CPoint ptOldOrg = pRender->OffsetWindowOrg(ptOffset);
			pControl->AlphaPaint(pRender, rcNewPaint);
			pRender->SetWindowOrg(ptOldOrg);
		}
	}

	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
		m_pHorizontalScrollBar->AlphaPaint(pRender, rcPaint);
	}

	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
		m_pVerticalScrollBar->AlphaPaint(pRender, rcPaint);
	}

	static bool bFirstPaint = true;
	if (bFirstPaint) {
		bFirstPaint = false;
		LoadImageCache(true);
	}
}
void TreeComponent::SetScrollPos(ui::CSize szPos)
{
	if (szPos.cy < 0) {
		szPos.cy = 0;
		m_scrollAnimation.Reset();
	}
	else if (szPos.cy > GetScrollRange().cy) {
		szPos.cy = GetScrollRange().cy;
		m_scrollAnimation.Reset();
	}

	int cx = 0;
	int cy = 0;
	if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsValid()) {
		int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
		m_pVerticalScrollBar->SetScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
	}

	if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsValid()) {
		int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
		m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
	}

	if (cx == 0 && cy == 0) return;
	if (m_pWindow != NULL) {
		m_pWindow->SendNotify(this, ui::kEventScrollChange);
	}
	Update(true);	
}
bool TreeComponent::ShowTreeLine() const
{
	return show_tree_line_;
}
bool TreeComponent::ShowCheckbox() const
{
	return show_check_box_;
}
bool TreeComponent::ShowExpandButton() const
{
	return show_expand_button_;
}
unsigned int TreeComponent::GetLevelSpace() const
{
	return level_space_widht_;
}
void TreeComponent::SetExpandImage(const std::wstring& expand, const std::wstring& collapse)
{	
	expand_expand_image_.SetImageString(expand);
	GetImage(expand_expand_image_);
	expand_collapse_image_.SetImageString(collapse);
	GetImage(expand_collapse_image_);
	expand_button_width_ = expand_expand_image_.imageCache->nX;
}
void TreeComponent::SetCheckImage(const std::wstring& sel, const std::wstring& unsel, const std::wstring& partsel)
{
	checkbox_sel_image_.SetImageString(sel);
	GetImage(checkbox_sel_image_);
	checkbox_unsel_image_.SetImageString(unsel);
	GetImage(checkbox_unsel_image_);
	checkbox_partsel_image_.SetImageString(partsel);
	GetImage(checkbox_partsel_image_);
	checkbox_button_width_ =  checkbox_sel_image_.imageCache->nX;
}
bool TreeComponent::GetExpandImage(ui::Image& expand, ui::Image& collapse)
{
	bool ret = expand_expand_image_.IsValid() && expand_collapse_image_.IsValid();
	if (ret)
	{
		expand = expand_expand_image_;
		collapse = expand_collapse_image_;
	}	
	return ret;
}
bool TreeComponent::GetCheckboxImage(ui::Image& sel, ui::Image& unsel, ui::Image& partsel)
{
	bool ret = checkbox_sel_image_.IsValid() && checkbox_unsel_image_.IsValid() && checkbox_partsel_image_.IsValid();
	if (ret)
	{
		sel = checkbox_sel_image_;
		unsel = checkbox_unsel_image_;
		partsel = checkbox_partsel_image_;
	}
	return ret;
}
void TreeComponent::OnItemSelected(TreeItemIDType id)
{
	auto sel = std::dynamic_pointer_cast<NulTreeItem>(doc_->GetItem(id));
	if (sel != nullptr && sel != selected_item_)
	{
		if (selected_item_ != nullptr)
		{
			selected_item_->GetUI()->SetSelected(false);
			auto ui = GetUIStyle(selected_item_);
			ui->Selected(false, false);
		}			
		sel->GetUI()->SetSelected(true);
		auto ui = GetUIStyle(sel);
		ui->Selected(true, false);
		selected_item_ = sel;
		Update(false);
		InvokeSafeCallbackArray(selected_chenge_callback_, selected_item_);
	}
}
void TreeComponent::OnRemoveItemCallback(const TreeDocItemList& list)
{
	for (auto& it : list)
	{
		auto item = std::dynamic_pointer_cast<NulTreeItem>(it);
		if (item != nullptr)
		{
			visible_item_list_.erase(item->GetItemID());
			visible_item_list_old_.erase(item->GetItemID());
			auto style_group = catched_style_.find(item->GetIUIStyleName());
			if (style_group != catched_style_.end())
			{
				auto catch_it = style_group->second.find(item->GetItemID());
				if (catch_it != style_group->second.end())
					style_group->second.erase(catch_it);
			}
		}
	}
	Update(true);
}
void TreeComponent::RegisterStyleUI(const std::string& style_name, const std::function<std::shared_ptr<ITreeItemUIStyle>()>& creator)
{
	style_factory_.insert(std::make_pair(style_name, creator));
}
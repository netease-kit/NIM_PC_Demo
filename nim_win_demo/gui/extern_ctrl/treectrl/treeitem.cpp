#include "stdafx.h"
#include "treeitem.h"


TreeItemUI::TreeItemUI(ITreeDocItem* doc_item):
	height_(0),child_height_(0), get_style_height_(nullptr), state_(TreeItemUIState_EXPANDED), doc_item_(doc_item)
{
	AttachDocItemEvent(doc_item_);
}
TreeItemUI::~TreeItemUI()
{

}
std::shared_ptr<TreeItemUI> TreeItemUI::GetParentUI()
{
	auto parent_doc_item = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem()->GetParent());
	if (parent_doc_item != nullptr)
		return std::dynamic_pointer_cast<NulTreeItem>(parent_doc_item)->GetUI();
	return nullptr;
}
std::string TreeItemUI::GetIUIStyleName()
{	

	return std::dynamic_pointer_cast<NulTreeItem>(GetDocItem())->GetIUIStyleName();
}
void TreeItemUI::Expand()
{
	SetState(TreeItemUIState_EXPANDED, 0);
	NotifyParentHeightChange(child_height_, true);
}
void TreeItemUI::Collapse()
{
	SetState(0, TreeItemUIState_EXPANDED);
	NotifyParentHeightChange(-child_height_, true);
}
DWORD TreeItemUI::GetState(DWORD state_mask /*= TreeItemUIState_ALLSTATE*/) const
{
	return state_ & state_mask;
}
void TreeItemUI::SetState(DWORD add_status, DWORD remove_status)
{
	if (add_status&TreeItemUIState_EXPANDED)
		add_status |= TreeItemUIState_EXPANDEDONCE;

	if (remove_status&TreeItemUIState_EXPANDEDONCE)
		remove_status |= TreeItemUIState_EXPANDED;

	state_ |= add_status;
	state_ &= ~remove_status;
	return;
}
bool TreeItemUI::IsShowCheckBox() const
{
	return (GetState() & TreeItemUIState_SHOWCHECKBOX) != NULL;
}

void TreeItemUI::ShowCheckBox(bool show)
{
	SetState((show ? TreeItemUIState_SHOWCHECKBOX : 0), (show ? 0 : TreeItemUIState_SHOWCHECKBOX));
}

bool TreeItemUI::GetSelected() const
{
	return (GetState()&TreeItemUIState_SELECTED) != NULL;
}

void TreeItemUI::SetSelected(bool selected)
{
	SetState((selected ? TreeItemUIState_SELECTED : 0), (selected ? 0 : TreeItemUIState_SELECTED));
}

int TreeItemUI::GetCheck() const
{
	if ((GetState() &TreeItemUIState_CHECKEDMASK) == (DWORD)TreeItemUIState_CHECKEDPART)
		return 2;
	if (GetState() & TreeItemUIState_CHECKED)
		return 1;
	return 0;
}

void TreeItemUI::SetCheck(bool check)
{
	SetCheck_i(check);	
	auto&& subitem_list = GetDocItem()->GetSubItemList();
	for (auto it : subitem_list)
	{
		auto item = std::dynamic_pointer_cast<NulTreeItem>(it);
		if (item != nullptr)
			item->GetUI()->SetCheck(GetCheck() != 0);
	}
}
void TreeItemUI::SetCheck_i(bool check)
{
	SetState((check ? TreeItemUIState_CHECKED : TreeItemUIState_CHECKED_NULL), (check ? TreeItemUIState_CHECKED_NULL : TreeItemUIState_CHECKED));
}
bool TreeItemUI::IsExpand() const
{
	return (GetState()&TreeItemUIState_EXPANDED) != NULL;
}

void TreeItemUI::Paint(ui::IRenderContext* pRender, ui::UiRect rc)
{
	TreeDocItemPtr doc_item = GetDocItem();
	if (doc_item != nullptr)
	{
		auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
		if (This__ != nullptr)
		{
			if(This__->GetTreeComponent()->ShowTreeLine())
				DrawTreeLine(doc_item, pRender, rc);
			if (This__->GetTreeComponent()->ShowExpandButton())
				DrawTreeButton(doc_item, pRender, rc);
			if (This__->GetTreeComponent()->ShowCheckbox())
				DrawCheckBox(doc_item, pRender, rc);
		}		
	}	
}
void TreeItemUI::DrawTreeLine(const TreeDocItemPtr& doc_item, ui::IRenderContext* pRender, ui::UiRect rc)
{
	int space = 14;
	auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
	if(This__ != nullptr)
		space = This__->GetTreeComponent()->GetLevelSpace();
	DWORD dwBorderColor = ui::GlobalManager::GetTextColor(L"splitline_level1");
	
	ui::CPoint pt_begin(rc.left, rc.top);
	ui::CPoint pt_end(rc.left, rc.top + rc.GetHeight());
	//先画父节点线
	for (int i = 0; i < doc_item->GetLevel(); i++)
	{
		auto node_parent = GetNodeAtDepth(i);
		if (!IsLast(node_parent))
		{
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);				// ┃										
		}
		pt_begin.x += space;
		pt_end.x += space;
	}
	ui::CPoint pt_temp_begin(pt_begin);
	ui::CPoint pt_temp_end(pt_end);
	if (doc_item->GetLevel() == 0)
	{
		if (IsLast(GetDocItem()) && IsFirst(GetDocItem()))// ━ 
		{
			pt_begin.y += rc.GetHeight() / 2;
			pt_end.x += space;
			pt_end.y -= rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
		else if (IsFirst(GetDocItem()))// ┏
		{
			pt_begin.y += rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
			pt_end.y -= rc.GetHeight() / 2;
			pt_end.x += space;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
		else if (IsLast(GetDocItem()))// ┗
		{
			pt_end.y -= rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
			pt_begin.y += rc.GetHeight() / 2;
			pt_end.x += space;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
		else// ┣
		{
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
			pt_begin.y += rc.GetHeight() / 2;
			pt_end.x += space;
			pt_end.y -= rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
	}
	else
	{
		if (IsLast(GetDocItem()))// ┗
		{
			pt_end.y -= rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
			pt_begin.y += rc.GetHeight() / 2;
			pt_end.x += space;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
		else// ┣
		{
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
			pt_begin.y += rc.GetHeight() / 2;
			pt_end.x += space;
			pt_end.y -= rc.GetHeight() / 2;
			pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
		}
	}
	//if (!IsFirst(this))
	//{
	//	pt_begin = pt_temp_begin;
	//	pt_end = pt_temp_end;
	//	pt_begin.x += space_;
	//	pt_end.x += space_;
	//	pt_end.y -= rc.bottom / 2;
	//	pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
	//}
	if ((IsExpand() && doc_item->GetSubItemCount() > 0))
	{
		pt_begin = pt_temp_begin;
		pt_end = pt_temp_end;
		pt_begin.x += space;
		pt_begin.y += rc.GetHeight() / 2;
		pt_end.x += space;
		pt_end.y = rc.bottom;
		pRender->DrawLine(ui::UiRect(pt_begin.x, pt_begin.y, pt_end.x, pt_end.y), 1, dwBorderColor);
	}
}
void TreeItemUI::DrawTreeButton(const TreeDocItemPtr& doc_item, ui::IRenderContext* pRender, ui::UiRect rc)
{
	if (doc_item->GetSubItemCount() <= 0)
		return;
	int space = 14;
	int button_width = 14;
	bool use_image = false;
	ui::Image expand_image, collapse_image;
	int offset = 0;
	auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
	if (This__ != nullptr)
	{
		button_width = This__->GetTreeComponent()->GetExpandButtonWidth();
		space = This__->GetTreeComponent()->GetLevelSpace();
		use_image = This__->GetTreeComponent()->GetExpandImage(expand_image, collapse_image);
		if (This__->GetTreeComponent()->ShowTreeLine())
			offset += space;
		offset += (doc_item->GetLevel()) * space;
	}	
	ui::CPoint ptCenter;
	ptCenter.x = rc.left + offset;
	
	ptCenter.y = rc.top + rc.GetHeight() / 2 ;

	ui::UiRect rcButton(-button_width/2, -button_width/2, button_width/2, button_width/2);
	rcButton.Offset(ptCenter);
	rc_expand_button_ = rcButton;
	if (use_image)
	{
		DrawImage(pRender, IsExpand() ? collapse_image : expand_image, rcButton);			
	}
	else
	{
		DWORD dwBorderColor = ui::GlobalManager::GetTextColor(L"lightcolor");
		pRender->DrawColor(rcButton, ui::GlobalManager::GetTextColor(L"white"));
		rcButton.Inflate(ui::UiRect(-1, -1, -1, -1));
		pRender->DrawRect(rcButton, 1, dwBorderColor);
		rcButton.Inflate(ui::UiRect(-2, -2, -2, -2));

		ptCenter.x = rcButton.left + rcButton.GetWidth() / 2;
		ptCenter.y = rcButton.top + rcButton.GetHeight() / 2;
		// draw plus button [+]
		if (!IsExpand())
		{
			pRender->DrawLine(ui::UiRect(rcButton.left, ptCenter.y, rcButton.right, ptCenter.y), 1, dwBorderColor);
			pRender->DrawLine(ui::UiRect(ptCenter.x, rcButton.top, ptCenter.x, rcButton.bottom), 1, dwBorderColor);
		}
		// draw minus button [-]
		if (IsExpand())
		{
			pRender->DrawLine(ui::UiRect(rcButton.left, ptCenter.y, rcButton.right, ptCenter.y), 1, dwBorderColor);
		}
	}	
}
void TreeItemUI::OnDocItemReleaseChildCallback(const TreeDocItemPtr& sub_item)
{
	auto sub = std::dynamic_pointer_cast<NulTreeItem>(sub_item);
	if (sub != nullptr)
	{
		int chg_height = sub->GetUI()->GetWholeHeight();
		child_height_ -= chg_height;
		if(IsExpand())
			NotifyParentHeightChange(-chg_height,true);
	}
};
void TreeItemUI::OnDocItemAddChildCallback(const TreeDocItemPtr& sub_item)
{
	auto sub = std::dynamic_pointer_cast<NulTreeItem>(sub_item);
	if (sub != nullptr)
	{
		auto name = sub->GetIUIStyleName();
		int chg_height = 0;
		chg_height = sub->GetTreeComponent()->GetUIStyleSize(name).cy;		
		child_height_ += chg_height;
		if (IsExpand())
			NotifyParentHeightChange(chg_height,true);
	}
}
void TreeItemUI::NotifyParentHeightChange(int chg_value, bool change_child_height)
{
	auto parent = GetParentUI();
	if (parent != nullptr)
		parent->OnChildHeightChange(chg_value,change_child_height);
}
void TreeItemUI::NotifyParentCheck(int state)
{
	auto parent = GetParentUI();
	if (parent != nullptr)
		parent->OnChildChecked(state);
}
void TreeItemUI::OnChildHeightChange(int chg_value, bool change_child_height)
{
	if(change_child_height)
		child_height_ += chg_value;
	NotifyParentHeightChange(chg_value, change_child_height);
}
int TreeItemUI::GetWholeHeight() /*const*/ 
{ 
	auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
	if (height_ == 0)
	{
		height_ = This__->GetTreeComponent()->GetUIStyleSize(GetIUIStyleName()).cy;
	}	
	return height_ + (IsExpand() ? child_height_ : 0); 
}
void TreeItemUI::DrawCheckBox(const TreeDocItemPtr& doc_item, ui::IRenderContext* pRender, ui::UiRect rc)
{	
	int space = 14;
	int button_width = 14;
	bool use_image = false;
	ui::Image sel, unsel,partsel;
	int offset = 0;
	auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
	if (This__ != nullptr)
	{
		button_width = This__->GetTreeComponent()->GetCheckboxButtonWidth();
		space = This__->GetTreeComponent()->GetLevelSpace();
		use_image = This__->GetTreeComponent()->GetCheckboxImage(sel,unsel,partsel);

		if (This__->GetTreeComponent()->ShowTreeLine())
			offset += space;
		if (This__->GetTreeComponent()->ShowExpandButton())
			offset += This__->GetTreeComponent()->GetExpandButtonWidth() + 2;
		offset += (doc_item->GetLevel()) * space;
	}
	ui::CPoint ptCenter;
	ptCenter.x = rc.left + offset;
	ptCenter.y = rc.top + rc.GetHeight() / 2;

	ui::UiRect rcButton(-button_width / 2, -button_width / 2, button_width / 2, button_width / 2);
	rcButton.Offset(ptCenter);
	rc_checkbox_ = rcButton;
	switch (GetCheck())
	{
	case 0:
		DrawImage(pRender, unsel, rcButton);
		break;
	case 1:
		DrawImage(pRender, sel, rcButton);
		break;
	case 2:
		DrawImage(pRender, partsel, rcButton);
		break;
	default:
		break;
	}
}
bool TreeItemUI::HandleMessage(ui::EventArgs& event)
{
	bool ret = false;
	if (event.Type == ui::kEventMouseDoubleClick)
	{
		auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
		if (This__ != nullptr)
		{
			if (IsExpand())
				Collapse();
			else
				Expand();
			if (GetDocItem()->GetSubItemCount() > 0)
				This__->GetTreeComponent()->Update(true);
			else
				This__->GetTreeComponent()->Update(false);
			ret = true;
		}		
	}
	if (event.Type == ui::kEventClick)
	{
		auto This__ = std::dynamic_pointer_cast<NulTreeItem>(GetDocItem());
		if (This__ != nullptr)
		{
			if (This__->GetTreeComponent()->ShowExpandButton() && rc_expand_button_.IsPointIn(event.ptMouse))
			{
				if (IsExpand())
					Collapse();
				else
					Expand();
				if (GetDocItem()->GetSubItemCount() > 0)
					This__->GetTreeComponent()->Update(true);
				else
					This__->GetTreeComponent()->Update(false);
				ret = true;
			}
			if (This__->GetTreeComponent()->ShowCheckbox() && rc_checkbox_.IsPointIn(event.ptMouse))
			{
				SetCheck(!GetCheck());				
				NotifyParentCheck(GetCheck());
				This__->GetTreeComponent()->Update(false);
				return true;
			}
			else
			{
				This__->GetTreeComponent()->OnItemSelected(GetDocItem()->GetItemID());
			}
		}
	}	
	return ret;
}
void TreeItemUI::OnChildChecked(int state)
{
	auto ret = GetCheck();
	auto new_ret = GetChildChedk(state);
	if (ret != new_ret)
	{
		switch (new_ret)
		{
		case 2:
			SetState(TreeItemUIState_CHECKEDMASK, TreeItemUIState_SELECTED);
			break;
		case 0:
			SetCheck_i(false);
			break;
		case 1:
			SetCheck_i(true);
			break;
		}
		NotifyParentCheck(GetCheck());
	}	
}
int TreeItemUI::GetChildChedk(int state)
{
	if (state == 2)
		return 2;
	auto&& subitem_list = GetDocItem()->GetSubItemList();
	for (auto it : subitem_list)
	{
		auto item = std::dynamic_pointer_cast<NulTreeItem>(it);
		if (item != nullptr)
		{
			if (item->GetUI()->GetCheck() != state)
				return 2;
		}
	}
	return state;
}
bool TreeItemUI::IsFirst(const TreeDocItemPtr& node)
{
	if (node->GetLevel() == -1)
		return true;
	auto parent = node->GetParent();
	return parent->IsFirstSubItem(node->GetItemID());
}
bool TreeItemUI::IsLast(const TreeDocItemPtr& node)
{
	if (node->GetLevel() == -1)
		return true;
	auto parent = node->GetParent();
	return parent->IsLastSubItem(node->GetItemID());
}
TreeDocItemPtr TreeItemUI::GetNodeAtDepth(int depth)
{
	auto node = GetDocItem();
	while (depth != node->GetLevel())
	{
		node = node->GetParent();
	}
	return node;
}
bool TreeItemUI::DrawImage(ui::IRenderContext* pRender, ui::Image& duiImage, const ui::UiRect& rc)
{

	if (!duiImage.imageCache) {
		ASSERT(FALSE);
		duiImage.imageAttribute.Init();
		return false;
	}
	ui::ImageAttribute newImageAttribute = duiImage.imageAttribute;
	ui::UiRect rcNewDest = rc;
	if (newImageAttribute.rcDest.left != DUI_NOSET_VALUE && newImageAttribute.rcDest.top != DUI_NOSET_VALUE
		&& newImageAttribute.rcDest.right != DUI_NOSET_VALUE && newImageAttribute.rcDest.bottom != DUI_NOSET_VALUE) {
		rcNewDest.left = rc.left + newImageAttribute.rcDest.left;
		rcNewDest.right = rc.left + newImageAttribute.rcDest.right;
		rcNewDest.top = rc.top + newImageAttribute.rcDest.top;
		rcNewDest.bottom = rc.top + newImageAttribute.rcDest.bottom;
	}
	ui::UiRect rcNewSource = newImageAttribute.rcSource;
	if (rcNewSource.left == DUI_NOSET_VALUE || rcNewSource.top == DUI_NOSET_VALUE
		|| rcNewSource.right == DUI_NOSET_VALUE || rcNewSource.bottom == DUI_NOSET_VALUE) {
		rcNewSource.left = 0;
		rcNewSource.top = 0;
		rcNewSource.right = duiImage.imageCache->nX;
		rcNewSource.bottom = duiImage.imageCache->nY;
	}
	int iFade = newImageAttribute.bFade;
	ui::ImageInfo* imageInfo = duiImage.imageCache.get();
	pRender->DrawImage(rc, duiImage.GetCurrentHBitmap(), imageInfo->IsAlpha(),
		rcNewDest, rcNewSource, newImageAttribute.rcCorner, iFade, newImageAttribute.bTiledX, newImageAttribute.bTiledY);
	return true;
}
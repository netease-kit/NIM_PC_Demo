#include "stdafx.h"
#include "treedoc.h"
const TreeItemIDType TreeDocItemBase::kDEFAULT_ITEM_ID = "";
void TreeDoc::OnDocItemAddChildCallback(const TreeDocItemPtr& item)
{
	data_indexs_[item->GetItemID()] = item;
}
void TreeDoc::OnDocItemReleaseChildCallback(const TreeDocItemPtr& item)
{
	TreeDocItemList removed_item_list;
	RemoveItem(item, removed_item_list);
	if (item_remove_callback_ != nullptr)
		item_remove_callback_(removed_item_list);
}
void TreeDoc::RemoveItem(const TreeDocItemPtr& item, TreeDocItemList& removed_item_list)
{
	auto&& sublist = item->GetSubItemList();
	for (auto& it : sublist)
	{
		if (it->GetSubItemCount() > 0)
			RemoveItem(it, removed_item_list);
	}
	removed_item_list.emplace_back(item);
	data_indexs_.erase(item->GetItemID());
}
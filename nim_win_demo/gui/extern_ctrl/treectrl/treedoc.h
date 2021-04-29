#pragma once
#include "treectrl_interface.h"

class TreeDocItemBase : virtual public ITreeDocItem
{
public:
	TreeDocItemBase() : item_id_(kDEFAULT_ITEM_ID), item_data_(nullptr), level_(-1)
	{}
	virtual ~TreeDocItemBase() {
		sub_item_list_.clear();		
	};
	virtual TreeItemIDType GetItemID() const  override { return item_id_; }
	virtual void SetItemID(TreeItemIDType id) { item_id_ = id; }
	virtual TreeItemDataType GetItemData() const { return item_data_; }
	virtual void SetItemData(TreeItemDataType data) { item_data_ = data; }
	virtual TreeDocItemPtr GetParent() const { return (parent_.expired()) ? nullptr : parent_.lock(); }
	virtual void SetParent(const TreeDocItemPtr& parent) { parent_ = parent; }
	virtual void AddSubItem(const TreeDocItemPtr& sub_item) {
		sub_item->SetParent(shared_from_this());
		sub_item_list_.emplace_back(sub_item);
		InvokeSafeCallbackArray<1, decltype(OnAddItem), TreeDocItemPtr>(OnAddItem, sub_item);
	}
	virtual void RemoveSubItem(const TreeDocItemPtr& sub_item) {
		auto it = std::find_if(sub_item_list_.begin(), sub_item_list_.end(), [&](TreeDocItemPtr item) {return item->GetItemID() == sub_item->GetItemID(); });
		if (it != sub_item_list_.end())
		{
			InvokeSafeCallbackArray<1, decltype(OnRelease), TreeDocItemPtr>(OnRelease, *it);
			sub_item_list_.erase(it);
		}
	}
	virtual int GetLevel() const { return level_; }
	virtual void SetLevel(int level) {
		auto parent_ = GetParent();
		if (parent_ != nullptr && parent_->GetLevel() <= level)
			return;
		if (level_ != level)
		{
			level_ = level;
			for (auto it : sub_item_list_)
				it->SetLevel(level_ + 1);
		}
	}
	virtual TreeDocItemList GetSubItemList() const
	{
		return sub_item_list_;
	}
	virtual int GetSubItemCount() const
	{
		return (int)sub_item_list_.size();
	}
	virtual bool IsFirstSubItem(TreeItemIDType id) const
	{
		auto it = std::find_if(sub_item_list_.begin(), sub_item_list_.end(), [&](const TreeDocItemPtr& item) {
			return item->GetItemID() == id;
		});
		if (it != sub_item_list_.end() && it == sub_item_list_.begin())
			return true;
		return false;
	}
	virtual bool IsLastSubItem(TreeItemIDType id) const
	{
		auto it = std::find_if(sub_item_list_.begin(), sub_item_list_.end(), [&](const TreeDocItemPtr& item) {
			return item->GetItemID() == id;
		});
		if (it != sub_item_list_.end() && (*it)->GetItemID() == (*sub_item_list_.rbegin())->GetItemID())
			return true;
		return false;
	}
protected:
	int level_;
	TreeItemIDType item_id_;
	TreeItemDataType item_data_;
	TreeDocItemWeakPtr parent_;
	TreeDocItemList sub_item_list_;
private:
	static const TreeItemIDType kDEFAULT_ITEM_ID;
};
class TreeDoc : public ItreeDocItemEventHandler
{
public:
	TreeDoc() : root_(std::make_shared<TreeDocItemBase>()), item_remove_callback_(nullptr)
	{
		root_->SetLevel(-1);
		AttachDocItemEvent(std::dynamic_pointer_cast<ITreeDocItemEvent>(root_));
	}
public:
	void AddItem(const TreeDocItemPtr& sub_item, const TreeDocItemPtr& parent) {
		sub_item->SetLevel(parent->GetLevel() + 1);
		parent->AddSubItem(sub_item);
		AttachDocItemEvent(std::dynamic_pointer_cast<ITreeDocItemEvent>(sub_item));
	}
	void AddItem(const TreeDocItemPtr& sub_item) {
		AddItem(sub_item, root_);
	}
	void RemoveItem(const TreeItemIDType& id) {
		auto item = data_indexs_.find(id);
		if (item != data_indexs_.end())
		{
			auto parent = item->second.lock()->GetParent();
			if (parent != nullptr)
				parent->RemoveSubItem(item->second.lock());
		}
	}
	TreeDocItemPtr GetRootItem() const {
		return root_;
	}
	TreeDocItemPtr GetItem(const TreeItemIDType& id) const {
		auto item = data_indexs_.find(id);
		if (item != data_indexs_.end())
			return item->second.lock();
		return nullptr;
	}
	void AttachRemoveItem(const std::function<void(const TreeDocItemList&)>& cb) { item_remove_callback_ = cb; };
protected:
	virtual void OnDocItemAddChildCallback(const TreeDocItemPtr& item);
	virtual void OnDocItemReleaseChildCallback(const TreeDocItemPtr& item);
private:
	void RemoveItem(const TreeDocItemPtr& item, TreeDocItemList& removed_item_list);
private:
	WeakTreeDocItemMap data_indexs_;//数据索引
	TreeDocItemPtr root_;//根节点	维持树型结构
	std::function<void(const TreeDocItemList&)> item_remove_callback_;
};

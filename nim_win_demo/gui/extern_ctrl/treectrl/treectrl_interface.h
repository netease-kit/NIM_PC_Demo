#pragma once

template<typename TCB, typename... TParams>
void InvokeSafeCallback(const TCB& cb, const TParams&... params)
{
	if (cb != nullptr)
		cb(params...);
}
template<typename TCBArray, typename... TParams>
void InvokeSafeCallbackArray(const TCBArray& cbarray, const TParams&... params)
{
	for (auto& it : cbarray)
	{
		it(params...);
	}
}
template<unsigned int index,typename TCBArray, typename... TParams>
void InvokeSafeCallbackArray(const TCBArray& cbarray, const TParams&... params)
{
	for (auto& it : cbarray)
	{		
		std::get<index>(it)(params...);
	}	
}

using TreeItemIDType = std::string;
using TreeItemDataType = void*;
class ITreeDocItem;
using TreeDocItemPtr = std::shared_ptr<ITreeDocItem>;
using TreeDocItemWeakPtr = std::weak_ptr<ITreeDocItem>;
using WeakTreeDocItemMap = std::map<TreeItemIDType, TreeDocItemWeakPtr>;
using TreeDocItemList = std::list<TreeDocItemPtr>;
//事件相关
using TreeItemAddChildCB = std::function<void(const TreeDocItemPtr&)>;
using TreeItemReleaseChildCB = std::function<void(const TreeDocItemPtr&)>;
using TreeItemEventDetatch = std::function<void()>;

class ItreeDocItemEventHandler;
class ITreeDocItemEvent : virtual public nbase::SupportWeakCallback
{
public:
	ITreeDocItemEvent() {};
	virtual ~ITreeDocItemEvent() {};
public:
	virtual TreeItemEventDetatch AttachAddItem(ItreeDocItemEventHandler* handler,const TreeItemAddChildCB& cb) {		
		OnAddItem[handler] = cb;		
		return ToWeakCallback([this, handler]() {	OnAddItem.erase(handler);});
	}
	virtual TreeItemEventDetatch AttachReleaseItem(ItreeDocItemEventHandler* handler,const TreeItemReleaseChildCB& cb) {		
		OnRelease[handler] = cb; 
		return ToWeakCallback([this, handler]() {OnRelease.erase(handler); });
	}
protected:
	std::map< ItreeDocItemEventHandler*,TreeItemAddChildCB> OnAddItem;
	std::map<ItreeDocItemEventHandler*,TreeItemReleaseChildCB> OnRelease;
};
class ItreeDocItemEventHandler : virtual public nbase::SupportWeakCallback
{
public:
	ItreeDocItemEventHandler() = default;
	virtual ~ItreeDocItemEventHandler()
	{
		InvokeSafeCallbackArray(cb_detach_);
	}
protected:
	virtual void AttachDocItemEvent(std::shared_ptr<ITreeDocItemEvent> item)
	{
		cb_detach_.emplace_back(item->AttachAddItem(this, ToWeakCallback(std::bind(&ItreeDocItemEventHandler::OnDocItemAddChildCallback, this, std::placeholders::_1))));
		cb_detach_.emplace_back(item->AttachReleaseItem(this, ToWeakCallback(std::bind(&ItreeDocItemEventHandler::OnDocItemReleaseChildCallback, this, std::placeholders::_1))));
	}
	virtual void AttachDocItemEvent(ITreeDocItemEvent* item)
	{
		cb_detach_.emplace_back(item->AttachAddItem(this, ToWeakCallback(std::bind(&ItreeDocItemEventHandler::OnDocItemAddChildCallback, this, std::placeholders::_1))));
		cb_detach_.emplace_back(item->AttachReleaseItem(this, ToWeakCallback(std::bind(&ItreeDocItemEventHandler::OnDocItemReleaseChildCallback, this, std::placeholders::_1))));
	}
	virtual void OnDocItemAddChildCallback(const TreeDocItemPtr& item) = 0;
	virtual void OnDocItemReleaseChildCallback(const TreeDocItemPtr& item) = 0;
private:
	mutable std::vector<TreeItemEventDetatch> cb_detach_;
};
//数据相关
//class ITreeDocItem;
class ITreeDocItem : virtual public ITreeDocItemEvent,virtual public std::enable_shared_from_this<ITreeDocItem>
{
public:
	ITreeDocItem(){}
	virtual ~ITreeDocItem() {	};
	virtual TreeItemIDType GetItemID() const = 0;
	virtual void SetItemID(TreeItemIDType id) = 0;
	virtual TreeItemDataType GetItemData() const = 0;
	virtual void SetItemData(TreeItemDataType data) = 0;
	virtual TreeDocItemPtr GetParent() const = 0;
	virtual void SetParent(const TreeDocItemPtr& parent) = 0;
	virtual void AddSubItem(const TreeDocItemPtr& sub_item) = 0;
	virtual void RemoveSubItem(const TreeDocItemPtr& sub_item) = 0;
	virtual int GetLevel() const = 0;
	virtual void SetLevel(int level) = 0;
	virtual TreeDocItemList GetSubItemList() const = 0;
	virtual int GetSubItemCount() const = 0;
	virtual bool IsFirstSubItem(TreeItemIDType id) const = 0;
	virtual bool IsLastSubItem(TreeItemIDType id) const = 0;
};

//UI相关定义
class ITreeComponent
{
public:
	virtual void Update(bool recalc_visible) = 0;
	virtual ui::Box* GetComponent() = 0;
	virtual ui::CSize GetUIStyleSize(std::string style_name) = 0;
	virtual bool ShowTreeLine() const = 0;
	virtual bool ShowCheckbox() const = 0;
	virtual bool ShowExpandButton() const = 0;
	virtual unsigned int GetLevelSpace() const = 0;
	virtual bool GetExpandImage(ui::Image& expand, ui::Image& collapse) = 0;
	virtual bool GetCheckboxImage(ui::Image& sel, ui::Image& unsel, ui::Image& partsel) = 0;
	virtual int GetExpandButtonWidth() const = 0;
	virtual int GetCheckboxButtonWidth() const = 0;
	virtual void OnItemSelected(TreeItemIDType) = 0;
};

enum TreeItemUIState
{
	TreeItemUIState_SELECTED = 0x00000001,		// selected
	TreeItemUIState_EXPANDED = 0x00000002,		// expanded
	TreeItemUIState_EXPANDEDONCE = 0x00000004,		// expanded once

	TreeItemUIState_SHOWCHECKBOX = 0x00000010,		// show check box
	TreeItemUIState_SHOWTREEIMAGE = 0x00000040,		// show image


	TreeItemUIState_CHECKED = 0x00010000,		// [V]
	TreeItemUIState_CHECKED_NULL = 0x00020000,		// [ ]

	TreeItemUIState_CHECKEDPART = 0x00030000,		// [?]
	TreeItemUIState_CHECKEDMASK = 0x00030000,		// TLI_CHECKED | TLI_CHECKED_NULL

	TreeItemUIState_ALLSTATE = 0xFFFFFFFF,
};
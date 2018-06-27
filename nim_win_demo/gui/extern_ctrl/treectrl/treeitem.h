#pragma once
#include "treedoc.h"
#include "treectrl_interface.h"

class TreeItemUI : public ItreeDocItemEventHandler
{
public:
	TreeItemUI(ITreeDocItem* doc_item);
	virtual ~TreeItemUI();
	
	void Expand();
	void Collapse();
	DWORD GetState(DWORD state_mask = TreeItemUIState_ALLSTATE) const;
	void SetState(DWORD add_status, DWORD remove_status);
	bool IsShowCheckBox() const;
	void ShowCheckBox(bool show);
	bool GetSelected() const;
	void SetSelected(bool selected);
	int GetCheck() const;
	void SetCheck(bool check);
	bool IsExpand() const;
	//ui
	virtual void Paint(ui::IRenderContext* pRender, ui::UiRect rc);
	void OnChildHeightChange(int chg_value,bool change_child_height);
	void OnChildChecked(int state);
	int GetWholeHeight() /*const*/;
	std::shared_ptr<TreeItemUI> GetParentUI();
	virtual std::string GetIUIStyleName();
	bool HandleMessage(ui::EventArgs& event);
protected:
	virtual void OnDocItemAddChildCallback(const TreeDocItemPtr& item) override;
	virtual void OnDocItemReleaseChildCallback(const TreeDocItemPtr& item) override;

	virtual void DrawTreeLine(const TreeDocItemPtr& doc_item,ui::IRenderContext* pRender, ui::UiRect rc);
	virtual void DrawTreeButton(const TreeDocItemPtr& doc_item,ui::IRenderContext* pRender, ui::UiRect rc);
	virtual void DrawCheckBox(const TreeDocItemPtr& doc_item,ui::IRenderContext* pRender, ui::UiRect rc);
private:
	void NotifyParentHeightChange(int chg_value, bool change_child_height);
	void NotifyParentCheck(int state);
	bool IsFirst(const TreeDocItemPtr& node);
	bool IsLast(const TreeDocItemPtr& node);
	TreeDocItemPtr GetNodeAtDepth(int depth);
	inline TreeDocItemPtr GetDocItem()
	{			
		return doc_item_->shared_from_this();
	}
	bool DrawImage(ui::IRenderContext* pRender, ui::Image& duiImage, const ui::UiRect& rc);
	void SetCheck_i(bool check);
	int GetChildChedk(int state);
private:
	int height_;
	int child_height_;
	ui::UiRect rc_expand_button_;
	ui::UiRect rc_checkbox_;
	DWORD state_;
	std::function<int(const std::string&)> get_style_height_;
	ITreeDocItem* doc_item_;
};

template<typename... TList> class TreeItem {};
template<> class TreeItem<> : virtual public TreeDocItemBase
{
public:
	TreeItem()
	{
		item_ui_ = std::make_shared<TreeItemUI>(this);
	}
	~TreeItem()
	{

	}
	void SetTreeComponent(ITreeComponent* tree_component) {
		tree_component_ = tree_component;
	}
	ITreeComponent* GetTreeComponent() const {
		return tree_component_;
	}
	virtual std::string GetIUIStyleName() const { return OnGetIUIStyleName(); }
	std::shared_ptr<TreeItemUI> GetUI() const {	return item_ui_;	}

protected:
	virtual std::string OnGetIUIStyleName() const = 0;
private:
	std::shared_ptr<TreeItemUI> item_ui_;
	ITreeComponent* tree_component_;
};
template<typename T, typename... TList> class TreeItem<T, TList...> : virtual public T, virtual public TreeItem<TList...> {};
typedef TreeItem<> NulTreeItem;
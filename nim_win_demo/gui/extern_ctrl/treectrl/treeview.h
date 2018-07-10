#pragma once
#include <atomic>
#include <unordered_map>
#include "treectrl_interface.h"
#include "treedoc.h"
#include "treeitem.h"

class ITreeItemUIStyle;

class TreeComponent : public ui::ListBox, public ui::IControlFromPointFinder, public ITreeComponent
{
	std::map<std::string, std::map<TreeItemIDType, std::shared_ptr<ITreeItemUIStyle>>> catched_style_;

public:
	TreeComponent();
	virtual ~TreeComponent();
	virtual void SetWindow(ui::Window* pManager, ui::Box* pParent, bool bInit = true) override;	
	const std::shared_ptr<TreeDoc> GetDoc() const
	{
		return doc_;
	}
	virtual void Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint) override;
	virtual void SetPos(ui::UiRect rc) override;
	virtual void DoInit() override;
	virtual void SetScrollPos(ui::CSize szPos) override;
	virtual ui::Control* FindControlFromPoint(const ui::CPoint& pt) override;
	virtual void Update(bool recalc_visible) override;
	virtual ui::Box* GetComponent() override;
	virtual ui::CSize GetScrollPos() const override;
	virtual ui::CSize GetUIStyleSize(std::string style_name) override;
	virtual bool ShowTreeLine() const override;
	virtual bool ShowCheckbox() const override;
	virtual bool ShowExpandButton() const override;
	virtual unsigned int GetLevelSpace() const override;

	void AttachSelectChange(const std::function<bool(const std::shared_ptr<NulTreeItem>&)>& callback){ selected_chenge_callback_.emplace_back(callback); };
	void SetExpandImage(const std::wstring& expand, const std::wstring& collapse);
	void SetCheckImage(const std::wstring& sel, const std::wstring& unsel, const std::wstring& partsel);

	virtual bool GetExpandImage(ui::Image& expand, ui::Image& collapse) override;
	virtual bool GetCheckboxImage(ui::Image& sel, ui::Image& unsel, ui::Image& partsel) override;

	virtual int GetExpandButtonWidth() const  override { return (show_tree_line_ ? level_space_widht_ :  expand_button_width_); }
	virtual  int GetCheckboxButtonWidth() const  override { return (checkbox_button_width_); }
	virtual void OnItemSelected(TreeItemIDType) override;

	void RegisterStyleUI(const std::string& style_name, const std::function<std::shared_ptr<ITreeItemUIStyle>()>& creator);
protected:	
	std::shared_ptr<ITreeItemUIStyle> GetUIStyle(const std::shared_ptr<NulTreeItem>& item,bool& need_fill);
	std::shared_ptr<ITreeItemUIStyle> GetUIStyle(const std::shared_ptr<NulTreeItem>& item);
	ui::CSize ResetVisibleList(const ui::UiRect& rc_component);
	ui::CSize ResetVisibleList(const ui::CSize& scrollpos, const ui::UiRect& rc_component,ui::UiRect& rc, TreeDocItemPtr doc_item);
	virtual ui::CSize CalcRequiredSize(const ui::UiRect& rc) override;
	ui::CSize GetScrollPos_i() const;
	void OnRemoveItemCallback(const TreeDocItemList& list);
protected:
	std::shared_ptr<TreeDoc> doc_;
	std::map<std::string, std::function<std::shared_ptr<ITreeItemUIStyle>()>> style_factory_;
	std::map<std::string, ui::CSize> style_size_list_;
	std::unordered_map<TreeItemIDType,std::tuple<bool,ui::UiRect, std::shared_ptr<NulTreeItem>, std::shared_ptr<ITreeItemUIStyle>>> visible_item_list_;
	std::unordered_map<TreeItemIDType, std::shared_ptr<NulTreeItem>> visible_item_list_old_;
	TreeItemIDType begin_item_id_;
	TreeItemIDType end_item_id_;
	std::shared_ptr<NulTreeItem> selected_item_;
	ui::CSize required_size_;
	nbase::WeakCallbackFlag draw_timer_;
	bool show_tree_line_;
	bool show_check_box_;
	bool show_expand_button_;
	ui::Image expand_expand_image_;
	ui::Image expand_collapse_image_;

	ui::Image checkbox_sel_image_;
	ui::Image checkbox_unsel_image_;
	ui::Image checkbox_partsel_image_;
	int expand_button_width_;
	int checkbox_button_width_;
	int level_space_widht_;
	std::list<std::function<bool(const std::shared_ptr<NulTreeItem>&)>> selected_chenge_callback_;
};

#pragma once
#include "treeitem.h"
class ITreeComponent;
class ITreeItemUIStyle : public ui::ListContainerElement
{
public:
	virtual int GetHeight() = 0;
	virtual int GetWidth() {		
		return ui::ListContainerElement::GetFixedWidth();
	};
	virtual void DoPaint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint);
	virtual void HandleMessage(ui::EventArgs& event) override;
	virtual void HandleMessageTemplate(ui::EventArgs& event) override;
	virtual void Fill(const std::shared_ptr<NulTreeItem>& doc_item);
	virtual void SetPos(ui::UiRect rc,bool recalc_child = true);
	virtual int GetLevel() const;
	virtual void SetVirtualParent(ITreeComponent* virtual_parent) {
		virtual_parent_ = virtual_parent;
	}
	virtual ITreeComponent* GetVirtualParent() const {
		return virtual_parent_;
	}
	virtual void Activate() override;
	virtual bool IsPointInWithScrollOffset(const ui::CPoint& point) const override;
	ui::CSize GetRequiredSize() const { return requiredSize_; };
	virtual void Selected(bool bSelect, bool trigger) override;
protected:
	virtual void OnFill() = 0;
	virtual void OnHandleMessage(ui::EventArgs& event);
	virtual void OnHandleMessageTemplate(ui::EventArgs& event);
protected:
	std::shared_ptr<NulTreeItem> doc_item_;
	ITreeComponent* virtual_parent_;
	ui::CSize requiredSize_;
};
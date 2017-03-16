#ifndef UI_CONTROL_TREEVIEW_H_
#define UI_CONTROL_TREEVIEW_H_

#pragma once

namespace ui
{

#define ROOT_NODE_DEPTH  -1

class TreeView;
class UILIB_API TreeNode : public ListContainerElement
{
public:
	TreeNode();

	void SetTreeView(TreeView* pTreeView);
	virtual bool OnClickItem(EventArgs* param);
	virtual bool IsVisible() const override;
	virtual void SetInternVisible(bool bVisible) override;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true) override;

	TreeNode* GetParentNode();
	void SetParentNode(TreeNode* pParentTreeNode);

	bool AddChildNode(TreeNode* pTreeNode);
	bool AddChildNodeAt(TreeNode* pTreeNode, std::size_t iIndex);
	bool RemoveChildNodeAt(std::size_t index);
	bool RemoveChildNode(TreeNode* pTreeNode);
	void RemoveAllChildNode();

	int GetDescendantNodeCount();
	std::size_t GetChildNodeCount();
	TreeNode* GetChildNode(std::size_t _nIndex);
	int	GetChildNodeIndex(TreeNode* pTreeNode);

	bool IsExpand() const; 
	void SetExpand(bool bExpand);
	int GetDepth();

private:
	bool RemoveSelf();

private:
	int m_iDepth;
	bool m_bExpand;
	TreeView *m_pTreeView;
	TreeNode *m_pParentTreeNode;
	std::vector<TreeNode*> mTreeNodes;
};

class UILIB_API TreeView : public ListBox
{
public:
	TreeView(void);

	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;

	TreeNode* GetRootNode()	{ return m_rootNode.get(); }
	int GetIndent() { return m_iIndent;	}
	void SetIndent(int indent) { m_iIndent = indent; }

private:
	//以下函数故意私有化，表明禁止使用；应该使用TreeNode中的相关函数
	bool Add(Control* pControl) override;
	bool AddAt(Control* pControl, std::size_t iIndex) override;
	bool Remove(Control* pControl) override;
	bool RemoveAt(std::size_t iIndex) override;
	void RemoveAll() override;

	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true) override;

private:
	int m_iIndent;
	std::unique_ptr<TreeNode> m_rootNode;
};

}

#endif // UI_CONTROL_TREEVIEW_H_

// Copyright 2017, NetEase (Hangzhou) Network Co., Ltd. All rights reserved.
//
// zouqiwen
// 2015/7/22
//
// virtual ListBox class
#pragma once

namespace ui 
{

class VirtualListInterface
{
public:
	virtual Control* CreateElement() = 0;
	virtual void FillElement(Control *control, int index) = 0;
	virtual int GetElementtCount() = 0;
};

class VirtualLayout : public VLayout 
{
protected:
    virtual ui::CSize ArrangeChild(const std::vector<ui::Control*>& items, ui::UiRect rc);
};

class VirtualListBox : public ListBox
{
    friend class VirtualLayout;
public:
    VirtualListBox(ui::Layout* pLayout = new VirtualLayout);

	virtual void SetDataProvider(VirtualListInterface *pProvider);
	virtual void SetElementHeight(int nHeight);
	virtual void InitElement(int nMaxItemCount = 50);

	virtual void Refresh();
	virtual void RemoveAll() override;

	void SetForceArrange(bool bForce);
    // 获取当前所有可见控件的索引
    void GetDisplayCollection(std::vector<int>& collection);
    // 让控件在可见范围内
    void EnsureVisible(int iIndex, bool bToTop = false);

protected:
	void ReArrangeChild(bool bForce);

	void AddElement(int iIndex);
	void RemoveElement(int iIndex);

	virtual void SetScrollPos(ui::CSize szPos) override;
	virtual void HandleMessage(ui::EventArgs& event) override;
    virtual void SetPos(UiRect rc) override;

	virtual void OnKeyDown(TCHAR ch) {}
	virtual void OnKeyUp(TCHAR ch) {}

private:
	enum ScrollDirection
	{
		kScrollUp = -1,
		kScrollDown = 1
	};

	Control* CreateElement();
	void FillElement(Control *pControl, int iIndex);
	int GetElementCount();
	bool UseDefaultLayout();

    // 得到n个元素对应的高度和，-1表示全部元素
    int CalcElementsHeight(int nCount);

    // 得到可见范围内第一个元素的前一个元素索引，和其bottom对应的值
	int GetTopElementIndex(int &bottom);

    // 判断某个元素是否在可见范围内
    bool IsElementDisplay(int iIndex);

    // 判断是否要重新布局
	bool NeedReArrange(ScrollDirection &direction);

private:
	VirtualListInterface *m_pDataProvider; 
    int m_nElementHeight;	// 每个项的高度	
	int m_nMaxItemCount;	// 列表真实控件数量上限  
    int m_nOldYScrollPos;
    bool m_bArrangedOnce;
    bool m_bForceArrange;	// 强制布局标记
};

}
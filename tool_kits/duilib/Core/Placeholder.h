#ifndef UI_CORE_PLACE_HOLDER_H_
#define UI_CORE_PLACE_HOLDER_H_

#pragma once

namespace ui 
{

class UILIB_API PlaceHolder : public virtual nbase::SupportWeakCallback
{
public:
	PlaceHolder();
	virtual ~PlaceHolder();

	Box* GetParent() const {	return m_pParent;}

	std::wstring GetName() const;
	std::string GetUTF8Name() const;
	void SetName(const std::wstring& strName);
	void SetUTF8Name(const std::string& strName);

	virtual Window* GetWindow() const;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true);
	virtual void SetWindow(Window* pManager);

	virtual void Init();
	virtual void DoInit();

	virtual bool IsVisible() const;
	bool IsInternVisible() const;
	bool IsFloat() const;
	void SetFloat(bool bFloat = true);

	int GetFixedWidth() const;
	void SetFixedWidth(int cx, bool bArrange = true, bool bNeedDpiScale = true);
	int GetFixedHeight() const;
	void SetFixedHeight(int cy, bool bNeedDpiScale = true);
	int GetMinWidth() const;
	void SetMinWidth(int cx);
	int GetMaxWidth() const;
	void SetMaxWidth(int cx);
	int GetMinHeight() const;
	void SetMinHeight(int cy);
	int GetMaxHeight() const;
	void SetMaxHeight(int cy);
	int GetWidth() const;
	int GetHeight() const;

	HorAlignType GetHorAlignType() const;
	void SetHorAlignType(HorAlignType horAlignType);
	VerAlignType GetVerAlignType() const;
	void SetVerAlignType(VerAlignType verAlignType);

	bool IsReEstimateSize() const;
	void SetReEstimateSize(bool bReEstimateSize);
	virtual CSize EstimateSize(CSize szAvailable);
	virtual	UiRect GetPos(bool bContainShadow = true) const;
	virtual void SetPos(UiRect rc);

	virtual void Arrange();
	virtual void ArrangeAncestor();
	bool IsArranged() const;

	virtual void Invalidate() const;
	UiRect GetPosWithScrollOffset() const;
	CPoint GetScrollOffset() const;

protected:
	virtual void ArrangeSelf();

protected:
	std::wstring m_sName;
	Window *m_pWindow;
	Box* m_pParent;
	CSize m_cxyFixed;
	CSize m_cxyMin;
	CSize m_cxyMax;
	UiRect m_rcItem;
	HorAlignType m_horAlignType;
	VerAlignType m_verAlignType;
	bool m_bFloat;
	bool m_bReEstimateSize;
	bool m_bVisible;
	bool m_bInternVisible;
	bool m_bIsArranged;
};

bool IsChild(PlaceHolder* pAncestor, PlaceHolder* pChild);

} // namespace ui

#endif // UI_CORE_PLACE_HOLDER_H_

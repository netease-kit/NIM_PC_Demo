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

	Box* GetParent() const
	{
		return m_pParent;
	}

	std::wstring GetName() const;
	std::string GetUTF8Name() const;
	void SetName(const std::wstring& pstrName);
	void SetUTF8Name(const std::string&  pstrName);

	virtual Window* GetWindow() const;
	virtual void SetWindow(Window* pManager, Box* pParent, bool bInit = true);
	virtual void SetWindow(Window* pManager);

	virtual void Init();
	virtual void DoInit();

	virtual CSize EstimateSize(CSize szAvailable);
	
	HorAlignType GetHorAlignType() const
	{
		return m_horAlignType;
	}
	void SetHorAlignType(HorAlignType horAlignType)
	{
		m_horAlignType = horAlignType;
	}

	VerAlignType GetVerAlignType() const
	{
		return m_verAlignType;
	}
	void SetVerAlignType(VerAlignType verAlignType)
	{
		m_verAlignType = verAlignType;
	}

	virtual bool IsVisible() const;
	bool IsInternVisible() const;
	bool IsFloat() const;
	void SetFloat(bool bFloat = true);

	int GetFixedWidth() const;
	void SetFixedWidth(int cx, bool arrange = true);
	int GetFixedHeight() const;
	void SetFixedHeight(int cy);
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

	virtual	UiRect GetPos(bool bContainShadow = true) const;
	virtual void SetPos(UiRect rc);

	virtual void Arrange();
	virtual void ArrangeAncestor();
	
	virtual void Invalidate() const;
	UiRect GetPosWithScrollOffset() const;
	bool IsArranged() const;
	CPoint GetScrollOffset() const;

	void SetReEstimateSize(bool reEstimateSize)
	{
		m_bReEstimateSize = reEstimateSize;
	}
	bool GetReEstimateSize() const
	{
		return m_bReEstimateSize;
	}

protected:
	virtual void ArrangeSelf();

protected:
	Window* m_pWindow;
	std::wstring m_sName;
	CSize m_cxyFixed;
	CSize m_cxyMin;
	CSize m_cxyMax;
	Box* m_pParent;
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

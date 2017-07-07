#ifndef UI_CONTROL_LABEL_H_
#define UI_CONTROL_LABEL_H_

#pragma once

namespace ui
{

template<typename InheritType = Control>
class UILIB_API LabelTemplate : public InheritType
{
public:
	LabelTemplate();

	virtual std::wstring GetText() const;
	virtual std::string GetUTF8Text() const;
	virtual void SetText(const std::wstring& strText);
	virtual void SetUTF8Text(const std::string& strText);
	virtual void SetTextId(const std::wstring& strTextId);
	virtual void SetUTF8TextId(const std::string& strTextId);

	virtual CSize EstimateText(CSize szAvailable, bool& bReEstimateSize) override;
	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;
	virtual void PaintText(IRenderContext* pRender) override;

	void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;

	std::wstring GetStateTextColor(ControlStateType stateType);
	void SetStateTextColor(ControlStateType stateType, const std::wstring& dwTextColor);

	int GetFont() const;
	void SetFont(int index);
	UiRect GetTextPadding() const;
	void SetTextPadding(UiRect rc);
	bool IsSingleLine();
	void SetSingleLine(bool bSingleLine);
	bool IsLineLimit();
	void SetLineLimit(bool bLineLimit);

protected:
	int		m_iFont;
	UINT	m_uTextStyle;
	bool    m_bSingleLine;
	bool    m_bLineLimit;
	int		m_hAlign;
	int		m_vAlign;
	UiRect	m_rcTextPadding;
	std::wstring	m_sText;
	std::wstring	m_sTextId;
	StateColorMap	m_textColorMap;
};

#include "LabelImpl.h"

typedef LabelTemplate<Control> Label;
typedef LabelTemplate<Box> LabelBox;

}

#endif // UI_CONTROL_LABEL_H_
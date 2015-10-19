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

		// 文本相关
		virtual std::wstring GetText() const;
		virtual std::string GetUTF8Text() const;
		virtual void SetText(const std::wstring& pstrText);
		virtual void SetUTF8Text(const std::string& pstrText);
		virtual void SetTextId(const std::wstring& strTextId);
		virtual void SetUTF8TextId(const std::string& strTextId);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;

		void SetStateTextColor(ControlStateType stateType, const std::wstring& dwTextColor);
		std::wstring GetStateTextColor(ControlStateType stateType);

		void SetFont(int index);
		int GetFont() const;
		UiRect GetTextPadding() const;
		void SetTextPadding(UiRect rc);
		bool IsSingleLine();
		void SetSingleLine(bool bSingleLine);
		bool IsLineLimit();
		void SetLineLimit(bool bLineLimit);

		virtual CSize EstimateText(CSize szAvailable, bool& reEstimateSize) override;

		virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

		virtual void PaintText(HDC hDC) override;

	protected:
		int		m_iFont;
		UINT	m_uTextStyle;
		bool    m_bSingleLine;
		bool    m_bLineLimit;
		int		m_hAlign;
		int		m_vAlign;
		UiRect	m_rcTextPadding;
		std::wstring	m_TextValue;
		std::wstring	m_sText;
		std::wstring	m_TextId;
		StateColorMap	m_textColorMap;
	};

	#include "LabelImpl.h"

	typedef LabelTemplate<Control> Label;
	typedef LabelTemplate<Box> LabelBox;
}

#endif // UI_CONTROL_LABEL_H_
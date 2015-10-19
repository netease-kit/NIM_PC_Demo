#ifndef UI_CONTROL_OPTION_H_
#define UI_CONTROL_OPTION_H_

#pragma once

namespace ui
{


template<typename InheritType = Control>
class UILIB_API CheckBoxTemplate : public ButtonTemplate<InheritType>
{
public:
	CheckBoxTemplate();

	virtual void Activate() override;

	std::wstring GetSelectedStateImage(ControlStateType stateType);
	void SetSelectedStateImage(ControlStateType stateType, const std::wstring& pStrImage);

	void SetSelectedTextColor(const std::wstring& dwTextColor);
	std::wstring GetSelectedTextColor();

	void SetSelectedStateColor(ControlStateType stateType, const std::wstring& stateColor);
	std::wstring GetSelectStateColor(ControlStateType stateType);

	std::wstring GetSelectedForeStateImage(ControlStateType stateType);
	void SetSelectedForeStateImage(ControlStateType stateType, const std::wstring& pStrImage);

	bool IsSelected() const;
	virtual void Selected(bool bSelected, bool bTriggerEvent = false);

	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue) override;

	virtual void PaintStatusColor(HDC hDC) override;
	virtual void PaintStatusImage(HDC hDC) override;
	virtual void PaintText(HDC hDC) override;

	virtual Image* GetEstimateImage() override;

	void AttachSelect(const EventCallback& callback)
	{
		OnEvent[kEventSelect] += callback;
	}

	void AttachUnSelect(const EventCallback& callback)
	{
		OnEvent[kEventUnSelect] += callback;
	}

protected:
	bool			m_bSelected;
	std::wstring	m_dwSelectedTextColor;
	StateColorMap	m_selectedColorMap;
};


#include "CheckBoxImpl.h"

typedef CheckBoxTemplate<Control> CheckBox;
typedef CheckBoxTemplate<Box> CheckBoxBox;



} // namespace ui

#endif // UI_CONTROL_OPTION_H_
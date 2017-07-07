#ifndef UI_CONTROL_SLIDER_H_
#define UI_CONTROL_SLIDER_H_

#pragma once

namespace ui
{

class UILIB_API Slider : public Progress
{
public:
	Slider();

	int GetChangeStep();
	void SetChangeStep(int step);
	void SetThumbSize(CSize szXY);
	UiRect GetThumbRect() const;
	std::wstring GetThumbStateImage(ControlStateType stateType);
	void SetThumbStateImage(ControlStateType stateType, const std::wstring& pStrImage);
	UiRect GetProgressBarPadding() const;
	void SetProgressBarPadding(UiRect rc);

	virtual UiRect GetProgressPos() override;
	virtual void HandleMessage(EventArgs& event) override;
	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;
	virtual void PaintStatusImage(IRenderContext* pRender) override;

	virtual void ClearImageCache() override;

	void AttachValueChange(const EventCallback& callback) {	OnEvent[kEventValueChange] += callback;	}

protected:
	int m_nStep;
	CSize m_szThumb;
	StateImage m_thumbStateImage;
	UiRect	m_rcProgressBarPadding;
	ControlStateType m_uButtonState;
	std::wstring m_sImageModify;
};

}

#endif // UI_CONTROL_SLIDER_H_
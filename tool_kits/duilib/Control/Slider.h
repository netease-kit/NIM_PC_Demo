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

		void HandleMessage(EventArgs& event);
		void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);
		void PaintStatusImage(HDC hDC);

		void AttachValueChange(const EventCallback& callback)
		{
			OnEvent[kEventValueChange] += callback;
		}
		
		virtual UiRect GetProgressPos() override;

		UiRect GetProgressBarPadding() const;
		void SetProgressBarPadding(UiRect rc);

	protected:
		CSize m_szThumb;
		ControlStateType m_uButtonState;
		int m_nStep;
		std::wstring m_sImageModify;
		StateImage m_thumbStateImage;
		UiRect	m_progressBarPadding;
	};
}

#endif // UI_CONTROL_SLIDER_H_
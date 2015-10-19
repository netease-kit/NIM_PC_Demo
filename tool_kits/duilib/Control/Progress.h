#ifndef UI_CONTROL_PROGRESS_H_
#define UI_CONTROL_PROGRESS_H_

#pragma once

namespace ui
{
	class UILIB_API Progress : public LabelTemplate<Control>
	{
	public:
		Progress();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		bool IsStretchForeImage();
		void SetStretchForeImage(bool bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		double GetValue() const;
		void SetValue(double nValue);
		std::wstring GetProgressImage() const;
		void SetProgressImage(const std::wstring& pStrImage);
		std::wstring GetProgressColor() const
		{
			return m_dwProgressColor;
		}
		void SetProgressColor(const std::wstring& dwProgressColor);

		void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);
		void PaintStatusImage(HDC hDC);

		virtual UiRect GetProgressPos();

	protected:
		bool m_bHorizontal;
		bool m_bStretchForeImage;
		int m_nMax;
		int m_nMin;
		double m_nValue;
		std::wstring m_dwProgressColor;
		Image m_progressImage;
		std::wstring m_progressImageModify;
	};

} // namespace ui

#endif // UI_CONTROL_PROGRESS_H_

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
	int GetMinValue() const;
	void SetMinValue(int nMin);
	int GetMaxValue() const;
	void SetMaxValue(int nMax);
	double GetValue() const;
	void SetValue(double nValue);

	bool IsStretchForeImage();
	void SetStretchForeImage(bool bStretchForeImage = true);
	std::wstring GetProgressImage() const;
	void SetProgressImage(const std::wstring& strImage);
	std::wstring GetProgressColor() const;
	void SetProgressColor(const std::wstring& dwProgressColor);

	virtual void SetAttribute(const std::wstring& strName, const std::wstring& strValue) override;
	virtual void PaintStatusImage(HDC hDC) override;

	virtual UiRect GetProgressPos();

	virtual void ClearImageCache() override;

protected:
	bool m_bHorizontal;
	bool m_bStretchForeImage;
	int m_nMax;
	int m_nMin;
	double m_nValue;
	Image m_progressImage;
	std::wstring m_sProgressColor;
	std::wstring m_sProgressImageModify;
};

} // namespace ui

#endif // UI_CONTROL_PROGRESS_H_

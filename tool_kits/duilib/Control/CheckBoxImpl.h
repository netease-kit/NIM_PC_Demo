

template<typename InheritType>
CheckBoxTemplate<InheritType>::CheckBoxTemplate() :
	m_bSelected(false),
	m_dwSelectedTextColor(),
	m_selectedColorMap()
{
	m_selectedColorMap.SetControl(this);
}

template<typename InheritType>
bool CheckBoxTemplate<InheritType>::IsSelected() const
{
	return m_bSelected;
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::Selected(bool bSelected, bool bTriggerEvent)
{
	if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;

	if( m_pWindow != NULL ) {
		if (bTriggerEvent)
		{
			if (m_bSelected) {
				m_pWindow->SendNotify(this, kEventSelect);
			}
			else {
				m_pWindow->SendNotify(this, kEventUnSelect);
			}
		}
	}

	Invalidate();
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::Activate()
{
	if( !IsActivatable() ) 
		return;
	Selected(!m_bSelected, true);
}

template<typename InheritType>
std::wstring CheckBoxTemplate<InheritType>::GetSelectedStateImage(ControlStateType stateType)
{
	return m_imageMap.GetImagePath(kStateImageSelectedBk, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateImage(ControlStateType stateType, const std::wstring& pStrImage)
{
	m_imageMap.SetImage(kStateImageSelectedBk, stateType, pStrImage);
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedTextColor(const std::wstring& dwTextColor)
{
	m_dwSelectedTextColor = dwTextColor;
	Invalidate();
}

template<typename InheritType>
std::wstring CheckBoxTemplate<InheritType>::GetSelectedTextColor()
{
	return m_dwSelectedTextColor;
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedStateColor(ControlStateType stateType, const std::wstring& stateColor)
{
	m_selectedColorMap[stateType] = stateColor;
	Invalidate();
}

template<typename InheritType>
std::wstring CheckBoxTemplate<InheritType>::GetSelectStateColor(ControlStateType stateType)
{
	return m_selectedColorMap[stateType];
}

template<typename InheritType>
std::wstring CheckBoxTemplate<InheritType>::GetSelectedForeStateImage(ControlStateType stateType)
{
	return m_imageMap.GetImagePath(kStateImageSelectedFore, stateType);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetSelectedForeStateImage(ControlStateType stateType, const std::wstring& pStrImage)
{
	m_imageMap.SetImage(kStateImageSelectedFore, stateType, pStrImage);
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue)
{
	if( pstrName == _T("selected") ) Selected(pstrValue == _T("true"), true);
	else if( pstrName == _T("selectednormalimage") ) SetSelectedStateImage(kControlStateNormal, pstrValue);
	else if( pstrName == _T("selectedhotimage") ) SetSelectedStateImage(kControlStateHot, pstrValue);
	else if( pstrName == _T("selectedpushedimage") ) SetSelectedStateImage(kControlStatePushed, pstrValue);
	else if( pstrName == _T("selecteddisabledimage") ) SetSelectedStateImage(kControlStateDisabled, pstrValue);
	else if( pstrName == _T("selectedtextcolor") ) {
		SetSelectedTextColor(pstrValue);
	}
	else if( pstrName == _T("selectednormalcolor") ) SetSelectedStateColor(kControlStateNormal, pstrValue);
	else if( pstrName == _T("selectedhotcolor") ) SetSelectedStateColor(kControlStateHot, pstrValue);
	else if( pstrName == _T("selectedpushedcolor") ) SetSelectedStateColor(kControlStatePushed, pstrValue);
	else if( pstrName == _T("selecteddisabledcolor") ) SetSelectedStateColor(kControlStateDisabled, pstrValue);
	else if (pstrName == _T("selectedforenormalimage")) SetSelectedForeStateImage(kControlStateNormal, pstrValue);
	else if (pstrName == _T("selectedforehotimage")) SetSelectedForeStateImage(kControlStateHot, pstrValue);
	else if (pstrName == _T("selectedforepushedimage")) SetSelectedForeStateImage(kControlStatePushed, pstrValue);
	else if (pstrName == _T("selectedforedisabledimage")) SetSelectedForeStateImage(kControlStateDisabled, pstrValue);
	else __super::SetAttribute(pstrName, pstrValue);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintStatusColor(HDC hDC) 
{
	if (!IsSelected())
	{
		__super::PaintStatusColor(hDC);
		return;
	}

	m_selectedColorMap.PaintStatusColor(hDC, m_rcPaint, m_uButtonState);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintStatusImage(HDC hDC)
{
	if (!IsSelected())
	{
		__super::PaintStatusImage(hDC);
		return;
	}

	m_imageMap.PaintStatusImage(hDC, kStateImageSelectedBk, m_uButtonState);
	m_imageMap.PaintStatusImage(hDC, kStateImageSelectedFore, m_uButtonState);
}

template<typename InheritType>
void CheckBoxTemplate<InheritType>::PaintText(HDC hDC)
{
	if (!IsSelected())
	{
		__super::PaintText(hDC);
		return;
	}

	if( GetText().empty() ) return;
	UiRect rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	std::wstring newTextColor = m_dwSelectedTextColor.empty() ? m_textColorMap[kControlStateNormal] : m_dwSelectedTextColor;
	DWORD dwTextColor = GlobalManager::ConvertTextColor(newTextColor);
	DWORD dwDisabledTextColor = GlobalManager::ConvertTextColor(m_textColorMap[kControlStateDisabled]);
	RenderEngine::DrawText(hDC, rc, GetText(), IsEnabled()?dwTextColor:dwDisabledTextColor, \
		m_iFont, m_uTextStyle);
}

template<typename InheritType>
Image* CheckBoxTemplate<InheritType>::GetEstimateImage()
{
	Image* estimateImage = __super::GetEstimateImage();
	if (!estimateImage) {
		estimateImage = m_imageMap.GetEstimateImage(kStateImageSelectedBk);
	}

	return estimateImage;
}






template<typename InheritType>
LabelTemplate<InheritType>::LabelTemplate() :
	m_iFont(1),
	m_uTextStyle(DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_NOCLIP | DT_SINGLELINE),
    m_bSingleLine(true),
    m_bLineLimit(false),
	m_hAlign(DT_LEFT),
	m_vAlign(DT_CENTER),
	m_rcTextPadding(),
	m_TextValue(),
	m_sText(),
	m_TextId(),
	m_textColorMap()
{
	if (dynamic_cast<Box*>(this)) {
		m_cxyFixed.cx = m_cxyFixed.cy = DUI_LENGTH_STRETCH;
	}
	else {
		m_cxyFixed.cx = m_cxyFixed.cy = DUI_LENGTH_AUTO;
	}

	m_textColorMap[kControlStateNormal] = GlobalManager::GetDefaultTextColor();
	m_textColorMap[kControlStateDisabled] = GlobalManager::GetDefaultDisabledTextColor();
	m_textColorMap.SetControl(this);
}

template<typename InheritType>
std::wstring LabelTemplate<InheritType>::GetText() const
{
	std::wstring strText = m_sText;
	if (strText.empty()) {
		strText = MutiLanSupport::GetInstance()->GetStringViaID(m_TextId);
	}

	return strText;
}

template<typename InheritType>
std::string LabelTemplate<InheritType>::GetUTF8Text() const
{
	std::wstring unicodeText = GetText();
	int multiLength = WideCharToMultiByte(CP_UTF8, NULL, unicodeText.c_str(), -1, NULL, 0, NULL, NULL);
	if (multiLength <= 0)
		return "";
	std::unique_ptr<char[]> strText(new char[multiLength]);
	WideCharToMultiByte(CP_UTF8, NULL, unicodeText.c_str(), -1, strText.get(), multiLength, NULL, NULL);

	std::string res = strText.get();

	return res;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetText(const std::wstring& pstrText)
{
	if( m_sText == pstrText ) return;
	m_sText = pstrText;

	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetUTF8Text(const std::string& pstrText)
{
	int wideLength = MultiByteToWideChar(CP_UTF8, NULL, pstrText.c_str(), -1, NULL, 0);
	std::unique_ptr<wchar_t[]> strText(new wchar_t[wideLength]);
	MultiByteToWideChar(CP_UTF8, NULL, pstrText.c_str(), -1, strText.get(), wideLength);

	SetText(strText.get());
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetTextId(const std::wstring& strTextId)
{
	if( m_TextId == strTextId ) return;
	m_TextId = strTextId;

	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetUTF8TextId(const std::string& strTextId)
{
	int wideLength = MultiByteToWideChar(CP_UTF8, NULL, strTextId.c_str(), -1, NULL, 0);
	std::unique_ptr<wchar_t[]> strText(new wchar_t[wideLength]);
	MultiByteToWideChar(CP_UTF8, NULL, strTextId.c_str(), -1, strText.get(), wideLength);

	SetTextId(strText.get());
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetTextStyle(UINT uStyle)
{
	m_uTextStyle = uStyle;
	Invalidate();
}

template<typename InheritType>
UINT LabelTemplate<InheritType>::GetTextStyle() const
{
	return m_uTextStyle;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetStateTextColor(ControlStateType stateType, const std::wstring& dwTextColor)
{
	if (stateType == kControlStateHot) {
		m_animationManager.SetFadeHot(true);
	}
	m_textColorMap[stateType] = dwTextColor;
	Invalidate();
}

template<typename InheritType>
std::wstring LabelTemplate<InheritType>::GetStateTextColor(ControlStateType stateType)
{
	return m_textColorMap[stateType];
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetFont(int index)
{
	m_iFont = index;
	Invalidate();
}

template<typename InheritType>
int LabelTemplate<InheritType>::GetFont() const
{
	return m_iFont;
}

template<typename InheritType>
UiRect LabelTemplate<InheritType>::GetTextPadding() const
{
	return m_rcTextPadding;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetTextPadding(UiRect rc)
{
	m_rcTextPadding = rc;
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

template<typename InheritType>
bool LabelTemplate<InheritType>::IsSingleLine()
{
	return m_bSingleLine;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetSingleLine(bool bSingleLine)
{
	if( m_bSingleLine == bSingleLine ) return;

	m_bSingleLine = bSingleLine;
	Invalidate();
}

template<typename InheritType>
bool LabelTemplate<InheritType>::IsLineLimit()
{
	return m_bLineLimit;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetLineLimit(bool bLineLimit)
{
	if (m_bLineLimit == bLineLimit) return;

	m_bLineLimit = bLineLimit;
	Invalidate();
}

template<typename InheritType>
CSize LabelTemplate<InheritType>::EstimateText(CSize szAvailable, bool& reEstimateSize)
{
	if (m_bSingleLine) {
		m_uTextStyle |= DT_SINGLELINE;
	}
	else {
		m_uTextStyle &= ~DT_SINGLELINE;
	}

	int width = GetFixedWidth();
	if (width < 0) {
		width = 0;
	}
	CSize fixedSize;
	if (!GetText().empty()) {
		UiRect rect = RenderEngine::MeasureText(m_pWindow->GetPaintDC(), GetText(), m_iFont, m_uTextStyle, width);
		if (GetFixedWidth() == DUI_LENGTH_AUTO) {
			fixedSize.cx = rect.right - rect.left + m_rcTextPadding.left + m_rcTextPadding.right;
		}
		if (GetFixedHeight() == DUI_LENGTH_AUTO) {
			int estimateWidth = rect.right - rect.left;
			int estimateHeight = rect.bottom - rect.top;

			if (!m_bSingleLine && GetFixedWidth() == DUI_LENGTH_AUTO && GetMaxWidth() == DUI_LENGTH_STRETCH) {
				reEstimateSize = true;
				int maxWidth = szAvailable.cx - m_rcTextPadding.left - m_rcTextPadding.right;
				if (estimateWidth > maxWidth) {
					estimateWidth = maxWidth;
					UiRect newRect = RenderEngine::MeasureText(m_pWindow->GetPaintDC(), GetText(), m_iFont, m_uTextStyle, estimateWidth);
					estimateHeight = newRect.bottom - newRect.top;
				}
			}
			fixedSize.cx = estimateWidth + m_rcTextPadding.left + m_rcTextPadding.right;
			fixedSize.cy = estimateHeight + m_rcTextPadding.top + m_rcTextPadding.bottom;
		}
	}

	return fixedSize;
}

template<typename InheritType>
void LabelTemplate<InheritType>::SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue)
{
	if( pstrName == _T("align") ) {
		if( pstrValue.find(_T("left")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			m_uTextStyle |= DT_LEFT;
		}
		if( pstrValue.find(_T("center")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_LEFT | DT_RIGHT );
			m_uTextStyle |= DT_CENTER;
		}
		if( pstrValue.find(_T("right")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			m_uTextStyle |= DT_RIGHT;
		}
		if( pstrValue.find(_T("top")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
			m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
		}
		if( pstrValue.find(_T("vcenter")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_TOP | DT_BOTTOM );			
			m_uTextStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		if( pstrValue.find(_T("bottom")) != std::wstring::npos ) {
			m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
			m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
		}
	}
	else if( pstrName == _T("endellipsis") ) {
		if( pstrValue == _T("true") ) m_uTextStyle |= DT_END_ELLIPSIS;
		else m_uTextStyle &= ~DT_END_ELLIPSIS;
	}
	else if (pstrName == _T("linelimit")) {
		SetLineLimit(pstrValue == _T("true"));
	}
	else if( pstrName == _T("text") ) SetText(pstrValue);
	else if( pstrName == _T("textid") ) SetTextId(pstrValue);
	else if( pstrName == _T("font") ) SetFont(_ttoi(pstrValue.c_str()));
	else if( pstrName == _T("normaltextcolor") ) SetStateTextColor(kControlStateNormal, pstrValue);
	else if( pstrName == _T("hottextcolor") )	SetStateTextColor(kControlStateHot, pstrValue);
	else if( pstrName == _T("pushedtextcolor") )	SetStateTextColor(kControlStatePushed, pstrValue);
	else if( pstrName == _T("disabledtextcolor") )	SetStateTextColor(kControlStateDisabled, pstrValue);
	else if( pstrName == _T("textpadding") ) {
		UiRect rcTextPadding;
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue.c_str(), &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
	else if( pstrName == _T("singleline") ) SetSingleLine(pstrValue == _T("true"));
	else __super::SetAttribute(pstrName, pstrValue);
}

template<typename InheritType>
void LabelTemplate<InheritType>::PaintText(HDC hDC)
{
	if( GetText().empty() ) return;
	UiRect rc = m_rcItem;
	rc.left += m_rcTextPadding.left;
	rc.right -= m_rcTextPadding.right;
	rc.top += m_rcTextPadding.top;
	rc.bottom -= m_rcTextPadding.bottom;

	ControlStateType stateType = m_uButtonState;
	if( stateType == kControlStatePushed && GetStateTextColor(kControlStatePushed).empty() ) {
		stateType = kControlStateHot;
	}
	if( stateType == kControlStateHot && GetStateTextColor(kControlStateHot).empty() ) {
		stateType = kControlStateNormal;
	}
	if( stateType == kControlStateDisabled && GetStateTextColor(kControlStateDisabled).empty() ) {	
		stateType = kControlStateNormal;
	}
	std::wstring clrColor = GetStateTextColor(stateType);
	DWORD dwClrColor = GlobalManager::ConvertTextColor(clrColor);

	if (m_bSingleLine) {
		m_uTextStyle |= DT_SINGLELINE;
	}
	else {
		m_uTextStyle &= ~DT_SINGLELINE;
	}

	if (m_animationManager.GetAnimationPlayer(kAnimationHot)) {
		if ((stateType == kControlStateNormal || stateType == kControlStateHot)
			&& !GetStateTextColor(kControlStateHot).empty()) {
				std::wstring clrColor = GetStateTextColor(kControlStateNormal);
				if (!clrColor.empty()) {
					DWORD dwClrColor = GlobalManager::ConvertTextColor(clrColor);
					RenderEngine::DrawText(hDC, rc, GetText(), dwClrColor, m_iFont, m_uTextStyle, 255 , m_bLineLimit);
				}

				if (m_nHotAlpha > 0) {
					std::wstring clrColor = GetStateTextColor(kControlStateHot);
					if (!clrColor.empty()) {
						DWORD dwClrColor = GlobalManager::ConvertTextColor(clrColor);
						RenderEngine::DrawText(hDC, rc, GetText(), dwClrColor, m_iFont, m_uTextStyle, m_nHotAlpha, m_bLineLimit);
					}
				}

				return;
		}
	}

	RenderEngine::DrawText(hDC, rc, GetText(), dwClrColor, m_iFont, m_uTextStyle, 255, m_bLineLimit);
}




template<typename InheritType>
OptionTemplate<InheritType>::OptionTemplate() :
	m_sGroupName()
{

}

template<typename InheritType>
OptionTemplate<InheritType>::~OptionTemplate() 
{
	if( !m_sGroupName.empty() && m_pWindow ) m_pWindow->RemoveOptionGroup(m_sGroupName, this);
}

template<typename InheritType>
void OptionTemplate<InheritType>::SetWindow(Window* pManager, Box* pParent, bool bInit)
{
	__super::SetWindow(pManager, pParent, bInit);
	if( bInit && !m_sGroupName.empty() ) {
		if (m_pWindow) m_pWindow->AddOptionGroup(m_sGroupName, this);
	}
}

template<typename InheritType>
void OptionTemplate<InheritType>::SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue)
{
	if( pstrName == _T("group") ) SetGroup(pstrValue);
	else __super::SetAttribute(pstrName, pstrValue);
}

template<typename InheritType>
void OptionTemplate<InheritType>::SetGroup(const std::wstring& pStrGroupName)
{
	if( pStrGroupName.empty() ) {
		if( m_sGroupName.empty() ) return;
		m_sGroupName.clear();
	}
	else {
		if( m_sGroupName == pStrGroupName ) return;
		if (!m_sGroupName.empty() && m_pWindow) m_pWindow->RemoveOptionGroup(m_sGroupName, this);
		m_sGroupName = pStrGroupName;
	}

	if( !m_sGroupName.empty() ) {
		if (m_pWindow) m_pWindow->AddOptionGroup(m_sGroupName, this);
	}
	else {
		if (m_pWindow) m_pWindow->RemoveOptionGroup(m_sGroupName, this);
	}

	Selected(m_bSelected, true);
}

template<typename InheritType>
std::wstring OptionTemplate<InheritType>::GetGroup() const
{
	return m_sGroupName;
}

template<typename InheritType>
void OptionTemplate<InheritType>::Selected(bool bSelected, bool bTriggerEvent)
{
	//if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;

	if (m_pWindow != NULL) {
		if (m_bSelected) {
			if (!m_sGroupName.empty()) {
				std::vector<Control*>* aOptionGroup = m_pWindow->GetOptionGroup(m_sGroupName);
				assert(aOptionGroup);
				if (aOptionGroup) {
					for (auto it = aOptionGroup->begin(); it != aOptionGroup->end(); it++) {
						auto pControl = static_cast<OptionTemplate<InheritType>*>(*it);
						if (pControl != this) {
							pControl->Selected(false, bTriggerEvent);
						}
					}
				}
			}

			if (bTriggerEvent)
			{
				m_pWindow->SendNotify(this, kEventSelect);
			}
		}
		else {
			m_pWindow->SendNotify(this, kEventUnSelect);
		}

	}

	Invalidate();
}

template<typename InheritType>
void OptionTemplate<InheritType>::Activate()
{
	ButtonTemplate<InheritType>::Activate();
	if( !IsActivatable() ) return;
	Selected(true, true);
}





#include "stdafx.h"
#include "ui_menu.h"

namespace ui {

/////////////////////////////////////////////////////////////////////////////////////
//

Control* CMenuWnd::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == kMenuElementUIInterfaceName)
	{
		return new CMenuElementUI();
	}
	return NULL;
}

CMenuWnd::CMenuWnd(HWND hParent):
m_hParent(hParent),
m_xml(_T(""))
{}

void CMenuWnd::Init(STRINGorID xml, LPCTSTR pSkinType, POINT point, PopupPosType popupPosType)
{
	m_BasedPoint = point;
	m_popupPosType = popupPosType;

	if (pSkinType != NULL)
		m_sType = pSkinType;

	m_xml = xml;
	Create(m_hParent, L"²Ëµ¥", WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, true, UiRect());
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

std::wstring CMenuWnd::GetWindowClassName() const
{
    return _T("MenuWnd");
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_KILLFOCUS )
	{
		Close();
		return 0;
	}
	else if( uMsg == WM_KEYDOWN)
	{
		if( wParam == VK_ESCAPE)
		{
			Close();
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMenuWnd::Show()
{
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	UiRect rcWork = oMonitor.rcWork;
	UiRect monitor_rect = oMonitor.rcMonitor;
	ui::CSize szInit = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szInit = GetRoot()->EstimateSize(szInit);
	szInit.cx -= GetShadowLength().left + GetShadowLength().right;
	szInit.cy -= GetShadowLength().top + GetShadowLength().bottom;
	if (m_popupPosType == RIGHT_BOTTOM)
	{
		if (m_BasedPoint.y + szInit.cy > monitor_rect.bottom)
		{
			m_BasedPoint.y -= szInit.cy;
		}
	}
	else if (m_popupPosType == RIGHT_TOP)
	{
		if (m_BasedPoint.y - szInit.cy >= monitor_rect.top)
		{
			m_BasedPoint.y -= szInit.cy;
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	UiRect rc;
	rc.left = m_BasedPoint.x;
	rc.top = m_BasedPoint.y;
	if (rc.top < monitor_rect.top)
	{
		rc.top = monitor_rect.top;
	}

	//ÅÐ¶ÏÊÇ·ñ³¬³öÆÁÄ»
	if (rc.left > monitor_rect.right - szInit.cx)
	{
		rc.left = monitor_rect.right - szInit.cx;
	}
	if (rc.left < monitor_rect.left)
	{
		rc.left = monitor_rect.left;
	}
	rc.right = rc.left + szInit.cx;
	rc.bottom = rc.top + szInit.cy;

	SetPos(rc, SWP_SHOWWINDOW, HWND_TOPMOST, false);
	SetForegroundWindow(m_hWnd);
}

// MenuElementUI
const TCHAR* const kMenuElementUIInterfaceName = _T("MenuElement");

CMenuElementUI::CMenuElementUI()
{
	m_bMouseChildEnabled = false;
}

CMenuElementUI::~CMenuElementUI()
{}

bool CMenuElementUI::ButtonUp(EventArgs& msg)
{
	std::weak_ptr<nbase::WeakFlag> weakFlag = m_pWindow->GetWeakFlag();
	bool ret = __super::ButtonUp(msg);
	if (ret && !weakFlag.expired()) {
		m_pWindow->Close();
	}
	
	return ret;
}

} // namespace ui

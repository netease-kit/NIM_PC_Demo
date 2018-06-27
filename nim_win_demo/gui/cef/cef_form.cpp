#include "cef_form.h"
#include "cef_tip.h"
using namespace ui;
using namespace std;

const LPCTSTR CefForm::kClassName = L"CefForm";

CefForm::CefForm()
{
	btn_max_restore_ = nullptr;
	cef_tip_ = nullptr;
}

std::wstring CefForm::GetSkinFolder()
{
	return L"ceftest";
}

std::wstring CefForm::GetSkinFile()
{
	return L"cef_test.xml";
}

std::wstring CefForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring CefForm::GetWindowId() const
{
	return kClassName;
}

UINT CefForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}
void CefForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	cef_tip_ = new CefTip;
	ui::GlobalManager::FillBoxWithCache(cef_tip_, L"ceftest/cef_test_tip.xml", &CefTip::CreateControl);
	auto cef_box = dynamic_cast<ui::Box*>(FindControl(L"cef_box"));
	if (cef_box != nullptr)
		cef_box->Add(cef_tip_);
	btn_max_restore_ = static_cast<Button*>(FindControl(L"btn_max_restore"));
}

LRESULT CefForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
			OnWndSizeMax(false);
		else if (wParam == SIZE_MAXIMIZED)
			OnWndSizeMax(true);
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CefForm::OnWndSizeMax(bool max)
{
	if (btn_max_restore_)
		btn_max_restore_->SetClass(max ? L"btn_wnd_restore" : L"btn_wnd_max");
}

bool CefForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"btn_max_restore")
	{
		DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
		if (style & WS_MAXIMIZE)
			::ShowWindow(m_hWnd, SW_RESTORE);
		else
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	return false;
}

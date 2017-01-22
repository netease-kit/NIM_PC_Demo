#include "cef_native_form.h"

using namespace ui;
using namespace std;

const LPCTSTR CefNativeForm::kClassName = L"CefNativeForm";

CefNativeForm::CefNativeForm()
{
	cef_control_ = nullptr;
	btn_max_restore_ = nullptr;
}

std::wstring CefNativeForm::GetSkinFolder()
{
	return L"ceftest";
}

std::wstring CefNativeForm::GetSkinFile()
{
	return L"cef_native_test.xml";
}

std::wstring CefNativeForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring CefNativeForm::GetWindowId() const
{
	return kClassName;
}

UINT CefNativeForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

ui::Control* CefNativeForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefNativeControl")
	{
		return new CefNativeControl;
	}

	return NULL;
}

void CefNativeForm::InitWindow()
{
	SetTaskbarTitle(L"Cef有窗模式测试");
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefNativeForm::OnClicked, this, std::placeholders::_1));
	btn_max_restore_ = static_cast<Button*>(FindControl(L"btn_max_restore"));

	edit_url_ = static_cast<RichEdit*>(FindControl(L"edit_url"));
	edit_js_ = static_cast<RichEdit*>(FindControl(L"edit_js"));
	lbl_title_ = static_cast<Label*>(FindControl(L"title"));
	cef_control_ = static_cast<CefNativeControl*>(FindControl(L"cef_native_control"));
	edit_url_->AttachReturn(nbase::Bind(&CefNativeForm::OnReturn, this, std::placeholders::_1));

	cef_control_->AttachTitleChange(nbase::Bind(&CefNativeForm::OnTitleChange, this, std::placeholders::_1));
	cef_control_->AttachUrlChange(nbase::Bind(&CefNativeForm::OnUrlChange, this, std::placeholders::_1));
	cef_control_->AttachLinkClick(nbase::Bind(&CefNativeForm::OnLinkClick, this, std::placeholders::_1));
	cef_control_->AttachBeforeNavigate(nbase::Bind(&CefNativeForm::OnBeforeNavigate, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachLoadingStateChange(nbase::Bind(&CefNativeForm::OnLoadingStateChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachLoadStart(nbase::Bind(&CefNativeForm::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefNativeForm::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachLoadError(nbase::Bind(&CefNativeForm::OnLoadError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachJsCallback(nbase::Bind(&CefNativeForm::OnJsCallback, this, std::placeholders::_1, std::placeholders::_2));

	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test.html";
	cef_control_->LoadURL(html_path);
}

LRESULT CefNativeForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void CefNativeForm::OnWndSizeMax(bool max)
{
	if (btn_max_restore_)
		btn_max_restore_->SetClass(max ? L"btn_wnd_restore" : L"btn_wnd_max");
}

bool CefNativeForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"btn_back")
	{
		cef_control_->GoBack();
	}
	else if (name == L"btn_forward")
	{
		cef_control_->GoForward();
	}
	else if (name == L"btn_navigate")
	{
		cef_control_->LoadURL(edit_url_->GetText());
	}
	else if (name == L"btn_refresh")
	{
		cef_control_->Refresh();
	}
	else if (name == L"btn_send_js")
	{
		std::wstring text = edit_js_->GetText();
		CefString js_string = L"receiveMessage('" + text + L"');";
		cef_control_->ExecJavaScript(js_string);
	}
	else if(name == L"btn_max_restore")
	{
		DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
		if (style & WS_MAXIMIZE)
			::ShowWindow(m_hWnd, SW_RESTORE);
		else
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	return false;
}

bool CefNativeForm::OnReturn(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"edit_url")
	{
		cef_control_->LoadURL(edit_url_->GetText());
	}

	return false;
}

void CefNativeForm::OnTitleChange(const std::wstring& title)
{
	lbl_title_->SetText(title);
}

void CefNativeForm::OnUrlChange(const std::wstring& url)
{
	edit_url_->SetText(url);
}

bool CefNativeForm::OnLinkClick(const std::wstring& url)
{
	return true;
}

CefRequestHandler::ReturnValue CefNativeForm::OnBeforeNavigate(CefRefPtr<CefRequest> request, bool is_redirect)
{
	// 返回RV_CANCEL截断导航
	return RV_CONTINUE;
}

void CefNativeForm::OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward)
{
	return;
}

void CefNativeForm::OnLoadStart()
{
	return;
}

void CefNativeForm::OnLoadEnd(int httpStatusCode)
{
	return;
}

void CefNativeForm::OnLoadError(CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	return;
}

void CefNativeForm::OnJsCallback(const CefString& fun_name, const CefString& param)
{
	if (fun_name == L"NimCefWebFunction")
	{
		std::wstring content = std::wstring(L"收到来自JS的消息：");
		if (!param.empty())
			content += param.c_str();

		ShowMsgBox(GetHWND(), content, MsgboxCallback(), L"提示", L"确定", L"");
	}
}

#include "cef_form.h"

using namespace ui;
using namespace std;

const LPCTSTR CefForm::kClassName = L"CefForm";

CefForm::CefForm()
{
	cef_control_ = nullptr;
	btn_max_restore_ = nullptr;
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

ui::Control* CefForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new CefControl;
	}

	return NULL;
}

void CefForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefForm::OnClicked, this, std::placeholders::_1));
	btn_max_restore_ = static_cast<Button*>(FindControl(L"btn_max_restore"));

	edit_url_ = static_cast<RichEdit*>(FindControl(L"edit_url"));
	edit_js_ = static_cast<RichEdit*>(FindControl(L"edit_js"));
	lbl_title_ = static_cast<Label*>(FindControl(L"title"));
	cef_control_ = static_cast<CefControl*>(FindControl(L"cef_control"));
	edit_url_->AttachReturn(nbase::Bind(&CefForm::OnReturn, this, std::placeholders::_1));

	cef_control_->AttachTitleChange(nbase::Bind(&CefForm::OnTitleChange, this, std::placeholders::_1));
	cef_control_->AttachUrlChange(nbase::Bind(&CefForm::OnUrlChange, this, std::placeholders::_1));
	cef_control_->AttachLinkClick(nbase::Bind(&CefForm::OnLinkClick, this, std::placeholders::_1));
	cef_control_->AttachBeforeNavigate(nbase::Bind(&CefForm::OnBeforeNavigate, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachLoadingStateChange(nbase::Bind(&CefForm::OnLoadingStateChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachLoadStart(nbase::Bind(&CefForm::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefForm::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachLoadError(nbase::Bind(&CefForm::OnLoadError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachJsCallback(nbase::Bind(&CefForm::OnJsCallback, this, std::placeholders::_1, std::placeholders::_2));

	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test.html";
	cef_control_->LoadURL(html_path);
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

bool CefForm::OnReturn(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"edit_url")
	{
		cef_control_->LoadURL(edit_url_->GetText());
	}

	return false;
}

void CefForm::OnTitleChange(const std::wstring& title)
{
	lbl_title_->SetText(title);
}

void CefForm::OnUrlChange(const std::wstring& url)
{
	edit_url_->SetText(url);
}

bool CefForm::OnLinkClick(const std::wstring& url)
{
	return true;
}

CefRequestHandler::ReturnValue CefForm::OnBeforeNavigate(CefRefPtr<CefRequest> request, bool is_redirect)
{
	// ·µ»ØRV_CANCEL½Ø¶Ïµ¼º½
	return RV_CONTINUE;
}

void CefForm::OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward)
{
	return;
}

void CefForm::OnLoadStart()
{
	return;
}

void CefForm::OnLoadEnd(int httpStatusCode)
{
	return;
}

void CefForm::OnLoadError(CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	return;
}

void CefForm::OnJsCallback(const CefString& fun_name, const CefString& param)
{
	if (fun_name == L"NimCefWebFunction")
	{
		MutiLanSupport* multilan = MutiLanSupport::GetInstance();
		std::wstring content = nbase::StringPrintf(multilan->GetStringViaID(L"STRID_CEF_BROWSER_RECEIVE_JS_MSG").c_str(), param.c_str());		
		ShowMsgBox(GetHWND(), MsgboxCallback(), content, false);
	}
}

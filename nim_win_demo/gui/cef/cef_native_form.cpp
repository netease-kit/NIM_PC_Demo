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
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefNativeForm::OnClicked, this, std::placeholders::_1));
	btn_max_restore_ = static_cast<Button*>(FindControl(L"btn_max_restore"));

	edit_url_ = static_cast<RichEdit*>(FindControl(L"edit_url"));
	edit_js_ = static_cast<RichEdit*>(FindControl(L"edit_js"));
	lbl_title_ = static_cast<Label*>(FindControl(L"title"));
	cef_control_ = static_cast<CefNativeControl*>(FindControl(L"cef_native_control"));
	edit_url_->AttachReturn(nbase::Bind(&CefNativeForm::OnReturn, this, std::placeholders::_1));

	cef_control_->AttachBeforeContextMenu(nbase::Bind(&CefNativeForm::OnBeforeMenu, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachMenuCommand(nbase::Bind(&CefNativeForm::OnMenuCommand, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachTitleChange(nbase::Bind(&CefNativeForm::OnTitleChange, this, std::placeholders::_1));
	cef_control_->AttachUrlChange(nbase::Bind(&CefNativeForm::OnUrlChange, this, std::placeholders::_1));
	cef_control_->AttachLinkClick(nbase::Bind(&CefNativeForm::OnLinkClick, this, std::placeholders::_1));
	cef_control_->AttachBeforeNavigate(nbase::Bind(&CefNativeForm::OnBeforeNavigate, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachLoadingStateChange(nbase::Bind(&CefNativeForm::OnLoadingStateChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachLoadStart(nbase::Bind(&CefNativeForm::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefNativeForm::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachLoadError(nbase::Bind(&CefNativeForm::OnLoadError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test.html";
	if(ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test_en.html";
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
		std::string text = edit_js_->GetUTF8Text();
		Json::Value values;
		values["message"] = text;
		cef_control_->CallJSFunction(L"showJsMessage", nbase::UTF8ToUTF16(values.toStyledString()), ToWeakCallback([this](const std::string& json_result) {
			Json::Value values;
			Json::Reader reader;
			if (reader.parse(json_result, values))
			{
				std::wstring message = nbase::UTF8ToUTF16(values["message"].asString());
				ShowMsgBox(nullptr, MsgboxCallback(), message.c_str(), false);
			}
		}), L"");
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

void CefNativeForm::OnBeforeMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{

}

bool CefNativeForm::OnMenuCommand(CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
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
	// ·µ»ØRV_CANCEL½Ø¶Ïµ¼º½
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

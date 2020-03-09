#include "stdafx.h"
#include "cef_tip.h"
#include "module\cef\cef_global_methods.h"
using namespace ui;
using namespace std;


CefTip::CefTip()
{
	cef_control_ = nullptr;
}
ui::Control* CefTip::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == L"CefControl")
	{
		return new CefControl;
	}

	return NULL;
}
void CefTip::DoInit()
{
	AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CefTip::OnClicked, this, std::placeholders::_1));

	edit_url_ = static_cast<RichEdit*>(FindSubControl(L"edit_url"));
	edit_js_ = static_cast<RichEdit*>(FindSubControl(L"edit_js"));
	lbl_title_ = static_cast<Label*>(FindSubControl(L"title"));
	cef_control_ = static_cast<CefControl*>(FindSubControl(L"cef_control"));
	cef_control_dev_ = dynamic_cast<CefControl*>(FindSubControl(L"cef_control_dev"));
	edit_url_->AttachReturn(nbase::Bind(&CefTip::OnReturn, this, std::placeholders::_1));

	cef_control_->AttachBeforeContextMenu(nbase::Bind(&CefTip::OnBeforeMenu, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachMenuCommand(nbase::Bind(&CefTip::OnMenuCommand, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachTitleChange(nbase::Bind(&CefTip::OnTitleChange, this, std::placeholders::_1));
	cef_control_->AttachUrlChange(nbase::Bind(&CefTip::OnUrlChange, this, std::placeholders::_1));
	cef_control_->AttachLinkClick(nbase::Bind(&CefTip::OnLinkClick, this, std::placeholders::_1));
	cef_control_->AttachBeforeNavigate(nbase::Bind(&CefTip::OnBeforeNavigate, this, std::placeholders::_1, std::placeholders::_2));
	cef_control_->AttachLoadingStateChange(nbase::Bind(&CefTip::OnLoadingStateChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachLoadStart(nbase::Bind(&CefTip::OnLoadStart, this));
	cef_control_->AttachLoadEnd(nbase::Bind(&CefTip::OnLoadEnd, this, std::placeholders::_1));
	cef_control_->AttachLoadError(nbase::Bind(&CefTip::OnLoadError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cef_control_->AttachDevToolAttachedStateChange(nbase::Bind(&CefTip::OnDevToolVisibleChange, this, std::placeholders::_1));
	cef_control_->AttachAfterCreated(nbase::Bind(&CefTip::OnAfterCreated, this, std::placeholders::_1));
	cef_control_->AttachBeforeCLose(nbase::Bind(&CefTip::OnBeforeClose, this, std::placeholders::_1));
	cef_control_->AttachBeforeContextMenu(nbase::Bind(&CefTip::OnBeforeContextMenu, this, std::placeholders::_1, std::placeholders::_2));
	std::wstring html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test.html";
	if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		html_path = L"file://" + QPath::GetAppPath() + L"cef/html/cef_test_en.html";
	cef_control_->LoadURL(html_path);
}
bool CefTip::OnClicked(ui::EventArgs* arg)
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
				ShowMsgBox(GetWindow()->GetHWND(), MsgboxCallback(), message.c_str(), false);
			}
		}));
	}
	else if (name == L"btn_dev")
	{
		ShowDevView();
	}
	return false;
}

bool CefTip::OnReturn(ui::EventArgs* arg)
{
	std::wstring name = arg->pSender->GetName();
	if (name == L"edit_url")
	{
		cef_control_->LoadURL(edit_url_->GetText());
	}

	return false;
}

void CefTip::ShowMessage(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(params.c_str(), values))
	{
		if (values.isMember("message"))
		{
			std::wstring message = nbase::UTF8ToUTF16(values["message"].asString());
			MutiLanSupport* multilan = MutiLanSupport::GetInstance();
			std::wstring content = nbase::StringPrintf(multilan->GetStringViaID(L"STRID_CEF_BROWSER_RECEIVE_JS_MSG").c_str(), message.c_str());
			ShowMsgBox(GetWindow()->GetHWND(), MsgboxCallback(), content, false);

			if (callback)
				callback(false, R"({"message": "Call C++ function successfully."})");
		}
	}
}

void CefTip::OnBeforeMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{

}

bool CefTip::OnMenuCommand(CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
	return false;
}

void CefTip::OnTitleChange(const std::wstring& title)
{
	lbl_title_->SetText(title);
}

void CefTip::OnUrlChange(const std::wstring& url)
{
	edit_url_->SetText(url);
}

bool CefTip::OnLinkClick(const std::wstring& url)
{
	return true;
}

CefRequestHandler::ReturnValue CefTip::OnBeforeNavigate(CefRefPtr<CefRequest> request, bool is_redirect)
{
	// ·µ»ØRV_CANCEL½Ø¶Ïµ¼º½
	return RV_CONTINUE;
}

void CefTip::OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward)
{
	return;
}

void CefTip::OnLoadStart()
{
	return;
}

void CefTip::OnLoadEnd(int httpStatusCode)
{
	return;
}

void CefTip::OnLoadError(CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	return;
}

void CefTip::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	cef_control_->RegisterCppFunc(L"showCppMessage", nbase::Bind(&CefTip::ShowMessage, this, std::placeholders::_1, std::placeholders::_2));

	if (!global_functions_.get())
	{
		global_functions_.reset(new nim_comp::CefGlobalFunctions(cef_control_));
		global_functions_->Init();
	}
}

void CefTip::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	
}

void CefTip::OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

void CefTip::ShowDevView()
{
	if (cef_control_->IsAttachedDevTools())
		cef_control_->DettachDevTools();
	else
		cef_control_->AttachDevTools(cef_control_dev_);
}
void CefTip::OnDevToolVisibleChange(bool visible)
{
	cef_control_dev_->SetVisible(visible);
}
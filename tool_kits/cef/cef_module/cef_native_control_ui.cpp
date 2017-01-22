#include "cef_native_control.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "browser_handler.h"
#include "cef_manager.h"

namespace ui
{

CefNativeControl::CefNativeControl(void)
{
#if !defined(SUPPORT_CEF)
	ASSERT(FALSE && L"要使用Cef功能请开启SUPPORT_CEF宏");
#endif
}

CefNativeControl::~CefNativeControl(void)
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		// Request that the main browser close.
		browser_handler_->GetBrowserHost()->CloseBrowser(true);
		browser_handler_->SetHostWindow(NULL);
		browser_handler_->SetHandlerDelegate(NULL);
	}
}

void CefNativeControl::Init()
{
	if (browser_handler_.get() == nullptr)
	{
		LONG style = GetWindowLong(m_pWindow->GetHWND(), GWL_STYLE);
		SetWindowLong(m_pWindow->GetHWND(), GWL_STYLE, style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		ASSERT((::GetWindowLong(m_pWindow->GetHWND(), GWL_EXSTYLE) & WS_EX_LAYERED) == 0 && L"无法在分层窗口内使用本控件");

		browser_handler_ = new nim_cef::BrowserHandler;
		browser_handler_->SetHostWindow(m_pWindow->GetHWND());
		browser_handler_->SetHandlerDelegate(this);

		// 使用有窗模式
		CefWindowInfo window_info;
		window_info.SetAsChild(this->m_pWindow->GetHWND(), m_rcItem);

		CefBrowserSettings browser_settings;
		CefBrowserHost::CreateBrowser(window_info, browser_handler_, L"", browser_settings, NULL);
	}

	__super::Init();
}

void CefNativeControl::SetPos(UiRect rc)
{
	__super::SetPos(rc);

	HWND hwnd = GetCefHandle();
	if (hwnd) 
	{
		SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_NOZORDER);
	}
}

void CefNativeControl::HandleMessage(EventArgs& event)
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get() == NULL)
		return __super::HandleMessage(event);

	else if (event.Type == kEventInternalSetFocus)
	{
		browser_handler_->GetBrowserHost()->SetFocus(true);
	}
	else if (event.Type == kEventInternalKillFocus)
	{
		browser_handler_->GetBrowserHost()->SetFocus(false);
	}

	__super::HandleMessage(event);
}

void CefNativeControl::SetVisible(bool bVisible /*= true*/)
{
	__super::SetVisible(bVisible);
	
	HWND hwnd = GetCefHandle();
	if (hwnd)
	{
		if (bVisible)
		{
			ShowWindow(hwnd, SW_SHOW);
		}
		else
		{
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}
}

void CefNativeControl::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);

	HWND hwnd = GetCefHandle();
	if (hwnd)
	{
		if (bVisible)
		{
			ShowWindow(hwnd, SW_SHOW);
		}
		else
		{
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}
}

void CefNativeControl::Paint(HDC hDC, const UiRect& rcPaint)
{
//	return;
	__super::Paint(hDC, rcPaint);
}

HWND CefNativeControl::GetCefHandle() const 
{
	if (browser_handler_.get() && browser_handler_->GetBrowserHost().get())
		return browser_handler_->GetBrowserHost()->GetWindowHandle();

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
void CefNativeControl::LoadURL(const CefString& url)
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		CefRefPtr<CefFrame> frame = browser_handler_->GetBrowser()->GetMainFrame();
		if (!frame)
			return;

		frame->LoadURL(url);
	}
	else
	{
		if (browser_handler_.get())
		{
			StdClosure cb = ToWeakCallback([this, url]()
			{
				LoadURL(url);
			});
			browser_handler_->AddAfterCreateTask(cb);
		}
	}
}

void CefNativeControl::LoadString(const CefString& stringW, const CefString& url)
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		CefRefPtr<CefFrame> frame = browser_handler_->GetBrowser()->GetMainFrame();
		if (!frame)
			return;

		frame->LoadStringW(stringW, url);
	}
	else
	{
		if (browser_handler_.get())
		{
			StdClosure cb = ToWeakCallback([this, stringW, url]()
			{
				LoadString(stringW, url);
			});
			browser_handler_->AddAfterCreateTask(cb);
		}
	}
}

void CefNativeControl::GoBack()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->GoBack();
	}
}

void CefNativeControl::GoForward()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->GoForward();
	}
}

bool CefNativeControl::CanGoBack()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->CanGoBack();
	}
	return false;
}

bool CefNativeControl::CanGoForward()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->CanGoForward();
	}
	return false;
}

void CefNativeControl::Refresh()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->Reload();
	}
}

void CefNativeControl::StopLoad()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->StopLoad();
	}
}

bool CefNativeControl::IsLoading()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->IsLoading();
	}
	return false;
}

void CefNativeControl::ExecJavaScript(const CefString& js)
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		CefRefPtr<CefFrame> frame = browser_handler_->GetBrowser()->GetMainFrame();
		if (frame)
			frame->ExecuteJavaScript(js, L"", 0);
	}
}

CefString CefNativeControl::GetURL()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return browser_handler_->GetBrowser()->GetMainFrame()->GetURL();
	}

	return CefString();
}

std::string CefNativeControl::GetUTF8URL()
{
	if (browser_handler_.get() && browser_handler_->GetBrowser().get())
	{
		return nbase::UTF16ToUTF8(std::wstring(GetURL().c_str()));
	}

	return CefString();
}

CefString CefNativeControl::GetMainURL(const CefString& url)
{
	std::string temp = url.ToString();
	int end_pos = temp.find("#") == std::string::npos ? temp.length() : temp.find("#");
	temp = temp.substr(0, end_pos);
	return CefString(temp.c_str());
}

}
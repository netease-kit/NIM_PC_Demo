#include "cef_native_control.h"
#include "util/util.h"

namespace ui 
{

void CefNativeControl::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const std::string* buffer, int width, int height)
{
	return;
}

void CefNativeControl::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
	return;
}

void CefNativeControl::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
	return;
}

void CefNativeControl::UpdateWindowPos()
{
	this->SetPos(this->GetPos());
}
void CefNativeControl::UpdateUI()
{
	nbase::ThreadManager::PostTask(ThreadId::kThreadUI, ToWeakCallback([this](){Invalidate(); }));
}
void CefNativeControl::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	if (cb_before_menu_)
		cb_before_menu_(params, model);
}

bool CefNativeControl::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
	if (cb_menu_command_)
		return cb_menu_command_(params, command_id, event_flags);

	return false;
}

void CefNativeControl::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
	if (cb_url_change_)
		cb_url_change_(url);

	if (frame->IsMain())
	{
		auto old_url = url_;
		url_ = frame->GetURL();
		if (cb_main_url_change_ != nullptr && GetMainURL(old_url).compare(GetMainURL(url_)) != 0)
		{
			cb_main_url_change_(old_url, url_);
		}
	}	
}

void CefNativeControl::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	if (cb_title_change_)
		cb_title_change_(title);
}

void CefNativeControl::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
	if (cb_loadstate_change_)
		cb_loadstate_change_(isLoading, canGoBack, canGoForward);
}

void CefNativeControl::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	if (cb_load_start_)
		cb_load_start_();
}

void CefNativeControl::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	if (cb_load_end_)
		cb_load_end_(httpStatusCode);
}

void CefNativeControl::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
	if (cb_load_error_)
		cb_load_error_(errorCode, errorText, failedUrl);
}

bool CefNativeControl::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access)
{
	if (cb_link_click_ && !target_url.empty())
		return cb_link_click_(target_url);

	return true;
}

bool CefNativeControl::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	if (cb_after_created_)
		cb_after_created_(browser);

	return false;
}

void CefNativeControl::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{

}

bool CefNativeControl::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	if (cb_before_browser_)
		return cb_before_browser_(browser, frame, request, is_redirect);
}

void CefNativeControl::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution)
{
	if (cb_protocol_execution_)
		cb_protocol_execution_(browser, url, allow_os_execution);
}

CefRequestHandler::ReturnValue CefNativeControl::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
	if (cb_before_navigate_)
		return cb_before_navigate_(request, false);

	return RV_CONTINUE;
}

void CefNativeControl::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, CefRequestHandler::TerminationStatus status)
{
	return;
}

bool CefNativeControl::OnExecuteCppFunc(const CefString& function_name, const CefString& params, int js_callback_id, CefRefPtr<CefBrowser> browser)
{
	if (js_bridge_.get())
	{
		js_callback_thread_id_ = nbase::FrameworkThread::GetManagedThreadId();
		return js_bridge_->ExecuteCppFunc(function_name, params, js_callback_id, browser);
	}

	return false;
}

bool CefNativeControl::OnExecuteCppCallbackFunc(int cpp_callback_id, const CefString& json_string)
{
	if (js_bridge_.get())
	{
		if (js_callback_thread_id_ != -1)
		{
			nbase::ThreadManager::PostTask(js_callback_thread_id_, [this, cpp_callback_id, json_string]
			{
				js_bridge_->ExecuteCppCallbackFunc(cpp_callback_id, json_string);
			});
		}
		else
		{
			return js_bridge_->ExecuteCppCallbackFunc(cpp_callback_id, json_string);
		}

	}

	return false;
}

}
#include "cef_control.h"
#include "util.h"

namespace ui
{

	void CefControl::AttachJsCallback(const OnJsCallbackEvent& callback)
	{
		// 保存处理这个事件的对象所在的线程
		js_callback_thread_id_ = nbase::FrameworkThread::GetManagedThreadId();
		cb_js_callback_ = callback;
	}

	void CefControl::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const std::string* buffer, int width, int height)
	{
		if (NULL == buffer)
			return;

		if (type == PET_VIEW)
		{
			if (dc_cef_.GetWidth() != width || dc_cef_.GetHeight() != height)
				dc_cef_.Init(m_pWindow->GetPaintDC(), width, height);

			LPBYTE pDst = (LPBYTE)dc_cef_.GetBits();
			if (pDst)
				memcpy(pDst, (char*)buffer->c_str(), height * width * 4);
		}
		else if (type == PET_POPUP && dc_cef_.IsValid() && rect_popup_.width > 0 && rect_popup_.height > 0)
		{
			// 单独保存popup窗口的位图
			if (dc_cef_popup_.GetWidth() != width || dc_cef_popup_.GetHeight() != height)
				dc_cef_popup_.Init(m_pWindow->GetPaintDC(), width, height);

			LPBYTE pDst = (LPBYTE)dc_cef_popup_.GetBits();
			if (pDst)
				memcpy(pDst, (char*)buffer->c_str(), width * height * 4);
		}

		this->Invalidate();
	}

	void CefControl::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
	{
		if (!show)
		{
			// 当popup窗口隐藏时，刷新popup区域
			CefRect rect_dirty = rect_popup_;
			rect_popup_.Set(0, 0, 0, 0);
			browser->GetHost()->Invalidate(PET_VIEW);
		}
	}

	void CefControl::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
	{
		if (rect.width <= 0 || rect.height <= 0)
			return;

		rect_popup_ = rect;
	}

	void CefControl::UpdateWindowPos()
	{
		this->SetPos(this->GetPos());
	}

	void CefControl::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
	{
		if (cb_before_menu_)
			cb_before_menu_(params, model);
	}

	bool CefControl::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
	{
		if (cb_menu_command_)
			return cb_menu_command_(params, command_id, event_flags);

		return false;
	}

	void CefControl::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
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

	void CefControl::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		if (cb_title_change_)
			cb_title_change_(title);
	}

	void CefControl::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
	{
		if (cb_loadstate_change_)
			cb_loadstate_change_(isLoading, canGoBack, canGoForward);
	}

	void CefControl::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
	{
		if (cb_load_start_)
			cb_load_start_();
	}

	void CefControl::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
	{
		if (cb_load_end_)
			cb_load_end_(httpStatusCode);
	}

	void CefControl::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
	{
		if (cb_load_error_)
			cb_load_error_(errorCode, errorText, failedUrl);
	}

	bool CefControl::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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

	bool CefControl::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
	{
		return false;
	}

	CefRequestHandler::ReturnValue CefControl::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
	{
		if (cb_before_navigate_)
			return cb_before_navigate_(request, false);

		return RV_CONTINUE;
	}

	void CefControl::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, CefRequestHandler::TerminationStatus status)
	{
		return;
	}

	void CefControl::OnJsCallback(const CefString& fun_name, const CefString& param)
	{
		// 投递到注册JsCallbak的线程去处理
		if (cb_js_callback_)
		{
			if (js_callback_thread_id_ != -1)
			{
				nbase::ThreadManager::PostTask(js_callback_thread_id_, [this, fun_name, param]
				{
					cb_js_callback_(fun_name, param);
				});
			}
			else
				cb_js_callback_(fun_name, param);
		}
	}

}
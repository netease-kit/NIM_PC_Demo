/** @file cef_control_event.h
* @brief Cef控件事件处理接口
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @author Redrain
* @date 2016/7/22
*/
#pragma once
#include "include/cef_load_handler.h"
#include "include/cef_request_handler.h"

namespace ui 
{
	typedef std::function<void(const std::wstring& title)> OnTitleChangeEvent;
	typedef std::function<void(const std::wstring& url)> OnUrlChangeEvent;

	typedef std::function<bool(const std::wstring& url)> OnLinkClickEvent;
	typedef std::function<CefRequestHandler::ReturnValue(CefRefPtr<CefRequest> request, bool is_redirect)> OnBeforeBrowseEvent;
	typedef std::function<void(const CefString& old_url, const CefString& new_url)> OnMainURLChengeEvent;

	typedef std::function<void(bool isLoading, bool canGoBack, bool canGoForward)> OnLoadingStateChangeEvent;
	typedef std::function<void()> OnLoadStartEvent;
	typedef std::function<void(int httpStatusCode)> OnLoadEndEvent;
	typedef std::function<void(CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)> OnLoadErrorEvent;

	typedef std::function<void(const CefString& fun_name, const CefString& param)> OnJsCallbackEvent;
}
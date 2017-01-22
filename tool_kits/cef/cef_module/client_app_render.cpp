#include "client_app.h" 
#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "util.h"

#include "js_handler.h"
#include "ipc_string_define.h"

namespace nim_cef
{
//////////////////////////////////////////////////////////////////////////////////////////
// CefRenderProcessHandler methods.
void ClientApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) 
{
}

void ClientApp::OnWebKitInitialized() 
{
 	// js扩展代码，这里定义一个NimCefWebFunction函数来让web端触发native的CefV8Handler处理代码
 	std::string extensionCode =
 	 	"(function() {"
 	 	"  NimCefWebFunction = function(param) {"
 	 	"    native function NimCefWebFunction(param);"
 	 	"    return NimCefWebFunction(param);"
 	 	"    };"
 	 	"})();";
 	 
 	CefRefPtr<CefV8Handler> handler = new CefJSHandler();
 	CefRegisterExtension("v8/extern", extensionCode, handler);
}

void ClientApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
}

void ClientApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) 
{
}

CefRefPtr<CefLoadHandler> ClientApp::GetLoadHandler()
{
	return NULL;
}

bool ClientApp::OnBeforeNavigation(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	NavigationType navigation_type,
	bool is_redirect) 
{
	return false;
}

void ClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) 
{
}

void ClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,	CefRefPtr<CefV8Context> context) 
{
}

void ClientApp::OnUncaughtException
(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Exception> exception,
	CefRefPtr<CefV8StackTrace> stackTrace) 
{
}

void ClientApp::OnFocusedNodeChanged(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefDOMNode> node) 
{
	bool is_editable = (node.get() && node->IsEditable());
	if (is_editable != last_node_is_editable_)
	{
		// Notify the browser of the change in focused element type.
		last_node_is_editable_ = is_editable;
		CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(kFocusedNodeChangedMessage);

		message->GetArgumentList()->SetBool(0, is_editable);
		browser->SendProcessMessage(PID_BROWSER, message);
	}
}

bool ClientApp::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message) 
{
	ASSERT(source_process == PID_BROWSER);

	return false;
}
}
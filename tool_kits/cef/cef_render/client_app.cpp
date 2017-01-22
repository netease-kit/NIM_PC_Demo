#include "client_app.h" 
#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "util.h"

namespace nim_cef
{

ClientApp::ClientApp() 
{
	last_node_is_editable_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// CefApp methods.
void ClientApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void ClientApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
	// Default schemes that support cookies.
	cookieable_schemes_.push_back("http");
	cookieable_schemes_.push_back("https");
}

}
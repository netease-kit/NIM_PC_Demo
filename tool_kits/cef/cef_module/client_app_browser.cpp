#include "client_app.h" 

#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "util.h"

namespace nim_cef
{
//////////////////////////////////////////////////////////////////////////////////////////
// CefBrowserProcessHandler methods.
void ClientApp::OnContextInitialized() 
{
	// Register cookieable schemes with the global cookie manager.
	CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
	ASSERT(manager.get());
	manager->SetSupportedSchemes(cookieable_schemes_, NULL);

	// 这里可以删除了保存的Cooies信息
	// manager->DeleteCookies(L"", L"", nullptr);
}

void ClientApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
#if defined(SUPPORT_CEF_FLASH)	
	if (command_line->HasSwitch("type") && command_line->GetSwitchValue("type") == "ppapi" && command_line->HasSwitch("ppapi-flash-args"))
	{
		// 如果是flash进程创建
	}
	else
	{
		// 开启SUPPORT_CEF_FLASH宏后，会使用sandbox模块，某些情况下导致渲染子进程无法正常创建，所以对于非flash进程关掉sandbox功能
		command_line->AppendSwitch("no-sandbox");
	}
#endif
}

void ClientApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) 
{
}

}
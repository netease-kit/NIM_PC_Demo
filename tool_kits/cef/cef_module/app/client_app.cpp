#include "client_app.h" 
#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "manager/cef_manager.h"
#include "util/util.h"

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
	// Pass additional command-line flags to the browser process.
	if (process_type.empty()) 
	{
		// 设置子进程路径很关键，如果不设置，可能会触发cef的一个bug
		// cef在LoadUrl建立渲染进程时，会查找子进程的路径，可能会引发一个bug导致IDE在debug状态时卡死
		// 这里指定好子进程路径就可以了

		// 但是使用sandbox的话，不允许使用另外的子进程;不使用sandbox的话，第一次加载flash插件时会弹出个命令提示行，这是cef的bug。flash与子进程二者不可兼得
#if !defined(SUPPORT_CEF_FLASH)
#ifdef _DEBUG
 		command_line->AppendSwitchWithValue("browser-subprocess-path", "render_d.exe");
#else
		command_line->AppendSwitchWithValue("browser-subprocess-path", "render.exe");
#endif
#else
		command_line->AppendSwitchWithValue("ppapi-flash-version", "20.0.0.228");
		command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash\\pepflashplayer.dll");
#endif

		//同一个域下的使用同一个渲染进程
		command_line->AppendSwitch("process-per-site");
		command_line->AppendSwitch("disable-gpu");
		command_line->AppendSwitch("disable-gpu-compositing");
		//command_line->AppendSwitchWithValue("proxy-server", "SOCKS5://127.0.0.1:1080");	

		// 开启离屏渲染
		if (CefManager::GetInstance()->IsEnableOffsetRender())
		{
			command_line->AppendSwitch("disable-surfaces");
			command_line->AppendSwitch("enable-begin-frame-scheduling");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ClientApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
	// Default schemes that support cookies.
	cookieable_schemes_.push_back("http");
	cookieable_schemes_.push_back("https");
}

}
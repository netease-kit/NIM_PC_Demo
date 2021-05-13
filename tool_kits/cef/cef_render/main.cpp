#include "include/cef_client.h"
#include "app/client_app.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	_wsetlocale(LC_ALL, L"chs");

#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(L"Command:\n%s\n\n", lpszCmdLine);
#endif

	CefMainArgs main_args(GetModuleHandle(NULL));

	CefRefPtr<nim_cef::ClientApp> app(new nim_cef::ClientApp());

	// 执行子进程逻辑，此时会堵塞直到子进程退出。
	return CefExecuteProcess(main_args, app.get(), NULL);
}

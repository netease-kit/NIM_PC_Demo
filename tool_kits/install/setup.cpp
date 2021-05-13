// setup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atlbase.h>
#include "setup.h"
#include "main/main_thread.h"
#include "main/setup_data.h"
#include "base/util/cmd_line_args.h"

static bool update_yixin = false;

std::wstring GetAppDataPath()
{
	std::wstring app_data_path = nbase::win32::GetLocalAppDataDir()
		+ CSetupData::GetProductDoc();

	if (!nbase::FilePathIsExist(app_data_path, true)) {
		nbase::CreateDirectory(app_data_path);
	}
	return app_data_path;
}
std::wstring GetLogFilePath()
{
	std::wstring path;
	nbase::FilePathCompose(GetAppDataPath(), L"nim_setup_log.txt", path);

	return path;
}
void InitAppLog()
{
	std::wstring log_file_path = GetLogFilePath();
	std::wstring log_dir_path;
	nbase::FilePathApartDirectory(log_file_path, log_dir_path);

#ifdef _DEBUG
    QLogImpl::GetInstance()->SetLogLevel(LV_PRO);
#else
    QLogImpl::GetInstance()->SetLogLevel(LV_APP);
#endif
    QLogImpl::GetInstance()->SetLogFile(log_file_path);
}

static bool Init(HINSTANCE hInst)
{
	QLOG_APP(L"app: started");
	InitAppLog();

	return true;
}
void ParseCommandLine()
{
	nbase::CmdLineArgs args;
	for (auto iter = args.begin(); iter != args.end(); iter++) {
		if (*iter[0] == L'/') {
			if (!wcsicmp(*iter + 1, L"nim-update=true")) {
				update_yixin = true;
			}
		}
	}
}
bool CheckYixinSingletonRun() //返回true表示正在运行
{
	//由于nim可以同时运行多个，通过进程名来查找
	std::vector<int> exe;;
	return CSetupData::FindAppProcessID( CSetupData::GetProcessName(), exe );
}

bool CheckSingletonRun()  //返回true表示正在运行
{
	// 只允许一个安装程序在运行
#ifdef _DEBUG
	const wchar_t kMutexName[] = L"RunOnlyOneNIMSetupInstanceDebug";
#else
	const wchar_t kMutexName[] = L"RunOnlyOneNIMSetupInstance";
#endif
	HANDLE mutex = ::CreateMutex(NULL, TRUE, kMutexName);
	if (mutex == NULL || ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		if (mutex != NULL)
		{
			::CloseHandle(mutex);
		}
		return true;
	}
	return false;
}
void RegWritePca() //把本程序添加到注册表白名单，防止弹出“这个程序可能安装不正确”
{
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, \
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted", 0, KEY_READ | KEY_WRITE, &hKey))
	{
		wchar_t szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		DWORD dwCode = 1;
		RegSetValueEx(hKey, szPath, 0, REG_DWORD, (byte*)&dwCode, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}
void UnRegWritePca()
{
	HKEY hKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, \
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted", 0, KEY_READ | KEY_WRITE, &hKey))
	{
		wchar_t szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH);
		DWORD dwCode = 1;
		RegDeleteKey(hKey, szPath);
		RegCloseKey(hKey);
	}
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	nbase::AtExitManager at_manager;

	CComModule _Module;
	_Module.Init(NULL, hInstance);

	// CRT都会用到
	_wsetlocale(LC_ALL, L"chs");

#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONOUT$", "w+t", stderr);
	wprintf_s(L"cmd string:\n%s\n", lpCmdLine);
#endif

	if (!Init(hInstance))
		return 1;

	RegWritePca();
	ParseCommandLine();

	QLOG_APP(L"------- start setup -------");
	QLOG_APP(L"app: run message loop");

	{
		bool setup_single = CheckSingletonRun();
		bool yixin_single = CheckYixinSingletonRun();
		int check_num = 0;
		while(yixin_single && update_yixin && check_num < 20)
		{
			Sleep(100);
			yixin_single = CheckYixinSingletonRun();
			check_num++;
		}
		MainThread thread(setup_single, yixin_single, update_yixin);
		thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);
	}
	UnRegWritePca();
	QLOG_APP(L"app: leave message loop");

	return 0;
}

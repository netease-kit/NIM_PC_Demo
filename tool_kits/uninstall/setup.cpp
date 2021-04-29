// setup.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <atlbase.h>
#include <shellapi.h>
#include "setup.h"
#include "main/main_thread.h"
#include "main/setup_data.h"
#include "base/util/cmd_line_args.h"

static bool uninstall_begin_ = false;
static std::wstring uninstall_path_;

std::wstring GetAppDataPath()
{
	std::wstring app_data_path = nbase::win32::GetLocalAppDataDir()
		+ CSetupData::GetProductDoc();

	if (!nbase::FilePathIsExist(app_data_path, true)) {
		nbase::CreateDirectory(app_data_path);
	}
	return app_data_path;
}
std::wstring GetLocalTempPath()
{
	std::wstring app_data_path = nbase::win32::GetLocalAppDataDir();
	app_data_path += L"temp\\nim\\~u\\";
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
		std::wstring cmd = nbase::StringPrintf(L"%s", *iter);
		std::wstring key = L"/uninstall=";
		if (cmd.find(key.c_str()) == 0) {
			uninstall_path_ = cmd.substr(key.size());
			uninstall_begin_ = true;
		}
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
	ParseCommandLine();
	//bool copy_ret = false;
	if (!uninstall_begin_)
	{
		QLOG_APP(L"------- copy uninstall -------");
		std::wstring cur_file = nbase::win32::GetCurrentModulePathName();
		std::wstring cur_dir = nbase::win32::GetCurrentModuleDirectory();
		std::wstring temp_dir = GetLocalTempPath();
		CSetupData::DeleteOldFile(temp_dir + L"*.*");
		std::wstring dest_file;
		for (int i = 1; i <= 10; i++)
		{
			dest_file = nbase::StringPrintf(L"%s%du.exe", temp_dir.c_str(), i);
			if (nbase::CopyFile(cur_file, dest_file))
			{
				//copy_ret = true;
				std::wstring cmd = nbase::StringPrintf(L"\"/uninstall=%s\"", cur_dir.c_str());
				HINSTANCE inst = ::ShellExecute(NULL, L"open", dest_file.c_str(), cmd.c_str(), NULL, SW_SHOW);
				int ret = (int)inst;
				if (ret > 32)
				{
					QLOG_APP(L"start_run {0}, {1}") << dest_file.c_str() << cmd.c_str();
				}
				else
				{
					QLOG_ERR(L"start_run {0}, {1} failed {2}") << dest_file.c_str() << cmd.c_str() << ret;
				}
				return 0;
			}
		}
		QLOG_APP(L"------- copy uninstall fail -------");
	}
	//if (uninstall_begin_ || !copy_ret)
	{
		QLOG_APP(L"------- start uninstall -------");
		QLOG_APP(L"app: run message loop");

		{
			MainThread thread(uninstall_path_);
			thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);
		}

		QLOG_APP(L"app: leave message loop");
	}
	return 0;
}

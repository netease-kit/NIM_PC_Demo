#pragma once
#include "shared/util.h"
#include <DbgHelp.h>
#include "util/user.h"
#include "module/runtime_data/runtime_data_manager.h"

bool g_need_restart_after_dump = false;
#define APPDATA_DIR L"Netease"

BOOL CALLBACK MyMiniDumpCallback(PVOID, const PMINIDUMP_CALLBACK_INPUT input, PMINIDUMP_CALLBACK_OUTPUT output)
{
	if(input == NULL || output == NULL)
		return FALSE;

	BOOL ret = FALSE;
	switch(input->CallbackType)
	{
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		ret = TRUE;
		break;
	case ModuleCallback:
		{
			if( !(output->ModuleWriteFlags & ModuleReferencedByMemory) )
			{
				output->ModuleWriteFlags &= ~ModuleWriteModule;
			}
			ret = TRUE;
		}
		break;
	default:
		break;
	}

	return ret;
}

void WriteDump(EXCEPTION_POINTERS* exp, const std::wstring &path)
{
	HANDLE h = ::CreateFile(path.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION info;
	info.ThreadId = ::GetCurrentThreadId();
	info.ExceptionPointers = exp;
	info.ClientPointers = NULL;

	MINIDUMP_CALLBACK_INFORMATION mci; 
	mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
	mci.CallbackParam = 0; 

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), h, mdt, &info, NULL, &mci);
	::CloseHandle(h);
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* exp)
{
	nbase::Time::TimeStruct qt = nbase::Time::Now().ToTimeStruct(true);
	std::wstring file = nbase::StringPrintf(L"%04d%02d%02d_%02d%02d%02d.dmp", 
		qt.year_, qt.month_, qt.day_of_month_, qt.hour_, qt.minute_, qt.second_);

    std::wstring dir;
    if (nim_comp::RunTimeDataManager::GetInstance()->IsSDKInited())
    {
		dir = nbase::UTF8ToUTF16(nim::Tool::GetUserAppdataDir(""));
    }
    else
    {
#ifdef DEBUG
        dir = nbase::win32::GetLocalAppDataDir() + APPDATA_DIR L"\\NIM_DEBUG\\";
#else
        dir = nbase::win32::GetLocalAppDataDir() + APPDATA_DIR L"\\NIM\\";
#endif
    }

	dir.append(file);

	WriteDump(exp, dir);

	if (g_need_restart_after_dump)
	{
		std::wstring cmd = nbase::StringPrintf(L" /%s %s ", kCmdAppCrash.c_str(), dir.c_str());
		QCommand::RestartApp(cmd);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
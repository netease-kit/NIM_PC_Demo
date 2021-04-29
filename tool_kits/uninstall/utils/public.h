#include "stdafx.h"
#include <ShlObj.h>

//FolderPath
#define $INSTDIR		_T("$INSTDIR/")
#define $PREINSTDIR		_T("$PREINSTDIR/")
#define $SYSDIR			_T("$SYSDIR/")
//#define $PRINTERSDIR	_T("$PRINTERSDIR/")
#define $PROGRAMFILES	_T("$PROGRAMFILES/")
#define $STARTMENU		_T("$STARTMENU/")
#define $SMPROGRAMS		_T("$STARTMENU/")
#define $DESKTOP		_T("$DESKTOP/")
#define $QUICKLAUNCH	_T("$QUICKLAUNCH/")
#define $PERSONAL		_T("$PERSONAL/")
#define $LOCALAPPDATA	_T("$LOCALAPPDATA/")

//windows version
#define WIN_UNKNOW	_T("Unknow")
#define WIN_NT_4	_T("Windows NT 4.0")
#define WIN_95		_T("Windows 95")
#define WIN_98		_T("Windows 98")
#define WIN_ME		_T("Windows Me")
#define WIN_XP		_T("Windows XP")
#define WIN_2000	_T("Windows 2000")
#define WIN_2003	_T("Windows Server 2003")
#define WIN_Vista	_T("Windows Vista")
#define WIN_7		_T("Windows 7")
#define WIN_2008	_T("Windows Server 2008")
#define WIN_2008R2	_T("Windows Server 2008 R2")

//bit
#define BIT_32		_T(" 32-bit")
#define BIT_64		_T(" 64-bit")

//Reg
#define HKLM	_T("HKEY_LOCAL_MACHINE")
#define HKCU	_T("HKEY_CURRENT_USER")
#define HKCR	_T("HKEY_CLASSES_ROOT")


struct ValueInfo{
	std::wstring strValue;
	std::wstring strTitle;
};
typedef std::vector<ValueInfo> RegValueList;
struct RegInfo{
	std::wstring strKey;
	std::wstring strSubKey;
	RegValueList vecValueList;
	BOOL bDel;
};
typedef std::vector<RegInfo> RegInfo_List;

enum eFileOpt
{
	eFileOpt_RegDll = 1,	//RegDll
	eFileOpt_UnRegDll,		//UnRegDll
	eFileOpt_WaitCmd,		//WaitForCmd
	eFileOpt_Shell,		//ShellExecute
	eFileOpt_Unknow
};
struct FileOpt{
	eFileOpt eFileOptType;
	std::wstring strFile;
	std::wstring strCmd;
	std::wstring strOpt;
	UINT nFlags;
};
typedef std::vector<FileOpt> FileOpt_List;


static BOOL m_bAllUserPrograms = 1;

static std::wstring m_strInstDir;
static std::wstring m_strPreInstPath;
static std::wstring m_strOSVersion;
static std::wstring m_strSysDir;
static std::wstring m_strPrintersDir;
static std::wstring m_strProgramFilesDir;
static std::wstring m_strStartMenuDir;
static std::wstring m_strDeskTopDir;
static std::wstring m_strQuickLunch;
static std::wstring m_strPersonalDir;
static std::wstring m_strLocalAppDataDir;

static BOOL m_bVistaUp = FALSE;
//获取操作系统
std::wstring GetOS()
{
	OSVERSIONINFOEX osi;
	ZeroMemory(&osi,sizeof(osi));
	osi.dwOSVersionInfoSize = sizeof(osi);
	::GetVersionEx((OSVERSIONINFO*)&osi);

	SYSTEM_INFO si;
	ZeroMemory(&si,sizeof(si));
	::GetSystemInfo(&si);

	std::wstring szOS(WIN_UNKNOW);	
	if (osi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		m_bVistaUp = osi.dwMajorVersion>=6;
		switch (osi.dwMajorVersion)
		{
		case 4:
			{
				switch(osi.dwMinorVersion)
				{
				case 0:
					{
						if(osi.dwPlatformId==VER_PLATFORM_WIN32_NT)
						{
							szOS=_T("Microsoft Windows NT 4.0");
						}
						else if(osi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
						{
							szOS=_T("Microsoft Windows 95");
						}
					}
					break;
				case 10:
					szOS=_T("Microsoft Windows 98");
					break;
				case 90:
					szOS=_T("Microsoft Windows Me");
					break;
				}
			}
			break; 
		case 5:
			{
				switch(osi.dwMinorVersion)
				{
				case 0:
					szOS = WIN_2000;
					break;
				case 1:
					szOS = WIN_XP;
					break;
				case 2:
					szOS = WIN_2003;
					break;
				default:
					break;
				}
			}
			break;
		case 6:
			{
				switch(osi.dwMinorVersion)
				{
				case 0:
					{
						if (osi.wProductType == VER_NT_WORKSTATION)
						{
							szOS = WIN_Vista;
						}
						else
						{
							szOS = WIN_2008;
						}
					}
					break;
				case 1:
					{
						if (osi.wProductType == VER_NT_WORKSTATION)
						{
							szOS = WIN_7;
						}
						else
						{
							szOS = WIN_2008R2;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}

		if (osi.dwMajorVersion >= 6)
		{
			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
			{
				szOS += BIT_32;
			}
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				szOS += BIT_64;
			}
		}
		else
		{
			//m_bAllUserPrograms = 1;
		}
	}

	return szOS;
}
//获取操作系统
std::wstring GetOSVersion()
{
	if (m_strOSVersion.empty())
	{
		m_strOSVersion = GetOS();
	}
	return m_strOSVersion;
}
BOOL IsBit64()
{
	if (GetOSVersion().find(BIT_64)!=-1)
	{
		return TRUE;
	}
	return FALSE;
}
std::wstring GetSysDir()
{
	if (m_strSysDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL,szPath,CSIDL_SYSTEM, FALSE);
		m_strSysDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strSysDir;
}
#ifdef $PRINTERSDIR
std::wstring GetPrinterDir()
{
	if (m_strPrintersDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL,szPath,CSIDL_PRINTERS, FALSE);
		m_strPrintersDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strPrintersDir;
}
#endif
std::wstring GetProgramFilesDir()
{
	if (m_strProgramFilesDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_PROGRAM_FILES, FALSE);
		m_strProgramFilesDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strProgramFilesDir;
}
std::wstring GetPersonalDir()
{
	if (m_strPersonalDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE);
		m_strPersonalDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strPersonalDir;
}
std::wstring GetStartMenuDir()
{
	if (m_strStartMenuDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (m_bAllUserPrograms)
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_COMMON_PROGRAMS, FALSE);
		} 
		else
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_PROGRAMS, FALSE);
		}
		m_strStartMenuDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strStartMenuDir;
}
std::wstring GetDeskTopDir()
{
	if (m_strDeskTopDir.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (m_bAllUserPrograms)
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
		} 
		else
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_DESKTOPDIRECTORY, FALSE);
		}
		m_strDeskTopDir = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strDeskTopDir;
}
std::wstring GetLocalAppDataDir()
{
	if (m_strLocalAppDataDir.empty())
	{
		m_strLocalAppDataDir = nbase::win32::GetLocalAppDataDir();
	}
	return m_strLocalAppDataDir;
}
std::wstring GetQuickLaunch()
{
	if (m_strQuickLunch.empty())
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (m_bAllUserPrograms)
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
		} 
		else
		{
			SHGetSpecialFolderPath(NULL,szPath,CSIDL_DESKTOP, FALSE);
		}
		m_strQuickLunch = nbase::StringPrintf(L"%s/", szPath);
	}
	return m_strDeskTopDir;
}
std::wstring GetZipFilePath(std::wstring strZipPath, std::wstring strFilePath)
{
	strZipPath = strZipPath.substr(strFilePath.size());
	if (!strZipPath.empty())
	{
		if (strZipPath.at(0)==L'\\')
		{
			strZipPath = strZipPath.substr(1, strZipPath.size()-1);
		}
	} 
	else
	{
		int ipos = strFilePath.rfind(L'\\');
		strZipPath = strFilePath.substr(ipos-1);
	}
	return strZipPath;
}
std::wstring GetFullFolderPath(std::wstring strCopyPath, BOOL bAllStrCheck=FALSE)
{
	std::wstring strPath;
	std::wstring strHead;
	std::wstring strFolder;
	if (strCopyPath.find($INSTDIR)==0 || (bAllStrCheck&&strCopyPath.find($INSTDIR)>0))
	{
		strHead = $INSTDIR;
		strFolder = m_strInstDir;
	}
	else if (strCopyPath.find($PREINSTDIR) == 0 || (bAllStrCheck&&strCopyPath.find($PREINSTDIR) > 0))
	{
		strHead = $PREINSTDIR;
		strFolder = m_strPreInstPath;
	}
	else if (strCopyPath.find($SYSDIR)==0 || (bAllStrCheck&&strCopyPath.find($SYSDIR)>0))
	{
		strHead = $SYSDIR;
		strFolder = GetSysDir();
	}
#ifdef $PRINTERSDIR
	else if (strCopyPath.find($PRINTERSDIR)==0 || (bAllStrCheck&&strCopyPath.find($PRINTERSDIR)>0))
	{
		strHead = $PRINTERSDIR;
		strFolder = GetPrinterDir();
	}
#endif
	else if (strCopyPath.find($PROGRAMFILES)==0 || (bAllStrCheck&&strCopyPath.find($PROGRAMFILES)>0))
	{
		strHead = $PROGRAMFILES;
		strFolder = GetProgramFilesDir();
	}
	else if (strCopyPath.find($STARTMENU)==0 || (bAllStrCheck&&strCopyPath.find($STARTMENU)>0))
	{
		strHead = $STARTMENU;
		strFolder = GetStartMenuDir();
	}
	else if (strCopyPath.find($DESKTOP)==0 || (bAllStrCheck&&strCopyPath.find($DESKTOP)>0))
	{
		strHead = $DESKTOP;
		strFolder = GetDeskTopDir();
	}
	else if (strCopyPath.find($QUICKLAUNCH)==0 || (bAllStrCheck&&strCopyPath.find($QUICKLAUNCH)>0))
	{
		strHead = $QUICKLAUNCH;
		strFolder = GetDeskTopDir();
	}
	else if (strCopyPath.find($PERSONAL)==0 || (bAllStrCheck&&strCopyPath.find($PERSONAL)>0))
	{
		strHead = $PERSONAL;
		strFolder = GetPersonalDir();
	}
	else if (strCopyPath.find($LOCALAPPDATA) == 0 || (bAllStrCheck&&strCopyPath.find($LOCALAPPDATA) > 0))
	{
		strHead = $LOCALAPPDATA;
		strFolder = GetLocalAppDataDir();
	}
	else
	{
		strPath = strCopyPath;
		return strPath;
	}
	if (bAllStrCheck)
	{
		strPath = strCopyPath;
		nbase::StringReplaceAll(strHead, strFolder, strPath);
	}
	else
	{
		strPath = strFolder + strCopyPath.substr(strHead.size());
	}
	nbase::StringReplaceAll(L"/", L"\\", strPath);
	return strPath;
}
//检查并创建路径的文件夹，路径本身是文件夹的返回TRUE
BOOL CheckPathForlder(std::wstring strPath)
{
	BOOL bRet = FALSE;
	nbase::StringReplaceAll(L"\\", L"/", strPath);
	if (!strPath.empty() && strPath.at(strPath.size() - 1) == L'/')
	{
		bRet=TRUE;
	}
	int ipos = strPath.rfind(L'/');
	if (ipos!=-1)
	{
		strPath = strPath.substr(0, ipos+1);
	}
	CreateDirectory(strPath.c_str(), NULL);
	return bRet;
}
bool CheckZipPath(std::wstring strZip, std::wstring strFileInfoPath)
{
	strZip = nbase::MakeLowerString(strZip);
	strFileInfoPath = nbase::MakeLowerString(strFileInfoPath);
	if (!strFileInfoPath.empty())
	{
		if (!strFileInfoPath.empty() && strFileInfoPath.at(strFileInfoPath.size() - 1) == L'\\')
		{
			if (strZip.find(strFileInfoPath)==0)
			{
				return TRUE;
			}
		}
		else if (strZip == strFileInfoPath || strZip.find(strFileInfoPath+L"\\")==0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
BOOL SetRegValue(std::wstring strValue, std::wstring strTitle, HKEY hKey, BOOL bDel)
{
	LONG ret;
	if (bDel)
	{
		ret=(::RegDeleteValue(hKey, strValue.c_str()));
	} 
	else
	{
		ret = (::RegSetValueEx(hKey, strTitle.c_str(), NULL, REG_SZ, (BYTE*)strValue.c_str(), strValue.size()*2));
	}
	return ret==ERROR_SUCCESS;
}
BOOL SetReg(RegInfo regInfo)
{
	if (regInfo.strKey.empty() || regInfo.strSubKey.empty())
	{
		return FALSE;
	}
	HKEY hKey, hKey2;
	if (regInfo.strKey == HKCU)
	{
		hKey = HKEY_CURRENT_USER;
	}
	else if (regInfo.strKey == HKLM)
	{
		hKey = HKEY_LOCAL_MACHINE;
	}
	else if (regInfo.strKey == HKCR)
	{
		hKey = HKEY_CLASSES_ROOT;
	}
	else
	{
		return FALSE;
	}
	if (regInfo.vecValueList.size() == 0)
	{
		if (regInfo.bDel)
		{
			LONG ret=(::RegDeleteKey(hKey,regInfo.strSubKey.c_str()));
			return ret==ERROR_SUCCESS;
		}
	}
	else
	{
		LONG ret=(::RegCreateKeyEx(hKey,regInfo.strSubKey.c_str(),0,0,0,KEY_SET_VALUE,0,&hKey2,0));
		//LONG ret=(::RegOpenKeyEx(hKey,regInfo.strSubKey,0,KEY_WRITE,&hKey2));
		//if (ret==ERROR_FILE_NOT_FOUND)
		//{
		//	ret=(::RegCreateKeyEx(hKey,regInfo.strSubKey,0,0,0,KEY_SET_VALUE,0,&hKey2,0));
		//}
		if(ret==ERROR_SUCCESS)
		{
			for (uint32_t i=0;i<regInfo.vecValueList.size();i++)
			{
				ValueInfo valueInfo = regInfo.vecValueList.at(i);
				SetRegValue(valueInfo.strValue, valueInfo.strTitle, hKey2, regInfo.bDel);
			}
			::RegCloseKey(hKey2);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL RegDll(std::wstring strFile)
{
	BOOL bRet = FALSE;
	HMODULE mod = LoadLibraryEx(strFile.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (mod)
	{
		FARPROC regfunc = GetProcAddress(mod, "DllRegisterServer");
		if (!((*regfunc)()))
		{
			bRet = TRUE;
		}
		FreeLibrary(mod); 
	}
	return bRet;
}
BOOL UnRegDll(std::wstring strFile)
{
	BOOL bRet = FALSE;
	HMODULE mod = LoadLibraryEx(strFile.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (mod)
	{
		FARPROC regfunc = GetProcAddress(mod, "DllUnregisterServer");
		if (!((*regfunc)()))
		{
			bRet = TRUE;
		}
		FreeLibrary(mod);
	}
	return bRet;
}
void SafeWow64EnableWow64FsRedirection(BOOL Wow64FsEnableRedirection)
{
	HMODULE kernel = GetModuleHandle(L"kernel32");
	if (kernel)
	{
		FARPROC proc = GetProcAddress(kernel, "Wow64EnableWow64FsRedirection");
		if (proc)
		{
			typedef BOOL (WINAPI *Wow64EnableWow64FsRedirectionPtr)(BOOL);
			Wow64EnableWow64FsRedirectionPtr Wow64EnableWow64FsRedirectionFunc =
				(Wow64EnableWow64FsRedirectionPtr) proc;

			Wow64EnableWow64FsRedirectionFunc(Wow64FsEnableRedirection);
		}
	}
}
BOOL WaitForCmd(std::wstring strCmdline)
{
	BOOL bRet = FALSE;
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	if (CreateProcess(NULL, (LPWSTR)strCmdline.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{

		DWORD dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
		bRet = dwRet==0;

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	if (IsBit64())
	{
		SafeWow64EnableWow64FsRedirection(TRUE);
	}
	return bRet;
}

BOOL CreateLink(std::wstring strFullPath,std::wstring strLinkPath,std::wstring strIcoPath)
{
	HRESULT hRet;
	IShellLink *pLink;	//IShellLink对象指针
	IPersistFile *ppf;	//IPersisFil对象指针
	hRet = ::CoInitialize(NULL);
	if ( hRet != S_OK)
	{
		return FALSE;
	}
	hRet = ::CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void**)&pLink);
	if ( hRet == S_OK)
	{
		hRet = pLink->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if ( hRet == S_OK)
		{
			pLink->SetPath(strFullPath.c_str());
			int iPos = strFullPath.rfind(L'/');
			if (iPos!=-1)
			{
				pLink->SetWorkingDirectory(strFullPath.substr(0, iPos).c_str());
			}
			if (!strIcoPath.empty())
			{
				int iIco = 0;
				pLink->SetIconLocation(strIcoPath.c_str(), iIco);
			}
			hRet = ppf->Save(strLinkPath.c_str(), TRUE);
			ppf->Release();
		}
		pLink->Release();
	}
	::CoUninitialize();
	return hRet == S_OK;
}
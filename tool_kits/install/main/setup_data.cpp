//////////////////////////////////////////////////////////////////////////////////
//			by 高琦 2013/2/18
//	使用前先调用SetInstDir(std::wstring strPath) 设置安装路径,如果要用到前一次的安装路径需要先调用GetLastInstDir()
//	GetDelFileInfoListNum()	写入需要预先删除的文件
//	GetAddFileInfoListNum()	写入根据7z包拷贝文件的规则，默认放在安装目录下解压
//	GetDelRegInfoListNum()	写入需要预先删除的注册表信息
//	GetAddRegInfoListNum()	写入需要文件拷贝结束需要添加的注册表信息
//	GetFileOptInfoListNum()	写入需要最后需要操作文件的信息
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "setup_def.h"
#include "setup_data.h"
#include "utils/public.h"
#include <shellapi.h>
#include <TlHelp32.h>

//文件释放
#define _AddFile_os(x, y, z)	AddFileInfo(x, y, z);
#define _AddFile_copy(x, y)		_AddFile_os(x, y, L"")
#define _AddFile_ignore(x)		_AddFile_copy(x, L"")

//文件删除
#define _DelFile(x)				m_strAryDleFileList.push_back(GetFullFolderPath(x));

//注册表操作
#define _DelRegKey(x, y)		DelRegInfo(x, y);
#define _DelRegValue(x, y, z)	DelRegInfo(x, y, z);
#define _WriteRegValue(x,y,z,t)	AddRegInfo(x, y, t, z);

//文件操作
#define _RegDll(x)				AddFileOptInfo(eFileOpt_RegDll, GetFullFolderPath(x));
#define _UnRegDll(x)			AddFileOptInfo(eFileOpt_UnRegDll, GetFullFolderPath(x));
#define _WaitCmd(x)				AddFileOptInfo(eFileOpt_WaitCmd, L"", GetFullFolderPath(x, TRUE));
#define _Shell(opt, f,cmd,u)	AddFileOptInfo(eFileOpt_Shell, GetFullFolderPath(f), cmd, opt, u);
#define _ShellOpen(f,cmd,u)		_Shell(L"open", f,cmd,u)

//创建快捷方式
#define _Create_Lnk(f,name,ico)		CreateLink(GetFullFolderPath(f), GetFullFolderPath(name), GetFullFolderPath(ico));

static FileInfo_List m_vecCopyFileInfoList;
static std::vector<std::wstring> m_strAryDleFileList;
static RegInfo_List	m_vecDelRegInfoList;
static RegInfo_List	m_vecAddRegInfoList;
static FileOpt_List m_vecFileOptList;

//////////////////

CSetupData::CSetupData(void)
{
}

CSetupData::~CSetupData(void)
{
}

//是否匹配操作系统
BOOL CSetupData::IsOSVersion(std::wstring strOS)
{
	return GetOSVersion().find(strOS)!=-1;
}
void CSetupData::AddFileInfo(std::wstring strZipPath, std::wstring strCopyPath, std::wstring strOSVersion)
{
	FileInfo info;
	info.strZipPath = strZipPath;
	info.strCopyPath = strCopyPath;
	info.strOSVersion = strOSVersion;
	m_vecCopyFileInfoList.push_back(info);
}
//根据ZIP包文件解析释放路径，为空则跳过
std::wstring CSetupData::GetFileCopyPath(std::wstring strZipPath)
{
	if (strZipPath.empty())
		return L"";
	strZipPath = CheckPathAndReplace(strZipPath);
	GetAddFileInfoListNum();
	std::wstring strPath;
	for (uint32_t i = 0; i<m_vecCopyFileInfoList.size(); i++)
	{
		FileInfo info = m_vecCopyFileInfoList.at(i);
		if (CheckZipPath(strZipPath, info.strZipPath))
		{
			if ((info.strOSVersion.empty() || IsOSVersion(info.strOSVersion))&&!info.strCopyPath.empty())
			{
				std::wstring strFile = GetZipFilePath(strZipPath, info.strZipPath);
				std::wstring strFileFolder = GetFullFolderPath(info.strCopyPath);
				if (!strFileFolder.empty() && strFileFolder.at(strFileFolder.size() - 1) == L'\\')
				{
					strPath = strFileFolder+strFile;
#ifdef Not_UnZip_Folder
					if (strPath.Right(1)=="\\")
					{
						return "";
					}
#endif
					return strPath;
				}
				else
				{
					return strFileFolder;
				}
			} 
			return L"";
		}
	}
	strPath = m_strInstDir + strZipPath;
	return strPath;
}
//删除注册表信息
void CSetupData::DelRegInfo(std::wstring strKey, std::wstring strSubKey, std::wstring strValue)
{
	ValueInfo valueInfo;
	valueInfo.strValue = GetFullFolderPath(strValue, TRUE);
	valueInfo.strTitle = _T("");
	for (RegInfo_List::iterator it=m_vecDelRegInfoList.begin();it!=m_vecDelRegInfoList.end();it++)
	{
		if (it->strKey == strKey && it->strSubKey == strSubKey/* && it->bDel*/)
		{
			if (it->vecValueList.size()>0)
			{
				it->vecValueList.push_back(valueInfo);
			}
			return;
		}
	}
	RegInfo regInfo;
	regInfo.strKey = strKey;
	regInfo.strSubKey = strSubKey;
	regInfo.vecValueList.clear();
	regInfo.bDel = TRUE;
	if (!strValue.empty())
	{
		regInfo.vecValueList.push_back(valueInfo);
	}
	m_vecDelRegInfoList.push_back(regInfo);
}
//添加注册表信息
void CSetupData::AddRegInfo(std::wstring strKey, std::wstring strSubKey, std::wstring strValue, std::wstring strTitle)
{
	ValueInfo valueInfo;
	valueInfo.strValue = GetFullFolderPath(strValue, TRUE);
	valueInfo.strTitle = GetFullFolderPath(strTitle);
	for (RegInfo_List::iterator it=m_vecAddRegInfoList.begin();it!=m_vecAddRegInfoList.end();it++)
	{
		if (it->strKey == strKey && it->strSubKey == strSubKey/* && !it->bDel*/)
		{
			it->vecValueList.push_back(valueInfo);
			return;
		}
	}
	RegInfo regInfo;
	regInfo.strKey = strKey;
	regInfo.strSubKey = strSubKey;
	regInfo.vecValueList.clear();
	regInfo.bDel = FALSE;
	regInfo.vecValueList.push_back(valueInfo);
	m_vecAddRegInfoList.push_back(regInfo);
}
//添加文件操作信息
void CSetupData::AddFileOptInfo(int iType, std::wstring strFile, std::wstring strCmd, std::wstring strOpt, UINT nFlags)
{
	FileOpt info;
	info.eFileOptType = (eFileOpt)iType;
	info.strFile = strFile;
	info.strCmd = strCmd;
	info.strOpt = strOpt;
	info.nFlags = nFlags;
	m_vecFileOptList.push_back(info);
}
//设置安装目录
void CSetupData::SetInstDir(std::wstring strPath)
{
	strPath = CSetupData::CheckDirAndReplace(strPath);

	m_strInstDir = strPath;
}

std::wstring CSetupData::GetInstDir()
{
	return m_strInstDir;
}

//设置上次安装目录
std::wstring CSetupData::GetLastInstDir()
{
	if (m_strPreInstPath.empty())
	{
		std::wstring strPath = CSetupData::ReadRegisteTable(L"HKEY_LOCAL_MACHINE", PRODUCT_REGKEY, L"install_dir");
		if (!strPath.empty())
		{
			m_strPreInstPath = CSetupData::CheckDirAndReplace(strPath);
		}
	}
	return m_strPreInstPath;
}
//获取默认安装路径
std::wstring CSetupData::GetDefaultInstallDir()
{
	GetLastInstDir();
	if (!m_strPreInstPath.empty())
	{
		return m_strPreInstPath;
	}
	return GetFullFolderPath($PROGRAMFILES PRODUCT_DOC);
}
std::wstring CSetupData::GetProcessName()
{
	std::wstring process_name = PROCESS_NAME;
	return process_name;
}
std::wstring CSetupData::GetProductDoc()
{
	std::wstring doc = PRODUCT_DOC;
	return doc;
}
//批量删除文件
BOOL CSetupData::DeleteOldFile(uint32_t iNum)
{
	if (iNum>= m_strAryDleFileList.size())
	{
		return FALSE;
	}
	std::wstring strInfo = m_strAryDleFileList.at(iNum);
	return DeleteOldFile(strInfo);
}
//批量删除文件
BOOL CSetupData::DeleteOldFile(std::wstring strInfo)
{
	strInfo = CheckPathAndReplace(strInfo);
	BOOL bOnlyFile = FALSE;
	if (strInfo.size() >= 2 && strInfo.substr(strInfo.size() - 2) == L"\\*")
	{
		bOnlyFile = TRUE;
		strInfo += L".*";
	}
	std::wstring file_dir;
	int pos = strInfo.rfind(L'\\');
	if (pos != -1)
	{
		file_dir = strInfo.substr(0, pos + 1);
	}
	if (file_dir == strInfo)
	{
		RemoveDirectory(file_dir.c_str());
		return TRUE;
	}

	WIN32_FIND_DATA find_data;
	HANDLE hError = ::FindFirstFile(strInfo.c_str(), &find_data);
	if (hError != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::wstring filename(find_data.cFileName);
			if (filename == L"." || filename == L"..")
			{
				continue;
			}
			filename = file_dir + filename;
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!bOnlyFile)
				{
					DeleteOldFile(filename + _T("\\*.*"));
					RemoveDirectory(filename.c_str());
				}
			} 
			else 
			{ 
				DeleteFile(filename.c_str());
			} 
		}while (::FindNextFile(hError, &find_data));
		::FindClose(hError);
	}
	return TRUE; 
}
//删除注册表信息
BOOL CSetupData::DeleteRegInfo(uint32_t iNum)
{
	if (iNum>= m_vecDelRegInfoList.size())
	{
		return FALSE;
	}
	RegInfo Info = m_vecDelRegInfoList.at(iNum);
	return SetReg(Info);
}
//修改注册表信息
BOOL CSetupData::WriteRegInfo(uint32_t iNum)
{
	if (iNum>= m_vecAddRegInfoList.size())
	{
		return FALSE;
	}
	RegInfo Info = m_vecAddRegInfoList.at(iNum);
	return SetReg(Info);
}
//文件操作（注册，执行等）
BOOL CSetupData::FileOptInfo(uint32_t iNum)
{
	if (iNum>= m_vecFileOptList.size())
	{
		return FALSE;
	}
	FileOpt info = m_vecFileOptList.at(iNum);
	switch (info.eFileOptType)
	{
	case eFileOpt_RegDll:
		return RegDll(info.strFile);
	case eFileOpt_UnRegDll:
		return UnRegDll(info.strFile);
	case eFileOpt_WaitCmd:
		return WaitForCmd(info.strCmd);
	case eFileOpt_Shell:
		{
			HINSTANCE ret = ShellExecute(NULL, info.strOpt.c_str(), info.strFile.c_str(), info.strCmd.c_str(), NULL, info.nFlags);
			return ((int)ret > 32);
		}
	default:
		return FALSE;
	}
}

//获取批量删除文件信息
int CSetupData::GetDelFileInfoListNum()
{
	if (m_strAryDleFileList.size()==0)
	{
		//-------------------------------------------------
		//删除老文件
		if (!m_strPreInstPath.empty())
		{
			std::list<std::wstring> del_files = GetDelFilesList();
			for (auto it : del_files)
			{
				std::wstring file_path = $PREINSTDIR;
				file_path.append(it);
				_DelFile(file_path);
			}
		}

		//删除老的快捷方式
		_DelFile($SMPROGRAMS PRODUCT_NAME _T("\\*.*"));
		_DelFile($SMPROGRAMS PRODUCT_NAME _T("\\"));
		_DelFile($SMPROGRAMS PRODUCT_NAME _T(".lnk"));
		_DelFile($DESKTOP PRODUCT_NAME _T(".lnk"));
	}
	return m_strAryDleFileList.size();
}
//获取安装包文件释放信息
int CSetupData::GetAddFileInfoListNum()
{
	if (m_vecCopyFileInfoList.size()==0)
	{
		_AddFile_copy(L"bin\\", $INSTDIR);
	}

	return m_vecCopyFileInfoList.size();
}
//获取删除注册表信息
int CSetupData::GetDelRegInfoListNum()
{
	if (m_vecDelRegInfoList.size()==0)
	{
		//-------------------------------------------------
		//std::wstring str = GetFullFolderPath($DESKTOP) + PRODUCT_NAME + L".lnk";
		//ShellExecute(NULL, L"taskbarunpin", str.c_str(), NULL, NULL, 0);
		//_DelRegKey(HKLM, PRODUCT_UNINST_KEY);
		//_DelRegValue(HKCU, L"Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache", $INSTDIR L"Start.exe");
		//_DelRegValue(HKLM, "Software\ODA\Settings\Install", "Install_Dir");
		//-------------------------------------------------
	}
	return m_vecDelRegInfoList.size();
}
//获取添加注册表信息
int CSetupData::GetAddRegInfoListNum()
{
	if (m_vecAddRegInfoList.size()==0)
	{
		_WriteRegValue(HKLM, PRODUCT_REGKEY, _T("version"), PRODUCT_VERSION);
		_WriteRegValue(HKLM, PRODUCT_REGKEY, _T("install_dir"), $INSTDIR);
		_WriteRegValue(HKLM, PRODUCT_DIR_REGKEY, _T(""), $INSTDIR RUN_NAME);
		_WriteRegValue(HKLM, PRODUCT_UNINST_KEY, _T("DisplayName"), PRODUCT_NAME);
		_WriteRegValue(HKLM, PRODUCT_UNINST_KEY, _T("UninstallString"), $INSTDIR UNINSTALL_NAME);
		_WriteRegValue(HKLM, PRODUCT_UNINST_KEY, _T("DisplayIcon"), $INSTDIR RUN_NAME);
		_WriteRegValue(HKLM, PRODUCT_UNINST_KEY, _T("DisplayVersion"), PRODUCT_VERSION);
		_WriteRegValue(HKLM, PRODUCT_UNINST_KEY, _T("Publisher"), PRODUCT_PUBLISHER);
		//_WriteRegValue(HKCU, PRODUCT_AUTORUN_KEY, _T("NIMAPP"), _T("\"") $INSTDIR RUN_NAME  _T("\" /autorun 1"));

		//_WriteRegValue(HKCR, START_CMD_KEY, L"", L"URL Protocol");
		//_WriteRegValue(HKCR, START_CMD_KEY, L"URL Protocol", $INSTDIR RUN_NAME);
		//_WriteRegValue(HKCR, START_CMD_KEY L"\\shell\\open\\command", L"", L"\""$INSTDIR RUN_NAME L"\" \"%1\"");
		//_WriteRegValue(HKLM, L"SOFTWARE\\Classes\\" START_CMD_KEY L"\\shell\\open\\command", L"", L"\""$INSTDIR RUN_NAME L"\" \"%1\"");
	}
	return m_vecAddRegInfoList.size();
}
//获取文件操作（注册，执行等）信息
int CSetupData::GetFileOptInfoListNum()
{
	if (m_vecFileOptList.size()==0)
	{
	}
	return m_vecFileOptList.size();
}
void CSetupData::CreateLnkList()
{
	CreateDirectory(GetFullFolderPath($SMPROGRAMS PRODUCT_NAME).c_str(), NULL);
	_Create_Lnk($INSTDIR UNINSTALL_NAME, $SMPROGRAMS PRODUCT_NAME _T("\\卸载") PRODUCT_NAME _T(".lnk"), $INSTDIR UNINSTALL_NAME);
	_Create_Lnk($INSTDIR RUN_NAME, $SMPROGRAMS PRODUCT_NAME _T("\\") PRODUCT_NAME _T(".lnk"), $INSTDIR RUN_NAME);
	_Create_Lnk($INSTDIR RUN_NAME, $SMPROGRAMS PRODUCT_NAME _T(".lnk"), $INSTDIR RUN_NAME);
	_Create_Lnk($INSTDIR RUN_NAME, $DESKTOP PRODUCT_NAME _T(".lnk"), $INSTDIR RUN_NAME);
	//std::wstring str = GetFullFolderPath($INSTDIR RUN_NAME);
	//ShellExecute(NULL, _T("taskbarpin"), str.c_str(), NULL, NULL, 0);
}
//启动程序
void CSetupData::StartRun(std::wstring cmd)
{
	std::wstring str = GetFullFolderPath($INSTDIR RUN_NAME);

	bool open_uac = false;
	OSVERSIONINFOEX osver;
	osver.dwOSVersionInfoSize = sizeof( osver );
	if( GetVersionEx( (OSVERSIONINFO*) &osver ) )
	{
		if( VER_PLATFORM_WIN32_NT == osver.dwPlatformId && osver.dwMajorVersion > 5 )
		{
			if (/*CreateLowIntegrityProcess(str.c_str(), cmd.c_str())*/CreateProcessLow((wchar_t*)str.c_str(), (wchar_t*)cmd.c_str(), NULL, SW_SHOW))
			{
				QLOG_APP( L"start_run CreateLowIntegrityProcess {0}, {1}" ) << str.c_str(), cmd.c_str();
				return;
			}
		}
	}

	HINSTANCE inst = ::ShellExecute(NULL, L"open", str.c_str(), cmd.c_str(), NULL, SW_SHOW);
	int ret = (int)inst;
	if (ret > 32)
	{
		QLOG_APP(L"start_run {0}, {1}") << str.c_str() << cmd.c_str();
	}
	else
	{
		QLOG_ERR(L"start_run {0}, {1} failed {2}") << str.c_str() << cmd.c_str() << ret;
	}
}

std::wstring CSetupData::ReadRegisteTable(std::wstring root,std::wstring path, std::wstring key)
{
	HKEY hAppKey;
	TCHAR szDataFile[MAX_PATH];
	if(root == L"HKEY_LOCAL_MACHINE")
	{
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,path.c_str(),0,KEY_READ,&hAppKey))
		{
			ULONG cbSize = MAX_PATH*sizeof(TCHAR);
			DWORD dwFlag = RegQueryValueEx(hAppKey,key.c_str(),NULL,NULL, (LPBYTE)szDataFile,&cbSize);
			RegCloseKey (hAppKey);    
			if (ERROR_SUCCESS == dwFlag)   
			{
				std::wstring data = szDataFile;
				if (data.size()>0)
				{
					return data;
				}  
				else
				{
					return L"";
				}
			} 
			return L"";
		}   
	}
	if(root == L"HKEY_CURRENT_USER")
	{
		if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_CURRENT_USER,path.c_str(),NULL,KEY_READ,&hAppKey))
		{       
			ULONG cbSize = MAX_PATH*sizeof(TCHAR);    
			DWORD dwFlag = RegQueryValueEx(hAppKey, key.c_str(), NULL, NULL, (LPBYTE)szDataFile, &cbSize);
			RegCloseKey (hAppKey);    
			if (ERROR_SUCCESS == dwFlag)   
			{
				std::wstring data = szDataFile;
				if (data.size() > 0)
				{
					return data;
				}
				else
				{
					return L"";
				}
			} 
			return L"";
		}
	}
	return L""; 
}
//检查文件夹路径名便问 C:/a/b/的形式
std::wstring CSetupData::CheckDirAndReplace(std::wstring dir_path)
{
	if (dir_path.empty())
	{
		return dir_path;
	}
	dir_path += L"//";
	return CheckPathAndReplace(dir_path);
}
//检查文件路径名变为 C:\a\b的形式
std::wstring CSetupData::CheckPathAndReplace(std::wstring path)
{
	if (path.empty())
	{
		return path;
	}
	nbase::StringReplaceAll(L"/", L"\\", path);
	while (path.find(L"\\\\", 0) != -1)
	{
		nbase::StringReplaceAll(L"\\\\", L"\\", path);
	}
	return path;
}
//查找进程ID
bool CSetupData::FindAppProcessID( const std::wstring& strExeFile, std::vector<int> &exe )
{
	bool find = false;

	HANDLE handle = ::CreateToolhelp32Snapshot( TH32CS_SNAPALL, 0 );
	PROCESSENTRY32 Info;
	Info.dwSize = sizeof( PROCESSENTRY32 );
	if( ::Process32First( handle, &Info ) )
	{
		do
		{
			std::wstring ss = Info.szExeFile;
			if( strExeFile == ss )
			{
				find = true;
				exe.push_back( Info.th32ProcessID );
			}
		} while( ::Process32Next( handle, &Info ) );
		::CloseHandle( handle );
	}
	std::wstring text;
	text.append(std::to_wstring(find ? 1 : 0)).append(L"   ").append(strExeFile);
	// MessageBox(NULL, text.c_str(), text.c_str(), 0);
	return find;
}
//关闭进程
void CSetupData::CloseAppProcess()
{
	std::vector<int> exe;;
	if( FindAppProcessID( PROCESS_NAME, exe ) )
	{
		for( auto &i : exe )
		{
			HANDLE ProcessHandle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, i );
			if( ProcessHandle )
			{
				::TerminateProcess( ProcessHandle, 0 );
				CloseHandle( ProcessHandle );
			}
		}
	}
}
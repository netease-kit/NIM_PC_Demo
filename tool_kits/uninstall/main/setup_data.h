#pragma once
#include "stdafx.h"
#include <vector>

//
//#define Not_UnZip_Folder


struct FileInfo{
	std::wstring strZipPath;
	std::wstring strCopyPath;
	std::wstring strOSVersion;
};
typedef std::vector<FileInfo> FileInfo_List;

class CSetupData
{
public:
	CSetupData(void);
	~CSetupData(void);
	//设置安装目录
	static void SetInstDir(std::wstring strPath);
	//获取上次安装目录
	static std::wstring GetLastInstDir();
	//获取默认安装路径
	static std::wstring GetDefaultInstallDir();
	//获取启动进程名
	static std::wstring GetProcessName();
	//获取产品相关文件名
	static std::wstring GetProductDoc();
	//删除文件
	static BOOL DeleteOldFile(uint32_t iNum);
	//根据ZIP包文件解析释放路径，为空则跳过
	static std::wstring GetFileCopyPath(std::wstring strZipPath);
	//删除注册表信息
	static BOOL DeleteRegInfo(uint32_t iNum);
	//修改注册表信息
	static BOOL WriteRegInfo(uint32_t iNum);
	//文件操作（注册，执行等）
	static BOOL FileOptInfo(uint32_t iNum);

	//------------
	//获取批量删除文件信息
	static int GetDelFileInfoListNum(int step = 0);
	//获取安装包文件释放信息
	static int GetAddFileInfoListNum();
	//获取删除注册表信息
	static int GetDelRegInfoListNum();
	//获取添加注册表信息
	static int GetAddRegInfoListNum();
	//获取文件操作（注册，执行等）信息
	static int GetFileOptInfoListNum();
	//创建快捷方式
	static void CreateLnkList();
	//启动程序
	static void StartRun(std::wstring cmd);
	//获取注册表值
	static std::wstring ReadRegisteTable(std::wstring root,std::wstring path, std::wstring key);
	//检查文件夹路径名变为 C:\a\b\的形式
	static std::wstring CheckDirAndReplace(std::wstring dir_path);
	//检查文件路径名变为 C:\a\b的形式
	static std::wstring CheckPathAndReplace(std::wstring path);
	//查找进程
	static bool FindAppProcessID( const std::wstring& strExeFile, std::vector<int> &exe );
	//关闭进程
	static void CloseAppProcess();

//private:
	//是否匹配操作系统
	static BOOL IsOSVersion(std::wstring strOS);
	//批量删除文件
	static BOOL DeleteOldFile(std::wstring strInfo);
	//添加文件信息
	static void AddFileInfo(std::wstring strZipPath, std::wstring strCopyPath=L"", std::wstring strOSVersion=L"");
	//删除注册表信息
	static void DelRegInfo(std::wstring strKey, std::wstring strSubKey, std::wstring strValue=L"");
	//添加注册表信息
	static void AddRegInfo(std::wstring strKey, std::wstring strSubKey, std::wstring strValue=L"", std::wstring strTitle=L"");
	//添加文件操作信息
	static void AddFileOptInfo(int iType, std::wstring strFile, std::wstring strCmd=L"", std::wstring strOpt=L"", UINT nFlags=SW_HIDE);

protected:
};

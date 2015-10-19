#include "stdafx.h"
#include "tool.h"
#include <atlbase.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <shellapi.h>
//#include "openssl/md5.h"
//#include "third_party/deelx/deelx.h"
//#include "third_party/zlib/zlib.h"
#include "base/util/string_util.h"
#include "base/file/file_util.h"
#include "base/thread/framework_thread.h"
#include "base/macros.h"
#include "base/win32/path_util.h"
//#include "biz/core/sys_util.h"
#include <GdiPlus.h>

//#ifdef DEBUG
//#pragma comment(lib,"LibWinDevID_d.lib")
//#else
//#pragma comment(lib,"LibWinDevID.lib")
//#endif


DWORD g_main_thread_id = 0;

UINT g_show_wnd_forgroud_message = 0;

namespace shared
{
namespace tools
{
const wchar_t KShowWndForgroudMessageID[] = L"ShowWndForgroudMessageIdentifer";

void InitTool()
{
	g_main_thread_id = ::GetCurrentThreadId();
}

//////////////////////////////////////////////////////
// os
int GetOSVersion()
{
	static int os_version_ = -1;
	if (os_version_ < 0)
	{
		OSVERSIONINFO ovi = { sizeof(OSVERSIONINFO) };
		if (!::GetVersionEx(&ovi))
			return -1;

		os_version_ = (ovi.dwMajorVersion * 100 + ovi.dwMinorVersion);
	}
	return os_version_;
}

std::string GetOSVersionString()
{
	int ver = GetOSVersion();
	switch (ver)
	{
	case 500:
		return "Windows 2000";
	case 501:
		return "Windows XP";
	case 502:
		return "Windows Server 2003";
	case 600:
		return "Windows Vista";
	case 601:
		return "Windows 7";
	case 602:
		return "Windows 8";
	default:
		return nbase::StringPrintf("[%d]", ver);
	}
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
bool IsWow64()
{
	BOOL is_wow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if(NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&is_wow64))
			return false;
	}

	return is_wow64 ? true : false;
}
//////////////////////////////////////////////////////
// string
std::string FormatTime(nbase::Time::TimeStruct	t)
{
	return nbase::StringPrintf("%04u-%02u-%02u %02u:%02u:%02u", 
		t.year(), t.month(), t.day_of_month(),
		t.hour(), t.minute(), t.second());
}

std::wstring FormatBytes(double bytes)
{
	if (bytes < 1024 * 1024)
		return nbase::StringPrintf(L"%.2fKB", bytes / 1024);
	else if (bytes < 1024 * 1024 * 1024)
		return nbase::StringPrintf(L"%.2fMB", bytes / 1024 / 1024);
	else
		return nbase::StringPrintf(L"%.2fGB", bytes / 1024 / 1024 / 1024);
}

std::wstring FormatBps(double bytes)
{
	double bits = bytes * 8;
	
	if (bits < 100000)
		return nbase::StringPrintf(L"%dbps", (int)bits);
	else if (bits < 1024 * 1024)
		return nbase::StringPrintf(L"%dKbps", (int)(bits / 1024));
	else if (bits < 1024 * 1024 * 1024)
		return nbase::StringPrintf(L"%.2fMbps", bits / 1024 / 1024);
	else
		return nbase::StringPrintf(L"%.2fGbps", bits / 1024 / 1024 / 1024);
}

void SplitString(const std::string &str, const std::string &seperator, std::vector<std::string> &array) 
{
	array.clear();
	if (str.empty() || seperator.empty())
		return;

	size_t position;
	std::string tmp_str = str;

	position = tmp_str.find(seperator.c_str());        
	while (position != tmp_str.npos)
	{
		if (position)
			array.push_back(tmp_str.substr(0, position));
		tmp_str.erase(0, position + seperator.length());
		position = tmp_str.find(seperator);
	}

	if (!tmp_str.empty())
		array.push_back(tmp_str);
}

void SplitString(const std::wstring &str, const std::wstring &seperator, std::vector<std::wstring> &array) 
{
	array.clear();
	if (str.empty() || seperator.empty())
		return;

	size_t position;
	std::wstring tmp_str = str;

	position = tmp_str.find(seperator.c_str());        
	while (position != tmp_str.npos)
	{
		if (position)
			array.push_back(tmp_str.substr(0, position));
		tmp_str.erase(0, position + seperator.length());
		position = tmp_str.find(seperator);
	}

	if (!tmp_str.empty())
		array.push_back(tmp_str);
}

std::string StringReverse(const std::string &input)
{
	int len = input.size();
	std::string output;
	output.resize(len);
	for (int i = 0; i < len; i++)
	{
		output[i] = input[len - 1 - i];
	}

	return output;
}
std::string FilterUCS4(const std::string& str, bool replace_spaces)
{
	if (GetOSVersion() >= 602)
	{
		return str;
	}
	bool ret = true;
	std::string str_out;
	for (UINT i=0;i<str.size();)
	{
		UINT single_char = str.at(i);
		single_char &= 0xff;
		//int char_count = GetUtf8CharCount(single_char);
		if (single_char >= 0xf0)//过滤0xffff+
		{
			i += 4;
			if (single_char >= 0xf8)
			{
				i++;
			}
			if (single_char >= 0xfc)
			{
				i++;
			}
			ret = false;
			if (replace_spaces)
			{
				str_out += " ";
			}
			continue;
		} 
		else
		{
			if (single_char == 0xe2 && i+2 < str.size())//过滤其他无法显示的字符表情 0x26xx为表情符 0x2700-0x27bf为印刷符
			{
				UINT ch_u = 0x2000;
				UINT single_char_temp = str.at(i+1);
				single_char_temp &= 0x3f;
				ch_u += single_char_temp<<6;
				single_char_temp = str.at(i+2);
				single_char_temp &= 0x3f;
				ch_u += single_char_temp;
				if (GetOSVersion() >= 600)//win7 vista
				{
					if (ch_u == 0x2614 || ch_u == 0x2615 || ch_u == 0x2618 || ch_u == 0x2619
						|| ch_u == 0x2670 || ch_u == 0x2670 || ch_u == 0x2670
						|| (ch_u >= 0x267e && ch_u < 0x27c0) )
					{
						i += 3;
						ret = false;
						if (replace_spaces)
						{
							str_out += " ";
						}
						continue;
					}
				} 
				else//xp
				{
					if ((ch_u >= 0x2614 && ch_u <= 0x2619) || (ch_u >= 0x267e && ch_u < 0x27c0))
					{
						i += 3;
						ret = false;
						if (replace_spaces)
						{
							str_out += " ";
						}
						continue;
					}
				}
			}
			str_out += str.at(i);
			i++;
		}
	}
	return str_out;
}
//////////////////////////////////////////////////////
////regex
//bool IsMobilePhoneNumber(const std::string &str)
//{
//	/*
//	 * 中国移动使用的号段： 139 138 137 136 135 134 159 158 150 151 157 187 188 152 147
//	 * 中国联通使用的号段： 132 131 130 156 155 185 186 145
//	 * 中国电信使用的号段： 133 153 180 189
//	 */
//	std::string regex_str = "1[3,4,5,8]\\d{9}";
//	CRegexpT<char> regexp(regex_str.c_str());
//	MatchResult result = regexp.MatchExact(str.c_str());
//	return (result.IsMatched() != 0);
//}
//bool IsChinaPhoneNumber(const std::string &str)
//{
//	std::string regex_str = "((0086)?|(86)?|(0)?)1[34578][0-9]{9}";
//	CRegexpT<char> regexp(regex_str.c_str());
//	MatchResult result = regexp.MatchExact(str.c_str());
//	return (result.IsMatched() != 0);
//}
//
//bool IsAuthCode(const std::string &str)
//{
//	// 6位数字/字母
//	std::string regex_str = "[0-9a-zA-Z]{6}";
//	CRegexpT<char> regexp(regex_str.c_str());
//	MatchResult result = regexp.MatchExact(str.c_str());
//	return (result.IsMatched() != 0);
//}

//////////////////////////////////////////////////////
// file
std::wstring GetSpecialFolder(int id)
{
	std::wstring path;

	LPITEMIDLIST idl;		
	LPMALLOC shell_malloc;
	if (SHGetMalloc(&shell_malloc) == S_OK)
	{
		if (SHGetSpecialFolderLocation(NULL, id , &idl) == S_OK)
		{
			WCHAR *dir = new WCHAR[MAX_PATH];
			SHGetPathFromIDList(idl, dir);
			shell_malloc->Free(idl);
			path = dir;
			delete dir;
		}
		shell_malloc->Release();
	}

	return path;
}

//std::wstring GetUserAppDataPath(const std::string& uid)
//{
//	std::wstring appdata_path = GetAppDataPath();
//	std::string md5_uid = nbiz::GetMD5(uid);
//	std::wstring wuid = nbase::UTF8ToUTF16(md5_uid);
//	return appdata_path + wuid + L"\\";
//}

std::wstring GetResourcePath(void)
{
	std::wstring app_path = nbase::win32::GetCurrentModuleDirectory();
	return app_path + L"res\\";
}

void MakeExtendPath(std::wstring &path)
{
	std::wstring temp = nbase::StringPrintf(L"\\\\?\\%s", path.c_str());
	path = temp;
}

// 清空达到一定长度的文件
void ClearFile(const wchar_t *path, long file_length_limit)
{
	int fd;
	if ((fd = _wopen(path, O_CREAT | O_RDWR | O_APPEND, _S_IREAD | _S_IWRITE)) < 0)
		return;

	long len = _filelength(fd);
	_close(fd);
	if (len < file_length_limit)
		return;

	if ((fd = _wopen(path, O_CREAT | O_RDWR | O_TRUNC, _S_IREAD | _S_IWRITE)) >= 0)
		_close(fd);
}

bool FilePathIsExist(const std::string &utf8_filepath_in, bool is_directory)
{
	std::wstring filepath_in = nbase::UTF8ToUTF16(utf8_filepath_in);
	return nbase::FilePathIsExist(filepath_in, is_directory);
}

bool FindFile(const std::wstring &dir,const std::wstring &file_desc,std::wstring &file_path)
{
	WIN32_FIND_DATAW find_data;
	HANDLE file_handle;

	file_handle = ::FindFirstFileW(nbase::StringPrintf(L"%s%s",dir.c_str(),L"*.*").c_str(),&find_data);
	if (file_handle != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (find_data.cFileName == nbase::StringPrintf(L"%s.lnk",file_desc.c_str()))
			{
				file_path = nbase::StringPrintf(L"%s%s",dir.c_str(),find_data.cFileName);
				::FindClose(file_handle);
				return true;
			}
			// 如果是子文件夹
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && find_data.cFileName[0] != '.')
			{
				std::wstring in_dir = nbase::StringPrintf(L"%s%s\\",dir.c_str(),find_data.cFileName);
				// 递归遍历
				if(FindFile(in_dir,file_desc,file_path))
					return true;
			}
		} while (::FindNextFileW(file_handle,&find_data));

		::FindClose(file_handle);
		return false;
	}
	else
	{
		return false;
	}
}

//bool CompressFile(const std::wstring &file_path, std::string &compressed_data)
//{
//	bool ret = false;
//	int fd = _wsopen(file_path.c_str(), O_RDONLY | O_BINARY, _SH_DENYNO, _S_IREAD);
//	if (fd < 0)
//		return false;
//
//	long src_file_len = _filelength(fd);
//	long file_len_limit = 10 * 1024 * 1024;
//	if (src_file_len == 0 || src_file_len > file_len_limit)
//	{
//		_close(fd);
//		return false;
//	}
//
//	char *src_buf = new char[file_len_limit];
//	::ZeroMemory(src_buf, file_len_limit);
//	_read(fd, src_buf, src_file_len);
//	_close(fd);
//
//	uLongf zip_buf_len = 1024 * 1024;
//	char *zip_buf = new char[zip_buf_len];
//	::ZeroMemory(zip_buf, zip_buf_len);
//	errno_t err = compress((Bytef*)zip_buf, (uLongf*)&zip_buf_len, (const Bytef*)src_buf, (uLongf)src_file_len);
//	if (err == Z_OK)
//	{
//		compressed_data.clear();
//		compressed_data.assign(zip_buf, zip_buf_len);
//		ret = true;
//	}
//	else
//	{
//		ret = false;
//	}
//
//// 	SAFE_DELETE(src_buf);
//// 	SAFE_DELETE(zip_buf);
//	delete[] src_buf; 
//	delete[] zip_buf; 
//	return ret;
//}
//
//bool CompressData(const std::string &raw_data, std::string &compressed_data)
//{
//	if (raw_data.empty())
//		return false;
//
//	bool ret = false;
//	uLongf raw_data_size = raw_data.size();
//	uLongf zip_buf_len = compressBound(raw_data_size);
//	char *zip_buf = new char[zip_buf_len];
//	::ZeroMemory(zip_buf, zip_buf_len);
//	errno_t err = compress((Bytef*)zip_buf, (uLongf*)&zip_buf_len, 
//		(const Bytef*)raw_data.data(), (uLongf)raw_data.size());
//	if (err == Z_OK)
//	{
//		compressed_data.clear();
//		compressed_data.assign(zip_buf, zip_buf_len);
//		ret = true;
//	}
//	else
//	{
//		ret = false;
//	}
//
//	//SAFE_DELETE(zip_buf);
//	delete[] zip_buf; 
//	return ret;
//}
//
//bool UncompressData(const std::string &compressed_data, std::string &raw_data)
//{
//	if (compressed_data.empty())
//		return false;
//
//	bool ret = false;
//
//	uLongf raw_buf_len = 10 * 1024 * 1024;
//	char *raw_buf = new char[raw_buf_len];
//	::ZeroMemory(raw_buf, raw_buf_len);
//	errno_t err = uncompress((Bytef*)raw_buf, (uLongf*)&raw_buf_len, 
//		(const Bytef*)compressed_data.data(), (uLongf)compressed_data.size());
//	if (err == Z_OK)
//	{
//		raw_data.clear();
//		raw_data.assign(raw_buf, raw_buf_len);
//		ret = true;
//	}
//	else
//	{
//		ret = false;
//	}
//
//	//SAFE_DELETE(raw_buf);
//	delete[] raw_buf; 
//	return ret;
//}

std::wstring EnsureValidFileName(const std::wstring &file_name, const std::wstring &find /* = L"/:*?<>|" */, const std::wstring &replace /* = L"_" */)
{
	std::wstring valid = file_name;
	const size_t replace_len = replace.length();
	const wchar_t *find_ptr = find.c_str();
	const wchar_t *replace_ptr = replace.c_str();

	for (size_t pos = valid.find_first_of(find_ptr, 0); pos != std::wstring::npos && pos < valid.length(); pos = valid.find_first_of(find_ptr, pos))
	{
		valid.replace(pos, 1, replace_ptr, replace_len);
		pos += replace_len;
	}

	return valid;
}

//////////////////////////////////////////////////////
// math
int CalcAvg(const std::vector<int> &vec, bool is_revise/* = true*/)
{
	if (vec.empty())
		return 0;

	int sum = 0;
	size_t count = vec.size();

	if (count <= 5 || !is_revise)
	{
		for (size_t i = 0; i < count; i++)
		{
			sum += vec[i];
		}
		return sum / count;
	}
	// 去掉最小值和最大值
	else
	{
		std::vector<int>::const_iterator minIt = std::min_element(vec.begin(), vec.end());
		std::vector<int>::const_iterator maxIt = std::max_element(vec.begin(), vec.end());

		if (minIt == maxIt)
			return *minIt;

		for (std::vector<int>::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			if (it == minIt || it == maxIt)
				continue;

			sum += *it;
		}
		return sum / (count - 2);
	}
}

int CalcStDev(const std::vector<int> &vec)
{
	if (vec.empty())
		return 0;

	int avg = CalcAvg(vec);

	double square_sum = 0;
	size_t count = vec.size();
	for (size_t i = 0; i < count; i++)
	{
		int diff = abs(vec[i] - avg);
		square_sum += pow((double)diff, 2);
	}
	square_sum = square_sum / count;

	return (int)(sqrt(square_sum));
}

void SafeOpenUrl(const std::wstring &url, int show_cmd)
{
	std::wstring url_temp = url;
	std::wstring url_trim = nbase::StringTrimLeft(url_temp.c_str());
	url_trim = nbase::StringTrimRight(url_trim.c_str());
	HINSTANCE result = ::ShellExecute(NULL, _T("open"), url_trim.c_str(), NULL, NULL, show_cmd);	
	if ((DWORD)result <= 32)
	{
		::ShellExecute(NULL, _T("open"), _T("IEXPLORE.EXE"), url_trim.c_str(), NULL, show_cmd);
	}
}

void SafeOpenUrlEx(const std::string &url, int show_cmd)
{
	std::string url_temp = url;
	std::wstring open_url = nbase::UTF8ToUTF16(url_temp);
	SafeOpenUrl(open_url, show_cmd);
}

bool RunAppWithRedirection(const std::wstring &app_path, const std::wstring &app_cmd, std::string &output)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// ChildIn_Write是子进程的输入句柄，ChildIn_Read是父进程用于写入子进程输入的句柄   
	// ChildOut_Write是子进程的输出句柄，ChildOut_Read是父进程用于读取子进程输出的句柄 
	HANDLE child_in_read = NULL;
	HANDLE child_in_write = NULL;
	HANDLE child_out_read = NULL;
	HANDLE child_out_write = NULL;
	BOOL ret1 = CreatePipe(&child_in_read, &child_in_write, &sa, 0);
	BOOL ret2 = SetHandleInformation(child_in_write, HANDLE_FLAG_INHERIT, 0);
	BOOL ret3 = CreatePipe(&child_out_read, &child_out_write, &sa, 0);
	BOOL ret4 = SetHandleInformation(child_out_read, HANDLE_FLAG_INHERIT, 0);
	
	if (!ret1 || !ret2 || !ret3 || !ret4)
	{
		::CloseHandle(child_in_read);
		::CloseHandle(child_in_write);
		::CloseHandle(child_out_read);
		::CloseHandle(child_out_write);
		return false;
	}

	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = child_out_write;
	si.hStdOutput = child_out_write;
	si.hStdInput = child_in_read;
	si.dwFlags |= STARTF_USESTDHANDLES;   
	PROCESS_INFORMATION pi;

	if (!CreateProcess(app_path.c_str(), (LPWSTR)app_cmd.c_str(), NULL, NULL, TRUE, 
		CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		::CloseHandle(child_in_read);
		::CloseHandle(child_in_write);
		::CloseHandle(child_out_read);
		::CloseHandle(child_out_write);
		return false;
	}
	::CloseHandle(child_in_write);
	::CloseHandle(child_out_write);

	output.clear();
	while (true)
	{
		char buf[32 * 1024] = {0};
		DWORD len;
		if (::ReadFile(child_out_read, buf, 32 * 1024, &len, NULL) == FALSE)
			break;
		output.append(buf, len);
	}
	::CloseHandle(child_in_read);
	::CloseHandle(child_out_read);

	return true;
}

std::string GenerateTimeStamp()
{
	nbase::TimeDelta time_delta = nbase::TimeDelta::FromMicroseconds(nbase::Time::Now().ToInternalValue());
	std::string ret = nbase::StringPrintf("%I64u",time_delta.ToMilliseconds());
	return ret;
}

BOOL DllRegisterServer(LPCTSTR lpszFileName, BOOL bUnregister)
{
	typedef HRESULT (WINAPI * FREG)();

	HMODULE hDLL = ::LoadLibrary(lpszFileName);
	if (NULL == hDLL)
		return FALSE;

	CHAR * lpszFuncName;
	if (!bUnregister)
		lpszFuncName = "DllRegisterServer";
	else
		lpszFuncName = "DllUnregisterServer";

	BOOL bRet = FALSE;
	FREG lpfunc = (FREG)::GetProcAddress(hDLL, lpszFuncName);
	if (lpfunc != NULL)
	{
		lpfunc();
		bRet = TRUE;
	}

	::FreeLibrary(hDLL);

	return bRet;
}

BOOL DllRegisterServer(LPCTSTR lpszFileName)
{
	return DllRegisterServer(lpszFileName, FALSE);
}

BOOL DllUnregisterServer(LPCTSTR lpszFileName)
{
	return DllRegisterServer(lpszFileName, TRUE);
}


//void CalculateFileMd5(const std::wstring& file_path, std::string& md5_value)
//{
//	unsigned char md5[16];
//	char buffer[1024];
//
//	FILE* fp = NULL;
//	_wfopen_s(&fp, file_path.c_str(), L"rb");
//	if (fp == NULL)
//		return;
//
//	MD5_CTX mm;
//	MD5_Init(&mm);
//
//	int len = 0;
//	while (!feof(fp))
//	{
//		len = fread_s(buffer, 1024, sizeof(char), 1024, fp);
//		if(len > 0)
//			MD5_Update(&mm, buffer, len);
//		else
//			break;
//	}
//
//	fclose(fp);
//	MD5_Final(md5, &mm);
//
//	char tmp[4];
//	for (int i = 0; i < 16; i++)
//	{
//		len = sprintf_s(tmp, "%02x", md5[i]);
//		md5_value.append(tmp, len);
//	}
//}

void RegisterShowWndForgroudMessage()
{
	g_show_wnd_forgroud_message = ::RegisterWindowMessage(KShowWndForgroudMessageID);
}

UINT GetShowWndForgroudMessageID()
{
	return g_show_wnd_forgroud_message;
}

bool GetImageCLSID(const wchar_t* format, CLSID* pCLSID)
{
	UINT num = 0; 
	UINT size = 0; 
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL; 
	Gdiplus::GetImageEncodersSize(&num, &size); 

	if (size == 0) 
		return false; 

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL) 
		return false; 
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo); 

	for (UINT i = 0; i < num; ++i) 
	{
		if( wcscmp(pImageCodecInfo[i].MimeType, format) == 0 ) 
		{ 
			*pCLSID = pImageCodecInfo[i].Clsid; 
			free(pImageCodecInfo); 
			return true; 
		} 
	}
	free(pImageCodecInfo); 

	return false; 
}
////获取物理网卡mac地址
//std::string GetDevInfoMac()
//{
//	static std::string mac_id;
//	if (mac_id.empty())
//	{
//		std::wstring mac_id_temp;
//		if (ISC_WinDev_GetID(WDevInfo_MAC, NULL, mac_id_temp))
//		{
//			nbase::UTF16ToUTF8(mac_id_temp, mac_id);
//		}
//	}
//	return mac_id;
//}
////获取硬盘序列号
//std::string GetDevInfoHardDisk()
//{
//	static std::string harddisk_id;
//	if (harddisk_id.empty())
//	{
//		std::wstring harddisk_id_temp;
//		if (ISC_WinDev_GetID(WDevInfo_HardDisk, NULL, harddisk_id_temp))
//		{
//			nbase::UTF16ToUTF8(harddisk_id_temp, harddisk_id);
//		}
//	}
//	return harddisk_id;
//}
////获取主板序列号
//std::string GetDevInfoBIOS()
//{
//	static std::string bios_id;
//	if (bios_id.empty())
//	{
//		std::wstring bios_id_temp;
//		if (ISC_WinDev_GetID(WDevInfo_BIOS, NULL, bios_id_temp))
//		{
//			nbase::UTF16ToUTF8(bios_id_temp, bios_id);
//		}
//	}
//	return bios_id;
//}
////获取设备uuid
//std::string GetDevInfoUUID()
//{
//	static std::string uuid_id;
//	if (uuid_id.empty())
//	{
//		std::wstring uuid_id_temp;
//		if (ISC_WinDev_GetID(WDevInfo_MAC, NULL, uuid_id_temp))
//		{
//			nbase::UTF16ToUTF8(uuid_id_temp, uuid_id);
//		}
//	}
//	return uuid_id;
//}

//FolderHelper
//File
void File::GetFolder( const std::wstring &dir, StringVector &out, bool full_path /*= false*/ )
{
	std::wstring path = dir + L"\\*.*";

	WIN32_FIND_DATAW fd;
	HANDLE hFind = ::FindFirstFileW(path.c_str(), &fd);

	DWORD attr = 0;
	while(hFind != INVALID_HANDLE_VALUE)
	{
		attr = fd.dwFileAttributes;
		if(attr & FILE_ATTRIBUTE_HIDDEN) {
		}
		else if(attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			wchar_t* name = fd.cFileName;
			if( wcscmp(name, L".") == 0 || wcscmp(name, L"..") == 0 ){	
			}
			else
			{
				if(full_path)
					out.push_back( dir + L"\\" + name );
				else
					out.push_back( name );
			}
		}
		
		BOOL bRet = ::FindNextFileW(hFind, &fd);
		if(!bRet)
			break;
	}

	::FindClose(hFind);
}

void File::GetFile( const std::wstring &dir, std::vector<FileItem> &out, bool full_path /*= false*/ )
{
	std::wstring path = dir + L"\\*.*";

	WIN32_FIND_DATAW fd;
	HANDLE hFind = ::FindFirstFileW(path.c_str(), &fd);

	DWORD attr = 0;
	while(hFind != INVALID_HANDLE_VALUE)
	{
		attr = fd.dwFileAttributes;
		if(attr & FILE_ATTRIBUTE_HIDDEN || attr & FILE_ATTRIBUTE_DIRECTORY){
		}
		else
		{
			FileItem fi = { fd.cFileName, fd.nFileSizeLow };
			if(full_path)
				fi.file_ = dir + L"\\" + fd.cFileName;
			out.push_back(fi);
		}

		BOOL bRet = ::FindNextFileW(hFind, &fd);
		if(!bRet)
			break;
	}

	::FindClose(hFind);
}


}//namespace tools
}//namespace shared
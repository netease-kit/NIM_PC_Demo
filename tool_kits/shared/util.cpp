#include "stdafx.h"
#include "util.h"
#include "shared/log.h"
#include "shellapi.h"

//
std::string QString::GetGUID()
{
	const char* uuid = nim::Tool::GetUuid();
	std::string str(uuid);
	QString::NIMFreeBuf((char*)uuid);
	return str;
}

std::string QString::GetMd5(const std::string& input)
{
	const char* md5 = nim::Tool::GetMd5(input);
	std::string out(md5);
	QString::NIMFreeBuf((char*)md5);
	return out;
}

void QString::NIMFreeBuf(void *data)
{
	return nim::Global::FreeBuf(data);
}

//
std::wstring QPath::GetAppPath()
{
	return nbase::win32::GetCurrentModuleDirectory();
}

std::wstring QPath::GetUserAppDataDir(const std::string& app_account)
{
	std::wstring dir;
	const wchar_t* user_appdata_dir = nim::Tool::GetUserAppdataDir(app_account);
	dir = std::wstring(user_appdata_dir);
	QString::NIMFreeBuf((wchar_t*)user_appdata_dir);
	return dir;
}

std::wstring QPath::GetLocalAppDataDir()
{
	std::wstring dir;
	const wchar_t* local_appdata_dir = nim::Tool::GetLocalAppdataDir();
	dir = std::wstring(local_appdata_dir);
	QString::NIMFreeBuf((wchar_t*)local_appdata_dir);
	return dir;
}

std::wstring QPath::GetNimAppDataDir()
{
	std::wstring dir = QPath::GetLocalAppDataDir();
#ifdef _DEBUG
	dir.append(L"Netease\\NIM_Debug\\");
#else
	dir.append(L"Netease\\NIM\\");
#endif

	return dir;
}

//
std::map<std::wstring,std::wstring> QCommand::key_value_;

void QCommand::ParseCommand( const std::wstring &cmd )
{
	std::list<std::wstring> arrays = ui::StringHelper::Split(cmd, L"/");
	for(std::list<std::wstring>::const_iterator i = arrays.begin(); i != arrays.end(); i++)
	{
		std::list<std::wstring> object = ui::StringHelper::Split(*i, L" ");
		assert(object.size() == 2);
		key_value_[ *object.begin() ] = *object.rbegin();
	}
}

std::wstring QCommand::Get( const std::wstring &key )
{
	std::map<std::wstring,std::wstring>::const_iterator i = key_value_.find(key);
	if(i == key_value_.end())
		return L"";
	else
		return i->second;
}

void QCommand::Set( const std::wstring &key, const std::wstring &value )
{
	key_value_[key] = value;
}

void QCommand::Erase(const std::wstring &key)
{
	key_value_.erase(key);
}

bool QCommand::AppStartWidthCommand( const std::wstring &app, const std::wstring &cmd )
{
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app.c_str(), cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}

bool QCommand::RestartApp(const std::wstring &cmd)
{
	wchar_t app[1024] = { 0 };
	GetModuleFileName(NULL, app, 1024);
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app, cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}
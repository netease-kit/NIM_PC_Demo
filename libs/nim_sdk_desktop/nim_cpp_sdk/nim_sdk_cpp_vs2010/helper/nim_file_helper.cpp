/** @file nim_file_helper.cpp
  * @brief SDK文件操作辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/08
  */

#include "nim_file_helper.h"

namespace nim
{

HMODULE GetModuleHandleFromAddress(void* address)
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	DWORD result = ::VirtualQuery(address, &mbi, sizeof(mbi));
	if(result != sizeof(mbi))
	{
		assert(0);
	}
	return static_cast<HMODULE>(mbi.AllocationBase);
}

bool IsModuleHandleValid(HMODULE module_handle)
{
	if (!module_handle)
		return true;
	return module_handle == GetModuleHandleFromAddress(module_handle);
}

HMODULE GetCurrentModuleHandle()
{
	return GetModuleHandleFromAddress(GetCurrentModuleHandle);
}

std::wstring GetModulePathName(HMODULE module_handle)
{
	assert(IsModuleHandleValid(module_handle));
	std::wstring mod_path;
	mod_path.resize(MAX_PATH);
	mod_path.resize(::GetModuleFileNameW(module_handle, &mod_path[0], mod_path.size()));
	return mod_path;
}

const wchar_t kEndChar = L'\0';
const wchar_t kFilePathSeparators[] = L"\\/";

bool IsFilePathSeparator(const wchar_t separator)
{
	if (separator == kEndChar)
		return false;

	size_t len = sizeof(kFilePathSeparators) / sizeof(wchar_t);
	for (size_t i = 0; i < len; i++)
	{
		if (separator == kFilePathSeparators[i])
			return true;
	}

	return false;
}

bool FilePathApartDirectory(const std::wstring &filepath_in,	std::wstring &directory_out)
{
	size_t index = filepath_in.size() - 1;
	if (index <= 0 || filepath_in.size() == 0)
		return false;
	for (; index != 0; index--)
	{
		if (IsFilePathSeparator(filepath_in[index]))
		{
			if (index == filepath_in.size() - 1)
				directory_out = filepath_in;
			else
				directory_out = filepath_in.substr(0, index + 1);
			return true;
		}
	}
	return false;
}

std::wstring GetModuleDirectory(HMODULE module_handle)
{
	assert(IsModuleHandleValid(module_handle));
	std::wstring module_directory;
	if (FilePathApartDirectory(GetModulePathName(module_handle), module_directory))
		return module_directory;
	return L"";
}


std::wstring GetCurrentModuleDirectory()
{
	return GetModuleDirectory(GetCurrentModuleHandle());
}

}
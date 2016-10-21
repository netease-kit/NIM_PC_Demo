/** @file nim_sdk_helper.cpp
  * @brief SDK库辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/09/08
  */

#include "nim_sdk_helper.h"
#include "nim_string_helper.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT

#if !defined (WIN32)
	static const std::string kSdkNimDll = "libnim.so";
// #elif defined (_DEBUG) || defined (DEBUG)
// 	static const std::string kSdkNimDll = "nim_d.dll";
#else
	static const std::string kSdkNimDll = "nim.dll";
#endif

#ifdef WIN32
	HINSTANCE SDKFunction::instance_nim_ = NULL;
#else
	void *SDKFunction::instance_nim_ = NULL;
#endif
std::map<std::string, void*> SDKFunction::function_map;

bool SDKFunction::LoadSdkDll(const char *cur_module_dir)
{
	std::string dir(cur_module_dir);
	dir.append(kSdkNimDll);

#if defined (WIN32)
	std::wstring utf16_dir;
	UTF8ToUTF16(dir, utf16_dir);
	instance_nim_ = ::LoadLibraryW(utf16_dir.c_str());
#else
	instance_nim_ = dlopen(dir.c_str(), RTLD_LAZY);//so必须是绝对路径，如Android系统下是“/data/data/{程序包名}/lib/{so文件名}”
#endif

	if (instance_nim_ == NULL)
	{
		//QLOG_ERR(L"sdk nim load fail {0} {1}") << dir << GetLastError();
		return false;
	}

	return true;
}

void SDKFunction::UnLoadSdkDll()
{
	assert(instance_nim_);
	if (instance_nim_)
	{
		//QLOG_APP(L"client cleanup");
#if defined (WIN32)
		::FreeLibrary(instance_nim_);
#else
		dlclose(instance_nim_);
#endif

		instance_nim_ = NULL;
		//QLOG_APP(L"free nim library");
	}
}

#endif
}
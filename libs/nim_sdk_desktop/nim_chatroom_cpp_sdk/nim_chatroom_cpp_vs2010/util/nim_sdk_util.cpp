/** @file nim_sdk_util.cpp
  * @brief SDK库辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/09/08
  */

#include "nim_sdk_util.h"
#include "nim_string_util.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT

SDKInstance::SDKInstance()
{
	instance_nim_ = NULL;
}

SDKInstance::~SDKInstance()
{
	function_map.clear();
}

bool SDKInstance::LoadSdkDll(const char *cur_module_dir, const char *sdk_dll_file_name)
{
	std::string dir(cur_module_dir);
	dir.append(sdk_dll_file_name);

#if defined (WIN32)
	std::wstring utf16_dir;
	nim::UTF8ToUTF16(dir, utf16_dir);
	instance_nim_ = ::LoadLibraryW(utf16_dir.c_str());
#else
	//int flag = RTLD_GLOBAL | RTLD_LAZY;  //如果是RTLD_GLOBAL，则静态库中定义的全局变量在共享库中名同地址也同
	//int flag = RTLD_LOCAL | RTLD_LAZY;  //如果是RTLD_LOCAL，则静态库中定义的全局变量在共享库中名同地址不同
	instance_nim_ = dlopen(dir.c_str(), RTLD_LAZY);//so必须是绝对路径，如Android系统下是“/data/data/{程序包名}/lib/{so文件名}”
#endif

	if (instance_nim_ == NULL)
	{
		//QLOG_ERR(L"sdk nim load fail {0} {1}") << dir << GetLastError();
		return false;
	}

	return true;
}

void SDKInstance::UnLoadSdkDll()
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
/** @file nim_sdk_util.cpp
  * @brief SDK库辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/08
  */

#include "nim_sdk_util.h"
#include "nim_string_util.h"
#include "callback_proxy.h"
namespace nim
{
	 std::function<void(const std::function< void()>&)> CallbackProxy::docallback_async_ = nullptr;
SDKInstance::SDKInstance()
{
	instance_nim_ = NULL;
	get_proc_address_wrapper_ = [](const std::string&)->void* {return nullptr; };
	function_map.clear();
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
	instance_nim_ = ::LoadLibraryA(dir.c_str());
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
#ifdef WIN32
	get_proc_address_wrapper_ = [&](const std::string& function_name) ->void* {
		return ::GetProcAddress(instance_nim_, function_name.c_str());
	};
#else
	get_proc_address_wrapper_ = [&](const std::string& function_name) ->void* {
		return dlsym(instance_nim_, function_name.c_str());
	};
#endif
	return true;
}

void SDKInstance::UnLoadSdkDll()
{
	assert(instance_nim_);
	CallbackProxy::docallback_async_ = nullptr;
	get_proc_address_wrapper_ = nullptr;
	function_map.clear();
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
void SDKInstance::OnSDKInited()
{
	if (CallbackProxy::docallback_async_ == nullptr)
	{
		typedef void(*sdk_docallback_async)(const std::function<void()>&);
		static sdk_docallback_async nim_sdk_docallback_async = nullptr;
		nim_sdk_docallback_async = (sdk_docallback_async)GetFunction("nim_sdk_docallback_async");
		if (nim_sdk_docallback_async != nullptr)
			CallbackProxy::docallback_async_ = std::move(std::bind(nim_sdk_docallback_async, std::placeholders::_1));
	}	
}
}
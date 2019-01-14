/** @file nim_sdk_util.h
  * @brief SDK库辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/09/08
  */

#ifndef _NIM_SDK_CPP_NIM_SDK_UTIL_H_
#define _NIM_SDK_CPP_NIM_SDK_UTIL_H_

#include <string>
#include <list>
#include <map>
#include <functional>
#include "assert.h"
#include "json.h"
#include "nim_sdk_cpp_wrapper_dll.h"
#ifdef NIM_SDK_DLL_IMPORT
#ifdef WIN32
#include "wtypes.h"
#else
#	include <dlfcn.h>
#endif
#endif
  /** @enum BoolStatus 自定义的布尔值类型数据的替代数据类型 */
NIM_SDK_CPPWRAPPER_DLL_API enum BoolStatus
{
	BS_NOT_INIT = -1,	/**< 未初始化 */
	BS_FALSE = 0,	/**< false */
	BS_TRUE = 1		/**< true */
};
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
/** @class SDKInstance
  * @brief SDK库辅助类，提供加载/卸载SDK库以及获取接口的方法
  */
class NIM_SDK_CPPWRAPPER_DLL_API SDKInstance
{
public:
	SDKInstance();
	virtual ~SDKInstance();

	/** 加载SDK库 */
	bool LoadSdkDll(const char *cur_module_dir, const char *sdk_dll_file_name);

	/** 卸载SDK库 */
	void UnLoadSdkDll();

	/** SDK 初始化完成 */
	void OnSDKInited();

	/** 获得指定接口 */
	void* GetFunction(const std::string& function_name)
	{
		auto it = function_map.find(function_name);
		if (it != function_map.end()) {
			return it->second;
		}

#ifdef WIN32
		void* function_ptr = ::GetProcAddress(instance_nim_, function_name.c_str());
#else
		void* function_ptr = dlsym(instance_nim_, function_name.c_str());
#endif

		assert(function_ptr);
		function_map[function_name] = function_ptr;
		return function_ptr;
	}

private:
#ifdef WIN32
	HINSTANCE instance_nim_;
#else
	void *instance_nim_;
#endif

	std::map<std::string, void*> function_map;
};

static void nim_print_unfound_func_name(char* name)
{
	printf("function [ %s ] not found \n",name);
}

static void unfound_function_holder()
{

}

extern SDKInstance *g_nim_sdk_instance;

#define NIM_SDK_GET_FUNC(function_ptr)	\
						((function_ptr)g_nim_sdk_instance->GetFunction(#function_ptr) != NULL ? ((function_ptr)g_nim_sdk_instance->GetFunction(#function_ptr)) : (nim_print_unfound_func_name(#function_ptr),(function_ptr)unfound_function_holder))
#else
#define NIM_SDK_GET_FUNC(function_ptr) function_ptr
#endif
}

#endif //_NIM_SDK_CPP_NIM_SDK_UTIL_H_
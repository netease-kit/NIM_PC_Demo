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
#ifdef WIN32
#include "wtypes.h"
#else
#	include <dlfcn.h>
#endif

/** @enum BoolStatus 自定义的布尔值类型数据的替代数据类型 */
enum BoolStatus
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
/** @class SDKInstance
 * @brief SDK库辅助类，提供加载/卸载SDK库以及获取接口的方法
 */
class  SDKInstance
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
        void* function_ptr = nullptr;
        auto it = function_map.find(function_name);
        if (it != function_map.end())
        {
            function_ptr = it->second;
        }
        else
        {
            function_ptr = get_proc_address_wrapper_ == nullptr ? nullptr : get_proc_address_wrapper_(function_name.c_str());
            if (function_ptr)
                function_map[function_name] = function_ptr;
        }
        assert(function_ptr);
        return function_ptr;
    }
    
private:
#ifdef WIN32
    HINSTANCE instance_nim_;
#else
    void *instance_nim_;
#endif
    
    std::map<std::string, void*> function_map;
    std::function<void*(const std::string&)> get_proc_address_wrapper_;
};

static void nim_print_unfound_func_name(char* name)
{
    printf("function [ %s ] not found \n",name);
}

static void unfound_function_holder()
{
    
}

}

#define NIM_SDK_GET_FUNC_FROM_INSTANCE(instance,function_ptr) \
    (((function_ptr)instance->GetFunction(#function_ptr)) != NULL ? \
    ((function_ptr)instance->GetFunction(#function_ptr)) : \
    (nim::nim_print_unfound_func_name(#function_ptr),(function_ptr)nim::unfound_function_holder))

#endif //_NIM_SDK_CPP_NIM_SDK_UTIL_H_

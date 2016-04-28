/** @file nim_chatroom_sdk_helper.h
  * @brief 聊天室SDK库辅助方法
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/09/08
  */

#ifndef _NIM_CHATROOM_SDK_CPP_SDK_HELPER_H_
#define _NIM_CHATROOM_SDK_CPP_SDK_HELPER_H_

#include <string>
#include <list>
#include <map>
#include <functional>
#include "assert.h"
#include "wtypes.h"
#include "json.h"

/**
* @namespace nim_chatroom
* @brief 聊天室
*/
namespace nim_chatroom
{

/** @class SDKFunction
  * @brief SDK库辅助类，提供加载/卸载SDK库以及获取接口的方法
  */
class SDKFunction
{
public:
	/** 加载SDK库 */
	static bool LoadSdkDll();

	/** 卸载SDK库 */
	static void UnLoadSdkDll();

	/** 获得指定接口 */
	static void* GetFunction(const std::string& function_name)
	{
		auto it = function_map.find(function_name);
		if (it != function_map.end()) {
			return it->second;
		}
		void* function_ptr = ::GetProcAddress(instance_nim_, function_name.c_str());
		assert(function_ptr);
		function_map[function_name] = function_ptr;
		return function_ptr;
	}

private:
	static HINSTANCE instance_nim_;
	static std::map<std::string, void*> function_map;
};

#define NIM_SDK_GET_FUNC(function_ptr)	\
	((function_ptr)SDKFunction::GetFunction(#function_ptr))

}

#endif //_NIM_SDK_CPP_SDK_HELPER_H_
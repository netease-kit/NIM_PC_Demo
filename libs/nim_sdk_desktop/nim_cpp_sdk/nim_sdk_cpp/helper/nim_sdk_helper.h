#ifndef _NIM_SDK_CPP_SDK_HELPER_H_
#define _NIM_SDK_CPP_SDK_HELPER_H_

#include <string>
#include <list>
#include <map>
#include <functional>
#include "assert.h"
#include "wtypes.h"
#include "json.h"
#include "..\..\..\..\tool_kits/base/thread/thread_manager.h"
#include "..\..\..\..\tool_kits/base/callback/callback.h"

namespace nim
{
/** @brief SDK库辅助方法
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/09/08
  */

inline void PostTaskToUIThread(const StdClosure &closure)
{
	nbase::ThreadManager::PostTask(0, closure);
}

class SDKFunction
{
public:
	static bool LoadSdkDll();
	static void UnLoadSdkDll();

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
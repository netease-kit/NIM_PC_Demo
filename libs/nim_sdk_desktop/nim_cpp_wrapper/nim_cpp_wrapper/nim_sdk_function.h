#pragma once

#include <string>
#include <list>
#include <map>

namespace nim
{

inline void PostTaskToUIThread(const StdClosure &closure)
{
	nbase::ThreadManager::PostTask(kThreadUI, closure);
}

std::string ListToJson(const std::list<std::string>& list);
std::string PCharToString(const char* str);

class NimSdk
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

#define NIM_SDK_Get_Function(function_ptr)	\
	((function_ptr)NimSdk::GetFunction(#function_ptr))

}
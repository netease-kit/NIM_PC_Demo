#ifndef _NIM_SDK_CPP_NIM_SDK_LOADER_HELPER_H_
#define _NIM_SDK_CPP_NIM_SDK_LOADER_HELPER_H_
#include "public_define/nim_util_include.h"
#include "include/depend_lib/include/nim_sdk_util.h"
#include "include/depend_lib/include/callback_proxy.h"
#include "include/depend_lib/include/nim_string_util.h"
#include "include/depend_lib/include/nim_json_util.h"

namespace nim
{
	extern nim::SDKInstance* nim_sdk_instance;
}
#define NIM_SDK_GET_FUNC(function_ptr) \
NIM_SDK_GET_FUNC_FROM_INSTANCE(nim_sdk_instance,function_ptr)
#endif
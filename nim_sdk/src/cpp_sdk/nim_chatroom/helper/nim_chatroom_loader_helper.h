#ifndef _NIM_SDK_CPP_NIM_CHATROOM_SDK_LOADER_HELPER_H_
#define _NIM_SDK_CPP_NIM_CHATROOM_SDK_LOADER_HELPER_H_
#include "include/depend_lib/include/nim_sdk_util.h"
#include "include/depend_lib/include/callback_proxy.h"
#include "include/depend_lib/include/nim_string_util.h"
#include "include/depend_lib/include/nim_json_util.h"
#ifndef DEF_UNDER_NO_NAMESPACE
namespace nim_chatroom
{
#endif //DEF_UNDER_NO_NAMESPACE
#include "public_define/defines/nim_chatroom_define/nim_res_code_def.h"
#ifndef DEF_UNDER_NO_NAMESPACE
}
#endif //DEF_UNDER_NO_NAMESPACE

namespace nim_chatroom
{
	extern nim::SDKInstance* nim_chatroom_sdk_instance;
}
#define NIM_CHATROOM_SDK_GET_FUNC(function_ptr) \
NIM_SDK_GET_FUNC_FROM_INSTANCE(nim_chatroom_sdk_instance,function_ptr)
#endif
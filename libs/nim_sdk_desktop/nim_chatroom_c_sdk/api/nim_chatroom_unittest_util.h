#ifndef  __NIM_CHATROOM_UNITEST_UTIL_H__
#define __NIM_CHATROOM_UNITEST_UTIL_H__

#include "gtest/gtest.h"
#include "util/ftest_lib_util.h"


#ifdef FTEST_TESTING_DLL_IMPORT

extern ftest_dll_handle_t g_nim_chatroom_sdk_dll_handle;
#define FTEST_NIM_CHATROOM_INVOKE(function_ptr) FTEST_GET_FUNC(&g_nim_chatroom_sdk_dll_handle, function_ptr)

#else
#define FTEST_NIM_CHATROOM_INVOKE(function_ptr) FTEST_GET_FUNC(function_ptr)
#endif


bool nimChatroomLoad();
void nimChatroomUnload();


#endif //__NIM_CHATROOM_UNITEST_UTIL_H__

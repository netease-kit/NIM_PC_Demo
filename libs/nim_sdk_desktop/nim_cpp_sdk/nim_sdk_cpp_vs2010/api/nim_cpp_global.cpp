/** @file nim_cpp_global.cpp
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#include "nim_cpp_global.h"
#include "nim_sdk_helper.h"

namespace nim
{

typedef	void (*nim_global_free_str_buf)(char *str);
typedef	void (*nim_global_free_buf)(void *data);
typedef void (*nim_global_set_proxy)(NIMProxyType, const char*, int, const char*, const char*);

void Global::FreeStrBuf(char *str)
{
	return NIM_SDK_GET_FUNC(nim_global_free_str_buf)(str);
}

void Global::FreeBuf(void *data)
{
	return NIM_SDK_GET_FUNC(nim_global_free_buf)(data);
}

void Global::SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
{
	return NIM_SDK_GET_FUNC(nim_global_set_proxy)(type, host.c_str(), port, user.c_str(), password.c_str());
}

}
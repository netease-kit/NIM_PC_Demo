#include "nim_global_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

void Global::FreeStrBuf(char *str)
{
	return NIM_SDK_Get_Function(nim_global_free_str_buf)(str);
}

void Global::FreeBuf(void *data)
{
	return NIM_SDK_Get_Function(nim_global_free_buf)(data);
}

}
/** @file nim_global.h
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_API_NIM_GLOBAL_H_
#define NIM_SDK_DLL_API_NIM_GLOBAL_H_

#include "nim_sdk_dll.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_global_free_str_buf(char *str)    
  * 释放SDK内部分配的内存   
  * @param[in] str 由SDK内部分配内存的字符串
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_global_free_str_buf(char *str);

/** @fn void nim_global_free_buf(void *data) 
  * 释放SDK内部分配的内存   
  * @param[in] data 由SDK内部分配的内存
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_global_free_buf(void *data);
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_GLOBAL_H_
#pragma once


namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_global_def.h"

/** @class Global
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Global
{

private:
	typedef	void (*nim_global_free_str_buf)(char *str);
	typedef	void (*nim_global_free_buf)(void *data);


public:
	/** 
	* 释放SDK内部分配的内存
	* @param[in] str 由SDK内部分配内存的字符串
	* @return void 无返回值
	*/
	static void FreeStrBuf(char *str);

	/** 
	* 释放SDK内部分配的内存
	* @param[in] data 由SDK内部分配的内存
	* @return void 无返回值
	*/
	static void FreeBuf(void *data);
};

} 
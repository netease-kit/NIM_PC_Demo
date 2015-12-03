#ifndef _NIM_SDK_CPP_GLOBAL_H_
#define _NIM_SDK_CPP_GLOBAL_H_

namespace nim
{

#include "nim_global_def.h"

/** @class Global
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */
class Global
{

public:
	/** @fn void FreeStrBuf(char *str)
	* 释放SDK内部分配的内存
	* @param[in] str 由SDK内部分配内存的字符串
	* @return void 无返回值
	*/
	static void FreeStrBuf(char *str);

	/** @fn  void FreeBuf(void *data)
	* 释放SDK内部分配的内存
	* @param[in] data 由SDK内部分配的内存
	* @return void 无返回值
	*/
	static void FreeBuf(void *data);
};

} 

#endif //_NIM_SDK_CPP_GLOBAL_H_
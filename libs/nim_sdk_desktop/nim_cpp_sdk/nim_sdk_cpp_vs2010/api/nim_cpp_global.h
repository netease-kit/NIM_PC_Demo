/** @file nim_cpp_global.h
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_GLOBAL_H_
#define _NIM_SDK_CPP_GLOBAL_H_

#include <string>

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

#include "nim_global_def.h"

/** @class Global
  * @brief NIM SDK提供的一些全局接口
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
	
	/** @fn void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
    * 设置SDK统一的网络代理。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。   
    * @param[in] type 代理类型，见NIMProxyType定义
    * @param[in] host 代理地址
    * @param[in] port 代理端口
    * @param[in] user 代理用户名
    * @param[in] password 代理密码
    * @return void 无返回值
    */
	static void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password);
};

} 

#endif //_NIM_SDK_CPP_GLOBAL_H_
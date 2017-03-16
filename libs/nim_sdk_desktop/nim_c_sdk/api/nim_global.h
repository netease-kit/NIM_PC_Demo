/** @file nim_global.h
  * @brief NIM SDK提供的一些全局接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_API_NIM_GLOBAL_H_
#define NIM_SDK_DLL_API_NIM_GLOBAL_H_

#include "nim_sdk_dll.h"
#include "nim_global_def.h"

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

#if NIMAPI_UNDER_WIN_DESKTOP_ONLY
/** @fn void nim_global_set_proxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password)
  * 设置SDK统一的网络代理。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。   
  * @param[in] type 代理类型，见NIMProxyType定义,其中音视频和白板暂时只支持kNIMProxySocks5代理
  * @param[in] host 代理地址
  * @param[in] port 代理端口
  * @param[in] user 代理用户名
  * @param[in] password 代理密码
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_global_set_proxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);

/** @fn void nim_global_detect_proxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password, nim_global_detect_proxy_cb_func cb, const void *user_data)
* 测试代理。
* @param[in] type 代理类型，见NIMProxyType定义,其中音视频和白板暂时只支持kNIMProxySocks5代理
* @param[in] host 代理地址
* @param[in] port 代理端口
* @param[in] user 代理用户名
* @param[in] password 代理密码
* @param[in] cb 回调函数,定义见nim_global_def.h
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
*/
NIM_SDK_DLL_API	void nim_global_detect_proxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password, nim_global_detect_proxy_cb_func cb, const void *user_data);
#endif

/** @fn void nim_global_reg_sdk_log_cb(const char *json_extension, nim_sdk_log_cb_func cb, const void *user_data)
* (PC SDK该接口无效)注册输出sdk log回调
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb 输出sdk log的回调函数， nim_sdk_log_cb_func回调函数定义见nim_global_def.h
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
*/
NIM_SDK_DLL_API void nim_global_reg_sdk_log_cb(const char *json_extension, nim_sdk_log_cb_func cb, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_GLOBAL_H_
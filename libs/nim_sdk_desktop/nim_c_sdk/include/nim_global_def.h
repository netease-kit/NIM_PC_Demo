/** @file nim_global_def.h
  * @brief NIM SDK提供的一些全局定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_GLOBAL_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_GLOBAL_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif
/** @name json params for NIMResCode, see nim_res_code_def.h 
  * @{
  */ 
static const char *kNIMResCode		= "err_code";		/**< int, NIMResCode */
/** @}*/ //json params for NIMResCode, see nim_res_code_def.h

/** @typedef void (*nim_json_transport_cb_func)(const char *json_params, const void *user_data)  
  * nim callback function with json parameters from sdk and user's data from app
  * @param[out] json_params 输出的json字符串内容
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_json_transport_cb_func)(const char *json_params, const void *user_data);

/** @enum NIMSDKLogLevel NIM SDK log级别，级别越高，log越详细 */
enum NIMSDKLogLevel
{
	kNIMSDKLogLevelFatal = 1,	/**< SDK Fatal级别Log*/
	kNIMSDKLogLevelError = 2,	/**< SDK Error级别Log*/
	kNIMSDKLogLevelWarn = 3,	/**< SDK Warn级别Log*/
	kNIMSDKLogLevelApp = 5,	/**< SDK应用级别Log，正式发布时为了精简sdk log，可采用此级别*/
	kNIMSDKLogLevelPro = 6,	/**< SDK过程级别Log，更加详细，更有利于开发调试*/
};

/** @typedef void (*nim_sdk_log_cb_func)(const char *log)
* 输出sdk log回调
* @param[out] log_level log级别，见NIMSDKLogLevel
* @param[out] log log内容
* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_sdk_log_cb_func)(int log_level, const char *log, const void *user_data);

/** @enum NIMProxyType 代理类型 */
enum NIMProxyType
{
	kNIMProxyNone		= 0,	/**< 不使用代理*/
	kNIMProxyHttp11		= 1,	/**< HTTP 1.1 Proxy（暂不支持）*/
	kNIMProxySocks4		= 4,	/**< Socks4 Proxy*/
	kNIMProxySocks4a	= 5,	/**< Socks4a Proxy*/
	kNIMProxySocks5		= 6,	/**< Socks5 Proxy*/
};

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_GLOBAL_DEF_H_
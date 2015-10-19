/** @file nim_global_def.h
  * @brief NIM SDK提供的一些全局定义
  * @copyright (c) 2015, NetEase Inc. All rights reserved
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

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_GLOBAL_DEF_H_
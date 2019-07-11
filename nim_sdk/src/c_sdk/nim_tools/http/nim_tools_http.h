/** @file nim_tools_http.h
  * @brief NIM HTTP提供的http传输相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/4/30
  */

#ifndef NIM_TOOLS_HTTP_H
#define NIM_TOOLS_HTTP_H

#include "net/base/net_export.h"
#include "nim_tools_http_def.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn void nim_http_init()
  * NIM HTTP 初始化
  * @return void 无返回值
  */
NET_EXPORT void nim_http_init();

/** @fn void nim_http_uninit()
  * NIM HTTP 反初始化
  * @return void 无返回值
  */
NET_EXPORT void nim_http_uninit();

/** @fn void nim_http_init_log(const char* log_file_path)
* NIM HTTP 设置日志文件路径
* @param[in] log_file_path	日志文件保存路径
* @return void 无返回值
*/
NET_EXPORT void nim_http_init_log(const char* log_file_path);

/** @fn bool nim_http_is_init_log()
* NIM HTTP 是否设置过日志路径
* @return bool 是否设置过日志路径
*/
NET_EXPORT bool nim_http_is_init_log();

/** @fn HttpRequestID nim_http_post_request(HttpRequestHandle)
* NIM HTTP 发起任务
* @param[in] request_handle	http任务句柄
* @return HttpRequestID					任务id
*/
NET_EXPORT HttpRequestID nim_http_post_request(HttpRequestHandle request_handle);

/** @fn void nim_http_remove_request(HttpRequestID http_request_id)
* NIM HTTP 取消任务
* @param[in] http_request_id	任务id
* @return void				无返回值
*/
NET_EXPORT void nim_http_remove_request(HttpRequestID http_request_id);

/** @fn HttpRequestHandle nim_http_create_download_file_request(const char *url, const char *download_file_path, nim_http_request_completed_cb complete_cb, const void *user_data)
  * NIM HTTP 创建下载文件任务
  * @param[in] url					资源地址
  * @param[in] download_file_path	下载文件保存的本地路径
  * @param[in] complete_cb			结束回调
  * @param[in] user_data			自定义数据
  * @return HttpRequestHandle		http任务句柄
  */
NET_EXPORT HttpRequestHandle nim_http_create_download_file_request(const char *url, const char *download_file_path,
															nim_http_request_completed_cb complete_cb, const void *user_data);

/** @fn HttpRequestHandle nim_http_create_download_file_range_request(const char *url, const char *download_file_path, __int64 range_start, nim_http_request_completed_cb complete_cb, const void *user_data)
  * NIM HTTP 创建下载文件任务，支持断点续传
  * @param[in] url					资源地址
  * @param[in] download_file_path	下载文件保存的本地路径
  * @param[in] range_start			下载文件的起始点
  * @param[in] complete_cb			结束回调
  * @param[in] user_data			自定义数据
  * @return HttpRequestHandle		http任务句柄
  */
NET_EXPORT HttpRequestHandle nim_http_create_download_file_range_request(const char *url, const char *download_file_path,
															__int64 range_start, nim_http_request_completed_cb complete_cb, const void *user_data);

/** @fn HttpRequestHandle nim_http_create_request(const char* url, const char* post_body, size_t post_body_size, nim_http_request_response_cb response_cb, const void* user_data)
  * NIM HTTP 创建任务
  * @param[in] url				资源地址
  * @param[in] post_body		上传内容
  * @param[in] post_body_size	上传内容大小
  * @param[in] response_cb		结束回调，响应实体内容
  * @param[in] user_data		自定义数据
  * @return HttpRequestHandle	http任务句柄
  */
NET_EXPORT HttpRequestHandle nim_http_create_request(const char* url, const char* post_body, size_t post_body_size, 
															nim_http_request_response_cb response_cb, const void* user_data);

/** @fn void nim_http_add_request_header(HttpRequestHandle request_handle, const char* key, const char* value)
  * NIM HTTP 创建任务
  * @param[in] request_handle	http任务句柄
  * @param[in] key				头的key
  * @param[in] value			头的value
  * @return void				无返回值
  */
NET_EXPORT void nim_http_add_request_header(HttpRequestHandle request_handle, const char* key, const char* value);

/** @fn void nim_http_set_request_progress_cb(HttpRequestHandle request_handle, nim_http_request_progress_cb progress_callback, const void* user_data)
  * NIM HTTP 设置进度回调
  * @param[in] request_handle		http任务句柄
  * @param[in] progress_callback	进度回调函数
  * @param[in] user_data			自定义数据
  * @return void					无返回值
  */
NET_EXPORT void nim_http_set_request_progress_cb(HttpRequestHandle request_handle, nim_http_request_progress_cb progress_callback, const void* user_data);

/** @fn void nim_http_set_request_speed_cb(HttpRequestHandle request_handle, nim_http_request_speed_cb speed_callback, const void* user_data)
  * NIM HTTP 设置实时速度回调
  * @param[in] request_handle		http任务句柄
  * @param[in] speed_callback		速度回调函数
  * @param[in] user_data			自定义数据
  * @return void					无返回值
  */
NET_EXPORT void nim_http_set_request_speed_cb(HttpRequestHandle request_handle, nim_http_request_speed_cb speed_callback, const void* user_data);

/** @fn void nim_http_set_request_transfer_cb(HttpRequestHandle request_handle, nim_http_request_transfer_cb transfer_callback, const void* user_data)
  * NIM HTTP 设置传输信息回调
  * @param[in] request_handle		http任务句柄
  * @param[in] transfer_callback	结束回调，获取实际传输信息
  * @param[in] user_data			自定义数据
  * @return void					无返回值
  */
NET_EXPORT void nim_http_set_request_transfer_cb(HttpRequestHandle request_handle, nim_http_request_transfer_cb transfer_callback, const void* user_data);

/** @fn void nim_http_set_request_method_as_post(HttpRequestHandle request_handle)
  * NIM HTTP 强制设置http请求方法为post
  * @param[in] request_handle	http任务句柄
  * @return void				无返回值
  */
NET_EXPORT void nim_http_set_request_method_as_post(HttpRequestHandle request_handle);

/** @fn void nim_http_set_timeout(HttpRequestHandle request_handle, int timeout_ms)
  * NIM HTTP 设置超时
  * @param[in] request_handle	http任务句柄
  * @param[in] timeout_ms		超时时间，单位是毫秒
  * @return void				无返回值
  */
NET_EXPORT void nim_http_set_timeout(HttpRequestHandle request_handle, int timeout_ms);

/** @fn void nim_http_set_low_speed(HttpRequestHandle request_handle, int low_speed_limit, int low_speed_time)
  * NIM HTTP 设置最低传输速度
  * @param[in] request_handle	http任务句柄
  * @param[in] low_speed_limit	最低传输的字节数(大于0)
  * @param[in] low_speed_time	多少秒传输速度不得低于low_speed_limit，不满足条件则会终止传输(大于0)
  * @return void				无返回值
  */
NET_EXPORT void nim_http_set_low_speed(HttpRequestHandle request_handle, int low_speed_limit, int low_speed_time);

/** @fn void nim_http_set_proxy(HttpRequestHandle request_handle, int type, const char* host, short port, const char* user, const char* pass)
  * NIM HTTP 设置代理
  * @param[in] request_handle	http任务句柄
  * @param[in] type				代理类型NIMProxyType
  * @param[in] host				代理地址
  * @param[in] port				代理端口
  * @param[in] user				代理账号
  * @param[in] pass				代理密码
  * @return void				无返回值
  */
NET_EXPORT void nim_http_set_proxy(HttpRequestHandle request_handle, int type, const char* host, short port, const char* user, const char* pass);

/** @fn const char* const nim_http_get_response_head(HttpRequestID http_request_id)
* NIM HTTP 读取应答的http头信息
* @param[in] http_request_id	任务id
* @return char*					头信息
*/
NET_EXPORT const char* const nim_http_get_response_head(HttpRequestID http_request_id);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif // NIM_TOOLS_HTTP_H
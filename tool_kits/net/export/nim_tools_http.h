/** @file nim_http.h
  * @brief NIM HTTP提供的http传输相关接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/4/30
  */

#ifndef NET_NIM_HTTP_H_
#define NET_NIM_HTTP_H_

#include "net/base/net_export.h"

extern "C"
{
/** @enum NIMProxyType 代理类型 */
enum NIMProxyType
{
	kNIMProxyNone		= 0,	/**< 不使用代理*/
	kNIMProxyHttp11		= 1,	/**< HTTP 1.1 Proxy*/
	kNIMProxySocks4		= 4,	/**< Socks4 Proxy*/
	kNIMProxySocks4a	= 5,	/**< Socks4a Proxy*/
	kNIMProxySocks5		= 6,	/**< Socks5 Proxy*/
};

/** @typedef void* HttpRequestHandle
  * http任务句柄
  */
typedef void* HttpRequestHandle;

/** @typedef void (*nim_http_request_completed_cb)(const void* user_data, bool, int)
  * nim callback function that has been registered in nim_http_create_*** API
  * @param[out] user_data		回传的自定义数据
  * @param[out] result			传输结果，true代表传输成功，false代表传输失败
  * @param[out] response_code	http响应码
  * @return void				无返回值
  */
typedef void (*nim_http_request_completed_cb)(const void* user_data, bool result, int response_code);

/** @typedef void (*nim_http_request_response_cb)(const void* user_data, bool, int)
  * nim callback function that has been registered in nim_http_create_*** API
  * @param[out] user_data			回传的自定义数据
  * @param[out] result				传输结果，true代表传输成功，false代表传输失败
  * @param[out] response_code		http响应码
  * @param[out] response_content	http响应实体内容
  * @return void					无返回值
  */
typedef void (*nim_http_request_response_cb)(const void* user_data, bool result, int response_code, const char* response_content);

/** @typedef void (*nim_http_request_progress_cb)(const void* user_data, double uploaded_size, double total_upload_size, double downloaded_size, double total_download_size)
  * nim callback function that has been registered in nim_http_set_request_progress_cb API
  * @param[out] user_data				回传的自定义数据
  * @param[out] uploaded_size			已经上传的字节数
  * @param[out] total_upload_size		总的待上传的字节数
  * @param[out] downloaded_size			已经下载的字节数
  * @param[out] total_download_size		总的待下载的字节数
  * @return void						无返回值
  */
typedef void (*nim_http_request_progress_cb)(const void* user_data, double uploaded_size, double total_upload_size, double downloaded_size, double total_download_size);

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

/** @fn int nim_http_post_request(HttpRequestHandle)
* NIM HTTP 发起任务
* @param[in] request_handle	http任务句柄
* @return int					任务id
*/
NET_EXPORT int nim_http_post_request(HttpRequestHandle);

/** @fn void nim_http_remove_request(int http_request_id)
* NIM HTTP 取消任务
* @param[in] http_request_id	任务id
* @return void				无返回值
*/
NET_EXPORT void nim_http_remove_request(int http_request_id);

/** @fn HttpRequestHandle nim_http_create_download_file_request(const char* url, const char *download_file_path, nim_http_request_completed_cb complete_cb, const void* user_data)
  * NIM HTTP 创建下载文件任务
  * @param[in] url					资源地址
  * @param[in] download_file_path	下载文件保存的本地路径
  * @param[in] complete_cb			结束回调
  * @param[in] user_data			自定义数据
  * @return HttpRequestHandle		http任务句柄
  */
NET_EXPORT HttpRequestHandle nim_http_create_download_file_request(const char* url, const char *download_file_path,
															nim_http_request_completed_cb complete_cb, const void* user_data);

/** @fn HttpRequestHandle nim_http_create_download_file_range_request(const char* url, const char *download_file_path, __int64 range_start, nim_http_request_completed_cb complete_cb, const void* user_data)
  * NIM HTTP 创建下载文件任务，支持断点续传
  * @param[in] url					资源地址
  * @param[in] download_file_path	下载文件保存的本地路径
  * @param[in] range_start			下载文件的起始点
  * @param[in] complete_cb			结束回调
  * @param[in] user_data			自定义数据
  * @return HttpRequestHandle		http任务句柄
  */
NET_EXPORT HttpRequestHandle nim_http_create_download_file_range_request(const char* url, const char *download_file_path,
															__int64 range_start, nim_http_request_completed_cb complete_cb, const void* user_data);

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

}

#endif // NET_NIM_HTTP_H_
/** @file nim_nos_def.h
  * @brief nos define
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_

#include "../util/nim_base_types.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_nos_download_cb_func)(int rescode, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data)
  * nim callback function for nos http download
  * @param[out] rescode 			下载结果，一切正常200
  * @param[out] file_path			下载资源文件本地绝对路径
  * @param[out] call_id				如果下载的是消息中的资源，则为消息所属的会话id，否则为空
  * @param[out] res_id				如果下载的是消息中的资源，则为消息id，否则为空
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_nos_download_cb_func)(int rescode, const char *file_path, const char *call_id, const char *res_id, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_download_prg_cb_func)(int64_t downloaded_size, int64_t file_size, const char *json_extension, const void *user_data)
  * nim callback function for nos http download progress
  * @param[out] downloaded_size 		已下载数据大小
  * @param[out] file_size				文件大小
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_download_prg_cb_func)(int64_t downloaded_size, int64_t file_size, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_cb_func)(int rescode, const char *url, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload
  * @param[out] rescode 			上传结果，一切正常200
  * @param[out] url					url
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_cb_func)(int rescode, const char *url, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_prg_cb_func)(int64_t uploaded_size, int64_t file_size, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload progress
  * @param[out] uploaded_size 			已上传数据大小
  * @param[out] file_size				文件大小
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_prg_cb_func)(int64_t uploaded_size, int64_t file_size, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_
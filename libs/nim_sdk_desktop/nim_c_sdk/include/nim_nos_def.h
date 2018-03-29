/** @file nim_nos_def.h
  * @brief NOS云存储服务 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_

#include "../util/nim_base_types.h"
#include "../util/nim_build_config.h"

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

/** @typedef void (*nim_nos_download_speed_cb_func)(int64_t download_speed, const char *json_extension, const void *user_data)
  * nim callback function for nos http download speed
  * @param[out] download_speed 		下载速度
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_download_speed_cb_func)(int64_t download_speed, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_download_info_cb_func)(int64_t actual_download_size, int64_t download_speed, const char *json_extension, const void *user_data)
  * nim callback function for nos http download info
  * @param[out] actual_download_size 		最终实际下载数据大小
  * @param[out] download_speed				平均下载速度
  * @param[out] json_extension		json扩展数据（备用）
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_download_info_cb_func)(int64_t actual_download_size, int64_t download_speed, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_cb_func)(int rescode, const char *url, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload
  * @param[out] rescode 			上传结果，一切正常200
  * @param[out] url					url
  * @param[out] json_extension		json扩展数据，解析字段见 nim_nos_def.h "NOS扩展上传回调参数json_extension, Json key for upload cb"
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_cb_func)(int rescode, const char *url, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_prg_cb_func)(int64_t uploaded_size, int64_t file_size, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload progress
  * @param[out] uploaded_size 			已上传数据大小
  * @param[out] file_size				文件大小
  * @param[out] json_extension		json扩展数据，返回kNIMNosResId
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_prg_cb_func)(int64_t uploaded_size, int64_t file_size, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_speed_cb_func)(int64_t upload_speed, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload speed
  * @param[out] upload_speed 		上传速度
  * @param[out] json_extension		json扩展数据，返回kNIMNosResId
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_speed_cb_func)(int64_t upload_speed, const char *json_extension, const void *user_data);

/** @typedef void (*nim_nos_upload_info_cb_func)(int64_t actual_upload_size, int64_t upload_speed, const char *json_extension, const void *user_data)
  * nim callback function for nos http upload info
  * @param[out] actual_upload_size 		最终实际上传数据大小
  * @param[out] upload_speed			平均上传速度
  * @param[out] json_extension		json扩展数据，返回kNIMNosResId
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_nos_upload_info_cb_func)(int64_t actual_upload_size, int64_t upload_speed, const char *json_extension, const void *user_data);

/** @enum NIMNosUploadType NOS扩展类型  */
enum NIMNosUploadType
{
	kNIMNosUploadTypeNormal		= 0,			/**< 普通文件上传 */
	kNIMNosUploadTypeDocTrans	= 1,			/**< 文档转换上传 */
};

/** @name NOS扩展上传\下载接口参数json_extension Json key for nim_nos_upload_ex && nim_nos_download_ex */ 
static const char *kNIMNosLowLimit				= "low_limit";	/**< int HTTP通用配置，传输速度，每秒字节数（默认10）*/
static const char *kNIMNosLowTime				= "low_time";	/**< int HTTP通用配置，传输过程中当low_time秒时间内传输速度小于low_limit时(字节每秒)，下载任务会返回超时而取消（默认60）*/
static const char *kNIMNosTimeout				= "timeout";	/**< int HTTP通用配置，超时时间，单位ms，下载时最小10000，上传时最小30000，不设置默认30000*/
static const char *kNIMNosTaskId				= "task_id";		/**< string HTTP通用配置，任务ID，上传下载断点续传必填，如果传入的ID是曾经未完成的需要续传功能的传输任务的ID，则会开始续传（用户需要保证ID的唯一性） */
static const char *kNIMNosNeedContinueTrans		= "continue_trans"; /**< bool HTTP通用配置，任务是否需要续传功能*/
static const char *kNIMNosFileSize				= "download_filesize"; /**< int64 HTTP下载任务的文件大小，需要续传功能必填，单位Byte，其他情况不需要填*/
static const char *kNIMNosSaveAsFilePath		= "saveas_filepath"; /**< string HTTP下载任务的文件存放本地路径，不填则默认路径回调中返回*/
static const char *kNIMNosUploadType			= "upload_type";	/**< NIMNosUploadType HTTP上传任务的类型 */
static const char *kNIMNosDocTransName			= "name"; 			/**< string (HTTP上传转码文档使用)名称*/
static const char *kNIMNosDocTransSourceType	= "source_type"; 	/**< NIMDocTranscodingFileType (HTTP上传转码文档使用)转码源文档的文件类型, nim_doc_trans_def.h */
static const char *kNIMNosDocTransPicType		= "pic_type"; 		/**< NIMDocTranscodingImageType (HTTP上传转码文档使用)转码目标图片的文件类型, nim_doc_trans_def.h */
static const char *kNIMNosDocTransExt			= "doc_trans_ext";	/**< string (HTTP上传转码文档使用)文档转换时的扩展参数，在成功后能查询到 */
/** @}*/ //NOS扩展上传\下载接口参数json_extension Json key for nim_nos_upload_ex && nim_nos_download_ex

/** @name NOS扩展上传回调参数json_extension, Json key for upload cb */
static const char *kNIMNosResId		= "res_id";		/**< string 上传文件的id，如果是文档转换则为服务器的文档id */
#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
static const char *kNIMNosCallId	= "call_id";	/**< string 上传文件的会话id */
#endif
/** @}*/ //NOS扩展上传回调参数json_extension, Json key for upload cb


#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NOS_DEF_H_
/** @file nim_doc_trans_def.h
  * @brief 文档转换 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author gq
  * @date 2016/12/12
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_DOC_TRANS_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_DOC_TRANS_DEF_H_

#include "../util/nim_base_types.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @enum NIMDocTranscodingFileType 转码源文件格式 */
enum NIMDocTranscodingFileType
{
	kNIMDocTranscodingFileTypePPT	= 1,	/**< ppt */
	kNIMDocTranscodingFileTypePPTX	= 2,	/**< pptx */
	kNIMDocTranscodingFileTypePDF	= 3,	/**< pdf */
};


/** @enum NIMDocTranscodingImageType 转码目标图像文件类型 */
enum NIMDocTranscodingImageType
{
	kNIMDocTranscodingImageTypeJPG	= 10,	/**< 转码为 jpg 图片 */
	kNIMDocTranscodingImageTypePNG	= 11,	/**< 转码为 png 图片 */
};


/** @enum NIMDocTranscodingQuality 转码图像清晰度 */
enum NIMDocTranscodingQuality
{
	kNIMDocTranscodingQualityHigh		= 1,	/**< 高清转码质量 */
	kNIMDocTranscodingQualityMedium		= 2,	/**< 中等转码质量 */
	kNIMDocTranscodingQualityLow		= 3,	/**< 低清转码质量 */
};

/** @enum NIMDocContinueUploadState 续传过程状态 */
enum NIMDocContinueUploadState
{
	kNIMDocContinueUploadNone		= 0,	/**< 没有进行过上传 */
	kNIMDocContinueUploading		= 1,	/**< 文件续传中 */
	kNIMDocContinueUploadFailed		= 2,	/**< 文件续传失败 */
	kNIMDocContinueUploadCompleted	= 3,	/**< 文件续传完成 */
};

/** @enum NIMDocTranscodingState 转码过程状态 */
enum NIMDocTranscodingState
{
	kNIMDocTranscodingStatePreparing	= 1,	/**< 转码准备中 */
	kNIMDocTranscodingStateOngoing		= 2,	/**< 转码进行中 */
	kNIMDocTranscodingStateTimeout		= 3,	/**< 转码超时 */
	kNIMDocTranscodingStateCompleted	= 4,	/**< 转码完成 */
	kNIMDocTranscodingStateFailed		= 5,	/**< 转码失败 */
};

/** @enum NIMDocTranscodingFailFlag 转码失败原因 */
enum NIMDocTranscodingFailFlag
{
	kNIMDocTransFailFlagSuccess			= 0,	/**< 正常 */
	kNIMDocTransFailFlagNotExist		= 2,	/**< 找不到文件 */
	kNIMDocTransFailFlagFileTypeErr		= 3,	/**< 文件类型错误 */
	kNIMDocTransFailFlagRequstErr		= 4,	/**< 转码请求出现异常 */
	kNIMDocTransFailFlagLinkErr			= 5,	/**< 转码服务器连接错误 */
	kNIMDocTransFailFlagServerErr		= 6,	/**< 转码服务器内部错误 */
	kNIMDocTransFailFlagPicErr			= 7,	/**< 文档转码图片出错 */
	kNIMDocTransFailFlagQualityErr		= 8,	/**< 图片质量处理错误 */
	kNIMDocTransFailFlagPageLimit		= 9,	/**< 页数超限 */
	kNIMDocTransFailFlagNosErr			= 10,	/**< nos回调错误 */
	kNIMDocTransFailFlagDocParseErr		= 11,	/**< 文档解析出错 */
	kNIMDocTransFailFlagUnknown			= 100,	/**< 表示未知错误 */
};

/** @name json extension params for doc trans info
* for example pic_info:
* "pic_info":[{"height":1080,"quality":1,"size":77599,"width":1439},{"height":720,"quality":2,"size":46287,"width":959},{"height":480,"quality":3,"size":27577,"width":639}]
* for example one info:
* {"ext":"ext_test","id":"8c17c252...","name":"name_test","page_num":0,"pic_type":11,"size":568351,"source_type":2,"state":2,"url_prefix":"http://......"}
* {"ext":"ext","id":"7b004b82...","name":"name","page_num":1,"pic_info":[...],"pic_type":11,"size":106496,"source_type":1,"state":4,"url_prefix":"http..."}
* for example infolist:
* {"count":3,"infos":[{"ext":"ext_test" ...},{"ext":"ext_test" ...},{"ext":"ext_test" ...}]}
*/
static const char *kNIMDocTransCount			= "count"; 			/**< int32 计数 */
static const char *kNIMDocTransInfos			= "infos"; 			/**< key 分页获取时文档信息列表的key */
static const char *kNIMDocTransId				= "id"; 			/**< string 标识ID */
static const char *kNIMDocTransName				= "name"; 			/**< string 转码文档名称 */
static const char *kNIMDocTransSourceType		= "source_type"; 	/**< int32 转码源文档的文件类型NIMDocTranscodingFileType */
static const char *kNIMDocTransSize				= "size"; 			/**< int64 文件大小 */
static const char *kNIMDocTransPicType			= "pic_type"; 		/**< int32 转码目标图片的文件类型NIMDocTranscodingImageType */
static const char *kNIMDocTransState			= "state"; 			/**< int32 转码过程状态NIMDocTranscodingState */
static const char *kNIMDocTransUrlPrefix		= "url_prefix"; 	/**< string 转码后的下载地址前缀 */
static const char *kNIMDocTransPageNum			= "page_num"; 		/**< int32 转码文档总页数 */
static const char *kNIMDocTransExt				= "ext"; 			/**< string 发起文档转码时的附带信息 */
static const char *kNIMDocTransFlag				= "flag"; 			/**< int32 错误原因NIMDocTranscodingFailFlag，kNIMDocTranscodingStateFailed时有效 */
static const char *kNIMDocTransPicInfo			= "pic_info"; 		/**< key 图片信息的key */
static const char *kNIMDocTransQuality			= "quality"; 		/**< int32 转码图像清晰度 */
static const char *kNIMDocTransWidth			= "width"; 			/**< int32 图片宽度 */
static const char *kNIMDocTransHeight			= "height"; 		/**< int32 图片宽度 */
static const char *kNIMDocTransFilePath			= "file_path"; 		/**< string 上传文件的路径 */
static const char *kNIMDocTransUploadStatus		= "upload_status"; 	/**< int32 文件续传状态NIMDocContinueUploadState */
/** @}*/ //json extension params for doc trans info

/** @typedef void (*nim_doctrans_opt_cb_func)(int32_t code, const char *json_extension, const void *user_data)
  * nim callback function for doc trans result
  * @param[out] code 				200为成功，其他为失败
  * @param[out] json_extension		json扩展数据，如果查询成功返回文档信息
  * @param[out] user_data			APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_doctrans_opt_cb_func)(int32_t code, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_DOC_TRANS_DEF_H_
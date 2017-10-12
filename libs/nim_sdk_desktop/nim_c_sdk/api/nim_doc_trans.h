/** @file nim_doc_trans.h
  * @brief 文档转换 接口头文件，其中文档上传下载,请使用NOS云存储服务提供的上传下载接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_API_NIM_DOC_TRANS_H_
#define NIM_SDK_DLL_API_NIM_DOC_TRANS_H_

#include "nim_sdk_dll.h"
#include "nim_doc_trans_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
	
/** @fn void nim_doctrans_reg_notify_cb(const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data)
  * 注册文档转换的结果的回调通知（服务器异步转换，客户端需要等待通知才知道转换结果）
  * @param[in] json_extension 无效扩展字段
  * @param[in] cb 结果回调见nim_doc_trans_def.h，成功返回的json_extension中带有一条记录
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_doctrans_reg_notify_cb(const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);

/** @fn void nim_doctrans_get_info(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data)
  * 根据文档id查询文档信息
  * @param[in] id 文档id
  * @param[in] json_extension 无效扩展字段
  * @param[in] cb 结果回调见nim_doc_trans_def.h，成功返回的json_extension中带有一条记录
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_doctrans_get_info(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);

/** @fn void nim_doctrans_get_info_list(const char *id, int32_t limit, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data)
  * 根据文档id查询文档信息
  * @param[in] id		查询的起始docId，若为空，表示从头开始查找，按照文档转码的发起时间降序排列
  * @param[in] limit	查询的文档的最大数目，有最大值限制，目前为30
  * @param[in] json_extension 无效扩展字段
  * @param[in] cb		结果回调见nim_doc_trans_def.h，成功返回的json_extension中带有记录列表
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_doctrans_get_info_list(const char *id, int32_t limit, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);

/** @fn void nim_doctrans_del_info(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data) 
  * 根据文档id删除服务器记录，对于正在转码中的文档，删除后将不会收到转码结果的通知
  * @param[in] id 文档id
  * @param[in] json_extension 无效扩展字段
  * @param[in] cb 结果回调见nim_doc_trans_def.h，返回的json_extension无效
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */ 
NIM_SDK_DLL_API	void nim_doctrans_del_info(const char *id, const char *json_extension, nim_doctrans_opt_cb_func cb, const void *user_data);
	
/** @fn char *nim_doctrans_get_source_file_url(const char *url_prefix, NIMDocTranscodingFileType file_type)
  * 拼接文档源的下载地址
  * @param[in] url_prefix	文档信息中的url前缀
  * @param[in] file_type	文档源类型
  * @return char * 返回文档源的下载地址,需要上层调用nim_global.h提供的内存释放接口释放。
  */
NIM_SDK_DLL_API char *nim_doctrans_get_source_file_url(const char *url_prefix, NIMDocTranscodingFileType file_type);

/** @fn char *nim_doctrans_get_page_url(const char *url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num)
  * 拼接文档图片的下载地址
  * @param[in] url_prefix	文档信息中的url前缀
  * @param[in] img_type		文档转换的图片类型
  * @param[in] quality		需要的图片清晰度
  * @param[in] page_num		图片页码（从1开始计算）
  * @return char * 返回文档图片的下载地址,需要上层调用nim_global.h提供的内存释放接口释放。
  */
NIM_SDK_DLL_API char *nim_doctrans_get_page_url(const char *url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_DOC_TRANS_H_
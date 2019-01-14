/** @file nim_cpp_doc_trans.h
  * @brief 文档转换,其中文档上传下载,请使用NOS云存储服务提供的上传下载接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2016/12/15
  */

#ifndef _NIM_SDK_CPP_DOC_TRANS_H_
#define _NIM_SDK_CPP_DOC_TRANS_H_

#include <string>
#include <functional>
#include "nim_doc_trans_helper.h"
#include "nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @class DocTrans
  * @brief NIM SDK提供的文档转换相关接口
  */
class NIM_SDK_CPPWRAPPER_DLL_API DocTrans
{
public:
	typedef std::function<void(int32_t code, const DocTransInfo& doc_info)> DocInfoCallback;
	typedef std::function<void(int32_t code, int32_t count, const std::list<DocTransInfo>& doc_infos)> DocInfosCallback;

public:

	/** @fn void RegNotifyCb(const DocInfoCallback& cb)
	* 注册文档转换的结果的回调通知
	* @param[in] cb 返回错误码和文档信息
	* @return void 无返回值
	*/
	static void RegNotifyCb(const DocInfoCallback& cb);

	/** @fn void GetInfo(const std::string& id, const std::string& json_extension, const DocInfoCallback& cb)
	* 根据文档id查询文档信息
	* @param[in] id 文档id
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 返回错误码和文档信息
	* @return void 无返回值
	*/
	static void GetInfo(const std::string& id, const std::string& json_extension, const DocInfoCallback& cb);
	
	/** @fn void GetInfoList(const std::string& id, int32_t limit, const std::string& json_extension, const DocInfosCallback& cb)
	* 根据文档id查询文档信息
	* @param[in] 查询的起始docId，若为空，表示从头开始查找，按照文档转码的发起时间降序排列
	* @param[in] limit 查询的文档的最大数目，有最大值限制，目前为30
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 返回错误码和文档信息
	* @return void 无返回值
	*/
	static void GetInfoList(const std::string& id, int32_t limit, const std::string& json_extension, const DocInfosCallback& cb);

	/** @fn void DeleteInfo(const std::string& id, const std::string& json_extension, const DocInfoCallback& cb)
	* 根据文档id删除服务器记录，对于正在转码中的文档，删除后将不会收到转码结果的通知
	* @param[in] id 文档id
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 结果回调见nim_doc_trans_def.h，返回的json_extension无效
	* @return void 无返回值
	*/
	static void DeleteInfo(const std::string& id, const std::string& json_extension, const DocInfoCallback& cb);
	
	/** @fn std::string GetSourceFileUrl(const std::string& url_prefix, NIMDocTranscodingFileType file_type)
	* 拼接文档源的下载地址
	* @param[in] url_prefix	文档信息中的url前缀
	* @param[in] file_type	文档源类型
	* @return std::string 返回文档源的下载地址
	*/
	static std::string GetSourceFileUrl(const std::string& url_prefix, NIMDocTranscodingFileType file_type);
	
	/** @fn std::string GetPageUrl(const std::string& url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num)
	* 拼接文档图片的下载地址
	* @param[in] url_prefix	文档信息中的url前缀
	* @param[in] img_type 文档转换的图片类型
	* @param[in] quality 需要的图片清晰度
	* @param[in] page_num 图片页码（从1开始计算）
	* @param[in] cb 结果回调见nim_doc_trans_def.h，成功返回的json_extension中带有一条记录
	* @return std::string 返回文档图片的下载地址
	*/
	static std::string GetPageUrl(const std::string& url_prefix, NIMDocTranscodingImageType img_type, NIMDocTranscodingQuality quality, int32_t page_num);
	
};

} 

#endif //_NIM_SDK_CPP_DOC_TRANS_H_
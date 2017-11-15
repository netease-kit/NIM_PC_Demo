/** @file nim_nos_helper.h
  * @brief NOS数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/12/16
  */

#ifndef _NIM_SDK_CPP_NOS_HELPER_H_
#define _NIM_SDK_CPP_NOS_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "../util/nim_build_config.h"
#include "nim_sdk_defines.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @brief 上传完成的结果 */
struct UploadMediaResult
{
	std::string url_;				/**< 上传地址 */
	std::string res_id_;			
#if NIMAPI_UNDER_WIN_DESKTOP_ONLY
	std::string call_id_;
#endif

	/** 构造函数 */
	UploadMediaResult() {}
};

/** @brief 下载完成的结果 */
struct DownloadMediaResult
{
	std::string file_path_;
	std::string call_id_;
	std::string res_id_;

	/** 构造函数 */
	DownloadMediaResult() {}
};

/** @brief 传输过程中的数据 */
struct ProgressData
{
	std::string res_id_;			

	/** 构造函数 */
	ProgressData() {}
};

/** @fn bool ParseUploadResult(const std::string& url, const std::string& json, UploadMediaResult& res)
  * @brief 解析NOS上传结果
  * @param[in] url 上传结果地址
  * @param[in] json Json Value扩展数据
  * @param[out] res 结果结构体
  * @return bool 解析成功 或失败
  */
bool ParseUploadResult(const std::string& url, const std::string& json, UploadMediaResult& res);

/** @fn bool ParseDownloadResult(const std::string& file_path, const std::string& call_id, const std::string& res_id, DownloadMediaResult& res)
  * @brief 解析NOS上传过程数据
  * @param[in] file_path 下载文件地址
  * @param[in] call_id 如果下载的是消息中的资源，则为消息所属的会话id，否则为空
  * @param[int] res_id 如果下载的是消息中的资源，则为消息id，否则为空
  * @param[in] json Json Value扩展数据
  * @param[out] res 数据结构体
  * @return bool 解析成功 或失败
  */
bool ParseDownloadResult(const std::string& file_path, const std::string& call_id, const std::string& res_id, DownloadMediaResult& res);

/** @fn bool ParseUploadProgress(const std::string& json, ProgressData& res)
  * @brief 解析NOS上传过程数据
  * @param[in] json Json Value扩展数据
  * @param[out] res 数据结构体
  * @return bool 解析成功 或失败
  */
bool ParseProgressData(const std::string& json, ProgressData& res);

} //namespace nim

#endif //_NIM_SDK_CPP_NOS_HELPER_H_
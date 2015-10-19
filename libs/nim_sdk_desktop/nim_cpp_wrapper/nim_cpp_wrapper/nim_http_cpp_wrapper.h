#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_http_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_res_code_def.h"

/** @class Http
  * @brief NIM SDK提供的http接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Http
{

public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)> FetchMediaCallback;
	typedef std::function<void(__int64 downloaded_size, __int64 file_size)> FetchMediaProgressCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)> DownloadCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& url)> UploadResourceCallback;
	typedef std::function<void(__int64 uploaded_size, __int64 file_size)> UploadResourceProgressCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)> DownloadResourceCallback;
	typedef std::function<void(__int64 downloaded_size, __int64 file_size)> DownloadResourceProgressCallback;

	/** 
	* 注册下载回调，通过注册回调获得http下载结果通知，刷新资源
	* @param[in] cb 下载的回调函数
	* @return void 无返回值
	*/
	static void RegDownloadCb(const DownloadCallback& cb);

	/** 
	* 获取资源
	* @param[in] json_msg 消息体json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
	* @param[in] res_cb 下载的回调函数
	* @param[in] prg_cb 下载进度的回调函数
	* @return void 无返回值
	*/
	static void FetchMedia(const std::string& json_msg, const FetchMediaCallback& res_cb, const FetchMediaProgressCallback& prg_cb);

	/** @fn void nim_http_stop_fetch_media(const char *json_msg)
	* 停止获取资源（目前仅对文件消息类型有效）
	* @param[in] json_msg 消息体json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
	* @return void 无返回值
	*/
	static void StopFetchMedia(const std::string& json_msg);

	/** 
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] res_cb 上传的回调函数
	* @param[in] prg_cb 上传进度的回调函数
	* @return void 无返回值
	*/
	static void UploadResource(const std::string& local_file, const UploadResourceCallback& response_cb, const UploadResourceProgressCallback& progress_cb = UploadResourceProgressCallback());

	/** 
	* 下载资源
	* @param[in] nos_url 下载资源的URL
	* @param[in] res_cb 下载的回调函数
	* @param[in] prg_cb 下载进度的回调函数
	* @return void 无返回值
	*/
	static void DownloadResource(const std::string& nos_url, const DownloadResourceCallback& response_cb, const DownloadResourceProgressCallback& progress_cb = DownloadResourceProgressCallback());
};

} 
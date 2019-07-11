/** @file nim_cpp_nos.h
  * @brief NOS云存储服务
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_NOS_H_
#define _NIM_SDK_CPP_NOS_H_

#include <string>
#include <functional>
#include "src/cpp_sdk/nim/api//nim_cpp_talk.h"
#include "src/cpp_sdk/nim/helper/nim_nos_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class NOS
  * @brief NIM SDK提供的NOS云存储服务接口
  */
class NIM_SDK_CPPWRAPPER_DLL_API NOS
{

public:
	typedef std::function<void(const InitNosResult&)> InitNosResultCallback; /**< 初始化结果回调模板 */

	typedef std::function<void(NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)> DownloadMediaCallback; /**< 下载回调模板 */
	typedef std::function<void(NIMResCode res_code, const std::string& url)> UploadMediaCallback; /**< 上传回调模板 */
	typedef std::function<void(int64_t completed_size, int64_t file_size)> ProgressCallback; /**< 过程回调模板 */

	typedef std::function<void(NIMResCode res_code, const DownloadMediaResult& result)> DownloadMediaExCallback; /**< 扩展下载回调模板 */
	typedef std::function<void(NIMResCode res_code, const UploadMediaResult& result)> UploadMediaExCallback; /**< 扩展上传回调模板 */
	typedef std::function<void(int64_t completed_size, int64_t file_size, const ProgressData& result)> ProgressExCallback; /**< 扩展过程回调模板 */

	typedef std::function<void(int64_t speed)> SpeedCallback; /**< 速度回调模板 */
	typedef std::function<void(int64_t actual_size, int64_t speed)> TransferInfoCallback; /**< 最终传输信息回调模板 */

	typedef std::function<void(NIMResCode res_code, const std::string& origin_url)> SafeURLToOriginURLCallback;/**< 根据安全链接(短链)取得源链接 */

	/** @fn void InitConfig(const InitNosConfigParam& param, const InitNosResultCallback& cb)
	* Nos模块初始化接口，对上传资源时使用的各场景资源生命周期进行初始化，开发者最多可自定义10个场景，并指定场景资源的生命周期，并可以对缺省场景（kNIMNosDefaultTagResource、kNIMNosDefaultTagIM）进行覆盖（重新指定生命周期）
	* @param[in] param 初始化参数
	* @param[in] cb 结果回调函数
	* @return void 无返回值
	*/
	static void InitConfig(const InitNosConfigParam& param, const InitNosResultCallback& cb);

	/** @fn static void RegDownloadCb(const DownloadMediaCallback& cb)
	* (全局回调)注册下载回调，通过注册回调获得HTTP下载结果通知（所有触发HTTP下载任务的接口的参数列表里无法设置通知回调处理函数的通知都走这个通知，比如收到图片语音会触发SDK自动下载等，开发者可以通过监听这个广播通知结果刷新UI资源）
	* @param[in] cb 下载的回调函数
	* @return void 无返回值
	*/
	static void RegDownloadCb(const DownloadMediaCallback& cb);

	/** @fn static void RegUploadCb(const UploadMediaExCallback& cb)
	* (全局回调)注册上传回调，通过注册回调获得HTTP上传结果通知（所有触发HTTP上传任务的接口的参数列表里无法设置通知回调处理函数的通知都走这个通知，比如发送文件图片语音消息等）
	* @param[in] cb 回调函数
	* @return void 无返回值
	*/
	static void RegUploadCb(const UploadMediaExCallback& cb);

	/** @fn static bool FetchMedia(const IMMessage& msg, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress)
	* 获取资源
	* @param[in] msg 消息内容
	* @param[in] callback_result 下载的回调函数
	* @param[in] callback_progress 下载进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				10206:下载暂停
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool FetchMedia(const IMMessage& msg, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress);

	/** @fn static bool FetchMediaEx(const IMMessage& msg, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress, const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback())
	* 获取资源
	* @param[in] msg 消息内容
	* @param[in] json_extension json扩展参数
	* @param[in] callback_result 下载的回调函数
	* @param[in] callback_progress 下载进度的回调函数
	* @param[in] callback_speed 下载速度的回调函数
	* @param[in] callback_transfer 最终下载信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				10206:下载暂停
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool FetchMediaEx(const IMMessage& msg, const std::string& json_extension, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress, const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback());

	/** @fn static bool StopFetchMedia(const IMMessage& msg)
	* 停止获取资源（目前仅对文件消息类型有效）
	* @param[in] msg 消息内容
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	10206:成功
	*/
	static bool StopFetchMedia(const IMMessage& msg);

	/** @fn static bool UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback())
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] callback_result 上传的回调函数
	* @param[in] callback_progress 上传进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback());

	/** @fn static bool UploadResource2(const std::string& local_file, const std::string& tag, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback())
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] tag 场景标签，主要用于确定文件的保存时间
	* @param[in] callback_result 上传的回调函数
	* @param[in] callback_progress 上传进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool UploadResource2(const std::string& local_file, const std::string& tag, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback());

	/** @fn static bool UploadResourceEx(const std::string& local_file, const std::string& json_extension, const UploadExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback())
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] json_extension json扩展参数，拼接字段见 nim_nos_def.h "NOS扩展上传\下载接口参数json_extension Json key for nim_nos_upload_ex && nim_nos_download_ex"
	* @param[in] callback_result 扩展上传的回调函数
	* @param[in] callback_progress 扩展上传进度的回调函数
	* @param[in] callback_speed 上传速度的回调函数
	* @param[in] callback_transfer 最终上传信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				10200:上传暂停
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool UploadResourceEx(const std::string& local_file, const std::string& json_extension, const UploadMediaExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback());

	/** @fn static bool UploadResourceEx2(const std::string& local_file, const std::string& tag, const std::string& json_extension, const UploadExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback())
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] tag 场景标签，主要用于确定文件的保存时间
	* @param[in] json_extension json扩展参数，拼接字段见 nim_nos_def.h "NOS扩展上传\下载接口参数json_extension Json key for nim_nos_upload_ex && nim_nos_download_ex"
	* @param[in] callback_result 扩展上传的回调函数
	* @param[in] callback_progress 扩展上传进度的回调函数
	* @param[in] callback_speed 上传速度的回调函数
	* @param[in] callback_transfer 最终上传信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				10200:上传暂停
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool UploadResourceEx2(const std::string& local_file, const std::string& tag,const std::string& json_extension, const UploadMediaExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback());

	/** @fn static bool StopUploadResourceEx(const std::string& task_id, const std::string& json_extension = "")
	* 停止上传资源(只能用于调用了UploadResourceEx接口的上传任务)
	* @param[in] task_id 停止上传任务的ID
	* @param[in] json_extension json扩展参数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	10200:成功
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool StopUploadResourceEx(const std::string& task_id, const std::string& json_extension = "");

	/** @fn static bool DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback())
	* 下载资源
	* @param[in] nos_url 下载资源的URL
	* @param[in] callback_result 下载的回调函数
	* @param[in] callback_progress 下载进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback());

	/** @fn static bool DownloadResourceEx(const std::string& nos_url, const DownloadMediaExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback())
	* 下载资源
	* @param[in] nos_url 下载资源的URL
	* @param[in] json_extension json扩展参数，拼接字段见 nim_nos_def.h "NOS扩展上传\下载接口参数json_extension Json key for nim_nos_upload_ex && nim_nos_download_ex"
	* @param[in] callback_result 扩展下载的回调函数
	* @param[in] callback_progress 扩展下载进度的回调函数
	* @param[in] callback_speed 下载速度的回调函数
	* @param[in] callback_transfer 最终下载信息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				10206:下载暂停
	*				408:请求过程超时
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool DownloadResourceEx(const std::string& nos_url, const std::string& json_extension, const DownloadMediaExCallback& callback_result, const ProgressExCallback& callback_progress = ProgressExCallback(), const SpeedCallback& callback_speed = SpeedCallback(), const TransferInfoCallback& callback_transfer = TransferInfoCallback());

	/** @fn static bool StopDownloadResourceEx(const std::string& task_id, const std::string& json_extension = "")
	* 停止下载资源(只能用于调用了DownloadResourceEx接口的下载任务)
	* @param[in] task_id 停止下载任务的ID
	* @param[in] json_extension json扩展参数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	10206:成功
	*				其他:参见HTTP协议或CURL错误码
	*/
	static bool StopDownloadResourceEx(const std::string& task_id, const std::string& json_extension = "");

	/** @fn static void SafeURLToOriginURL(const std::string& safe_url, const SafeURLToOriginURLCallback& cb, const std::string& json_extension = "")
	* 根据安全链接(短链)换取源链接
	* @param[in] safe_url 安全链接(短链)
	* @param[in] cb 结果回调
	* @param[in] json_extension json扩展参数
	* @return void
	* @note 错误码	200:成功
	*				其他:404 不存在该短链或 safe_url 不是一个有效的短链
	*/
	static void SafeURLToOriginURL(const std::string& safe_url, const SafeURLToOriginURLCallback& cb, const std::string& json_extension = "");

	/** @fn void UnregNosCb()
	* 反注册Nos提供的所有回调
	* @return void 无返回值
	*/
	static void UnregNosCb();
	/** @fn static void SetSupportQuickTrans(bool bquick)
	* 打开或关闭文件快传开关
	* @param[in] bquick 是否打开 true:打开 false:关闭
	* @return void
	*/
	static void SetSupportQuickTrans(bool bquick);
};

} 

#endif //_NIM_SDK_CPP_NOS_H_

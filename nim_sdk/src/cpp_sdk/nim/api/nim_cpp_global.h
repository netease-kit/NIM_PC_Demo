/** @file nim_cpp_global.h
  * @brief 辅助能力
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_GLOBAL_H_
#define _NIM_SDK_CPP_GLOBAL_H_

#include <functional>
#include <string>
#include "public_define/nim_util_include.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{


/** @class Global
  * @brief NIM SDK提供的一些全局接口
  */
 class NIM_SDK_CPPWRAPPER_DLL_API Global
{
public:
	struct NIM_SDK_CPPWRAPPER_DLL_API CachedFileInfo
	{
		std::string file_type_;
		std::string file_path_;
		int			file_count_;
		int64_t		file_total_size_;
	};
	struct NIM_SDK_CPPWRAPPER_DLL_API SDKDBErrorInfo
	{
		enum DBOperation
		{
			kNIMDBOperationInsert	= 0x0001,	/**< 写操作*/
			kNIMDBOperationRead		= 0x0002,	/**< 读操作*/
			kNIMDBOperationUpdate	= 0x0004,	/**< 更新操作*/
			kNIMDBOperationDelete	= 0x0008,	/**< 删除操作*/
		};
		std::string db_name_;
		int error_code_;
		int operation_;
		std::string description_;
		std::string attach_;
	};
	typedef std::function<void(bool conncet, NIMProxyDetectStep step, const std::string& json_extension)> DetectProxyCallback; 
	typedef std::function<void(NIMSDKException exception,const std::string& log)> ExceptionCallback; 
	typedef std::function<void(NIMResCode rescode, const CachedFileInfo &info)> GetCachedFileInfoCallback;
	typedef std::function<void(NIMResCode rescode)> DeleteCachedFileCallback;
	typedef DeleteCachedFileCallback SDKFeedbackCallback;
	typedef std::function<void(const SDKDBErrorInfo& error_info)>  SDKDBErrorCallback;
public:
	/** @fn void FreeStrBuf(char *str)
	* 释放SDK内部分配的内存
	* @param[in] str 由SDK内部分配内存的字符串
	* @return void 无返回值
	*/
	static void FreeStrBuf(char *str);

	/** @fn  void FreeBuf(void *data)
	* 释放SDK内部分配的内存
	* @param[in] data 由SDK内部分配的内存
	* @return void 无返回值
	*/
	static void FreeBuf(void *data);

	/** @fn void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
    * 设置SDK统一的网络代理。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。   
    * @param[in] type 代理类型，见NIMProxyType定义
    * @param[in] host 代理地址
    * @param[in] port 代理端口
    * @param[in] user 代理用户名
    * @param[in] password 代理密码
    * @return void 无返回值
    */
	static void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password);

	/** @fn void DetectProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password, const DetectProxyCallback& callback)
    * 测试代理   
    * @param[in] type 代理类型，见NIMProxyType定义
    * @param[in] host 代理地址
    * @param[in] port 代理端口
    * @param[in] user 代理用户名
	* @param[in] password 代理密码
	* @param[in] callback 回调函数
    * @return void 无返回值
    */
	static void DetectProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password, const Global::DetectProxyCallback& callback);

	/** @fn void SetExceptionReportCallback(const std::string&json_extension, const ExceptionCallback& cb)
	* 注册输出系统环境异常的回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 
	* @return void 无返回值
	*/
	static void SetExceptionReportCallback(const std::string&json_extension, const ExceptionCallback& cb);

	/** @fn void GetSDKCachedFileInfoAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const GetCachedFileInfoCallback &cb);
	* 获取sdk缓存文件信息
	* @param[in] login_id 查询的账号ID
	* @param[in] file_type 文件类型，常量定义见nim_global_def.h 查询SDK文件缓存信息文件类型file_type
	* @param[in] end_timestamp  查询时间截止点（查询全部填0）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void GetSDKCachedFileInfoAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const GetCachedFileInfoCallback &cb);

	/** @fn void DeleteSDKCachedFileAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const DeleteCachedFileCallback &cb)
	* 删除sdk缓存文件
	* @param[in] login_id 查询的账号ID
	* @param[in] file_type 文件类型，常量定义见nim_global_def.h 查询SDK文件缓存信息文件类型file_type
	* @param[in] end_timestamp  删除时间截止点（查询全部填0）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void DeleteSDKCachedFileAsync(const std::string &login_id, const std::string &file_type, int64_t end_timestamp, const std::string &json_extension, const DeleteCachedFileCallback &cb);

	/** @fn void SDKFeedbackAsync(const std::string &url, const std::string &json_extension, const SDKFeedbackCallback &cb);
	* SDK 反馈
	* @param[in] url 开发者需将所需要反馈的文件（dump，应用日志等）上传至nos。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb
	* @return void 无返回值
	*/
	static void SDKFeedbackAsync(const std::string &url, const std::string &json_extension, const SDKFeedbackCallback &cb);

	/** @fn void RegSDKDBError(const SDKDBErrorCallback& cb)
	* 注册 SDK DB操作出错时的回调
	* @param[in] cb SDKDBErrorCallback 出错时的回调
	* @return void 无返回值
	*/
	static void RegSDKDBError(const SDKDBErrorCallback& cb);
};

} 

#endif //_NIM_SDK_CPP_GLOBAL_H_

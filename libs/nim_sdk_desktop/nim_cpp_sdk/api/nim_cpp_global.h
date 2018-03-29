/** @file nim_cpp_global.h
  * @brief 辅助能力
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_GLOBAL_H_
#define _NIM_SDK_CPP_GLOBAL_H_

#include <functional>
#include <string>
#include "nim_base_types.h"
#include "nim_sdk_defines.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{


/** @class Global
  * @brief NIM SDK提供的一些全局接口
  */
class Global
{
public:
#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
	struct CachedFileInfo
	{
		std::string file_type_;
		std::string file_path_;
		int			file_count_;
		int64_t		file_total_size_;
	};
	typedef std::function<void(bool conncet, NIMProxyDetectStep step, const std::string& json_extention)> DetectProxyCallback; 
	typedef std::function<void(NIMSDKException exception,const std::string& log)> ExceptionCallback; 
	typedef std::function<void(NIMResCode rescode, const CachedFileInfo &info)> GetCachedFileInfoCallback;
	typedef std::function<void(NIMResCode rescode)> DeleteCachedFileCallback;
#else
	typedef std::function<void(int log_level,const std::string& log)> SDKLogCallback; 
#endif

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

#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
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

#else
	/** @fn void nim_client_reg_kickout_other_client_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
	* 注册输出sdk log回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 输出sdk log的回调函数， nim_sdk_log_cb_func回调函数定义见nim_global_def.h
	* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void SetSDKLogCallback(const std::string&json_extension,const SDKLogCallback& callback);
#endif
};

} 

#endif //_NIM_SDK_CPP_GLOBAL_H_

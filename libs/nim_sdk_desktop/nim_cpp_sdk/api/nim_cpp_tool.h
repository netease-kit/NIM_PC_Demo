/** @file nim_cpp_tool.h
  * @brief NIM SDK提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5等
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/09/21
  */

#ifndef _NIM_SDK_CPP_TOOL_H_
#define _NIM_SDK_CPP_TOOL_H_

#include <string>
#include "nim_tool_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{


/** @class Tool
  * @brief NIM SDK提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5等
  */
class Tool
{

public:
	typedef std::function<void(int rescode, const std::string& text)> GetAudioTextCallback;	/**< 语音转文字回调模板 */

public:
	/** @fn std::string GetUserAppdataDir(const std::string& app_account)
	* 获取SDK里app account对应的app data目录（各个帐号拥有独立的目录，其父目录相同）
	* @param[in] app_account APP account。如果传入空字符串，则将获取到各个帐号目录的父目录（谨慎删除！）
	* @return std::string 返回的目录路径(UTF8)
	*/
	static std::string GetUserAppdataDir(const std::string& app_account);

	/** @fn std::string GetLocalAppdataDir()
	* 获取本地存储路径
	* @return std::string 返回的目录路径(UTF8)
	*/
	static std::string GetLocalAppdataDir(); 

	/** @fn std::string GetCurModuleDir()
	* 获取安装目录（SDK DLL所在的当前目录）
	* @return std::string 返回的目录路径(UTF8)
	*/
	static std::string GetCurModuleDir();

	/** @fn std::string GetMd5(const std::string& input)
	* 计算md5
	* @param[in] input 需要计算md5的内容
	* @return std::string 返回的md5
	*/
	static std::string GetMd5(const std::string& input);

	/** @fn std::string GetFileMd5(const std::string& file_path)
	* 计算文件的md5
	* @param[in] file_path 文件完整路径，UTF-8
	* @return std::string 返回的md5
	*/
	static std::string GetFileMd5(const std::string& file_path);

	/** @fn std::string GetUuid()
	* 生成UUID
	* @return std::string 返回的UUID
	*/
	static std::string GetUuid();

	/** @fn bool GetAudioTextAsync(const AudioInfo& audio_info, const GetAudioTextCallback& cb, const std::string& json_extension = "")
	* 语音转文字
	* @param[in] audio_info 语音信息
	* @param[in] cb 语音转文字回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:没有权限
	*				414:参数错误
	*				500:未知错误
	*				6101:语音转码解析失败
	*				6102:语音时长过长（超过60s
	*				6103:服务不可用
	*				6104:audioUrl不合法
	*/
	static bool GetAudioTextAsync(const AudioInfo& audio_info, const GetAudioTextCallback& cb, const std::string& json_extension = "");
};

} 

#endif //_NIM_SDK_CPP_TOOL_H_
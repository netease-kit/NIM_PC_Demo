#pragma once

#include <string>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_tools_def.h"

struct AudioInfo
{
	std::string mime;
	std::string samp;
	std::string url;
	unsigned __int64 dur;
};


/** @class Tool
  * @brief NIM SDK提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5等
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Tool
{

public:
	typedef std::function<void(NIMResCode rescode, const std::string& text)>	GetAudioTextCallback;

	/** 
	* 获取SDK里app account对应的app data目录（各个帐号拥有独立的目录，其父目录相同），需要调用nim_free_buf(void *data)释放内存
	* @param[in] app_account APP account。如果传入空字符串，则将获取到各个帐号目录的父目录（谨慎删除！）
	* @return const wchar_t * 返回的目录路径
	*/
	static const wchar_t * GetUserAppdataDir(const std::string& app_account);

	/** 
	* 获取本地存储路径，需要调用nim_free_buf(void *data)释放内存
	* @return const wchar_t * 返回的目录路径
	*/
	static const wchar_t * GetLocalAppdataDir(); 

	/** 
	* 获取安装目录（SDK DLL所在的当前目录），需要调用nim_free_buf(void *data)释放内存
	* @return const wchar_t * 返回的目录路径
	*/
	static const wchar_t * GetCurModuleDir();

	/** 
	* 计算md5，需要调用nim_free_buf(void *data)释放内存
	* @param[in] input 需要计算md5的内容
	* @return const char * 返回的md5
	*/
	static const char * GetMd5(const std::string& input);

	/** 
	* 计算文件的md5，需要调用nim_free_buf(void *data)释放内存
	* @param[in] file_path 文件完整路径，UTF-8
	* @return const char * 返回的md5
	*/
	static const char * GetFileMd5(const std::string& file_path);

	/** 
	* 生成UUID，需要调用nim_free_buf(void *data)释放内存
	* @return const char * 返回的UUID
	*/
	static const char * GetUuid();

	/** @fn const char * nim_tool_get_audio_text_async(const char *json_audio_info, const char *json_extension, nim_tool_get_audio_text_cb_func cb, const void *user_data)
	* 语音转文字
	* @param[in] json_audio_info 语音信息
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 语音转文字回调
	* @return void 无返回值
	*/
	static void GetAudioTextAsync(const AudioInfo& audio_info, const GetAudioTextCallback& cb, const std::string& json_extension = "");
};

} 
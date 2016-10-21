/** @file nim_tools.h
  * @brief NIM SDK提供的一些工具接口，主要包括获取SDK里app account对应的app data目录，计算md5等
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_TOOLS_H_
#define NIM_SDK_DLL_API_NIM_TOOLS_H_

#include "nim_sdk_dll.h"
#include "nim_tools_def.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn const char * nim_tool_get_user_appdata_dir(const char *app_account)  
  * 获取SDK里app account对应的app data目录（各个帐号拥有独立的目录，其父目录相同），需要调用nim_free_buf(void *data)释放内存 
  * @param[in] app_account APP account。如果传入空字符串，则将获取到各个帐号目录的父目录（谨慎删除！）
  * @return const char * 返回的目录路径
  */ 
NIM_SDK_DLL_API	const char * nim_tool_get_user_appdata_dir(const char *app_account);

/** @fn const char * nim_tool_get_user_specific_appdata_dir(const char *app_account, NIMAppDataType appdata_type)  
  * 获取SDK里app account对应的具体类型的app data目录（如图片消息文件存放目录，语音消息文件存放目录等），需要调用nim_free_buf(void *data)释放内存 
  * @param[in] app_account APP account。如果传入空字符串，则返回结果为空
  * @param[in] appdata_type 具体类型的app data。见NIMAppDataType定义
  * @return const char * 返回的目录路径（目录可能未生成，需要app自行判断是否已生成）
  */ 
NIM_SDK_DLL_API	const char * nim_tool_get_user_specific_appdata_dir(const char *app_account,enum NIMAppDataType appdata_type);

/** @fn const char * nim_tool_get_local_appdata_dir()  
  * 获取本地存储路径，需要调用nim_free_buf(void *data)释放内存
  * @return const char * 返回的目录路径
  */ 
NIM_SDK_DLL_API	const char * nim_tool_get_local_appdata_dir(); 

/** @fn const char * nim_tool_get_cur_module_dir()
  * 获取安装目录（SDK DLL所在的当前目录），需要调用nim_free_buf(void *data)释放内存
  * @return const char * 返回的目录路径
  */ 
NIM_SDK_DLL_API	const char * nim_tool_get_cur_module_dir();

/** @fn const char * nim_tool_get_md5(const char *input)
  * 计算md5，需要调用nim_free_buf(void *data)释放内存  
  * @param[in] input 需要计算md5的内容
  * @return const char * 返回的md5
  */
NIM_SDK_DLL_API	const char * nim_tool_get_md5(const char *input);

/** @fn const char * nim_tool_get_file_md5(const char *file_path)
  * 计算文件的md5，需要调用nim_free_buf(void *data)释放内存 
  * @param[in] file_path 文件完整路径，UTF-8
  * @return const char * 返回的md5
  */
NIM_SDK_DLL_API	const char * nim_tool_get_file_md5(const char *file_path);

/** @fn const char * nim_tool_get_uuid()
  * 生成UUID，需要调用nim_free_buf(void *data)释放内存
  * @return const char * 返回的UUID
  */
NIM_SDK_DLL_API	const char * nim_tool_get_uuid();

/** @fn void nim_tool_get_audio_text_async(const char *json_audio_info, const char *json_extension, nim_tool_get_audio_text_cb_func cb, const void *user_data)
  * 语音转文字
  * @param[in] json_audio_info 语音信息
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 语音转文字回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_tool_get_audio_text_async(const char *json_audio_info, const char *json_extension, nim_tool_get_audio_text_cb_func cb, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_TOOLS_H_
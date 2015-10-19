/** @file nim_audio.h
  * @brief NIM 提供的语音录制和播放接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/10
  */

#ifndef _NIM_AUDIO_MODULE_SDK_H_
#define _NIM_AUDIO_MODULE_SDK_H_

#include "include/nim_audio_def.h"
#include "nim_sdk_dll.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn bool nim_audio_init_module(const char* user_data_parent_path)
  * NIM SDK 初始化语音模块
  * @param[in] user_data_parent_path 用户目录
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_init_module(const char* user_data_parent_path);

/** @fn bool nim_audio_uninit_module()
  * NIM SDK 卸载语音模块（只有在主程序关闭时才有必要调用此接口）
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_uninit_module();

/** @fn bool nim_audio_play_audio(const char* file_path, const char* call_id, const char* res_id)
  * NIM SDK 播放,通过回调获取开始播放状态
  * @param[in] file_path 播放文件绝对路径
  * @param[in] call_id 用以定位资源的一级ID，可选
  * @param[in] res_id 用以定位资源的二级ID，可选
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_play_audio(const char* file_path, const char* call_id, const char* res_id);

/** @fn bool nim_audio_stop_play_audio()
  * NIM SDK 停止播放,通过回调获取停止播放状态
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_stop_play_audio();

/** @fn bool nim_audio_reg_start_play_cb(nim_rescode_id_cb cb)
  * NIM SDK 注册播放开始事件回调
  * @param[in] cb 回调函数 见nim_audio_def.h
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_reg_start_play_cb(nim_rescode_id_cb cb);

/** @fn bool nim_audio_reg_stop_play_cb(nim_rescode_id_cb cb)
  * NIM SDK 注册播放结束事件回调
  * @param[in] cb 回调函数 见nim_audio_def.h
  * @return bool true 调用成功，false 调用失败
  */
NIM_SDK_DLL_API bool nim_audio_reg_stop_play_cb(nim_rescode_id_cb cb);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //_NIM_AUDIO_MODULE_SDK_H_
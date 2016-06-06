/** @file nim_audio_def.h
  * @brief NIM 提供的语音录制和播放定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/10
  */

#ifndef _NIM_AUDIO_DLL_EXPORT_HEADERS_H_
#define _NIM_AUDIO_DLL_EXPORT_HEADERS_H_

#ifdef __cplusplus
extern"C"
{
#endif

/** @enum nim_audio_rescode audio模块调用返回错误码 */
enum nim_audio_rescode
{
	kSuccess				= 200,	/**< 成功 */
	kFailed					= 100,  /**< 操作失败 */
	kUninitError			= 101,	/**< 未初始化或未成功初始化 */
	kClientPlaying			= 102,	/**< 正在播放中，操作失败 */
	kClientCapturing		= 103,	/**< 正在采集中，操作失败 */
	kCaptureDeviceInitError = 104,	/**< 采集设备初始化失败（e.g. 找不到mic设备） */
	kClientNotExist 		= 105,	/**< 采集或播放对象或操作不存在 */
	kThreadError			= 300,  /**< 线程出错退出，需要重新初始化语音模块 */
};

/** @enum nim_audio_type 音频编码方式 */
enum nim_audio_type
{
	AAC		= 0,	/**< aac格式 */
	AMR		= 1, 	/**< amr格式 */
};

/** @typedef void (*nim_rescode_cb)(int rescode)
  * NIM VChat 	操作回调
  * @param[out] rescode 操作结果，一切正常200
  */
typedef void (*nim_rescode_cb)(int);

/** @typedef void (*nim_rescode_id_cb)(int rescode, const char* file_path, const char* call_id, const char* res_id)
  * NIM VChat 	操作回调
  * @param[out] rescode 操作结果，一切正常200
  * @param[out] file_path 播放文件绝对路径
  * @param[out] call_id 用以定位资源的一级ID，可选
  * @param[out] res_id 用以定位资源的二级ID，可选
  */
typedef void (*nim_rescode_id_cb)(int rescode, const char* file_path, const char* call_id, const char* res_id);


/** @typedef void (*nim_stop_capture_cb)(int rescode, const char* call_id, const char* res_id, const char* file_path, const char* file_ext, long file_size, int audio_duration)
  * NIM VChat 	操作回调
  * @param[out] rescode 操作结果，一切正常200
  * @param[out] call_id 用以定位资源的一级ID，可选
  * @param[out] res_id 用以定位资源的二级ID，可选
  * @param[out] file_path 文件绝对路径
  * @param[out] file_ext 文件扩展名
  * @param[out] file_size 文件大小
  * @param[out] audio_duration 语音时长
  */
typedef void (*nim_stop_capture_cb)(int rescode, const char* call_id, const char* res_id, const char* file_path, const char* file_ext, long file_size, int audio_duration);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //_NIM_AUDIO_DLL_EXPORT_HEADERS_H_
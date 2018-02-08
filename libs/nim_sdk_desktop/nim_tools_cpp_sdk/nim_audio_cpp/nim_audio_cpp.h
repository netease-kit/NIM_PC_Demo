/** @file nim_audio_cpp.h
* @brief NIM 提供的语音播放工具接口(CPP接口)
* @copyright (c) 2015-2016, NetEase Inc. All rights reserved
* @author towik, Oleg
* @date 2015/2/10
*/

#ifndef _NIM_TOOLS_SDK_CPP_AUDIO_H_
#define _NIM_TOOLS_SDK_CPP_AUDIO_H_

#include <windows.h>
#include <string>
#include <list>
#include <functional>
#include <assert.h>

/**
* @namespace nim_audio
* @brief NIM提供的语音播放工具
*/
namespace nim_audio
{

#include "../../nim_tools_c_sdk/audio/nim_audio_def.h"

/** @class Audio
  * @brief NIM 提供的语音录制和播放接口
  */
class Audio
{

public:
	/** bool Init(const std::wstring& user_data_parent_path)
	* NIM SDK 初始化语音模块
	* @param[in] user_data_parent_path 缓存目录
	* @return bool true 调用成功，false 调用失败
	*/
	static bool Init(const std::wstring& user_data_parent_path);

	/** void Cleanup()
	* NIM SDK 卸载语音模块（只有在主程序关闭时才有必要调用此接口）
	* @return bool true 调用成功，false 调用失败
	*/
	static void Cleanup();

	/** bool PlayAudio(const wchar_t* file_path, const char* call_id, const char* res_id, nim_audio_type audio_format)
	* NIM SDK 播放,通过回调获取开始播放状态
	* @param[in] file_path 播放文件绝对路径
	* @param[in] call_id 用以定位资源的一级ID，可选
	* @param[in] res_id 用以定位资源的二级ID，可选
	* @param[in] audio_format 播放音频格式，AAC : 0， AMR : 1
	* @return bool true 调用成功，false 调用失败
	*/
	static bool PlayAudio(const wchar_t* file_path, const char* call_id, const char* res_id, nim_audio_type audio_format);
	/** bool PlayAudio(const wchar_t* file_path, const char* call_id, const char* res_id, nim_audio_type audio_format, int seek)
	* NIM SDK 播放,通过回调获取开始播放状态
	* @param[in] file_path 播放文件绝对路径
	* @param[in] call_id 用以定位资源的一级ID，可选
	* @param[in] res_id 用以定位资源的二级ID，可选
	* @param[in] audio_format 播放音频格式，AAC : 0， AMR : 1
	* @param[in] seek 播放的起始位置(时间)，单位ms
	* @return bool true 调用成功，false 调用失败
	*/
	static bool PlayAudio(const wchar_t* file_path, const char* call_id, const char* res_id, nim_audio_type audio_format, int seek);

	/** bool StopPlayAudio()
	* NIM SDK 停止播放,通过回调获取停止播放状态
	* @return bool true 调用成功，false 调用失败
	*/
	static bool StopPlayAudio();

	/** bool RegStartPlayCb(nim_rescode_id_cb cb)
	* 注册播放开始事件回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStartPlayCb(nim_rescode_id_cb cb);

	/** bool RegStopPlayCb(nim_rescode_id_cb cb)
	* 注册播放结束事件回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStopPlayCb(nim_rescode_id_cb cb);

	/** @fn bool RegStartCaptureCb(nim_rescode_cb cb)
	* 注册录制语音回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStartCaptureCb(nim_rescode_cb cb);

	/** @fn bool RegStopCaptureCb()
	* 注册录制语音结束回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegStopCaptureCb(nim_stop_capture_cb cb);

	/** @fn bool RegCancelAudioCb(nim_rescode_cb cb)
	* 注册取消录制并删除临时文件事件回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegCancelAudioCb(nim_rescode_cb cb);

	/** @fn bool StartCapture(const char* call_id, const char* res_id, nim_audio_type audio_format = AAC, int volume = 180, const wchar_t* capture_device = nullptr)
	* NIM SDK 录制语音
	* @param[in] call_id 用以定位资源的一级ID，可选
	* @param[in] res_id 用以定位资源的二级ID，可选
	* @param[in] audio_format 音频格式，AAC : 0， AMR : 1
	* @param[in] volume 音量(0 - 255, 默认180)
	* @param[in] capture_device 采集设备
	* @return bool true 调用成功，false 调用失败
	*/
	static bool StartCapture(const char* call_id, const char* res_id, nim_audio_type audio_format = AAC, int volume = 180, const wchar_t* capture_device = nullptr);

	/** @fn bool StopCapture()
	* NIM SDK 停止录制语音
	* @return bool true 调用成功，false 调用失败
	*/
	static bool StopCapture();

	/** @fn bool CancelAudio(const wchar_t* file_path)
	* NIM SDK 取消录制并删除临时文件
	* @return bool true 调用成功，false 调用失败
	*/
	static bool CancelAudio(const wchar_t* file_path);

	/** @fn bool RegEnumCaptureDeviceCb(nim_enum_capture_device_cb cb)
	* 注册枚举本地录音采集设备回调
	* @param[in] cb 回调函数 见nim_audio_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool RegEnumCaptureDeviceCb(nim_enum_capture_device_cb cb);

	/** @fn bool EnumCaptureDevice()
	* NIM SDK 枚举本地录音采集设备
	* @return bool true 调用成功，false 调用失败
	*/
	static bool EnumCaptureDevice();

private:
	template <typename F>
	static F Function(const char* function_name)
	{
		F f = (F) ::GetProcAddress(instance_audio_, function_name);
		assert(f);
		return f;
	}

private:
	static HINSTANCE instance_audio_;
};

}

#endif //_NIM_TOOLS_SDK_CPP_AUDIO_H_
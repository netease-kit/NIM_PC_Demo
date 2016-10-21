#pragma once

namespace nim_comp
{
/** @class AudioCallback
  * @brief 语音消息事件回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class AudioCallback
{
public:
	/**
	* 开始播放语音消息的回调（播放语音消息需要和sdk交互，并非一定播放成功）
	* @param[in] code 返回码
	* @param[in] file_path 语音消息文件路径
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void OnPlayAudioCallback(int code, const char* file_path, const char* sid, const char* cid);

	/**
	* 停止播放语音消息的回调
	* @param[in] code 返回码
	* @param[in] file_path 语音消息文件路径
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @return void 无返回值
	*/
	static void OnStopAudioCallback(int code, const char* file_path, const char* sid, const char* cid);

	/**
	* 开始录制语音的回调
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	static void OnStartCaptureCallback(int code);

	/**
	* 录制语音完成的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] sid 会话id
	* @param[in] cid 消息id
	* @param[in] file_path 语音文件绝对路径
	* @param[in] file_ext 文件扩展名
	* @param[in] file_size 文件大小
	* @param[in] audio_duration 语音时长
	* @return void 无返回值
	*/
	static void OnStopCaptureCallback(int rescode, const char* sid, const char* cid, const char* file_path, const char* file_ext, long file_size, int audio_duration);

	/**
	* 取消录制语音的回调
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	static void OnCancelCaptureCallback(int code);
	
	/**
	* 枚举本地录音采集设备的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] device_list 设备列表
	* @return void 无返回值
	*/
	static void OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list);
};
}
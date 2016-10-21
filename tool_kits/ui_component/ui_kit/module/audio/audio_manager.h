#pragma once
#include "shared/auto_unregister.h"

typedef std::function<void(const std::string& session_id, int code)> StartCaptureCallback;
typedef std::function<void(int rescode, const std::string& sid, const std::string& cid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration)> StopCaptureCallback;
typedef std::function<void(const std::string& session_id, int code)> CancelCaptureCallback;
typedef std::function<void(int rescode, const wchar_t* device_list)> EnumCaptureDeviceCallback;

namespace nim_comp
{
/** @class AudioManager
  * @brief 语音功能管理类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/9/12
  */
class AudioManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(AudioManager);	
	AudioManager();

	/**
	* 初始化语音功能，注册回调函数
	* @param[in] user_data_path 语音文件缓存目录
	* @return bool true 初始化成功，false 初始化失败
	*/
	bool InitAudio(const std::string user_data_path);

	/**
	* 播放语音
	* @param[in] file_path 播放文件绝对路径
	* @param[in] session_id 会话id
	* @param[in] msd_id 消息id
	* @param[in] audio_format 音频格式，AAC : 0， AMR : 1
	* @return bool true 调用成功，false 调用失败
	*/
	bool PlayAudio(const std::string file_path, const std::string session_id, const std::string msg_id, nim_audio::nim_audio_type audio_format);

	/**
	* 停止播放语音
	* @param[in] session_id 会话id
	* @return bool true 调用成功，false 调用失败
	*/
	bool StopPlayAudio(const std::string session_id);

	/**
	* 是否正在播放语音
	* @return bool 是否正在录制语音
	*/
	bool IsPlaying();

	/**
	* 录制语音
	* @param[in] session_id 会话id
	* @param[in] msd_id 消息id
	* @param[in] audio_format 音频格式，AAC : 0， AMR : 1
	* @param[in] volume 音量(0 - 255, 默认180)
	* @param[in] loudness 默认0
	* @param[in] capture_device 采集设备
	* @return bool true 调用成功，false 调用失败
	*/
	bool StartCapture(const std::string session_id, const std::string msg_id, nim_audio::nim_audio_type audio_format = nim_audio::AAC, int volume = 180, int loudness = 0, const wchar_t* capture_device = nullptr);

	/** 
	* 停止录制语音
	* @param[in] session_id 会话id
	* @return bool true 调用成功，false 调用失败
	*/
	bool StopCapture(const std::string session_id);

	/**
	* 取消录制语音
	* @param[in] session_id 会话id
	* @return bool true 调用成功，false 调用失败
	*/
	bool CancelCapture(const std::string session_id, const std::string file_path);

	/**
	* 是否正在录制语音
	* @return bool 是否正在录制语音
	*/
	bool IsCapturing();

	/**
	* 枚举本地录音采集设备
	* @return bool true 调用成功，false 调用失败
	*/
	bool EnumCaptureDevice();
public:
	/**
	* 注册开始录制语音的回调
	* @param[in] callback 回调函数
	* @return void 反注册对象
	*/
	UnregisterCallback RegStartCapture(const StartCaptureCallback& callback); 

	/**
	* 注册停止录制语音的回调
	* @param[in] callback 回调函数
	* @return void 反注册对象
	*/
	UnregisterCallback RegStopCapture(const StopCaptureCallback& callback);

	/**
	* 注册取消录制语音的回调
	* @param[in] callback 回调函数
	* @return void 反注册对象
	*/
	UnregisterCallback RegCancelCapture(const CancelCaptureCallback& callback);

	/**
	* 注册枚举录音采集设备的回调
	* @param[in] callback 回调函数
	* @return void 反注册对象
	*/
	UnregisterCallback RegEnumCaptureDevice(const EnumCaptureDeviceCallback& callback);

	/**
	* 开始录制语音的回调
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	void OnStartCaptureCallback(int code);

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
	void OnStopCaptureCallback(int rescode, const std::string& sid, const std::string& cid, const std::string& file_path, const std::string& file_ext, long file_size, int audio_duration);

	/**
	* 取消录制语音的回调
	* @param[in] code 返回码
	* @return void 无返回值
	*/
	void OnCancelCaptureCallback(int code);

	/**
	* 枚举本地录音采集设备的回调
	* @param[in] rescode 操作结果，一切正常200
	* @param[in] device_list 设备列表
	*/
	void OnEnumCaptureDeviceCallback(int rescode, const wchar_t* device_list);

	/**
	* 返回当前正在播放的语音消息所在的会话id
	* @return std::string 返回会话id
	*/
	std::string GetPlaySid();

	/**
	* 返回当前正在播放的语音消息的消息id
	* @return std::string 返回消息id
	*/
	std::string GetPlayCid();

	/**
	* 清除正在播放的语音消息所在的会话id和消息id
	* @return void 无返回值
	*/
	void ClearPlayId();

	/**
	* 返回当前正在录制的语音消息所在的会话id
	* @return std::string 返回会话id
	*/
	std::string GetCaptureSid();

	/**
	* 返回当前正在录制的语音消息的消息id
	* @return std::string 返回消息id
	*/
	std::string GetCaptureCid();

	/**
	* 清除正在录制的语音消息所在的会话id和消息id
	* @return void 无返回值
	*/
	void ClearCaptureId();
private:
	//当前正在播放的会话窗口id及语音消息id
	std::string play_sid_;
	std::string play_cid_;

	//当前正在录制语音的会话窗口id及语音消息id
	std::string capture_sid_;
	std::string capture_cid_;

	std::map<int, std::unique_ptr<StartCaptureCallback>> start_capture_cb_list_;
	std::map<int, std::unique_ptr<StopCaptureCallback>> stop_capture_cb_list_;
	std::map<int, std::unique_ptr<CancelCaptureCallback>> cancel_capture_cb_list_;
	std::map<int, std::unique_ptr<EnumCaptureDeviceCallback>> enum_device_cb_list_;
};
}
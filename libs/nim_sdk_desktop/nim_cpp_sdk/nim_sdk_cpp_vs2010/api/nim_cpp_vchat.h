/** @file nim_cpp_vchat.h
  * @brief NIM VChat提供的音视频（包括设备）相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/4/30
  */

#ifndef _NIM_SDK_CPP_VCHAT_H_
#define _NIM_SDK_CPP_VCHAT_H_

#include <string>
#include <functional>

namespace nim
{

#include "nim_vchat_def.h"
#include "nim_device_def.h"

class VChat
{

public:
	typedef std::function<void(bool ret, int code, const std::string& file, __int64 time)>  Mp4OptCallback;
	/** 
	* NIM VCHAT初始化，需要在SDK的nim_client_init成功之后
	* @param[in] json_extension 无效的扩展字段
	* @return bool 初始化结果，如果是false则以下所有接口调用无效
	*/
	static bool Init(const std::string& json_info);

	/** 
	* NIM VCHAT清理，需要在SDK的nim_client_cleanup之前
	* @param[in] json_extension 无效的扩展字段
	* @return void 无返回值
	*/
	static void Cleanup();

	/** 
	* NIM VCHAT DEVICE 遍历设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @param[in] json_extension 无效的扩展字段
	* @param[in] cb 结果回调见nim_device_def.h
	* @param[in] user_data 无效的扩展字段
	* @return void 无返回值
	*/
	static void EnumDeviceDevpath(nim::NIMDeviceType type, nim_vchat_enum_device_devpath_sync_cb_func cb);

	/** 
	* NIM VCHAT DEVICE 启动设备，同一NIMDeviceType下设备将不重复启动，不同的设备会先关闭前一个设备开启新设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @param[in] device_path 无效的扩展字段
	* @param[in] fps 摄像头为采样频率（一般传电源频率取50）,其他NIMDeviceType无效（麦克风采样频率由底层控制，播放器采样频率也由底层控制）
	* @param[in] json_extension 无效的扩展字段
	* @param[in] cb 结果回调见nim_device_def.h
	* @param[in] user_data 无效的扩展字段
	* @return void 无返回值
	*/
	static void StartDevice(nim::NIMDeviceType type, const std::string& device_path, unsigned fps, nim_vchat_start_device_cb_func cb);

	/** 
	* NIM VCHAT DEVICE 结束设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @param[in] json_extension 无效的扩展字段
	* @return void 无返回值
	*/
	static void EndDevice(nim::NIMDeviceType type);

	/** 
	* NIM VCHAT DEVICE 添加设备监听（摄像头和麦克风） 注意监听设备后底层会定时检查设备情况，在不需要监听后请移除
	* @param[in] type NIMDeviceType（kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo有效） 见nim_device_def.h
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void AddDeviceStatusCb(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb);
	
	/** 
	* NIM VCHAT DEVICE 移除设备监听（摄像头和麦克风）
	* @param[in] type NIMDeviceType（kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo有效） 见nim_device_def.h
	* @return void 无返回值
	*/
	static void RemoveDeviceStatusCb(nim::NIMDeviceType type);

	/** 
	* NIM VCHAT DEVICE 监听音频数据（可以不监听，通过启动设备kNIMDeviceTypeAudioOut和kNIMDeviceTypeAudioOutChat由底层播放）
	* @param[in] cb 结果回调见nim_device_def.h
	* @param[in] capture true 标识监听麦克风采集数据，false 标识监听通话中对方音频数据
	* @return void 无返回值
	*/
	static void SetAudioDataCb(bool capture, nim_vchat_audio_data_cb_func cb);

	/** 
	* NIM VCHAT DEVICE 监听视频数据
	* @param[in] cb 结果回调见nim_device_def.h
	* @param[in] capture true 标识监听采集数据，false 标识监听通话中对方视频数据
	* @return void 无返回值
	*/
	static void SetVideoDataCb(bool capture, nim_vchat_video_data_cb_func cb);

	/** 
	* NIM VCHAT DEVICE 设置音量 默认255,且音量均由软件换算得出,设置麦克风音量自动调节后麦克风音量参数无效
	* @param[in] volumn 结果回调见nim_device_def.h
	* @param[in] capture true 标识设置麦克风音量，false 标识设置播放音量
	* @return void 无返回值
	*/
	static void SetAudioVolumn(unsigned char volumn, bool capture);

	/** 
	* NIM VCHAT DEVICE 获取nim_vchat_set_audio_volumn中设置的音量
	* @param[in] capture true 标识获取麦克风音量，false 标识获取播放音量
	* @return unsigned char 音量值
	*/
	static unsigned char GetAudioVolumn(bool capture);

	/** 
	* NIM VCHAT DEVICE 设置麦克风音量自动调节
	* @param[in] auto_volumn true 标识麦克风音量自动调节，false 标识麦克风音量不调节，这时nim_vchat_set_audio_volumn中麦克风音量参数起效
	* @return void 无返回值
	*/
	static void SetAudioInputAutoVolumn(bool auto_volumn);

	/** 
	* NIM VCHAT DEVICE 获取是否自动调节麦克风音量
	* @return bool true 标识麦克风音量自动调节，false 标识麦克风音量不调节，这时nim_vchat_set_audio_volumn中麦克风音量参数起效
	*/
	static bool GetAudioInputAutoVolumn();

	/** 
	* NIM VCHAT 设置通话回调或者服务器通知
	* @param[in] cb 结果回调见nim_vchat_def.h
	* @return void 无返回值
	*/
	static void SetCbFunc(nim_vchat_cb_func cb);

	/** 
	* NIM VCHAT 启动通话，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] mode NIMVideoChatMode 启动音视频通话类型 见nim_vchat_def.h
	* @param[in] json_extension Json string 扩展，kNIMVChatUids成员id列表,kNIMVChatCustomVideo自主视频数据和kNIMVChatCustomAudio自主音频 如{"uids":["uid_temp"],"custom_video":1, "custom_audio":1}
	* @param[in] user_data 无效的扩展字段
	* @return bool true 调用成功，false 调用失败可能有正在进行的通话
	*/
	static bool Start(NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& json_info);

	/** 
	* NIM VCHAT 设置通话模式，在更改通话模式后，通知底层
	* @param[in] mode NIMVideoChatMode 音视频通话类型 见nim_vchat_def.h
	* @param[in] json_extension 无效的扩展字段
	* @return bool true 调用成功，false 调用失败
	*/
	static bool SetTalkingMode(NIMVideoChatMode mode, const std::string& json_extension);

	/** 
	* NIM VCHAT 回应音视频通话邀请，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] channel_id 音视频通话通道id
	* @param[in] accept true 接受，false 拒绝
	* @param[in] json_extension Json string 扩展，kNIMVChatCustomVideo自主视频数据和kNIMVChatCustomAudio自主音频 如{"custom_video":1, "custom_audio":1}
	* @return bool true 调用成功，false 调用失败（可能channel_id无匹配，如要接起另一路通话前先结束当前通话）
	*/
	static bool CalleeAck(unsigned __int64 channel_id, bool accept, const std::string& json_extension);

	/** 
	* NIM VCHAT 音视频通话控制，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] channel_id 音视频通话通道id
	* @param[in] type NIMVChatControlType 见nim_vchat_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool Control(unsigned __int64 channel_id, NIMVChatControlType type);

	/** 
	* NRTC 开始录制MP4文件，一次只允许一个录制文件，在通话开始的时候才有实际数据
	* @param[in] path 文件录制路径
	* @param[in] cb 结果回调
	* @return void 无返回值
	*/
	static void StartRecord(const std::string& path, Mp4OptCallback cb);

	/** 
	* NRTC 停止录制MP4文件
	* @param[in] cb 结果回调
	* @return void 无返回值
	*/
	static void StopRecord(Mp4OptCallback cb);

	/** 
	* NIM VCHAT 结束通话(需要主动在通话结束后调用，用于底层挂断和清理数据)
	* @param[in] json_extension 无效的扩展字段
	* @return void 无返回值
	*/
	static void End();
};
}

#endif //_NIM_SDK_CPP_VCHAT_H_
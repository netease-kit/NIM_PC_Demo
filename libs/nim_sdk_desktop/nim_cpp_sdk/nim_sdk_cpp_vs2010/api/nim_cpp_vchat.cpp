/** @file nim_cpp_vchat.cpp
  * @brief NIM VChat提供的音视频（包括设备）相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/4/30
  */

#include "nim_cpp_vchat.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"

namespace nim
{

typedef	bool(*nim_vchat_init)(const char *json_extension);
typedef	void(*nim_vchat_cleanup)(const char *json_extension);

typedef void(*nim_vchat_enum_device_devpath)(nim::NIMDeviceType type, const char *json_extension, nim_vchat_enum_device_devpath_sync_cb_func cb, const void *user_data);
typedef void(*nim_vchat_start_device)(nim::NIMDeviceType type, const char* device_path, unsigned fps, const char* json_extension, nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_end_device)(nim::NIMDeviceType type, const char *json_extension);
typedef void(*nim_vchat_add_device_status_cb)(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb, const void *user_data);
typedef void(*nim_vchat_remove_device_status_cb)(nim::NIMDeviceType type);
typedef void(*nim_vchat_set_audio_data_cb)(bool capture, const char* json_extension, nim_vchat_audio_data_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_video_data_cb)(bool capture, const char* json_extension, nim_vchat_video_data_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_audio_volumn)(unsigned char volumn, bool capture);
typedef unsigned char(*nim_vchat_get_audio_volumn)(bool capture);
typedef void(*nim_vchat_set_audio_input_auto_volumn)(bool auto_volumn);
typedef bool(*nim_vchat_get_audio_input_auto_volumn)();

//设置回掉
typedef void(*nim_vchat_set_cb_func)(nim_vchat_cb_func cb, const void *user_data);
//启动通话
//json_info 见nim_vchat_def.h
typedef bool(*nim_vchat_start)(NIMVideoChatMode mode, const char* apns_text, const char* custom_info, const char* json_extension, const void *user_data);
//设置通话模式
typedef bool(*nim_vchat_set_talking_mode)(NIMVideoChatMode mode, const char* json_extension);
//回应邀请
typedef bool(*nim_vchat_callee_ack)(__int64 channel_id, bool accept, const char* json_extension, const void *user_data);
//通话控制
typedef bool(*nim_vchat_control)(__int64 channel_id, NIMVChatControlType type, const char* json_extension, const void *user_data);
//开始录制MP4文件，一次只允许一个录制文件，在通话开始的时候才有实际数据
typedef void(*nim_vchat_start_record)(const char *path, const char *json_extension, nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);
//停止录制MP4文件
typedef void(*nim_vchat_stop_record)(const char *json_extension, nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);
//结束通话
typedef void(*nim_vchat_end)(const char* json_extension);


//dll-------------------------------
//NIM vchat初始化
bool VChat::Init(const std::string& json_info)
{
	return NIM_SDK_GET_FUNC(nim_vchat_init)(json_info.c_str());
}
//NIM vchat清理
void VChat::Cleanup()
{
	NIM_SDK_GET_FUNC(nim_vchat_cleanup)("");
}

//device ---------------------------
//遍历设备
//type 见NIMDeviceType定义
void VChat::EnumDeviceDevpath(nim::NIMDeviceType type, nim_vchat_enum_device_devpath_sync_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_enum_device_devpath)(type, "", cb, nullptr);
}

//启动设备，(同一NIMDeviceType下)同一个设备将不重复启动，不同的设备会先关闭前一个设备开启新设备
//音频分2类设备分别是本地音频播放和通话时对方语音播放
//fps 摄像头为采样频率（一般传电源频率取50）
//麦克风采样频率由底层控制，播放器采样频率也由底层控制
//type 见NIMDeviceType定义
void VChat::StartDevice(nim::NIMDeviceType type, const std::string& device_path, unsigned fps, nim_vchat_start_device_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_start_device)(type, device_path.c_str(), fps, "", cb, nullptr);
}
//结束设备
void VChat::EndDevice(nim::NIMDeviceType type)
{
	NIM_SDK_GET_FUNC(nim_vchat_end_device)(type, "");
}

//添加设备监听（摄像头和麦克风） 注意监听设备后底层会定时检查设备情况，在不需要监听后请移除
void VChat::AddDeviceStatusCb(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_add_device_status_cb)(type, cb, nullptr);
}
//移除设备监听（摄像头和麦克风）
void VChat::RemoveDeviceStatusCb(nim::NIMDeviceType type)
{
	NIM_SDK_GET_FUNC(nim_vchat_remove_device_status_cb)(type);
}

//capture = true	监听音频采集数据（麦克风）
//capture = false	监听音频接收数据（接收到的语音数据，由底层播放，可以不监听）
void VChat::SetAudioDataCb(bool capture, nim_vchat_audio_data_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_data_cb)(capture, "", cb, nullptr);
}
//capture = true	监听视频采集数据（摄像头）
//capture = false	监听视频接收数据（接收到的图像数据，由上层播放）
void VChat::SetVideoDataCb(bool capture, nim_vchat_video_data_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_video_data_cb)(capture, "", cb, nullptr);
}

//设置相关接口
//音量控制  
//capture=true	设置麦克风音量 默认255,设置麦克风音量自动调节后此参数无效
//capture=false	设置播放音量（采集音频播放的声音由麦克风音频调节，这里只调节通话对方的音量）默认255
void VChat::SetAudioVolumn(unsigned char volumn, bool capture)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_volumn)(volumn, capture);
}
//capture=true	获取麦克风音量
//capture=false	获取播放音量（采集音频播放的声音由麦克风音频调节，这里只调节通话对方的音量）
unsigned char VChat::GetAudioVolumn(bool capture)
{
	return NIM_SDK_GET_FUNC(nim_vchat_get_audio_volumn)(capture);
}
//自动调节麦克风音量
void VChat::SetAudioInputAutoVolumn(bool auto_volumn)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_input_auto_volumn)(auto_volumn);
}
//是否自动调节麦克风音量
bool VChat::GetAudioInputAutoVolumn()
{
	return NIM_SDK_GET_FUNC(nim_vchat_get_audio_input_auto_volumn)();
}

//音视频通话-------------------------------
//设置回掉
void VChat::SetCbFunc(nim_vchat_cb_func cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_cb_func)(cb, nullptr);
}
//启动通话
//json_info 见nim_vchat_def.h
bool VChat::Start(NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& json_info)
{
	return NIM_SDK_GET_FUNC(nim_vchat_start)(mode, apns_text.c_str(), custom_info.c_str(), json_info.c_str(), nullptr);
}
//设置通话模式
bool VChat::SetTalkingMode(NIMVideoChatMode mode, const std::string& json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_set_talking_mode)(mode, json_extension.c_str());
}
//回应邀请
bool VChat::CalleeAck(unsigned __int64 channel_id, bool accept, const std::string& json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_callee_ack)(channel_id, accept, json_extension.c_str(), nullptr);
}
//通话控制
bool VChat::Control(unsigned __int64 channel_id, NIMVChatControlType type)
{
	return NIM_SDK_GET_FUNC(nim_vchat_control)(channel_id, type, "", nullptr);
}
static void mp4_record_opt_cb(bool ret, int code, const char *file, __int64 time, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		VChat::Mp4OptCallback* cb_pointer = (VChat::Mp4OptCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(ret, code, PCharToString(file), time);
		}
		delete user_data;
	}
}
void VChat::StartRecord(const std::string& path, Mp4OptCallback cb)
{
	Mp4OptCallback* cb_pointer = new Mp4OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_start_record)(path.c_str(), "", mp4_record_opt_cb, cb_pointer);
}
//停止录制MP4文件
void VChat::StopRecord(Mp4OptCallback cb)
{
	Mp4OptCallback* cb_pointer = new Mp4OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_stop_record)("", mp4_record_opt_cb, cb_pointer);
}
//结束通话
void VChat::End()
{
	NIM_SDK_GET_FUNC(nim_vchat_end)("");
}

}  // namespace nim

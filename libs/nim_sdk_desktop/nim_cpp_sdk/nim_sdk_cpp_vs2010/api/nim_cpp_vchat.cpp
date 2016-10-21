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
typedef void(*nim_vchat_start_extend_camera)(const char *id, const char *device_path, unsigned fps, const char *json_extension, nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_stop_extend_camera)(const char *id, const char *json_extension);

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
//自定义视频数据
typedef bool(*nim_vchat_custom_video_data)(unsigned __int64 time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension);

//NIM 通话中修改分辨率
typedef void(*nim_vchat_set_video_quality)(int video_quality, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data); 
typedef void(*nim_vchat_set_video_bitrate)(int video_bitrate, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_frame_rate)(NIMVChatVideoFrameRate frame_rate, const char* json_extension, nim_vchat_opt_cb_func cb, const void *user_data);

//NIM 通话中修改自定义音视频数据模式
typedef void(*nim_vchat_set_custom_data)(bool custom_audio, bool custom_video, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);

/** @fn void nim_vchat_set_viewer_mode(bool viewer)
* NIM VCHAT 设置观众模式（多人模式下），全局有效（重新发起时也生效），观众模式能减少运行开销
* @param[in] viewer 是否观众模式
* @return void 无返回值
*/
typedef void(*nim_vchat_set_viewer_mode)(bool viewer);

/** @fn bool nim_vchat_get_viewer_mode()
* NIM VCHAT 获取当前是否是观众模式
* @return bool true 观众模式，false 非观众模式
*/
typedef bool(*nim_vchat_get_viewer_mode)();

/** @fn void nim_vchat_set_audio_mute(bool muted)
* NIM VCHAT 设置音频静音，全局有效（重新发起时也生效）
* @param[in] muted true 静音，false 不静音
* @return void 无返回值
*/
typedef void(*nim_vchat_set_audio_mute)(bool muted);

/** @fn bool nim_vchat_audio_mute_enabled()
* NIM VCHAT 获取音频静音状态
* @return bool true 静音，false 不静音
*/
typedef bool(*nim_vchat_audio_mute_enabled)();

/** @fn void nim_vchat_set_rotate_remote_video(bool rotate)
* NIM VCHAT 设置不自动旋转对方画面，默认打开，全局有效（重新发起时也生效）
* @param[in] rotate true 自动旋转，false 不旋转
* @return void 无返回值
*/
typedef void(*nim_vchat_set_rotate_remote_video)(bool rotate);

/** @fn bool nim_vchat_rotate_remote_video_enabled()
* NIM VCHAT 获取自动旋转对方画面设置状态
* @return bool true 自动旋转，false 不旋转
*/
typedef bool(*nim_vchat_rotate_remote_video_enabled)();

/** @fn void nim_vchat_set_member_in_blacklist(const char *uid, bool add, bool audio, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data)
* NIM VCHAT 设置单个成员的黑名单状态，当前通话有效(只能设置进入过房间的成员)
* @param[in] uid 成员account
* @param[in] add true表示添加到黑名单，false表示从黑名单移除
* @param[in] audio true表示音频黑名单，false表示视频黑名单
* @param[in] json_extension 无效扩展字段
* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_vchat_set_member_in_blacklist)(const char *uid, bool add, bool audio, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);

/** @fn void nim_vchat_create_room(const char *room_name, const char *custom_info, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data)
* NIM VCHAT 创建一个多人房间（后续需要主动调用加入接口进入房间）
* @param[in] room_name 房间名
* @param[in] custom_info 自定义的房间信息（加入房间的时候会返回）
* @param[in] json_extension 无效扩展字段
* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_vchat_create_room)(const char *room_name, const char *custom_info, const char *json_extension, nim_vchat_opt2_cb_func cb, const void *user_data);

/** @fn bool nim_vchat_join_room(const char *room_name, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data)
* NIM VCHAT 加入一个多人房间（进入房间后成员变化等，等同点对点nim_vchat_cb_func）
* @param[in] mode NIMVideoChatMode 音视频通话类型 见nim_vchat_def.h
* @param[in] room_name 房间名
* @param[in] json_extension 可选 如{"custom_video":0, "custom_audio":0, "video_quality":0, "session_id":"1231sda"}
* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension扩展字段中包含 kNIMVChatCustomInfo,kNIMVChatSessionId
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return bool true 调用成功，false 调用失败可能有正在进行的通话
*/
typedef bool(*nim_vchat_join_room)(NIMVideoChatMode mode, const char *room_name, const char *json_extension, nim_vchat_opt2_cb_func cb, const void *user_data);

/** @fn void nim_vchat_update_rtmp_url(const char *rtmp_url, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data)
* NIM 通话中修改直播推流地址（主播有效）
* @param[in] rtmp_url 新的rtmp推流地址
* @param[in] json_extension 无效扩展字段
* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_vchat_update_rtmp_url)(const char *rtmp_url, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_streaming_mode)(bool streaming, const char* json_info, nim_vchat_opt_cb_func cb, const void *user_data);

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
void VChat::StartDevice(nim::NIMDeviceType type, const std::string& device_path, unsigned fps, int width, int height, nim_vchat_start_device_cb_func cb)
{
	std::string json_value;
	if (width * height > 0)
	{
		Json::FastWriter fs;
		Json::Value value;
		value[nim::kNIMDeviceWidth] = width;
		value[nim::kNIMDeviceHeight] = height;
		json_value = fs.write(value);
	}
	//if (type == kNIMDeviceTypeVideo)
	//{
	//	NIM_SDK_GET_FUNC(nim_vchat_start_extend_camera)("video_extend", device_path.c_str(), fps, json_value.c_str(), cb, nullptr);
	//} 
	//else
	{
		NIM_SDK_GET_FUNC(nim_vchat_start_device)(type, device_path.c_str(), fps, json_value.c_str(), cb, nullptr);
	}
}
//结束设备
void VChat::EndDevice(nim::NIMDeviceType type)
{
	//if (type == kNIMDeviceTypeVideo)
	//{
	//	NIM_SDK_GET_FUNC(nim_vchat_stop_extend_camera)("video_extend", "");
	//}
	//else
	{
		NIM_SDK_GET_FUNC(nim_vchat_end_device)(type, "");
	}
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
	Json::FastWriter fs;
	Json::Value value;
	value[nim::kNIMVideoSubType] = kNIMVideoSubTypeI420;
	std::string json_value = fs.write(value);
	NIM_SDK_GET_FUNC(nim_vchat_set_video_data_cb)(capture, json_value.c_str(), cb, nullptr);
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
			//PostTaskToUIThread(std::bind((*cb_pointer), ret, code, PCharToString(file), time));
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
void VChat::End(const std::string& json_extension)
{
	NIM_SDK_GET_FUNC(nim_vchat_end)(json_extension.c_str());
}
void VChat::SetVideoQuality(int video_quality)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_video_quality)(video_quality, "", nullptr, nullptr);
}
void VChat::SetVideoBitrate(int video_bitrate)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_video_bitrate)(video_bitrate, "", nullptr, nullptr);
}
void VChat::SetFrameRate(NIMVChatVideoFrameRate frame_rate)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_frame_rate)(frame_rate, "", nullptr, nullptr);
}
void VChat::SetCustomData(bool custom_audio, bool custom_video)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_custom_data)(custom_audio, custom_video, "", nullptr, nullptr);
}
bool VChat::CustomVideoData(unsigned __int64 time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_custom_video_data)(time, data, size, width, height, json_extension);
}

void VChat::SetViewerMode(bool viewer)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_viewer_mode)(viewer);
}
bool VChat::GetViewerMode()
{
	return NIM_SDK_GET_FUNC(nim_vchat_get_viewer_mode)();
}

void VChat::SetAudioMuted(bool muted)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_mute)(muted);
}
bool VChat::GetAudioMuteEnabled()
{
	return NIM_SDK_GET_FUNC(nim_vchat_audio_mute_enabled)();
}
void VChat::SetRotateRemoteVideo(bool rotate)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_rotate_remote_video)(rotate);
}
bool VChat::IsRotateRemoteVideo()
{
	return NIM_SDK_GET_FUNC(nim_vchat_rotate_remote_video_enabled)();
}
static void OnOptCallback(bool ret, int code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		VChat::OptCallback* cb_pointer = (VChat::OptCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(ret, code, PCharToString(json_extension));
			//PostTaskToUIThread(std::bind((*cb_pointer), ret, code, PCharToString(json_extension)));
		}
		delete user_data;
	}
}
void VChat::SetMemberBlacklist(const std::string& uid, bool add, bool audio, const std::string& json_extension, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_set_member_in_blacklist)(uid.c_str(), add, audio, json_extension.c_str(), OnOptCallback, cb_pointer );
}

static void OnOpt2Callback(int code, __int64 cannel_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		VChat::Opt2Callback* cb_pointer = (VChat::Opt2Callback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(code, cannel_id, PCharToString(json_extension));
			//PostTaskToUIThread(std::bind((*cb_pointer), code, cannel_id, PCharToString(json_extension)));
		}
		delete user_data;
	}
}
void VChat::CreateRoom(const std::string& room_name, const std::string& custom_info, const std::string& json_extension, Opt2Callback cb)
{
	Opt2Callback* cb_pointer = new Opt2Callback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_create_room)(room_name.c_str(), custom_info.c_str(), json_extension.c_str(), OnOpt2Callback, cb_pointer);
}
bool VChat::JoinRoom(NIMVideoChatMode mode, const std::string& room_name, const std::string& json_extension, Opt2Callback cb)
{
	Opt2Callback* cb_pointer = new Opt2Callback(cb);
	return NIM_SDK_GET_FUNC(nim_vchat_join_room)(mode, room_name.c_str(), json_extension.c_str(), OnOpt2Callback, cb_pointer);
}
void VChat::UpdateRtmpUrl(const std::string& rtmp_url, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	return NIM_SDK_GET_FUNC(nim_vchat_update_rtmp_url)(rtmp_url.c_str(), "", OnOptCallback, cb_pointer);
}
void VChat::SetStreamingMode(bool streaming, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	return NIM_SDK_GET_FUNC(nim_vchat_set_streaming_mode)(streaming, "", OnOptCallback, cb_pointer);
}

}  // namespace nim

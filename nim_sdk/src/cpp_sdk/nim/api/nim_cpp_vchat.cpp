/** @file nim_cpp_vchat.cpp
  * @brief NIM VChat提供的音视频（包括设备）相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/4/30
  */

#include "src/cpp_sdk/nim/api//nim_cpp_vchat.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"

namespace nim
{

#ifdef NIM_SDK_DLL_IMPORT
typedef	bool(*nim_vchat_init)(const char *json_extension);
typedef	void(*nim_vchat_set_proxy)(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);
typedef	void(*nim_vchat_cleanup)(const char *json_extension);
typedef uint64_t(*nim_vchat_net_detect)(const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_rate)(int rating, const char* description, const char* json_extension, const nim_vchat_rate_callback cb, const void* user_data);

typedef void(*nim_vchat_enum_device_devpath)(nim::NIMDeviceType type, const char *json_extension, nim_vchat_enum_device_devpath_sync_cb_func cb, const void *user_data);
typedef void(*nim_vchat_start_device)(nim::NIMDeviceType type, const char* device_path, unsigned fps, const char* json_extension, nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_end_device)(nim::NIMDeviceType type, const char *json_extension);
typedef void(*nim_vchat_add_device_status_cb)(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb, const void *user_data);
typedef void(*nim_vchat_remove_device_status_cb)(nim::NIMDeviceType type);
typedef void(*nim_vchat_start_extend_camera)(const char *id, const char *device_path, unsigned fps, const char *json_extension, nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_stop_extend_camera)(const char *id, const char *json_extension);

typedef void(*nim_vchat_set_audio_data_cb)(bool capture, const char* json_extension, nim_vchat_audio_data_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_audio_data_cb_ex)(int type, const char *json_extension, nim_vchat_audio_data_cb_func_ex cb, const void *user_data);
typedef void(*nim_vchat_set_video_data_cb)(bool capture, const char* json_extension, nim_vchat_video_data_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_audio_volumn)(unsigned char volumn, bool capture);
typedef unsigned char(*nim_vchat_get_audio_volumn)(bool capture);
typedef void(*nim_vchat_set_audio_input_auto_volumn)(bool auto_volumn);
typedef bool(*nim_vchat_get_audio_input_auto_volumn)();
typedef void(*nim_vchat_set_audio_process_info)(bool aec, bool ns, bool vad);
//typedef void(*nim_vchat_set_audio_howling_suppression)(bool work);
/** @fn void nim_vchat_preload_effect(int id, const char *path, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data)
  * NIM VCHAT DEVICE 预加载音效文件（空路径时为移除），音效个数限制5个，时长限制10秒
  * @param[in] id 音效id，重复时覆盖
  * @param[in] path 音效文件的本地路径，如果填空为移除音效
  * @param[in] json_extension 扩展
  * @param[in] cb 结果回调见nim_device_def.h；异步回调操作结果
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void(*nim_vchat_preload_effect)(int id, const char *path, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data);

/** @fn void nim_vchat_play_effect(int id, int loop_count, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data)
  * NIM VCHAT DEVICE 播放音效，由kNIMDeviceTypeAudioOutChat播放
  * @param[in] id 音效id
  * @param[in] loop_count 播放次数，0无效，小于零无限循环
  * @param[in] json_extension 扩展
  * @param[in] cb 结果回调见nim_device_def.h；异步回调操作结果，成功则在播放结束后回调；当音效未加载或者前一次同一个id的音效未播放完成，则失败
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void(*nim_vchat_play_effect)(int id, int loop_count, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data);

/** @fn void nim_vchat_stop_effect(int id, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data)
  * NIM VCHAT DEVICE 结束播放音效
  * @param[in] id 音效id
  * @param[in] json_extension 扩展
  * @param[in] cb 结果回调见nim_device_def.h；异步回调操作结果
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void(*nim_vchat_stop_effect)(int id, const char *json_extension, nim_vchat_audio_effect_opt_cb_func cb, const void *user_data);

//设置回掉
typedef void(*nim_vchat_set_cb_func)(nim_vchat_cb_func cb, const void *user_data);
//启动通话
//json_info 见nim_vchat_def.h
typedef bool(*nim_vchat_start)(NIMVideoChatMode mode, const char* apns_text, const char* custom_info, const char* json_extension, const void *user_data);
//设置通话模式
typedef bool(*nim_vchat_set_talking_mode)(NIMVideoChatMode mode, const char* json_extension);
//回应邀请
typedef bool(*nim_vchat_callee_ack)(int64_t channel_id, bool accept, const char* json_extension, const void *user_data);
//通话控制
typedef bool(*nim_vchat_control)(int64_t channel_id, NIMVChatControlType type, const char* json_extension, const void *user_data);
//开始录制MP4文件，一次只允许一个录制文件，在通话开始的时候才有实际数据
typedef void(*nim_vchat_start_record)(const char *path, const char *json_extension, nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);
//停止录制MP4文件
typedef void(*nim_vchat_stop_record)(const char *json_extension, nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);
//NIM VCHAT 开始录制音频文件，一次只允许一个音频录制文件
typedef void(*nim_vchat_start_audio_record)(const char *path, const char *json_extension, nim_vchat_audio_record_opt_cb_func cb, const void *user_data);
//NIM VCHAT 停止录制音频文件
typedef void(*nim_vchat_stop_audio_record)(const char *json_extension, nim_vchat_audio_record_opt_cb_func cb, const void *user_data);
//结束通话
typedef void(*nim_vchat_end)(const char* json_extension);
typedef void(*nim_vchat_relogin)(const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
//自定义视频数据
typedef bool(*nim_vchat_custom_video_data)(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension);
typedef bool(*nim_vchat_custom_audio_data)(uint64_t time, const char *data, uint32_t size, const char *json_extension);
typedef bool(*nim_vchat_accompanying_sound)(unsigned char id, unsigned __int64 time, const char *data, unsigned int size, unsigned int rate, unsigned int channels, const char *json_extension);

//NIM 通话中修改分辨率
typedef void(*nim_vchat_set_video_quality)(int video_quality, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data); 
typedef void(*nim_vchat_set_video_bitrate)(int video_bitrate, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_frame_rate)(NIMVChatVideoFrameRate frame_rate, const char* json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_video_frame_scale)(NIMVChatVideoFrameScaleType type);
typedef int(*nim_vchat_get_video_frame_scale_type)();
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
typedef void(*nim_vchat_set_audio_play_mute)(bool muted);
typedef bool(*nim_vchat_audio_play_mute_enabled)();

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

typedef void(*nim_vchat_select_video_adaptive_strategy)(NIMVChatVideoEncodeMode mode, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
//typedef void(*nim_vchat_set_streaming_mode)(bool streaming, const char* json_info, nim_vchat_opt_cb_func cb, const void *user_data);

typedef void (*nim_vchat_set_uid_picture_as_main)(const char *uid, const char* json_extension, nim_vchat_opt_cb_func cb, const void* user_data);

//订阅相关
typedef bool(*nim_vchat_set_auto_publish)(bool auto_pub, bool auto_sub);
typedef bool(*nim_vchat_is_auto_publish_video)();
typedef bool(*nim_vchat_is_auto_subscribe_video)();
typedef bool(*nim_vchat_is_auto_subscribe_audio)();
typedef bool(*nrtc_chat_set_local_video_simulcast_mode)(NIMVChatPublishVideoStreamMode video_stream_mode);
typedef void(*nim_vchat_publish_video)(const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_unpublish_video)(const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_subscribe_video)(const char *uid, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_unsubscribe_video)(const char *uid, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_subscribe_audio)(bool sub, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);

typedef void(*nim_vchat_set_remote_audio_data_cb)(const char *uid, nim_vchat_remote_audio_data_cb_func remote_data_cb, const void *remote_data_cb_user_data, const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
#else
#include "nim_vchat.h"
#endif

static void OnOptCallback(bool ret, int code, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<VChat::OptCallback>(user_data, [=](const VChat::OptCallback& cb){

		CallbackProxy::Invoke(cb, ret, code, PCharToString(json_extension));

	}, true);
}

static void OnEffectOptCallback(int id, bool ret, int code, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<VChat::EffectOptCallback>(user_data, [=](const VChat::EffectOptCallback& cb) {

		CallbackProxy::Invoke(cb, id, ret, code, PCharToString(json_extension));

	}, true);
}

static void CallbackNetDetect(bool ret, int rescode, const char *json_params, const void *user_data)
{

	CallbackProxy::DoSafeCallback<VChat::NetDetectCallback>(user_data, [=](const VChat::NetDetectCallback& cb){

		NetDetectCbInfo res;
		res.res_code_ = rescode;
		nim_cpp_wrapper_util::Json::Reader reader;
		nim_cpp_wrapper_util::Json::Value values;
		if (reader.parse(PCharToString(json_params), values) && values.isObject())
		{
			res.loss_ = values[kNIMNetDetectLoss].asInt();
			res.rtt_max_ = values[kNIMNetDetectRttmax].asInt();
			res.rtt_min_ = values[kNIMNetDetectRttmin].asInt();
			res.rtt_avg_ = values[kNIMNetDetectRttavg].asInt();
			res.rtt_mdev_ = values[kNIMNetDetectRttmdev].asInt();
			res.expand_info_ = values[kNIMNetDetectDetail].asString();
		}
		CallbackProxy::Invoke(cb, rescode, res);

	}, true);
}
//dll-------------------------------
//NIM vchat初始化
bool VChat::Init(const std::string& server_setting_path)
{
	std::string json_value;
	if (!server_setting_path.empty())
	{
		nim_cpp_wrapper_util::Json::FastWriter fs;
		nim_cpp_wrapper_util::Json::Value value;
		value[nim::kNIMVChatServerSettingPath] = server_setting_path;
		json_value = fs.write(value);
	}
	return NIM_SDK_GET_FUNC(nim_vchat_init)(json_value.c_str());
}
void VChat::SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_proxy)(type, host.c_str(), port, user.c_str(), password.c_str());
}
//NIM vchat清理
void VChat::Cleanup()
{
	NIM_SDK_GET_FUNC(nim_vchat_cleanup)("");
}
uint64_t VChat::NetDetect(NetDetectCallback cb)
{
	NetDetectCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new NetDetectCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_vchat_net_detect)("", CallbackNetDetect, cb_pointer);
}
uint64_t VChat::NetDetectEx(int32_t ms_limit, nim::NIMNetDetectType type, nim::NIMNetDetectVideoQuality quality_type, NetDetectCallback cb)
{
	std::string json_value;
	nim_cpp_wrapper_util::Json::FastWriter fs;
	nim_cpp_wrapper_util::Json::Value value;
	if (ms_limit > 0)
	{
		value[nim::kNIMNetDetectTimeLimit] = ms_limit;
	}
	if (quality_type != kNIMNDVideoQualityDefault)
	{
		value[nim::kNIMNetDetectQualityType] = quality_type;
	}
	value[nim::kNIMNetDetectType] = type;
	json_value = fs.write(value);
	NetDetectCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new NetDetectCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_vchat_net_detect)(json_value.c_str(), CallbackNetDetect, cb_pointer);
}
void VChat::VChatRate(int rating, const std::string& description, OptCallback cb)
{
	OptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new OptCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_vchat_rate)(rating, description.c_str(), "", OnOptCallback, cb_pointer);
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
		nim_cpp_wrapper_util::Json::FastWriter fs;
		nim_cpp_wrapper_util::Json::Value value;
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
void VChat::SetAudioDataCbEx(int type, std::string json, nim_vchat_audio_data_cb_func_ex cb)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_data_cb_ex)(type, json.c_str(), cb, nullptr);
}
//capture = true	监听视频采集数据（摄像头）
//capture = false	监听视频接收数据（接收到的图像数据，由上层播放）
void VChat::SetVideoDataCb(bool capture, nim_vchat_video_data_cb_func cb)
{
	nim_cpp_wrapper_util::Json::FastWriter fs;
	nim_cpp_wrapper_util::Json::Value value;
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
void VChat::SetAudioProcess(bool aec, bool ns, bool vad)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_process_info)(aec, ns, vad);
}
//音效

void VChat::PreloadEffect(int id, const std::string& path, EffectOptCallback cb)
{
	EffectOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new EffectOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_vchat_preload_effect)(id, path.c_str(), "", OnEffectOptCallback, cb_pointer);
}
void VChat::PlayEffect(int id, int loop_count, EffectOptCallback cb)
{
	EffectOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new EffectOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_vchat_play_effect)(id, loop_count, "", OnEffectOptCallback, cb_pointer);
}
void VChat::StopEffect(int id, EffectOptCallback cb)
{
	EffectOptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new EffectOptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_vchat_stop_effect)(id, "", OnEffectOptCallback, cb_pointer);
}

/** @fn void SetAudioHowlingSuppression(bool work)
* NIM VCHAT DEVICE 设置底层针对麦克风采集数据处理开关接口，默认关闭啸叫检测（此接口是全局接口，在sdk初始化后设置一直有效）
* @param[in] work true 标识打开啸叫检测功能，false 标识关闭
* @return void 无返回值
*/
//static void SetAudioHowlingSuppression(bool work);
//void VChat::SetAudioHowlingSuppression(bool work)
//{
//	NIM_SDK_GET_FUNC(nim_vchat_set_audio_howling_suppression)(work);
//}

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
bool VChat::CalleeAck(uint64_t channel_id, bool accept, const std::string& json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_callee_ack)(channel_id, accept, json_extension.c_str(), nullptr);
}
//通话控制
bool VChat::Control(uint64_t channel_id, NIMVChatControlType type)
{
	return NIM_SDK_GET_FUNC(nim_vchat_control)(channel_id, type, "", nullptr);
}
static void mp4_record_opt_cb(bool ret, int code, const char *file, int64_t time, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<VChat::Mp4OptCallback>(user_data, [=](const VChat::Mp4OptCallback& cb){

		CallbackProxy::Invoke(cb, ret, code, PCharToString(file), time);
	}, true);
}
void VChat::StartRecord(const std::string& path, const std::string& uid, Mp4OptCallback cb)
{
	Mp4OptCallback* cb_pointer = new Mp4OptCallback(cb);
	std::string json_value;
	if (!uid.empty())
	{
		nim_cpp_wrapper_util::Json::FastWriter fs;
		nim_cpp_wrapper_util::Json::Value value;
		value[nim::kNIMVChatUid] = uid;
		json_value = fs.write(value);
	}
	NIM_SDK_GET_FUNC(nim_vchat_start_record)(path.c_str(), json_value.c_str(), mp4_record_opt_cb, cb_pointer);
}
//停止录制MP4文件
void VChat::StopRecord(const std::string& uid, Mp4OptCallback cb)
{
	Mp4OptCallback* cb_pointer = new Mp4OptCallback(cb);
	std::string json_value;
	if (!uid.empty())
	{
		nim_cpp_wrapper_util::Json::FastWriter fs;
		nim_cpp_wrapper_util::Json::Value value;
		value[nim::kNIMVChatUid] = uid;
		json_value = fs.write(value);
	}
	NIM_SDK_GET_FUNC(nim_vchat_stop_record)(json_value.c_str(), mp4_record_opt_cb, cb_pointer);
}
static void audio_record_opt_cb(bool ret, int code, const char *file, int64_t time, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		VChat::AudioRecordCallback* cb_pointer = (VChat::AudioRecordCallback*)user_data;
		if (*cb_pointer)
		{
			CallbackProxy::Invoke(*cb_pointer,ret, code, PCharToString(file), time);
		}
		delete user_data;
	}
}
void VChat::StartAudioRecord(const std::string& path, AudioRecordCallback cb)
{
	AudioRecordCallback* cb_pointer = new AudioRecordCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_start_audio_record)(path.c_str(), "", audio_record_opt_cb, cb_pointer);
}
void VChat::StopAudioRecord(AudioRecordCallback cb)
{
	AudioRecordCallback* cb_pointer = new AudioRecordCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_stop_audio_record)("", audio_record_opt_cb, cb_pointer);
}
//结束通话
void VChat::End(const std::string& json_extension)
{
	NIM_SDK_GET_FUNC(nim_vchat_end)(json_extension.c_str());
}
void VChat::Relogin(const std::string& session_id, OptCallback cb)
{
	std::string json_value;
	if (!session_id.empty())
	{
		nim_cpp_wrapper_util::Json::FastWriter fs;
		nim_cpp_wrapper_util::Json::Value value;
		value[nim::kNIMVChatSessionId] = session_id;
		json_value = fs.write(value);
	}
	OptCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new OptCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_vchat_relogin)(json_value.c_str(), OnOptCallback, cb_pointer);
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
bool VChat::CustomVideoData(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_custom_video_data)(time, data, size, width, height, json_extension);
}
bool VChat::CustomAudioData(uint64_t time, const char *data, unsigned int size, const char *json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_custom_audio_data)(time, data, size, json_extension);
}
bool VChat::AccompanyingSound(unsigned char id, unsigned __int64 time, const char *data, unsigned int size, unsigned int rate, unsigned int channels, const char *json_extension)
{
	return NIM_SDK_GET_FUNC(nim_vchat_accompanying_sound)(id, time, data, size, rate, channels, json_extension);
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
void VChat::SetAudioPlayMuted(bool muted)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_audio_play_mute)(muted);
}
bool VChat::GetAudioMutePlayEnabled()
{
	return NIM_SDK_GET_FUNC(nim_vchat_audio_play_mute_enabled)();
}
void VChat::SetRotateRemoteVideo(bool rotate)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_rotate_remote_video)(rotate);
}
bool VChat::IsRotateRemoteVideo()
{
	return NIM_SDK_GET_FUNC(nim_vchat_rotate_remote_video_enabled)();
}
void VChat::SetVideoFrameScaleType(NIMVChatVideoFrameScaleType type)
{
	NIM_SDK_GET_FUNC(nim_vchat_set_video_frame_scale)(type);
}
int VChat::GetVideoFrameScaleType()
{
	return NIM_SDK_GET_FUNC(nim_vchat_get_video_frame_scale_type)();
}
void VChat::SetMemberBlacklist(const std::string& uid, bool add, bool audio, const std::string& json_extension, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_set_member_in_blacklist)(uid.c_str(), add, audio, json_extension.c_str(), OnOptCallback, cb_pointer );
}

static void OnOpt2Callback(int code, int64_t cannel_id, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<VChat::Opt2Callback>(user_data, [=](const VChat::Opt2Callback& cb){

		CallbackProxy::Invoke(cb, code, cannel_id, PCharToString(json_extension));
	}, true);
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
	NIM_SDK_GET_FUNC(nim_vchat_update_rtmp_url)(rtmp_url.c_str(), "", OnOptCallback, cb_pointer);
}
void VChat::SelectVideoAdaptiveStrategy(NIMVChatVideoEncodeMode mode, const std::string& json_extension, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_select_video_adaptive_strategy)(mode, json_extension.c_str(), OnOptCallback, cb_pointer);
}
//void VChat::SetStreamingMode(bool streaming, OptCallback cb)
//{
//	OptCallback* cb_pointer = new OptCallback(cb);
//	return NIM_SDK_GET_FUNC(nim_vchat_set_streaming_mode)(streaming, "", OnOptCallback, cb_pointer);
//}

void VChat::SetUidAsMainPicture(const std::string& uid, const std::string& json_extension, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_set_uid_picture_as_main)(uid.c_str(), json_extension.c_str(), OnOptCallback, cb_pointer);
}
bool VChat::SetAutoPublish(bool auto_pub, bool auto_sub)
{
	return NIM_SDK_GET_FUNC(nim_vchat_set_auto_publish)(auto_pub, auto_sub);
}
bool VChat::IsAutoPublishVideo()
{
	return NIM_SDK_GET_FUNC(nim_vchat_is_auto_publish_video)();
}
bool VChat::IsAutoSubscribeVideo()
{
	return NIM_SDK_GET_FUNC(nim_vchat_is_auto_subscribe_video)();
}
bool VChat::IsAutoSubscribeAudio()
{
	return NIM_SDK_GET_FUNC(nim_vchat_is_auto_subscribe_audio)();
}
bool VChat::SetLocalVideoSimulcastMode(NIMVChatPublishVideoStreamMode video_stream_mode)
{
	return NIM_SDK_GET_FUNC(nrtc_chat_set_local_video_simulcast_mode)(video_stream_mode);
}
void VChat::PublishVideo(OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_publish_video)( "", OnOptCallback, cb_pointer);
}
void VChat::UnpublishVideo(OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_unpublish_video)("", OnOptCallback, cb_pointer);
}
void VChat::SubscribeVideo(const std::string& uid, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_subscribe_video)(uid.c_str(), "", OnOptCallback, cb_pointer);
}
void VChat::UnsubscribeVideo(const std::string& uid, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_unsubscribe_video)(uid.c_str(), "", OnOptCallback, cb_pointer);
}

void VChat::SubscribeAudio(bool sub, OptCallback cb)
{
	OptCallback* cb_pointer = new OptCallback(cb);
	NIM_SDK_GET_FUNC(nim_vchat_subscribe_audio)(sub, "", OnOptCallback, cb_pointer);
}
static VChat::RemoteAudioDataCallback g_remote_audio_data_cb_ = nullptr;
void VChat::SetRemoteAudioDataCb(RemoteAudioDataCallback cb)
{
    g_remote_audio_data_cb_ = cb;
}
static void CallbackRemoteAudioData(unsigned __int64 time, const char *data, unsigned int size, int channels, int rate, const char *json_extension, const void *user_data)
{
    if (user_data)
    {
        VChat::RemoteAudioDataCallback* cb = (VChat::RemoteAudioDataCallback*)user_data;
        if (*cb)
        {
            nim_cpp_wrapper_util::Json::Reader reader;
            nim_cpp_wrapper_util::Json::Value values;
            if (reader.parse(PCharToString(json_extension), values) && values.isObject())
            {
                std::string uid = values[kNIMVChatAccount].asString();
                (*cb)(uid, time, data, size, channels, rate);
            }
        }
    }
}
void VChat::AddRemoteAudioDataCb(const std::string& uid, OptCallback cb)
{
    OptCallback* cb_pointer = new OptCallback(cb);
    NIM_SDK_GET_FUNC(nim_vchat_set_remote_audio_data_cb)(uid.c_str(), CallbackRemoteAudioData, &g_remote_audio_data_cb_, "", OnOptCallback, cb_pointer);
}
void VChat::RemoveRemoteAudioDataCb(const std::string& uid, OptCallback cb)
{
    OptCallback* cb_pointer = new OptCallback(cb);
    NIM_SDK_GET_FUNC(nim_vchat_set_remote_audio_data_cb)(uid.c_str(), nullptr, nullptr, "", OnOptCallback, cb_pointer);
}

}  // namespace nim

#pragma once

namespace nim_comp
{
/** @class RtsCallback
* @brief 音视频事件回调类
* @copyright (c) 2016, NetEase Inc. All rights reserved
* @date 2016/09/13
*/
class VChatCallback
{
public:
	/**
	* 枚举设备回调结果
	* @param[in] ret 结果代码，true表示成功
	* @param[in] type 设备类型NIMDeviceType，其中kNIMDeviceTypeAudioOut和kNIMDeviceTypeAudioOutChat等效
	* @param[in] json Json string 设备列表，可能为空"", 例：json_extension = "[{"name":"Webcam","path":"\\\\?\\usb......"},{"name":"Webcam2","path":"\\\\?\\usb......"}]
	* @return void 无返回值
	*/
	static void EnumDevCb(bool ret, nim::NIMDeviceType type, const char* json, const void*);

	/**
	* 设备状态改变的回调
	* @param[in] type 设备类型NIMDeviceType，其中kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo有效
	* @param[in] status 为NIMDeviceStatus的多状态
	* @param[in] path 当kNIMDeviceStatusReset状态时需要关注此参数，kNIMDeviceStatusReset时有可能选用了非用户选定的设备，这里返回的是重新启动的设备地址
	* @param[in] json 无效的扩展字段
	* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void DeviceStatusCb(nim::NIMDeviceType type, UINT status, const char* path, const char* json, const void *user_data);

	/**
	* 启动设备的回调
	* @param[in] type 设备类型NIMDeviceType
	* @param[in] ret 启动结果，true表示成功
	* @param[in] json 无效的扩展字段
	* @return void 无返回值
	*/
	static void StartDeviceCb(nim::NIMDeviceType type, bool ret, const char* json, const void*);

	/**
	* 采集到视频数据的回调
	* @param[in] time 时间毫秒级
	* @param[in] data 视频数据，默认为ARGB格式
	* @param[in] size data的数据长度
	* @param[in] width  画面宽度
	* @param[in] height  画面高度
	* @param[in] json_extension Json string kNIMVideoSubType（缺省为kNIMVideoSubTypeARGB），收到对方视频数据返回kNIMDeviceDataUid和kNIMDeviceDataAccount，如果是辅助摄像头数据则返回kNIMDeviceId
	* @return void 无返回值
	*/
	static void VideoCaptureData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char* json, const void *);

	/**
	* 接收到视频数据的回调
	* @param[in] time 时间毫秒级
	* @param[in] data 视频数据，默认为ARGB格式
	* @param[in] size data的数据长度
	* @param[in] width  画面宽度
	* @param[in] height  画面高度
	* @param[in] json_extension Json string kNIMVideoSubType（缺省为kNIMVideoSubTypeARGB），收到对方视频数据返回kNIMDeviceDataUid和kNIMDeviceDataAccount，如果是辅助摄像头数据则返回kNIMDeviceId
	* @return void 无返回值
	*/
	static void VideoRecData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char* json, const void *);

	/**
	* 音视频通话中状态返回的回调
	* @param[in] type NIMVideoChatSessionType
	* @param[in] channel_id 通话的通道id
	* @param[in] code 结果类型或错误类型
	* @param[in] json_extension Json string 扩展
	* @return void 无返回值
	*/
	static void VChatCb(nim::NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json, const void*);
};

}

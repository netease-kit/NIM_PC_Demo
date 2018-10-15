/** @file nim_device_def.h
* @brief 音视频设备 接口相关的常量函数等定义头文件
* @copyright (c) 2015-2017, NetEase Inc. All rights reserved
* @author gq
* @date 2015/4/24
*/

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_DEVICE_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_DEVICE_DEF_H_

#include "../util/stdbool.h"
#include "../util/nim_base_types.h"
#include "../util/nim_build_config.h"
#ifdef __cplusplus
extern"C"
{
#endif
	/** @enum NIMDeviceType 设备类型 */
	enum NIMDeviceType
	{
		kNIMDeviceTypeAudioIn				= 0,	/**< 麦克风设备 */
		kNIMDeviceTypeAudioOut				= 1,	/**< 听筒设备用于播放本地采集音频数据，麦克风试音 */
		kNIMDeviceTypeAudioOutChat			= 2,	/**< 听筒设备用于通话音频数据（nim_vchat_start_device和nim_vchat_end_device中使用） */
		kNIMDeviceTypeVideo					= 3,	/**< 摄像头 */
		kNIMDeviceTypeSoundcardCapturer		= 4,	/**< 声卡声音采集，得到的数据混音到发送的通话声音中(此模式使用条件苛刻不建议使用) */
		kNIMDeviceTypeAudioHook				= 5,	/**< 伴音，启动第三方播放器并获取音频数据（只允许存在一个进程钩子）,只混音到发送的通话声音中 */
	};

	/** @enum NIMVideoSubType 视频格式类型 */
	enum NIMVideoSubType
	{
		kNIMVideoSubTypeARGB	= 0,	/**< 32位位图格式 存储 (B,G,R,A)... */
		kNIMVideoSubTypeRGB		= 1,	/**< 24位位图格式 存储 (B,G,R)... */
		kNIMVideoSubTypeI420	= 2,	/**< YUV格式，存储 yyyyyyyy...uu...vv... */
	};

	/** @enum NIMDeviceStatus 设备状态类型 */
	enum NIMDeviceStatus
	{
		kNIMDeviceStatusNoChange	= 0x0,	/**< 设备没有变化 */
		kNIMDeviceStatusChange		= 0x1,	/**< 设备有变化 */
		kNIMDeviceStatusWorkRemove	= 0x2,	/**< 工作设备被移除 */
		kNIMDeviceStatusReset		= 0x4,	/**< 设备重新启动 */
		kNIMDeviceStatusStart		= 0x8,	/**< 设备开始工作 */
		kNIMDeviceStatusEnd			= 0x10,	/**< 设备停止工作 */
	};



	/** @enum NIMAudioDataCbType 音频数据监听类型 */
	enum NIMAudioDataCbType
	{
		kNIMAudioDataCbTypeHook			= 1,	/**< 实时返回伴音数据，伴音数据保留原始的格式，并伴音同时混音到通话数据中 */
		kNIMAudioDataCbTypeHookAndMic	= 2,	/**< 定时返回伴音和麦克风、声卡的混音数据，允许重采样（json中带kNIMDeviceSampleRate和kNIMVolumeWork），返回单声道数据，并伴音同时混音到通话数据中 */
	};

	/** @name json extension params for vchat device key
	* @{
	*/
	static const char *kNIMDeviceName			= "name"; 			/**< 设备名称 string */
	static const char *kNIMDevicePath			= "path"; 			/**< 设备路径 string */
	static const char *kNIMDeviceSampleRate		= "sample_rate"; 	/**< 采样频率int32 */
	static const char *kNIMDeviceSampleBit		= "sample_bit"; 	/**< 采样位深int32 */
	static const char *kNIMDeviceDataUid		= "uid"; 			/**< 用户id int64 */
	static const char *kNIMDeviceDataAccount	= "account";		/**< 用户账号 string */
	static const char *kNIMDeviceWidth			= "width"; 			/**< int32 画面宽 */
	static const char *kNIMDeviceHeight			= "height"; 		/**< int32 画面高 */
	static const char *kNIMVideoSubType			= "subtype"; 		/**< int32 视频数据类型，NIMVideoSubType */
	static const char *kNIMDeviceId				= "id"; 			/**< string 标识ID */
	static const char *kNIMVolumeWork			= "volume_work"; 	/**< int32 大于0标识数据将使用音量参数进行换算(暂时只针对伴音数据) */
	/** @}*/ //json extension params for vchat device key


	/** @typedef void (*nim_vchat_enum_device_devpath_sync_cb_func)(bool ret, NIMDeviceType type, const char *json_extension, const void *user_data)
	* NIM Device 枚举设备返回回调同步接口
	* @param[out] ret 结果代码，true表示成功
	* @param[out] type 设备类型NIMDeviceType，其中kNIMDeviceTypeAudioOut和kNIMDeviceTypeAudioOutChat等效
	* @param[out] json_extension Json string 设备列表，可能为空"", 例：json_extension = "[{"name":"Webcam","path":"\\\\?\\usb......"},{"name":"Webcam2","path":"\\\\?\\usb......"}]
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_enum_device_devpath_sync_cb_func)(bool ret, enum NIMDeviceType type, const char *json_extension, const void *user_data);

	/** @typedef void (*nim_vchat_device_status_cb_func)(NIMDeviceType type, unsigned int status, const char *device_path, const char *json_extension, const void *user_data)
	* NIM Device 设备状态监听返回接口
	* @param[out] type 设备类型NIMDeviceType，其中kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo、kNIMDeviceTypeAudioHook有效
	* @param[out] status 为NIMDeviceStatus的多状态
	* @param[out] device_path 当kNIMDeviceStatusReset状态时需要关注此参数，kNIMDeviceStatusReset时有可能选用了非用户选定的设备，这里返回的是重新启动的设备地址
	* @param[out] json_extension 无效的扩展字段
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_device_status_cb_func)(enum NIMDeviceType type, uint32_t status, const char *device_path, const char *json_extension, const void *user_data);


	/** @typedef void(*nim_vchat_audio_data_cb_func_ex)(uint64_t time, const char *data, uint32_t size, int channels, int rate, int volume, const char *json_extension, const void *user_data)
	* NIM Device 音频数据监听接口
	* @param[out] time 时间毫秒级，暂时无效
	* @param[out] data 音频数据pcm格式
	* @param[out] size data的数据长度
	* @param[out] channels 通道数
	* @param[out] rate 采样频
	* @param[out] volume 音量值0-100
	* @param[out] json_extension 扩展
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_audio_data_cb_func_ex)(uint64_t time, const char *data, uint32_t size, int channels, int rate, int volume, const char *json_extension, const void *user_data);

	/** @typedef void (*nim_vchat_start_device_cb_func)(NIMDeviceType type, bool ret, const char *json_extension, const void *user_data)
	* NIM Device 启动设备异步返回接口
	* @param[out] type 设备类型NIMDeviceType
	* @param[out] ret 启动结果，true表示成功
	* @param[out] json_extension 无效的扩展字段
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_start_device_cb_func)(enum NIMDeviceType type, bool ret, const char *json_extension, const void *user_data);

	/** @typedef void (*nim_vchat_audio_data_cb_func)(uint64_t time, const char *data, unsigned int size, const char *json_extension, const void *user_data)
	* NIM Device 音频数据监听接口
	* @param[out] time 时间毫秒级
	* @param[out] data 音频数据pcm格式
	* @param[out] size data的数据长度
	* @param[out] json_extension Json string 返回kNIMDeviceSampleRate
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_audio_data_cb_func)(uint64_t time, const char *data, uint32_t size, const char *json_extension, const void *user_data);

	/** @typedef void (*nim_vchat_video_data_cb_func)(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension, const void *user_data)
	* NIM Device 视频数据监听接口
	* @param[out] time 时间毫秒级
	* @param[out] data 视频数据，默认为ARGB格式
	* @param[out] size data的数据长度
	* @param[out] width  画面宽度
	* @param[out] height  画面高度
	* @param[out] json_extension Json string kNIMVideoSubType（缺省为kNIMVideoSubTypeARGB），收到对方视频数据返回kNIMDeviceDataUid和kNIMDeviceDataAccount，如果是辅助摄像头数据则返回kNIMDeviceId
	* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	typedef void(*nim_vchat_video_data_cb_func)(uint64_t time, const char *data, uint32_t size, uint32_t width, uint32_t height, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_DEVICE_DEF_H_

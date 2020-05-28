/** @file nim_cpp_vchat.h
  * @brief 音视频与设备
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/4/30
  */

#ifndef _NIM_SDK_CPP_VCHAT_H_
#define _NIM_SDK_CPP_VCHAT_H_

#include <string>
#include <functional>
#include "public_define/nim_util_include.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @brief 网络探测回调信息 */
struct NIM_SDK_CPPWRAPPER_DLL_API NetDetectCbInfo
{
	int32_t res_code_;					/**< 返回的错误码 */
	int32_t loss_;
	int32_t rtt_max_;
	int32_t rtt_min_;
	int32_t rtt_avg_;
	int32_t rtt_mdev_;
	std::string expand_info_;

	NetDetectCbInfo()
	{
		res_code_ = 0;
		loss_ = 0;
		rtt_max_ = 0;
		rtt_min_ = 0;
		rtt_avg_ = 0;
		rtt_mdev_ = 0;
	}
};
class NIM_SDK_CPPWRAPPER_DLL_API VChat
{

public:
	typedef std::function<void(int, NetDetectCbInfo)> NetDetectCallback;	/**< 网络探测回调模板 */
	typedef std::function<void(bool ret, int code, const std::string& file, int64_t time)>  Mp4OptCallback;		/**< MP4录制事件通知回调模板 */
	typedef Mp4OptCallback  AudioRecordCallback; /**< 音频录制事件通知回调模板 */
	typedef std::function<void(bool ret, int code, const std::string& json_extension)> OptCallback;				/**< 操作回调模板 */
	typedef std::function<void(int code, int64_t channel_id, const std::string& json_extension)> Opt2Callback;	/**< 操作回调模板 */
	typedef std::function<void(int id, bool ret, int code, const std::string& json_extension)> EffectOptCallback;				/**< 操作回调模板 */
    typedef std::function<void(const std::string& accid, uint64_t time, const char *data, uint32_t size, int32_t channels, int32_t rate)> RemoteAudioDataCallback;    /**< 远端音频数据回调 */

	/** @fn static bool Init(const std::string& json_info)
	* NIM VCHAT初始化，需要在SDK的nim_client_init成功之后
	* @param[in] server_setting_path 服务器配置文件路径
	* @return bool 初始化结果，如果是false则以下所有接口调用无效
	*/
	static bool Init(const std::string& server_setting_path);

	/** @fn void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
	* 设置SDK音视频的网络代理，暂时只支持socks5代理，全局代理接口也能设置音视频的代理，两接口没有优先级区别。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。
	* @param[in] type 代理类型，见NIMProxyType定义,其中音视频和白板暂时只支持kNIMProxySocks5代理
	* @param[in] host 代理地址
	* @param[in] port 代理端口
	* @param[in] user 代理用户名
	* @param[in] password 代理密码
	* @return void 无返回值
	*/
	static void SetProxy(NIMProxyType type, const std::string& host, int port, const std::string& user, const std::string& password);

	/** @fn static void Cleanup()
	* NIM VCHAT清理，需要在SDK的nim_client_cleanup之前
	* @return void 无返回值
	*/
	static void Cleanup();

	/** @fn uint64_t NetDetect(NetDetectCallback cb)
	* 音视频网络探测
	* @param[in] cb 操作结果的回调函数
	* @return uint64_t 探测任务id
	* @note 错误码	200:成功
	*				0:流程错误
	*				400:非法请求格式
	*				417:请求数据不对
	*				606:ip为内网ip
	*				607:频率超限
	*				20001:探测类型错误
	*				20002:ip错误
	*				20003:sock错误
	*/
	static uint64_t NetDetect(NetDetectCallback cb);

	/** @fn uint64_t NetDetectEx(int32_t ms_limit, nim::NIMNetDetectType type, nim::NIMNetDetectVideoQuality quality_type, NetDetectCallback cb)
	* 音视频网络探测扩展
	* @param[in] ms_limit 探测的时间限制，大于零有效
	* @param[in] type 探测类型
	* @param[in] quality_type 视频探测类型
	* @param[in] cb 操作结果的回调函数
	* @return uint64_t 探测任务id
	* @note 错误码	200:成功
	*				0:流程错误
	*				400:非法请求格式
	*				417:请求数据不对
	*				606:ip为内网ip
	*				607:频率超限
	*				20001:探测类型错误
	*				20002:ip错误
	*				20003:sock错误
	*/
	static uint64_t NetDetectEx(int32_t ms_limit, nim::NIMNetDetectType type, nim::NIMNetDetectVideoQuality quality_type, NetDetectCallback cb);

	/** @fn void VChatRate(int rating, const std::string& description, OptCallback cb)
	  * 打分反馈
	  * @param[in] rating 评分，分值为1-5
	  * @param[in] description 问题描述
	  * @param[in] cb 反馈结果回调
	  * @return void 无返回值
	  */
	static void VChatRate(int rating, const std::string& description, OptCallback cb);

	/** @fn static void EnumDeviceDevpath(nim::NIMDeviceType type, nim_vchat_enum_device_devpath_sync_cb_func cb)
	* NIM VCHAT DEVICE 遍历设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void EnumDeviceDevpath(nim::NIMDeviceType type, nim_vchat_enum_device_devpath_sync_cb_func cb);

	/** @fn static void StartDevice(nim::NIMDeviceType type, const std::string& device_path, unsigned fps, nim_vchat_start_device_cb_func cb)
	* NIM VCHAT DEVICE 启动设备，同一NIMDeviceType下设备将不重复启动，不同的设备会先关闭前一个设备开启新设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @param[in] device_path 可选参数，通过EnumDeviceDevpath遍历得到的设备路径。空则底层取默认设备
	* @param[in] fps 摄像头为采样频率,其他NIMDeviceType无效（麦克风采样频率由底层控制，播放器采样频率也由底层控制）
	* @param[in] width 摄像头采集宽度期望值，取0则底层选默认值
	* @param[in] height 摄像头采集高度期望值，取0则底层选默认值
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void StartDevice(nim::NIMDeviceType type, const std::string& device_path, unsigned fps, int width, int height, nim_vchat_start_device_cb_func cb);

	/** @fn static void EndDevice(nim::NIMDeviceType type)
	* NIM VCHAT DEVICE 结束设备
	* @param[in] type NIMDeviceType 见nim_device_def.h
	* @return void 无返回值
	*/
	static void EndDevice(nim::NIMDeviceType type);

	/** @fn static void AddDeviceStatusCb(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb)
	* NIM VCHAT DEVICE 添加设备监听（摄像头和麦克风） 注意监听设备后底层会定时检查设备情况，在不需要监听后请移除
	* @param[in] type NIMDeviceType（kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo有效） 见nim_device_def.h
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void AddDeviceStatusCb(nim::NIMDeviceType type, nim_vchat_device_status_cb_func cb);
	
	/** @fn static void RemoveDeviceStatusCb(nim::NIMDeviceType type)
	* NIM VCHAT DEVICE 移除设备监听（摄像头和麦克风）
	* @param[in] type NIMDeviceType（kNIMDeviceTypeAudioIn和kNIMDeviceTypeVideo有效） 见nim_device_def.h
	* @return void 无返回值
	*/
	static void RemoveDeviceStatusCb(nim::NIMDeviceType type);

	/** @fn static void SetAudioDataCb(bool capture, nim_vchat_audio_data_cb_func cb)
	* NIM VCHAT DEVICE 监听音频数据（可以不监听，通过启动设备kNIMDeviceTypeAudioOut和kNIMDeviceTypeAudioOutChat由底层播放）
	* @param[in] capture true 标识监听麦克风采集数据，false 标识监听通话中对方音频数据
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void SetAudioDataCb(bool capture, nim_vchat_audio_data_cb_func cb);


	/** @fn static void SetAudioDataCbEx(int type, std::string json, nim_vchat_audio_data_cb_func_ex cb)
	* NIM VCHAT DEVICE 监听音频数据扩展接口
	* @param[in] type 指定NIMAudioDataCbType，监听伴音数据
	* @param[in] json_extension 参考NIMAudioDataCbType的说明
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void SetAudioDataCbEx(int type, std::string json_extension, nim_vchat_audio_data_cb_func_ex cb);

	/** @fn static void SetVideoDataCb(bool capture, nim_vchat_video_data_cb_func cb)
	* NIM VCHAT DEVICE 监听视频数据
	* @param[in] capture true 标识监听采集数据，false 标识监听通话中对方视频数据
	* @param[in] cb 结果回调见nim_device_def.h
	* @return void 无返回值
	*/
	static void SetVideoDataCb(bool capture, nim_vchat_video_data_cb_func cb);

	/** @fn static void SetAudioVolumn(unsigned char volumn, bool capture)
	* NIM VCHAT DEVICE 设置音量 默认255,且音量均由软件换算得出,设置麦克风音量自动调节后麦克风音量参数无效
	* @param[in] volumn 结果回调见nim_device_def.h
	* @param[in] capture true 标识设置麦克风音量，false 标识设置播放音量
	* @return void 无返回值
	*/
	static void SetAudioVolumn(unsigned char volumn, bool capture);

	/** @fn static unsigned char GetAudioVolumn(bool capture)
	* NIM VCHAT DEVICE 获取nim_vchat_set_audio_volumn中设置的音量
	* @param[in] capture true 标识获取麦克风音量，false 标识获取播放音量
	* @return unsigned char 音量值
	*/
	static unsigned char GetAudioVolumn(bool capture);

	/** @fn static void SetAudioInputAutoVolumn(bool auto_volumn)
	* NIM VCHAT DEVICE 设置麦克风音量自动调节
	* @param[in] auto_volumn true 标识麦克风音量自动调节，false 标识麦克风音量不调节，这时nim_vchat_set_audio_volumn中麦克风音量参数起效
	* @return void 无返回值
	*/
	static void SetAudioInputAutoVolumn(bool auto_volumn);

	/** @fn static bool GetAudioInputAutoVolumn()
	* NIM VCHAT DEVICE 获取是否自动调节麦克风音量
	* @return bool true 标识麦克风音量自动调节，false 标识麦克风音量不调节，这时nim_vchat_set_audio_volumn中麦克风音量参数起效
	*/
	static bool GetAudioInputAutoVolumn();

	/** @fn static void SetAudioProcess(bool aec, bool ns, bool vad)
	* NIM VCHAT DEVICE 设置底层针对麦克风采集数据处理开关接口，默认全开（此接口是全局接口，在sdk初始化后设置一直有效）
	* @param[in] aec true 标识打开回音消除功能，false 标识关闭
	* @param[in] ns true 标识打开降噪功能，false 标识关闭
	* @param[in] vad true 标识打开人言检测功能，false 标识关闭
	* @return void 无返回值
	*/
	static void SetAudioProcess(bool aec, bool ns, bool vad);

	/** @fn void PreloadEffect(int id, const std::string& path, EffectOptCallback cb)
	  * NIM VCHAT DEVICE 预加载音效文件（空路径时为移除），音效个数限制5个，时长限制10秒；暂时只支持wav和mp3格式
	  * @param[in] id 音效id，重复时覆盖
	  * @param[in] path 音效文件的本地路径，如果填空为移除音效
	  * @param[in] cb 结果回调
	  * @return void 无返回值
	  */
	static void PreloadEffect(int id, const std::string& path, EffectOptCallback cb);

	/** @fn void PlayEffect(int id, int loop_count, EffectOptCallback cb)
	  * NIM VCHAT DEVICE 播放音效，由kNIMDeviceTypeAudioOutChat播放
	  * @param[in] id 音效id
	  * @param[in] loop_count 播放次数，0无效，小于零无限循环
	  * @param[in] cb 结果回调；异步回调操作结果，成功则在播放结束后回调；当音效未加载或者前一次同一个id的音效未播放完成，则失败
	  * @return void 无返回值
	  */
	static void PlayEffect(int id, int loop_count, EffectOptCallback cb);

	/** @fn void StopEffect(int id, EffectOptCallback cb)
	  * NIM VCHAT DEVICE 结束播放音效
	  * @param[in] id 音效id
	  * @param[in] cb 结果回调
	  * @return void 无返回值
	  */
	static void StopEffect(int id, EffectOptCallback cb);

	/** @fn static void SetCbFunc(nim_vchat_cb_func cb)
	* NIM VCHAT 设置通话回调或者服务器通知
	* @param[in] cb 结果回调见nim_vchat_def.h
	* @return void 无返回值
	* @note 错误码	200:成功
	*				9103:已经在其他端接听/拒绝过这通电话
	*				11001:通话不可送达，对方离线状态
	*/
	static void SetCbFunc(nim_vchat_cb_func cb);

	/** @fn static bool Start(NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& json_info)
	* NIM VCHAT 启动通话，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] mode NIMVideoChatMode 启动音视频通话类型 见nim_vchat_def.h
	* @param[in] apns_text 推送文本
	* @param[in] custom_info 自定义信息
	* @param[in] json_info Json string 扩展，kNIMVChatUids成员id列表,kNIMVChatCustomVideo自主视频数据和kNIMVChatCustomAudio自主音频 如{"uids":["uid_temp"],"custom_video":1, "custom_audio":1}
	* @return bool true 调用成功，false 调用失败可能有正在进行的通话
	*/
	static bool Start(NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& json_info);

	/** @fn static bool SetTalkingMode(NIMVideoChatMode mode, const std::string& json_extension)
	* NIM VCHAT 设置通话模式，在更改通话模式后，通知底层
	* @param[in] mode NIMVideoChatMode 音视频通话类型 见nim_vchat_def.h
	* @param[in] json_extension 无效的扩展字段
	* @return bool true 调用成功，false 调用失败
	*/
	static bool SetTalkingMode(NIMVideoChatMode mode, const std::string& json_extension);

	/** @fn static bool CalleeAck(uint64_t channel_id, bool accept, const std::string& json_extension)
	* NIM VCHAT 回应音视频通话邀请，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] channel_id 音视频通话通道id
	* @param[in] accept true 接受，false 拒绝
	* @param[in] json_extension Json string 扩展，kNIMVChatCustomVideo自主视频数据和kNIMVChatCustomAudio自主音频 如{"custom_video":1, "custom_audio":1}
	* @return bool true 调用成功，false 调用失败（可能channel_id无匹配，如要接起另一路通话前先结束当前通话）
	*/
	static bool CalleeAck(uint64_t channel_id, bool accept, const std::string& json_extension);

	/** @fn static bool Control(uint64_t channel_id, NIMVChatControlType type)
	* NIM VCHAT 音视频通话控制，异步回调nim_vchat_cb_func 见nim_vchat_def.h
	* @param[in] channel_id 音视频通话通道id
	* @param[in] type NIMVChatControlType 见nim_vchat_def.h
	* @return bool true 调用成功，false 调用失败
	*/
	static bool Control(uint64_t channel_id, NIMVChatControlType type);

	/** @fn static void StartRecord(const std::string& path, Mp4OptCallback cb)
	* NIM 开始录制MP4文件，一个成员一次只允许录制一个MP4文件，在通话开始的时候才有实际数据
	* @param[in] path 文件录制路径
	* @param[in] uid 录制的成员，如果是自己填空
	* @param[in] cb 结果回调
	* @return void 无返回值
	* @note 错误码	200:MP4文件创建
	*				400:MP4文件已经存在
	*				403:MP4文件创建失败
	*				404:通话不存在
	*/
	static void StartRecord(const std::string& path, const std::string& uid, Mp4OptCallback cb);

	/** @fn static void StopRecord(Mp4OptCallback cb)
	* NIM 停止录制MP4文件
	* @param[in] uid 录制的成员，如果是自己填空
	* @param[in] cb 结果回调
	* @return void 无返回值
	* @note 错误码	0:MP4结束
	*				404:通话不存在
	*/
	static void StopRecord(const std::string& uid, Mp4OptCallback cb);

	/** @fn static void StartAudioRecord(const std::string& path, AudioRecordCallback cb)
	* NIM 开始录制音频文件，一次只允许录制一个音频文件，在通话开始的时候才有实际数据
	* @param[in] path 文件录制路径
	* @param[in] cb 结果回调
	* @return void 无返回值
	* @note 错误码	200:文件创建
	*				400:文件已经存在
	*				403:文件创建失败
	*				404:通话不存在
	*/
	static void StartAudioRecord(const std::string& path, AudioRecordCallback cb);

	/** @fn static void StopAudioRecord(AudioRecordCallback cb)
	* NIM 停止录制音频文件
	* @param[in] cb 结果回调
	* @return void 无返回值
	* @note 错误码	0:结束
	*				404:通话不存在
	*/
	static void StopAudioRecord(AudioRecordCallback cb);

	/** @fn static void End(const std::string& json_extension)
	* NIM VCHAT 结束通话(需要主动在通话结束后调用，用于底层挂断和清理数据)
	* @param[in] json_extension 扩展字段
	* @return void 无返回值
	*/
	static void End(const std::string& json_extension);

	/** @fn static void Relogin(const std::string& session_id, OptCallback cb)
	* NIM VCHAT 重连
	* @param[in] session_id 会话的本地session id
	* @param[in] cb 操作结果
	* @return void 无返回值
	*/
	static void Relogin(const std::string& session_id, OptCallback cb);

	/** @fn static void SetVideoQuality(int video_quality)
	* NIM 通话中修改分辨率
	* @param[in] video_quality 分辨率模式
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void SetVideoQuality(int video_quality);

	/** @fn void SetVideoBitrate(int video_bitrate)
	* NIM 通话中修改视频码率，有效区间[100kb,5000kb]，如果设置video_bitrate为0则取默认码率
	* @param[in] video_bitrate 视频码率值
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void SetVideoBitrate(int video_bitrate);

	/** @fn void SetFrameRate(NIMVChatVideoFrameRate frame_rate)
	* NIM 实时设置视频发送帧率上限
	* @param[in] frame_rate  帧率类型 见NIMVChatVideoFrameRate定义
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void SetFrameRate(NIMVChatVideoFrameRate frame_rate);

	/** @fn static void SetCustomData(bool custom_audio, bool custom_video)
	* NIM 通话中修改自定义音视频数据模式
	* @param[in] custom_audio true表示使用自定义的音频数据，false表示不使用
	* @param[in] custom_video true表示使用自定义的视频数据，false表示不使用
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void SetCustomData(bool custom_audio, bool custom_video);

	/** @fn static bool CustomAudioData(uint64_t time, const char *data, unsigned int size, const char *json_extension)
	* NIM VCHAT 自定义音频数据接口, 采样位深只支持16或32,[Linux sdk只支持16] kNIMDeviceSampleRate支持8000，16000，32000，44100
	* @param[in] time 时间毫秒级，暂时无效
	* @param[in] data 音频数据pcm格式
	* @param[in] size data的数据长度
	* @param[in] json_extension 扩展Json string kNIMDeviceSampleRate采样频和kNIMDeviceSampleBit采样位深 默认如{"sample_rate":16000, "sample_bit":16}
	* @return bool true 调用成功，false 调用失败
	*/
	static bool CustomAudioData(uint64_t time, const char *data, unsigned int size, const char *json_extension);

	/** @fn static bool CustomVideoData(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension)
	* NIM VCHAT 自定义视频数据接口
	* @param[in] time 时间毫秒级，暂时无效
	* @param[in] data 视频数据， 默认为yuv420格式
	* @param[in] size data的数据长度
	* @param[in] width  画面宽度，必须是偶数
	* @param[in] height  画面高度，必须是偶数
	* @param[in] json_extension  扩展Json string，kNIMVideoSubType视频数据格式（缺省为kNIMVideoSubTypeI420）
	* @return bool true 调用成功，false 调用失败
	*/
	static bool CustomVideoData(uint64_t time, const char *data, unsigned int size, unsigned int width, unsigned int height, const char *json_extension);

	/** @fn bool AccompanyingSound(unsigned char id, unsigned __int64 time, const char *data, unsigned int size, unsigned int rate, unsigned int channels, const char *json_extension)
	* NIM VCHAT 自定义音频伴音数据接口，不需要打开自定义数据开关, 采样时间必须为10ms的整数倍, 采样位深只支持16
	* @param[in] id 伴音数据id
	* @param[in] time 时间毫秒级
	* @param[in] data 音频数据pcm格式
	* @param[in] size data的数据长度
	* @param[in] rate 音频采样频
	* @param[in] channels 声道数
	* @param[in] json_extension 扩展
	* @return bool true 调用成功，false 调用失败
	*/
	static bool AccompanyingSound(unsigned char id, unsigned __int64 time, const char *data, unsigned int size, unsigned int rate, unsigned int channels, const char *json_extension);

	/** @fn static void SetViewerMode(bool viewer)
	* NIM VCHAT 设置观众模式（多人模式下），全局有效（重新发起时也生效），观众模式能减少运行开销
	* @param[in] viewer 是否观众模式
	* @return void 无返回值
	*/
	static void SetViewerMode(bool viewer);

	/** @fn static bool GetViewerMode()
	* NIM VCHAT 获取当前是否是观众模式
	* @return bool true 观众模式，false 非观众模式
	*/
	static bool GetViewerMode();

    /** @fn static NIMVChatConnectStatus GetConnectStatus()
    * NIM VCHAT 获取当前通话状态
    * @return NIMVChatConnectStatus 通话连接状态枚举
    */
    static NIMVChatConnectStatus GetConnectStatus();

	/** @fn static void SetAudioMuted(bool muted)
	* NIM VCHAT 设置音频发送静音，全局有效（重新发起时也生效）
	* @param[in] muted true 静音，false 不静音
	* @return void 无返回值
	*/
	static void SetAudioMuted(bool muted);

	/** @fn static bool GetAudioMuteEnabled()
	* NIM VCHAT 获取音频发送静音状态
	* @return bool true 静音，false 不静音
	*/
	static bool GetAudioMuteEnabled();

	/** @fn static void SetAudioPlayMuted(bool muted)
	* NIM VCHAT 设置音频播放静音，全局有效（重新发起时也生效）；不影响底层解码录制等
	* @param[in] muted true 静音，false 不静音
	* @return void 无返回值
	*/
	static void SetAudioPlayMuted(bool muted);

	/** @fn static bool GetAudioMutePlayEnabled()
	* NIM VCHAT 获取音频播放静音状态
	* @return bool true 静音，false 不静音
	*/
	static bool GetAudioMutePlayEnabled();

	/** @fn void SetRotateRemoteVideo(bool rotate)
	* NIM VCHAT 设置不自动旋转对方画面，默认打开，全局有效（重新发起时也生效）
	* @param[in] rotate true 自动旋转，false 不旋转
	* @return void 无返回值
	*/
	static void SetRotateRemoteVideo(bool rotate);

	/** @fn bool IsRotateRemoteVideo()
	* NIM VCHAT 获取自动旋转对方画面设置状态
	* @return bool true 自动旋转，false 不旋转
	*/
	static bool IsRotateRemoteVideo();

	/** @fn void SetVideoFrameScaleType(NIMVChatVideoFrameScaleType type)
	* NIM VCHAT 设置发送时视频画面的长宽比例裁剪模式，裁剪的时候不改变横竖屏（重新发起时也生效）
	* @param[in] type 裁剪模式NIMVChatVideoFrameScaleType
	* @return void 无返回值
	*/
	static void SetVideoFrameScaleType(NIMVChatVideoFrameScaleType type);

	/** @fn int GetVideoFrameScaleType()
	* NIM VCHAT 获取视频画面的裁剪模式
	* @return int 返回NIMVChatVideoFrameScaleType
	*/
	static int GetVideoFrameScaleType();

	/** @fn static void SetMemberBlacklist(const std::string& uid, bool add, bool audio, const std::string& json_extension, OptCallback cb)
	* NIM VCHAT 设置单个成员的黑名单状态，当前通话有效(只能设置进入过房间的成员)
	* @param[in] uid 成员account
	* @param[in] add true表示添加到黑名单，false表示从黑名单移除
	* @param[in] audio true表示音频黑名单，false表示视频黑名单
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void SetMemberBlacklist(const std::string& uid, bool add, bool audio, const std::string& json_extension, OptCallback cb);

	/** @fn static void CreateRoom(const std::string& room_name, const std::string& custom_info, const std::string& json_extension, Opt2Callback cb)
	* NIM VCHAT 创建一个多人房间（后续需要主动调用加入接口进入房间）
	* @param[in] room_name 房间名
	* @param[in] custom_info 自定义的房间信息（加入房间的时候会返回）
	* @param[in] json_extension 房间配置信息，详见nim_vchat_def.h，eg.{"room_config":{"rtmpTasks":[{"taskId":"stream_1","streamUrl":"rtmp://test.url","layoutMode":"0","layoutPara":{"k1":"v1"},"record":true,"accid":"123"}]}}
	* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
	* @return void 无返回值
	* @note 错误码	200:成功
	*				417:提示已经创建好频道
	*/
	static void CreateRoom(const std::string& room_name, const std::string& custom_info, const std::string& json_extension, Opt2Callback cb);

	/** @fn static bool JoinRoom(NIMVideoChatMode mode, const std::string& room_name, const std::string& json_extension, Opt2Callback cb)
	* NIM VCHAT 加入一个多人房间（进入房间后成员变化等，等同点对点nim_vchat_cb_func）
	* @param[in] mode NIMVideoChatMode 音视频通话类型 见nim_vchat_def.h
	* @param[in] room_name 房间名
	* @param[in] json_extension 可选 如{"custom_video":0, "custom_audio":0, "video_quality":0, "session_id":"1231sda"}
	* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension扩展字段中包含 kNIMVChatCustomInfo,kNIMVChatSessionId
	* @return bool true 调用成功，false 调用失败可能有正在进行的通话
	* @note 错误码	200:成功
	*/
	static bool JoinRoom(NIMVideoChatMode mode, const std::string& room_name, const std::string& json_extension, Opt2Callback cb);

	/** @fn void UpdateRtmpUrl(const std::string& rtmp_url, OptCallback cb)
	* NIM 通话中修改直播推流地址（主播有效）
	* @param[in] rtmp_url 新的rtmp推流地址
	* @param[in] cb 结果回调见nim_vchat_def.h，返回的json_extension无效
	* @return void 无返回值
	* @note 错误码	0:成功
	*				11403:无效的操作
	*/
	static void UpdateRtmpUrl(const std::string& rtmp_url, OptCallback cb);

	/** @fn static void SelectVideoAdaptiveStrategy(NIMVChatVideoEncodeMode mode, const std::string& json_extension, OptCallback cb)
	* NIM VCHAT 通话中修改视频编码模式
	* @param[in] mode 选用的策略模式
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 结果回调见nim_chat_def.h，返回的json_extension无效
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void SelectVideoAdaptiveStrategy(NIMVChatVideoEncodeMode mode, const std::string& json_extension, OptCallback cb);

	/** @fn static void SetUidAsMainPicture(int uid, const std::string& json_extension, OptCallback cb)
	* NIM VCHAT 互动直播模式设置uid为房间内的主画面
	* @param[in] uid 用户uid
	* @param[in] json_extension 无效扩展字段
	* @param[in] cb 结果回调见nim_chat_def.h,返回的json_extension无效
	* @return void 无返回值
	* @note 错误码	200:成功
	*				400:非法请求格式
	*				401: 认证错误
	*				404:房间不存在
	*				405:房间下的uid不存在
	*				417:请求数据不对
	*				500: 内部错误
	*				600:服务器内部错误
	*				11403:无效的操作
	*/
	static void SetUidAsMainPicture(const std::string& uid, const std::string& json_extension, OptCallback cb);

	//订阅
	/** @fn bool SetAutoPublish(bool auto_pub, bool auto_sub)
	* 设置通话中自动订阅对端新加入成员音视频数据，及通话开启后自动发布视频数据的逻辑；音频发布由sdk控制，如果要不发音频单独使用静音接口。该接口仅限于通话前设置，通话中设置失败。
	* @param[in] auto_pub 是否自动发布视频数据，默认开启；其中音频不支持设置自动发布功能，强制发布。自动发布时，PublishVideo接口会调用失败。
	* @param[in] auto_sub 是否自动订阅音视频数据，默认开启；自动订阅时SubscribeVideo、SubscribeAudio会调用失败。
	* @return bool 是否调用成功
	*/
	static bool SetAutoPublish(bool auto_pub, bool auto_sub);

	/** @fn bool IsAutoPublishVideo()
	* 获取通话设置中是否自动发布视频数据
	* @return bool 是否自动发布
	*/
	static bool IsAutoPublishVideo();

	/** @fn bool IsAutoSubscribeVideo()
	* 获取通话设置中是否自动订阅视频数据
	* @return bool 是否自动订阅
	*/
	static bool IsAutoSubscribeVideo();

	/** @fn bool IsAutoSubscribeAudio()
	* 获取通话设置中是否自动订阅音频数据
	* @return bool 是否自动订阅
	*/
	static bool IsAutoSubscribeAudio();

	/** @fn bool SetLocalVideoSimulcastMode(NIMVChatPublishVideoStreamMode video_stream_mode)
	* 设置本地默认发布的视频流模式，默认为单流（自动发布模式下有效）
	* @param[in] video_stream_mode 目前仅支持单/双流模式。
	* @return bool 是否调用成功
	*/
	static bool SetLocalVideoSimulcastMode(NIMVChatPublishVideoStreamMode video_stream_mode);

	/** @fn void PublishVideo( OptCallback cb)
	* 通话中设置自己的视频数据发布接口。通话未发起时设置失败。
	* 回调结果只代表本地的操作结果，远端是否成功等对应的通知kNIMVChatNotifyPublishVideoRet
	* @param[in] cb 操作结果，错误码见NIMVChatPubSubErrorCode。
	* @return void 无返回值
	*/
	static void PublishVideo(OptCallback cb);

	/** @fn void UnpublishVideo( OptCallback cb)
	* 通话中设置自己的视频数据取消发布接口。该接口仅限于多人通话模式，且通话未发起时设置失败。
	* 回调结果只代表本地的操作结果，远端是否成功等对应的通知kNIMVChatNotifyUnpublishVideoRet
	* @param[in] cb 操作结果，错误码见NIMVChatPubSubErrorCode。
	* @return void 无返回值
	*/
	static void UnpublishVideo( OptCallback cb);

	/** @fn void SubscribeVideo(const std::string& uid,OptCallback cb)
	* 通话中设置订阅某一方的视频数据，针对同一成员的设置如果还未返回则重复设置失败，通话未发起时设置失败。
	* 回调结果只代表本地的操作结果，远端是否成功等对应的通知kNIMVChatNotifySubscribeVideoRet
	* @param[in] uid 用户账号
	* @param[in] cb 结果回调错误码见NIMVChatPubSubErrorCode。
	* @return void 无返回值
	*/
	static void SubscribeVideo(const std::string& uid, OptCallback cb);


	/** @fn void UnsubscribeVideo(const std::string& uid, OptCallback cb)
	* 通话中设置取消订阅某一方的视频数据，针对同一成员的设置如果还未返回则重复设置失败，通话未发起时设置失败。
	* 回调结果只代表本地的操作结果，远端是否成功等对应的通知NIMVChatNotifyUnsubscribeVideoRet
	* @param[in] uid 用户账号
	* @param[in] cb 结果回调错误码见NIMVChatPubSubErrorCode。
	* @return void 无返回值
	*/
	static void UnsubscribeVideo(const std::string& uid, OptCallback cb);

	/** @fn void SubscribeAudio(bool sub, OptCallback cb);
	* 通话中设置订阅音频数据，默认自动订阅。音频数据没有单独订阅某一成员的接口。通话未发起时设置失败。
	* 回调结果只代表本地的操作结果，远端是否成功等对应的通知kNIMVChatNotifySubscribeAudioRet及kNIMVChatNotifyUnsubscribeAudioRet
	* @param[in] sub 是否订阅音频数据
	* @param[in] cb 结果回调错误码见NIMVChatPubSubErrorCode。
	* @return void 无返回值
	*/
	static void SubscribeAudio(bool sub, OptCallback cb);


    /** @fn SetRemoteAudioDataCb(RemoteAudioDataCallback cb)
    * 全局设置远端音频数据回调接口，
    * 针对不同用户的数据回调都由该接口返回，不允许回调使用过程中修改;该数据回调为同步回调，需要保证快速处理，防止影响音频播放。
    * @param[in] cb 数据回调接口RemoteAudioDataCallback。
    * @return void 无返回值
    */
    static void SetRemoteAudioDataCb(RemoteAudioDataCallback cb);

    /** @fn void AddRemoteAudioDataCb(const std::string& uid, OptCallback cb)
    * 通话中设置用户远端数据回调
    * 该成员加入通话后设置才有效，否则cb会返回失败，数据回调由SetRemoteAudioDataCb接口设置
    * @param[in] uid 用户账号
    * @param[in] cb 接口调用结果。
    * @return void 无返回值
    */
    static void AddRemoteAudioDataCb(const std::string& uid, OptCallback cb);

    /** @fn void RemoveRemoteAudioDataCb(const std::string& uid, OptCallback cb)
    * 通话中取消用户远端数据回调
    * 该成员加入通话后设置才有效，否则cb会返回失败
    * @param[in] uid 用户账号
    * @param[in] cb 接口调用结果。
    * @return void 无返回值
    */
    static void RemoveRemoteAudioDataCb(const std::string& uid, OptCallback cb);

};
}

#endif //_NIM_SDK_CPP_VCHAT_H_
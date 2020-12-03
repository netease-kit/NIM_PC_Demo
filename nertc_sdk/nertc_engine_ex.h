/** @file nertc_engine_ex.h
* @brief NERTC SDK扩展接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_ENGINE_EX_H
#define NERTC_ENGINE_EX_H

#include "nertc_base.h"
#include "nertc_base_types.h"
#include "nertc_engine_defines.h"
#include "nertc_engine_event_handler.h"
#include "nertc_engine_media_stats_observer.h"
#include "nertc_engine.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** RtcEngine 类提供了供 App 调用的主要接口方法。

 IRtcEngineEx 是 NERTC SDK 的扩展接口类。创建一个 IRtcEngine 对象并调用这个对象的方法可以激活 NERTC SDK 的通信功能。
 */
class IRtcEngineEx : public IRtcEngine
{
public:
    virtual ~IRtcEngineEx() {}

    /** Gets the connection state of the SDK.

     @return #NERtcConnectionStateType.
     */
    virtual NERtcConnectionStateType getConnectionState() = 0;

    /** 开关本地音频发送。该方法用于允许/禁止往网络发送本地音频流。

    @note 该方法不影响录音状态，因为并没有禁用录音设备。

    @param[in] mute 静音/取消静音:
    - true: 静音本地音频
    - false: 取消静音本地音频（默认）

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int muteLocalAudioStream(bool mute) = 0;

    /** 设置音频编码属性。

    @note
    - 该方法需要在 \ref IRtcEngine::joinChannel "joinChannel" 之前设置好， \ref IRtcEngine::joinChannel "joinChannel" 之后设置不生效。
    - 音乐场景下，建议将 profile 设置为 #kNERtcAudioProfileHighQuality (4).

    @param[in]  profile 设置采样率，码率，编码模式和声道数: #NERtcAudioProfileType.
    @param[in]  scenario 设置音频应用场景: #NERtcAudioScenarioType.

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setAudioProfile(NERtcAudioProfileType profile, NERtcAudioScenarioType scenario) = 0;

    /** 订阅／取消订阅指定音频流。

    @param[in] uid 指定用户的 ID
    @param[in] subscribe
    - true: 订阅指定音频流（默认）；
    - false: 取消订阅指定音频流。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。

    */
    virtual int subscribeRemoteAudioStream(uid_t uid, bool subscribe) = 0;

    /** 设置视频配置。

	  该方法设置视频配置。每个属性对应一套视频参数，如分辨率等，会在摄像头重启后生效。 所有设置的参数均为理想情况下的最大值。当视频引擎因网络环境等原因无法达到设置的分辨率的最大值时，会取最接近最大值的那个值。

     @param[in] config 视频配置: NERtcVideoConfig
     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int setVideoConfig(const NERtcVideoConfig& config) = 0;

    /** 设置视频双流发送。

    @note 调用该方法控制是否支持发送双流，只对摄像头数据生效，自定义输入、取屏等无效。设置后，会在摄像头重启后生效。

    @param[in] enable
    - true: 双流发送（默认）
    - false: 只发送一个流

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int enableDualStreamMode(bool enable) = 0;

    /** 设置本地视图显示模式。

    该方法设置本地视图显示模式。 App 可以多次调用此方法更改显示模式。

    @param[in] scaling_mode  视频显示模式: #NERtcVideoScalingMode
    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setLocalRenderMode(NERtcVideoScalingMode scaling_mode) = 0;

    /** 设置本地视频镜像模式。

    该方法设置本地视频镜像模式。 App 可以多次调用此方法更改镜像模式。

    @param[in] mirror_mode  视频镜像模式: #NERtcVideoMirrorMode
    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setLocalVideoMirrorMode(NERtcVideoMirrorMode mirror_mode) = 0;

    /** 设置远端视图显示模式。

    该方法设置远端视图显示模式。App 可以多次调用此方法更改显示模式。

    @param[in] uid 远端用户 ID。
    @param[in] scaling_mode  视频显示模式: #NERtcVideoScalingMode
    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setRemoteRenderMode(uid_t uid, NERtcVideoScalingMode scaling_mode) = 0;


    /** 开启视频预览。

     该方法用于在进入频道前启动本地视频预览。调用该 API 前，必须:

     - 调用 \ref IRtcEngine::setupLocalVideoCanvas "setupLocalVideoCanvas" 设置预览窗口；

     @note 启用了本地视频预览后，在进入频道前，本地预览必须先关闭，需要调用 \ref IRtcEngineEx::stopVideoPreview "stopVideoPreview" 。

     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int startVideoPreview() = 0;

    /** 停止视频预览。

     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int stopVideoPreview() = 0;

    /** 开关本地视频发送。

    @note 调用该方法禁视频流时，SDK 不再发送本地视频流，但摄像头仍然处于工作状态。相比于 \ref nertc::IRtcEngine::enableLocalVideo "enableLocalVideo" (false) 用于控制本地视频流发送的方法，该方法响应速度更快。该方法不影响本地视频流获取，没有禁用摄像头。

    @param[in] mute
    - true: 不发送本地视频流
    - false: 发送本地视频流（默认）

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int muteLocalVideoStream(bool mute) = 0;

    /** 通过 JSON 配置 SDK 提供技术预览或特别定制功能。以标准化方式公开 JSON 选项。

    @param[in] parameters JSON 字符串形式的参数

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setParameters(const char* parameters) = 0;

    /** 设置录制的声音格式。该方法设置 \ref nertc::INERtcAudioFrameObserver::onAudioFrameDidRecord "onAudioFrameDidRecord" 回调的录制声音格式。

     joinChannel 前/后都允许更改设置。取消监听后，后重置为空。

     @param format 指定 *onAudioFrameDidRecord* 中返回数据的采样率和数据的通道数。允许传入 NULL，默认为 NULL。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setRecordingAudioFrameParameters(NERtcAudioFrameRequestFormat *format) = 0;

    /** 设置播放的声音格式。该方法设置 \ref nertc::INERtcAudioFrameObserver::onAudioFrameWillPlayback "onAudioFrameWillPlayback" 回调的播放声音格式。

     joinChannel 前/后都允许更改设置。取消监听后，后重置为空。

     @param format 指定 *onAudioFrameWillPlayback* 中返回数据的采样率和数据的通道数。允许传入 NULL，默认为 NULL。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setPlaybackAudioFrameParameters(NERtcAudioFrameRequestFormat *format) = 0;

    /** 注册语音观测器对象。joinChannel 前/后都允许更改设置。

     该方法用于注册语音观测器对象，即注册回调。当需要引擎给出 \ref nertc::INERtcAudioFrameObserver::onAudioFrameDidRecord "onAudioFrameDidRecord" 或 \ref nertc::INERtcAudioFrameObserver::onAudioFrameWillPlayback "onAudioFrameWillPlayback" 回调时，需要使用该方法注册回调。

     @param observer 接口对象实例。如果传入 NULL，则取消注册，同时会清理NERtcAudioFrameRequestFormat相关设置。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setAudioFrameObserver(INERtcAudioFrameObserver *observer) = 0;

    /** 开启音频dump。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int startAudioDump() = 0;
    /** 结束音频dump。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int stopAudioDump() = 0;

    /** 开始播放伴奏。

    指定本地或在线音频文件来和录音设备采集的音频流进行混音。成功调用该方法后，如果播放状态改变，本地会触发 \ref nertc::IRtcEngineEventHandlerEx::onAudioMixingStateChanged "onAudioMixingStateChanged" 回调。

    @note 请在频道内调用该方法，如果在频道外调用该方法可能会出现问题。

    @param[in] option 创建混音任务配置的选项，包括混音任务类型、混音文件全路径或URL等。支持的音频格式包括: AAC、MP3 和 WAVE。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int startAudioMixing(NERtcCreateAudioMixingOption *option) = 0;

    /** 停止播放伴奏。

    该方法停止播放伴奏。请在频道内调用该方法。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int stopAudioMixing() = 0;

    /** 暂停播放伴奏。

    该方法暂停播放伴奏。请在频道内调用该方法。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int pauseAudioMixing() = 0;

    /** 恢复播放伴奏。

    该方法恢复混音，继续播放伴奏。请在频道内调用该方法。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int resumeAudioMixing() = 0;

    /** 调节伴奏发送音量。

    该方法调节混音里伴奏的发送音量大小。请在频道内调用该方法。

    @param[in] volume 伴奏音量范围为 0~100。默认 100 为原始文件音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setAudioMixingSendVolume(uint32_t volume) = 0;

    /** 获取伴奏发送音量。

    该方法获取混音里伴奏的发送音量大小。请在频道内调用该方法。

    @param[out] volume 伴奏发送音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getAudioMixingSendVolume(uint32_t *volume) = 0;

    /** 调节伴奏播放音量。

    该方法调节混音里伴奏的播放音量大小。请在频道内调用该方法。

    @param[in] volume 伴奏音量范围为 0~100。默认 100 为原始文件音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setAudioMixingPlaybackVolume(uint32_t volume) = 0;

    /** 获取伴奏播放音量。

    该方法获取混音里伴奏的播放音量大小。请在频道内调用该方法。

    @param[out] volume 伴奏播放音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getAudioMixingPlaybackVolume(uint32_t *volume) = 0;

    /** 获取伴奏时长。

    该方法获取伴奏时长，单位为毫秒。请在频道内调用该方法。

    @param[out] duration 伴奏时长，单位为毫秒。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getAudioMixingDuration(uint64_t *duration) = 0;

    /** 获取音乐文件的播放进度。

    该方法获取当前伴奏播放进度，单位为毫秒。请在频道内调用该方法。

    @param[out] position 伴奏播放进度，单位为毫秒。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getAudioMixingCurrentPosition(uint64_t *position) = 0;

    /** 设置音乐文件的播放位置。

    该方法可以设置音频文件的播放位置，这样你可以根据实际情况播放文件，而非从头到尾播放整个文件。

    @param[in] seek_position 进度条位置，单位为毫秒。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setAudioMixingPosition(uint64_t seek_position) = 0;

    //virtual int preloadEffect(uint32_t effect_id, const char *file_path) = 0;
    //virtual int unloadEffect(uint32_t effect_id) = 0;

    /** 播放指定音效文件。

    你可以多次调用该方法，通过传入不同的音效文件的 effect_id 和 option ，同时播放多个音效文件，实现音效叠加。为获得最佳用户体验，我们建议同时播放的音效文件不要超过 3 个。成功调用该方法后，如果播放结束，本地会触发 \ref nertc::IRtcEngineEventHandlerEx::onAudioEffectFinished "onAudioEffectFinished" 回调。

    @note 请在频道内调用该方法，如果在频道外调用该方法可能会出现问题。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。
    @param[in] option 创建音效任务配置的选项，包括混音任务类型、混音文件全路径或URL等。支持的音频格式包括: AAC、MP3 和 WAVE。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int playEffect(uint32_t effect_id, NERtcCreateAudioEffectOption *option) = 0;

    /** 停止播放指定音效文件。

    请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int stopEffect(uint32_t effect_id) = 0;

    /** 停止播放所有音效文件。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int stopAllEffects() = 0;

    /** 暂停音效文件播放。

    请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int pauseEffect(uint32_t effect_id) = 0;

    /** 恢复播放指定音效文件。

    请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int resumeEffect(uint32_t effect_id) = 0;

    /** 暂停所有音效文件播放。

    请在频道内调用该方法。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int pauseAllEffects() = 0;

    /** 恢复播放所有音效文件。

    请在频道内调用该方法。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int resumeAllEffects() = 0;

    /** 调节音效发送音量。

    该方法调节音效的发送音量大小。请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。
    @param[in] volume 音效音量范围为 0~100。默认 100 为原始文件音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setEffectSendVolume(uint32_t effect_id, uint32_t volume) = 0;
    /** 获取音效发送音量。

    该方法获取音效的发送音量大小。请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。
    @param[out] volume 音效发送音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getEffectSendVolume(uint32_t effect_id, uint32_t *volume) = 0;

    /** 调节音效播放音量。

    该方法调节音效的播放音量大小。请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。
    @param[in] volume 音效音量范围为 0~100。默认 100 为原始文件音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setEffectPlaybackVolume(uint32_t effect_id, uint32_t volume) = 0;

    /** 获取音效播放音量。

    该方法获取音效的播放音量大小。请在频道内调用该方法。

    @param[in] effect_id 指定音效的 ID。每个音效均有唯一的 ID。
    @param[out] volume 音效播放音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int getEffectPlaybackVolume(uint32_t effect_id, uint32_t *volume) = 0;

    /** 开启或关闭耳返。

    请在频道内调用该方法。

    @param[in] enabled 开启或关闭。
    @param[in] volume 耳返音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int enableEarback(bool enabled, uint32_t volume) = 0;

    /** 设置耳返音量。

    请在频道内调用该方法。

    @param[in] volume 耳返音量。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setEarbackVolume(uint32_t volume) = 0;

    /** 注册统计信息观测器。

    @param[in] observer 统计信息观测器

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setStatsObserver(IRtcMediaStatsObserver *observer) = 0;

    /** 启用说话者音量提示。该方法允许 SDK 定期向 App 反馈当前谁在说话以及说话者的音量。
    
     启用该方法后，无论频道内是否有人说话，都会在 \ref nertc::IRtcEngineEventHandlerEx::onRemoteAudioVolumeIndication "onRemoteAudioVolumeIndication" 回调中按设置的间隔时间返回音量提示。

     @param enable 是否启用说话者音量提示。
     @param interval 指定音量提示的时间间隔，单位为毫秒。必须设置为 100 毫秒的整数倍值。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int enableAudioVolumeIndication(bool enable, uint64_t interval) = 0;

    /** 通过指定区域共享屏幕。共享一个屏幕或该屏幕的部分区域。用户需要在该方法中指定想要共享的屏幕区域。

     该方法仅适用于 Windows。

     @param  screen_rect 指定待共享的屏幕相对于虚拟屏的位置。
     @param  region_rect (可选) 指定待共享区域相对于整个屏幕屏幕的位置。如果设置的共享区域超出了屏幕的边界，则只共享屏幕内的内容；如果将 width 或 height 设为 0, 则共享整个屏幕。
     @param  capture_params 屏幕共享的编码参数配置。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int startScreenCaptureByScreenRect(const NERtcRectangle& screen_rect, const NERtcRectangle& region_rect, const NERtcScreenCaptureParameters& capture_params) = 0;

    /** 通过屏幕 ID 共享屏幕。共享一个屏幕或该屏幕的部分区域。用户需要在该方法中指定想要共享的屏幕 ID。
    
     该方法仅适用于 macOS。

     @param  display_id 指定待共享的屏幕 ID。开发者需要通过该参数指定你要共享的那个屏幕。
     @param  region_rect （可选）指定待共享的区域相对于整个窗口的位置。如果设置的共享区域超出了窗口的边界，则只共享窗口内的内容；如果宽或高为 0，则共享整个窗口。
     @param  capture_params 屏幕共享的编码参数配置。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int startScreenCaptureByDisplayId(unsigned int display_id, const NERtcRectangle& region_rect, const NERtcScreenCaptureParameters& capture_params) = 0;

    /** 通过窗口 ID 共享窗口。共享一个窗口或该窗口的部分区域。用户需要在该方法中指定想要共享的窗口 ID。

     @param  window_id 指定待共享的窗口 ID。
     @param  region_rect （可选）指定待共享的区域相对于整个窗口的位置。如果设置的共享区域超出了窗口的边界，则只共享窗口内的内容；如果宽或高为 0，则共享整个窗口。
     @param  capture_params 屏幕共享的编码参数配置。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int startScreenCaptureByWindowId(void *window_id, const NERtcRectangle& region_rect, const NERtcScreenCaptureParameters& capture_params) = 0;

    /** 在共享屏幕或窗口时，更新共享的区域。
    @param  region_rect （可选）指定待共享的区域相对于整个窗口的位置。如果设置的共享区域超出了窗口的边界，则只共享窗口内的内容；如果宽或高为 0，则共享整个窗口。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int updateScreenCaptureRegion(const NERtcRectangle& region_rect) = 0;

    /** 停止屏幕共享。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
    */
    virtual int stopScreenCapture() = 0;

    /** 暂停屏幕共享。

     @return
     - 0: 方法调用成功
     - 其他: 方法调用失败
    */
    virtual int pauseScreenCapture() = 0;

    /** 恢复屏幕共享。

     @return
     - 0: 方法调用成功
     - 其他: 方法调用失败
    */
    virtual int resumeScreenCapture() = 0;


    /** 开启或关闭外部视频源数据输入

    该方法启用外部视频数据输入功能。并且需要通过\ref IVideoDeviceManager::setDevice 设置kNERtcExternalVideoDeviceID为外部视频输入源ID

    @note 该方法设置内部引擎为启用状态，在 \ref IRtcEngine::leaveChannel "leaveChannel" 后仍然有效。

    @param[in] enabled 是否外部视频源数据输入:
    - true: 开启外部视频源数据输入；
    - false: 关闭外部视频源数据输入 (默认)。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int setExternalVideoSource(bool enabled) = 0;

    /** 推送外部视频源数据输入

    该方法将数据帧设置给内部引擎。

    @note 该方法设置内部引擎为启用状态，在 \ref IRtcEngine::leaveChannel "leaveChannel" 后不再有效。

    @param[in] frame 视频桢数据:

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int pushExternalVideoFrame(NERtcVideoFrame* frame) = 0;

    /** 查询 SDK 版本号。

     @param[out] build 编译号。
     @return 当前的 SDK 版本号，格式为字符串，如1.0.0.
     */
    virtual const char* getVersion(int* build) = 0;

    /** 获取错误描述。

	 @param[in] error_code #NERtcErrorCode 。来自 \ref IRtcEngineEventHandler::onError "onError" 提供的错误码。
     @return 详细错误码描述
     */
    virtual const char* getErrorDescription(int error_code) = 0;

    /** 上传SDK 信息（如log文件和Audio dump文件）。

     @return void
     */
    virtual void uploadSdkInfo() = 0;

    /** 添加房间推流任务，成功添加后当前用户可以收到该直播流的状态通知。通话中有效。
     @param[in] info 直播任务信息。 #NERtcLiveStreamTaskInfo
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int addLiveStreamTask(const NERtcLiveStreamTaskInfo& info) = 0;

    /** 更新修改房间推流任务。通话中有效。
     @param[in] info 直播任务信息。 #NERtcLiveStreamTaskInfo
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int updateLiveStreamTask(const NERtcLiveStreamTaskInfo& info) = 0;

    /** 删除房间推流任务。通话中有效。
     @param[in] task_id  直播任务id
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int removeLiveStreamTask(const char* task_id) = 0;
};

} //namespace nertc

#endif

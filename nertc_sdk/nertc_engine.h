/** @file nertc_engine.h
* @brief NERTC SDK接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/


/**
 @mainpage Introduction
 @brief <p>网易云通信 SDK 提供完善的音视频通话开发框架，提供基于网络的点对点视频通话和语音通话功能，还提供多人视频和音频会议功能，支持通话中音视频设备控制和实时音视频模式切换，支持视频采集数据回调以实现美颜等自定义功能。</p>
 <p><ul>
 <li>\ref nertc::IRtcEngine "IRtcEngine" 接口类包含应用程序调用的主要方法。</li>
 <li>\ref nertc::IRtcEngineEx "IRtcEngineEx" 接口类包含应用程序调用的扩展方法。</li>
 <li>\ref nertc::IRtcEngineEventHandler "IRtcEngineEventHandler" 接口类用于向应用程序发送回调通知。</li>
 <li>\ref nertc::IRtcEngineEventHandlerEx "IRtcEngineEventHandlerEx" 接口类用于向应用程序发送扩展回调通知。</li>
 <li>\ref nertc::IRtcMediaStatsObserver "IRtcMediaStatsObserver" 接口类用于向应用程序发送扩展的媒体回调通知。</li>
 </ul></p>
 
 @section 方法
 
 ### 核心方法
 主要包括创建 NERTC 引擎和基本通信的方法。本组方法适用于云信如下产品：语音通话、视频通话。

 - createNERtcEngine()
 - \ref nertc::IRtcEngine::initialize "initialize"
 - \ref nertc::IRtcEngine::release "release"
 - \ref nertc::IRtcEngine::joinChannel "joinChannel"
 - \ref nertc::IRtcEngine::leaveChannel "leaveChannel"
 - \ref nertc::IRtcEngine::setChannelProfile "setChannelProfile"

 ### 核心音频方法
 本组方法适用于云信如下产品： 语音通话、视频通话。
 
 - \ref nertc::IRtcEngine::enableLocalAudio "enableLocalAudio"
 - \ref nertc::IRtcEngineEx::setAudioProfile "setAudioProfile"
 - \ref nertc::IRtcEngineEx::muteLocalAudioStream "muteLocalAudioStream"

 ### 核心视频方法
 本组方法适用于云信如下产品： 视频通话。
 
 - \ref nertc::IRtcEngine::enableLocalVideo "enableLocalVideo"
 - \ref nertc::IRtcEngineEx::setVideoConfig "setVideoConfig"
 - \ref nertc::IRtcEngine::setupLocalVideoCanvas "setupLocalVideoCanvas"
 - \ref nertc::IRtcEngine::setupRemoteVideoCanvas "setupRemoteVideoCanvas"
 - \ref nertc::IRtcEngineEx::setLocalRenderMode "setLocalRenderMode"
 - \ref nertc::IRtcEngineEx::setRemoteRenderMode "setRemoteRenderMode"
 - \ref nertc::IRtcEngineEx::startVideoPreview "startVideoPreview"
 - \ref nertc::IRtcEngineEx::stopVideoPreview "stopVideoPreview"
 - \ref nertc::IRtcEngineEx::muteLocalVideoStream "muteLocalVideoStream"
 
 ### 其他方法
 本组方法适用于云信如下产品： 语音通话、视频通话。
 
 - \ref nertc::IRtcEngineEx::getVersion "getVersion"
 - \ref nertc::IRtcEngineEx::getErrorDescription "getErrorDescription"

 ### 定制方法
    本组方法适用于云信如下产品： 语音通话、视频通话。
 
 - \ref nertc::IRtcEngineEx::setParameters "setParameters"
 - \ref nertc::IRtcEngineEx::setRecordingAudioFrameParameters "setRecordingAudioFrameParameters"
 - \ref nertc::IRtcEngineEx::setPlaybackAudioFrameParameters "setPlaybackAudioFrameParameters"

 @section 回调
 
 ### 核心事件回调
 本组方法适用于云信如下产品： 语音通话、视频通话。

 - \ref nertc::IRtcEngineEventHandler::onError "onError"
 - \ref nertc::IRtcEngineEventHandler::onWarning "onWarning"
 - \ref nertc::IRtcEngineEventHandler::onJoinChannel "onJoinChannel"
 - \ref nertc::IRtcEngineEventHandler::onRejoinChannel "onRejoinChannel"
 - \ref nertc::IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel"
 - \ref nertc::IRtcEngineEventHandler::onDisconnect "onDisconnect" 
 - \ref nertc::IRtcEngineEventHandler::onUserJoined "onUserJoined"
 - \ref nertc::IRtcEngineEventHandler::onUserLeft "onUserLeft"

 ### 媒体事件回调
 本组方法适用于云信如下产品： 语音通话、视频通话。
 - \ref nertc::IRtcEngineEventHandler::onUserAudioStart "onUserAudioStart"
 - \ref nertc::IRtcEngineEventHandler::onUserAudioStop "onUserAudioStop"
 - \ref nertc::IRtcEngineEventHandler::onUserVideoStart "onUserVideoStart"
 - \ref nertc::IRtcEngineEventHandler::onUserVideoStop "onUserVideoStop"
 - \ref nertc::IRtcEngineEventHandlerEx::onUserAudioMute "onUserAudioMute"
 - \ref nertc::IRtcEngineEventHandlerEx::onUserVideoMute "onUserVideoMute" 
 - \ref nertc::IRtcEngineEventHandlerEx::onUserVideoProfileUpdate "onUserVideoProfileUpdate"
 - \ref nertc::IRtcEngineEventHandlerEx::onAudioDeviceStateChanged "onAudioDeviceStateChanged"
 - \ref nertc::IRtcEngineEventHandlerEx::onVideoDeviceStateChanged "onVideoDeviceStateChanged"
 - \ref nertc::IRtcEngineEventHandlerEx::onFirstAudioDataReceived "onFirstAudioDataReceived"
 - \ref nertc::IRtcEngineEventHandlerEx::onFirstVideoDataReceived "onFirstVideoDataReceived"
 - \ref nertc::INERtcAudioFrameObserver::onAudioFrameDidRecord "onAudioFrameDidRecord"
 - \ref nertc::INERtcAudioFrameObserver::onAudioFrameWillPlayback "onAudioFrameWillPlayback"
 - \ref nertc::IRtcEngineEventHandlerEx::onCaptureVideoFrame "onCaptureVideoFrame"

*/

/**
 @defgroup createNERtcEngine Create an NERTC Engine
 */

#ifndef NERTC_ENGINE_H
#define NERTC_ENGINE_H

#include "nertc_base.h"
#include "nertc_base_types.h"
#include "nertc_engine_defines.h"
#include "nertc_engine_event_handler_ex.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** RtcEngineContext 定义
*/
struct NERtcEngineContext
{
    const char *app_key;		/**< 用户注册云信的APP Key。如果你的开发包里面缺少 APP Key，请申请注册一个新的 APP Key。*/
    IRtcEngineEventHandlerEx *event_handler;	/**< 用于 SDK 向 App 发送回调事件通知。*/
    const char *log_dir_path;	/**< 日志目录的完整路径，采用UTF-8 编码。*/
    NERtcLogLevel log_level;	/**< 日志级别，默认级别为 kNERtcLogLevelInfo。*/
    uint32_t log_file_max_size_KBytes;	/**< 指定 SDK 输出日志文件的大小上限，单位为 KB。如果设置为 0，则默认为 20 M。*/
    bool video_prefer_hw_encoder; /**< 视频硬件编码加速，优先使用硬件编码视频数据。仅mac下有效*/
    bool video_prefer_hw_decoder; /**< 视频硬件解码加速，优先使用硬件解码视频数据。仅mac下有效*/
    bool video_use_exnternal_render; /**< 是否使用外部渲染，默认为false。仅mac下有效*/
};

/** RtcEngine 类提供了供 App 调用的主要接口方法。

 IRtcEngine 是 NERTC SDK 的基础接口类。创建一个 IRtcEngine 对象并调用这个对象的方法可以激活 NERTC SDK 的通信功能。
 */
class IRtcEngine
{
public:
    virtual ~IRtcEngine() {}

    /** 初始化 NERTC SDK 服务。

     该方法用来进行初始化 NERTC 服务。传入云信为开发者签发的厂商秘钥进行初始化。 在调用 createNERtcEngine() 方法创建 IRtcEngine 
	 对象后，必须先调用该方法进行初始化，才能使用其他方法。初始化成功后，默认处于音视频通话模式。

     @warning
     - 必须使用同一个 App Key 才能进行通话。
     - 一个 IRtcEngine 实例对象只能使用一个 App Key。如需更换 App Key，必须先调用 \ref IRtcEngine::release "release" 方法销毁当前实例，再调用本方法重新创建实例。

     @param[in] context 传入的RTC engine context对象: NERtcEngineContext.
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int initialize(const NERtcEngineContext& context) = 0;

    /** 销毁 IRtcEngine 对象。

     @param[in] sync

     - true: 同步调用。在等待 IRtcEngine 对象资源释放后再返回。App 不应该在 SDK 产生的回调中调用该接口，否则由于 SDK 要等待回调返回才能回收相关的对象资源，会造成死锁。SDK 会自动检测这种死锁并转为异步调用，但是检测本身会消耗额外的时间。
     - false: 异步调用。不等 IRtcEngine 对象资源释放就立即返回。SDK 会自行释放所有资源。使用异步调用时要注意，不要在该调用后立即卸载 SDK 动态库，否则可能会因为 SDK 的清理线程还没有退出而崩溃。
     */
    virtual void release(bool sync = false) = 0;


    /** 设置通话模式

     @note 必须在加入通话前设置，开始通话后设置无效，结束通话后保留之前的设置。

     @param[in] profile 场景模式 NERtcChannelProfileType
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setChannelProfile(NERtcChannelProfileType profile) = 0;

    /** 加入频道。如果频道还未创建，会自动尝试创建频道。

     该方法让用户加入通话频道，在同一个频道内的用户可以互相通话，多个用户加入同一个频道，可以群聊。 使用不同 App Key 的 App 是不能互通的。如果已在通话中，用户必须调用 \ref IRtcEngine::leaveChannel "leaveChannel" 退出当前通话，才能进入下一个频道。

     @note 频道内每个用户的用户 ID 必须是唯一的。
     
     @param[in] token 动态秘钥。安全要求不高: 将值设为 空字符串。安全要求高: 将值设置为 Token。如果你已经启用了 App Certificate, 请务必使用 Token。
     @param[in] channel_name 标识通话的频道名称，长度在 64 字节以内的字符串。以下为支持的字符集范围（共 89 个字符）: a-z, A-Z, 0-9, space, !#$%&()+-:;&le;.,>? @[]^_{|}~”
     @param[in] uid 用户ID。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int joinChannel(const char* token, const char* channel_name, uid_t uid) = 0;

    /** 离开频道。

     离开频道，即挂断或退出通话。

     当调用 \ref IRtcEngine::joinChannel "joinChannel" 方法后，必须调用 \ref IRtcEngine::leaveChannel "leaveChannel" 结束通话，否则无法开始下一次通话。 不管当前是否在通话中，都可以调用 leaveChannel，没有副作用。该方法会把会话相关的所有资源释放掉。

     该方法是异步操作，调用返回时并没有真正退出频道。在真正退出频道后，SDK 会触发 \ref IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel" 回调。

     @note
     - 如果你调用了 \ref IRtcEngine::leaveChannel "leaveChannel" 后立即调用 \ref IRtcEngine::release "release" , SDK 将无法触发 \ref IRtcEngineEventHandler::onLeaveChannel "onLeaveChannel" 回调。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int leaveChannel() = 0;

    /** Retrieves the pointer to the device manager object.

     @param iid ID of the interface.
     @param inter Pointer to the *DeviceManager* object.
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int queryInterface(NERtcInterfaceIdType iid, void** inter) = 0;

    /** 开启或关闭本地语音采集或处理

    该方法可以重新开启本地语音功能，重新开始本地音频采集及处理。

    该方法不影响接收或播放远端音频流。

    @note
    - 该方法与 \ref IRtcEngineEx::muteLocalAudioStream "muteLocalAudioStream" 的区别在于:
    - \ref IRtcEngine::enableLocalAudio "enableLocalAudio": 开启本地语音采集及处理
    - \ref IRtcEngineEx::muteLocalAudioStream "muteLocalAudioStream": 停止或继续发送本地音频流

    @note 该方法设置内部引擎为启用状态，在 \ref IRtcEngine::leaveChannel "leaveChannel" 后仍然有效。

    @param[in] enabled
    - true: 重新开启本地语音功能，即开启本地语音采集或处理（默认）
    - false: 关闭本地语音功能，即停止本地语音采集或处理

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int enableLocalAudio(bool enabled) = 0;

    /** 设置本地视图。

     该方法设置本地视频显示信息。App 通过调用此接口绑定本地视频流的显示视窗(view)。 在 App 开发中，通常在初始化后调用该方法进行本地视频设置，然后再加入频道。

     @param[in] canvas 视频画布信息
     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int setupLocalVideoCanvas(NERtcVideoCanvas* canvas) = 0;

    /** 设置远端用户视图。

     该方法绑定远端用户和显示视图，即设定 uid 指定的用户用哪个视图显示。调用该接口时需要指定远端视频的 uid，一般可以在进频道前提前设置好。

     如果 App 不能事先知道对方的 uid，可以在 APP 收到 \ref IRtcEngineEventHandler::onUserJoined "onUserJoined" 事件时设置。
     退出频道后，SDK 会把远端用户的绑定关系清除掉。

     @param[in] uid 远端用户 ID。
     @param[in] canvas 视频画布信息
     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int setupRemoteVideoCanvas(uid_t uid, NERtcVideoCanvas* canvas) = 0;

    /** 开启或关闭本地视频采集和渲染

    该方法启用本地视频采集功能。

    @note 该方法设置内部引擎为启用状态，在 \ref IRtcEngine::leaveChannel "leaveChannel" 后仍然有效。

    @param[in] enabled 是否启用本地视频:
    - true: 开启本地视频采集和渲染 (默认)；
    - false: 关闭使用本地摄像头设备。关闭后，远端用户会接收不到本地用户的视频流；但本地用户依然可以接收远端用户的视频流。设置为 false 时，该方法不需要本地有摄像头。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int enableLocalVideo(bool enabled) = 0;

    /** 订阅 / 取消订阅指定远端用户的视频流。对方打开视频后需要主动订阅

    @param[in] uid 指定用户的用户 ID。
    @param[in] type 流类型。 #NERtcRemoteVideoStreamType
    @param[in] subscribe
    - true: 订阅指定远端用户的视频流；
    - false: 取消订阅指定远端用户的视频流。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int subscribeRemoteVideoStream(uid_t uid, NERtcRemoteVideoStreamType type, bool subscribe) = 0;
};

} //namespace nertc


////////////////////////////////////////////////////////
/** \addtogroup createNERtcEngine
 @{
 */
////////////////////////////////////////////////////////

/** 创建 RTC 引擎对象并返回指针。

 @return RTC 引擎对象的指针。
 */
NERTC_API nertc::IRtcEngine* NERTC_CALL createNERtcEngine();

/** 销毁 RTC 引擎对象。
 */
NERTC_API void NERTC_CALL destroyNERtcEngine(void *& nertc_engine_inst);

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////

#endif

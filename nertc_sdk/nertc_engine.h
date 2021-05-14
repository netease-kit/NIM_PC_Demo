/** @file nertc_engine.h
* @brief NERTC SDK接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2021, NetEase Inc. All rights reserved
* @author Harrison
* @date 2021/03/25
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
struct NERtcEngineContext {
    const char* app_key; /**< 用户注册云信的APP Key。如果你的开发包里面缺少 APP Key，请申请注册一个新的 APP Key。*/
    IRtcEngineEventHandlerEx* event_handler; /**< 用于 SDK 向 App 发送回调事件通知。*/
    const char* log_dir_path;                /**< 日志目录的完整路径，采用UTF-8 编码。*/
    NERtcLogLevel log_level;                 /**< 日志级别，默认级别为 kNERtcLogLevelInfo。*/
    uint32_t log_file_max_size_KBytes; /**< 指定 SDK 输出日志文件的大小上限，单位为 KB。如果设置为 0，则默认为 20 M。*/
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

     在调用 createNERtcEngine() 方法创建 IRtcEngine 对象后，必须先调用该方法进行初始化，才能使用其他方法。初始化成功后，默认处于音视频通话模式。

     @warning
     - 必须使用同一个 App Key 才能进行通话。
     - 一个 IRtcEngine 实例对象只能使用一个 App Key。如需更换 App Key，必须先调用 \ref IRtcEngine::release "release" 方法销毁当前实例，再调用本方法重新创建实例。

     @param[in] context 传入的RTC engine context对象: NERtcEngineContext.
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int initialize(const NERtcEngineContext& context) = 0;

    /** 销毁 NERtc 实例，释放资源。
     
     该方法释放 NERTC SDK 使用的所有资源。有些 App 只在用户需要时才进行实时音视频通信，不需要时则将资源释放出来用于其他操作，该方法适用于此类情况。
     
     调用 release 方法后，您将无法再使用 SDK 的其它方法和回调。如需再次使用实时音视频通话功能，您必须重新创建一个新的 NERtc 实例。
     
     @note 如果需要重新使用IRtcEngine，release 后不能再 initialize，需要 destroyNERtcEngine 后重新 createNERtcEngine。

     @param[in] sync 默认为 true 且只能设置为 true，表示同步调用，等待 IRtcEngine 对象资源释放后再返回。<br>App 不应该在 SDK 产生的回调中调用该接口，否则由于 SDK 要等待回调返回才能回收相关的对象资源，会造成死锁。SDK 会自动检测这种死锁并转为异步调用，但是检测本身会消耗额外的时间。
     */
    virtual void release(bool sync = true) = 0;
    
    /** 在直播场景中设置用户角色。
    
     用户角色支持设置为主播或观众，主播和观众的权限不同。

     - 主播：可以开关摄像头等设备、可以发布流、可以操作互动直播推流相关接口、上下线对其他房间内用户可见。
     - 观众：不可以开关摄像头等设备、不可以发布流、不可以操作互动直播推流相关接口、上下线对其他房间内用户不可见。

     @note 
     - 默认情况下用户以主播角色加入房间。
     - 在加入房间前，用户可以调用本接口切换本端角色为观众。在加入房间后，用户也可以通过本接口切换用户角色。
     - 用户切换为观众角色时，SDK 会自动关闭音视频设备。

     @param[in] role 用户角色。 NERtcClientRole
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setClientRole(NERtcClientRole role) = 0;

    /** 设置房间场景。

     房间场景可设置为通话或直播场景，不同的场景中 QoS 策略不同。

     @note 必须在加入通话前设置，开始通话后设置无效，结束通话后保留之前的设置。

     @param[in] profile 设置房间场景。详细信息请参考 NERtcChannelProfileType。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int setChannelProfile(NERtcChannelProfileType profile) = 0;

    /** 加入音视频房间。
     
     加入音视频房间时，如果指定房间尚未创建，云信服务器内部会自动创建一个同名房间。
     - SDK 加入房间后，同一个房间内的用户可以互相通话，多个用户加入同一个房间，可以群聊。使用不同 App Key 的 App 之间不能互通。
     - 成功调用该方加入房间后，本地会触发onJoinChannel回调，远端会触发onUserJoined回调。
     - 用户成功加入房间后，默认订阅房间内所有其他用户的音频流，可能会因此产生用量并影响计费。如果想取消自动订阅，可以在通话前通过调用setParameters方法实现。
     - 若您已在通话中，可以通过 switchChannel 切换房间。

     @note 房间内每个用户的用户 ID 必须是唯一的。
     

     @param[in] token 安全认证签名（NERTC Token）。可设置为：
     - null。非安全模式下可设置为 null。安全性不高，建议在产品正式上线前联系对应商务经理转为安全模式。
     - 已获取的NERTC Token。安全模式下必须设置为获取到的 Token 。若未传入正确的 Token 将无法进入房间。推荐使用安全模式。
     @param[in] channel_name 房间名称，设置相同房间名称的用户会进入同一个通话房间。字符串格式，长度为1~ 64 字节。支持以下89个字符：a-z, A-Z, 0-9, space, !#$%&()+-:;≤.,>? @[]^_{|}~”
     @param[in] uid 用户的唯一标识 id，房间内每个用户的 uid 必须是唯一的。不可设置为 0 或负数。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int joinChannel(const char* token, const char* channel_name, uid_t uid) = 0;

    /** 快速切换音视频房间。
   
    房间场景为直播场景时，房间中角色为观众的成员可以调用该方法从当前房间快速切换至另一个房间。

    成功调用该方切换房间后，本端会先收到离开房间的回调 onLeaveChannel，再收到成功加入新房间的回调 onJoinChannel。远端用户会收到 onUserLeave 和 onUserJoined 的回调。
   
    房间成员成功切换房间后，默认订阅房间内所有其他成员的音频流，因此产生用量并影响计费。如果想取消订阅，可以通过调用相应的 subscribeRemoteAudio 方法传入false实现。

     @note 该方法仅适用于直播场景中，角色为观众的音视频房间成员。即已通过接口 setchannelprofile 设置房间场景为直播，通过 setClientRole 设置房间成员的角色为观众。

     @param[in] token 安全认证签名（NERTC Token）。可设置为：
     - null。非安全模式下可设置为 null。安全性不高，建议在产品正式上线前联系对应商务经理转为安全模式。
     - 已获取的NERTC Token。安全模式下必须设置为获取到的 Token 。若未传入正确的 Token 将无法进入房间。推荐使用安全模式。
     @param[in] channel_name 期望切换到的目标房间名称。

     @return
     -0: 方法调用成功
     -其他：方法调用失败
     */
    virtual int switchChannel(const char* token, const char* channel_name) = 0;

    /** 离开房间。

     离开房间，即挂断或退出通话。

     结束通话时，必须调用leaveChannel结束通话，否则无法开始下一次通话。

     成功调用该方法离开房间后，本地会触发onLeaveChannel回调，远端会触发onUserLeave回调。

     @note
     - 该方法是异步操作，调用返回时并没有真正退出房间。在真正退出房间后，SDK 会触发onLeaveChannel回调。
     - 如果您调用了leaveChannel后立即调用release，SDK 将无法触发onLeaveChannel回调。

     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int leaveChannel() = 0;

    /** 获取设备管理员对象的指针。

     @param[in] iid 想要获取的接口的iid.
     @param[in] inter 指向 DeviceManager 对象的指针。
     @return
     - 0: 方法调用成功；
     - 其他: 方法调用失败。
     */
    virtual int queryInterface(NERtcInterfaceIdType iid, void** inter) = 0;

    /** 开启/关闭本地音频采集

    该方法可以重新开启本地语音功能，重新开始本地音频采集及处理。

    该方法不影响接收或播放远端音频流。

    @note 该方法与 \ref IRtcEngineEx::muteLocalAudioStream "muteLocalAudioStream" 的区别在于:
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

     该方法设置本地视频显示信息。只影响本地用户看到的视频画面，不影响远端。

     App 通过调用此接口绑定本地视频流的显示视窗(view)。 在 App 开发中，通常在初始化后调用该方法进行本地视频设置，然后再加入房间。

     @note mac端若使用外部渲染，必须在 SDK 初始化时设置。

     @param[in] canvas 视频画布信息
     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int setupLocalVideoCanvas(NERtcVideoCanvas* canvas) = 0;

    /** 设置远端用户视图。

     该方法绑定远端用户和显示视图，并设置远端用户视图在本地显示时的渲染模式和镜像模式，只影响本地用户看到的视频画面。
    @note
    - 调用该接口时需要指定远端视频的 uid，一般可以在用户加入后设置好。
    - 如果 App 无法事先知道对方的用户 ID，可以在 APP 收到 onUserJoined 事件时设置。- 解除某个用户的绑定视图可以把 canvas 设置为空。
    - 退出房间后，SDK 会清除远程用户和视图的绑定关系。

     @param[in] uid 远端用户 ID。
     @param[in] canvas 视频画布信息
     @return
	 - 0: 方法调用成功；
	 - 其他: 方法调用失败。
     */
    virtual int setupRemoteVideoCanvas(uid_t uid, NERtcVideoCanvas* canvas) = 0;

    /** 开启或关闭本地视频采集和渲染。

    该方法启用本地视频采集功能。

    @note 
     - 该方法在加入房间前和加入房间后均可调用。
     - 该方法设置内部引擎为启用状态，在 \ref IRtcEngine::leaveChannel "leaveChannel" 后仍然有效。
     - 成功启用或禁用本地视频采集和渲染后，远端会触发 onUserVideoStop 或 onUserVideoStart  回调。


    @param[in] enabled 是否启用本地视频采集和渲染:
    - true: 开启本地视频采集和渲染 (默认)；
    - false: 关闭使用本地摄像头设备。关闭后，远端用户会接收不到本地用户的视频流；但本地用户依然可以接收远端用户的视频流。设置为 false 时，该方法不需要本地有摄像头。

    @return
    - 0: 方法调用成功；
    - 其他: 方法调用失败。
    */
    virtual int enableLocalVideo(bool enabled) = 0;

    /** 订阅或取消订阅指定远端用户的视频流。

    - 用户加入房间之后，默认不订阅远端用户的视频流，如果希望看到指定远端用户的视频，可以在监听到对方加入房间或发布视频流之后，通过此方法订阅该用户的视频流。
    - 该方法需要在加入房间后调用。

    @param[in] uid 指定用户的用户 ID。
    @param[in] type 订阅的视频流类型。 #NERtcRemoteVideoStreamType
    @param[in] subscribe
    - true: （默认）订阅指定视频流。
    - false: 不订阅指定视频流。

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

 @note 同时只支持一个IRtcEngine对象，新创建前必须先释放前一个IRtcEngine。

 @return RTC 引擎对象的指针。
 */
NERTC_API nertc::IRtcEngine* NERTC_CALL createNERtcEngine();

/** 销毁 RTC 引擎对象。
 @note 释放前需要先调用\ref nertc::IRtcEngine::release "release"
 */
NERTC_API void NERTC_CALL destroyNERtcEngine(void *& nertc_engine_inst);

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////

#endif

/** @file nertc_engine_event_handler.h
* @brief NERTC SDK回调接口头文件。
* NERTC SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @author Harrison
* @date 2018/11/20
*/

#ifndef NERTC_ENGINE_EVENT_HANDLER_H
#define NERTC_ENGINE_EVENT_HANDLER_H

#include "nertc_base_types.h"
#include "nertc_error_code.h"
#include "nertc_warn_code.h"
#include "nertc_engine_defines.h"

 /**
 * @namespace nertc
 * @brief namespace nertc
 */
namespace nertc
{
/** IRtcEngineEventHandler 回调接口类用于 SDK 向 App 发送回调事件通知，App 通过继承该接口类的方法获取 SDK 的事件通知。

 接口类的所有方法都有缺省（空）实现，App 可以根据需要只继承关心的事件。在回调方法中，App 不应该做耗时或者调用可能会引起阻塞的 API（如开启音频或视频等），否则可能影响 SDK 的运行。
 */
class IRtcEngineEventHandler
{
public:
    virtual ~IRtcEngineEventHandler() {}

    /** 发生错误回调。

     该回调方法表示 SDK 运行时出现了（网络或媒体相关的）错误。通常情况下，SDK上报的错误意味着SDK无法自动恢复，需要 App 干预或提示用户。

     @param error_code 错误码。详细信息请参考 NERtcDMErrorCode
     @param msg 错误描述。
     */
    virtual void onError(int error_code, const char* msg) {
        (void)error_code;
        (void)msg;
    }

    /** 发生警告回调。

     该回调方法表示 SDK 运行时出现了（网络或媒体相关的）警告。通常情况下，SDK 上报的警告信息 App 可以忽略，SDK 会自动恢复。

     @param warn_code 错误代码: #NERtcWarnCode.
     @param msg 警告描述。
     */
    virtual void onWarning(int warn_code, const char* msg) {
        (void)warn_code;
        (void)msg;
    }

    /** 释放硬件资源的回调。

     SDK提示释放硬件资源是否成功。

     @param result 返回结果。
     */
    virtual void onReleasedHwResources(NERtcErrorCode result) {
        (void)result;
    }

    /** 加入房间回调，表示客户端已经登入服务器。

     @param cid  客户端加入的房间 ID。
     @param uid  用户 ID。
     @param result  返回结果。
     @param elapsed 从 joinChannel 开始到发生此事件过去的时间，单位为毫秒。
     */
    virtual void onJoinChannel(channel_id_t cid, uid_t uid, NERtcErrorCode result, uint64_t elapsed) {
        (void)cid;
        (void)uid;
        (void)result;
        (void)elapsed;
    }

    /** 触发重连。

      有时候由于网络原因，客户端可能会和服务器失去连接，SDK会进行自动重连，开始自动重连后触发此回调。

     @param cid  房间 ID。
     @param uid  用户 ID。
     */
    virtual void onReconnectingStart(channel_id_t cid, uid_t uid) {
        (void)cid;
        (void)uid;
    }

    /** 网络连接状态已改变回调。

      该回调在网络连接状态发生改变的时候触发，并告知用户当前的网络连接状态和引起网络状态改变的原因。

     @param state  当前的网络连接状态。
     @param reason  引起当前网络连接状态发生改变的原因。
     */
    virtual void onConnectionStateChange(NERtcConnectionStateType state, NERtcReasonConnectionChangedType reason) {
        (void)state;
        (void)reason;
    }

    /** 重新加入房间回调。

	  在弱网环境下，若客户端和服务器失去连接，SDK会自动重连。自动重连成功后触发此回调方法。

     @param cid  客户端加入的房间 ID。
     @param uid  用户 ID。
     @param result  返回结果。
     @param elapsed 从开始重连到发生此事件过去的时间，单位为毫秒。
     */
    virtual void onRejoinChannel(channel_id_t cid, uid_t uid, NERtcErrorCode result, uint64_t elapsed) {
        (void)cid;
        (void)uid;
        (void)result;
    }

    /** 退出房间回调。

     App 调用 leaveChannel 方法后，SDK 提示 App 退出房间是否成功。

     @param result 返回结果。
     */
    virtual void onLeaveChannel(NERtcErrorCode result) {
        (void)result;
    }

    /** 网络连接中断

     @note
     - SDK 在调用 joinChannel 加入房间成功后，如果和服务器失去连接且连续 3 次重连失败，就会触发该回调。
	 - 由于非网络原因，客户端可能会和服务器失去连接，此时SDK无需自动重连，直接触发此回调方法。

     @param reason  网络连接中断原因。
     */
    virtual void onDisconnect(NERtcErrorCode reason) {
        (void)reason;
    }
    
    /** 直播场景下用户角色已切换回调。
    
    本地用户加入房间后，通过 \ref IRtcEngine::setClientRole "setClientRole" 切换用户角色后会触发此回调。例如主播切换为观众、从观众切换为主播。

    @note 直播场景下，如果您在加入房间后调用该方法切换用户角色，调用成功后，会触发以下回调：
    - 主播切观众，本端触发onClientRoleChanged回调，远端触发\ref nertc::IRtcEngineEventHandler::onUserLeft "onUserLeft"回调。
    - 观众切主播，本端触发onClientRoleChanged回调，远端触发\ref nertc::IRtcEngineEventHandler::onUserJoined "onUserJoined"回调。

     @param oldRole  切换前的角色。
     @param newRole  切换后的角色。
     */
    virtual void onClientRoleChanged(NERtcClientRole oldRole, NERtcClientRole newRole) {
        (void)oldRole;
        (void)newRole;
    }

    /** 远端用户加入当前房间回调。

     该回调提示有远端用户加入了房间，并返回新加入用户的 ID；如果加入之前，已经有其他用户在房间中了，新加入的用户也会收到这些已有用户加入房间的回调。

     @param uid 新加入房间的远端用户 ID。
     @param user_name 新加入频道的远端用户名。
     */
    virtual void onUserJoined(uid_t uid, const char * user_name) {
        (void)uid;
        (void)user_name;
    }

    /** 远端用户离开当前房间的回调。

     提示有远端用户离开了房间（或掉线）。通常情况下，用户离开房间有两个原因，即正常离开和超时掉线：
     - 正常离开的时候，远端用户会收到正常离开房间的回调提醒，判断用户离开房间。
     - 超时掉线的依据是，在一定时间内（40~50s），用户没有收到对方的任何数据包，则判定为对方掉线。

     @param uid 离开房间的远端用户 ID。
     @param reason 远端用户离开原因。
     */
    virtual void onUserLeft(uid_t uid, NERtcSessionLeaveReason reason) {
        (void)uid;
        (void)reason;
    }
    /** 远端用户开启音频的回调。
    
     @param uid 远端用户ID。
     */
    virtual void onUserAudioStart(uid_t uid) {
        (void)uid;
    }
    /** 远端用户停用音频的回调。

     @param uid 远端用户ID。
     */
    virtual void onUserAudioStop(uid_t uid) {
        (void)uid;
    }
    /** 远端用户开启视频的回调。

     @param uid 远端用户ID。
     @param max_profile 视频编码的分辨率，用于衡量编码质量。
     */
    virtual void onUserVideoStart(uid_t uid, NERtcVideoProfileType max_profile) {
        (void)uid;
        (void)max_profile;
    }
    /** 远端用户停用视频的回调。

     @param uid 远端用户ID。
     */
    virtual void onUserVideoStop(uid_t uid) {
        (void)uid;
    }
};
} //namespace nertc

#endif

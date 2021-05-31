/**
 * @file avchat_component.h
 * @brief 呼叫组件头文件
 * @copyright (c) 2014-2021, NetEase Inc. All rights reserved
 * @author Martin
 * @date 2021/05/28
 */

#ifndef AVCHAT_COMPONENT_H_
#define AVCHAT_COMPONENT_H_

#include "src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "nertc_sdk/nertc_engine.h"
#include "nertc_sdk/nertc_engine_ex.h"
#include "nertc_sdk/nertc_audio_device_manager.h"
#include "nertc_sdk/nertc_video_device_manager.h"
#include "ui_kit/util/net_call_helper.h"

namespace nim_comp
{
    /**
     * 呼叫类型，与nim::NIMSignalingType同值
     */
    enum AVCHAT_CALL_TYPE {
        kAvChatAudio = 1,        /**< 音频类型 */
        kAvChatVideo = 2,        /**< 视频类型 */
        kAvChatCustom= 3,        /**< 自定义 */
    };

    /**
     * 呼叫模式类型
     */
    enum AVCHAT_MODULE_TYPE
    {
        kAvChatP2P = 0, /**< 单人 */
        kAvChatMulti    /**< 多人 */
    };

    /**
     * @brief 组件事件回调接口类（通话过程发生状态变化时回调）
     */
    class IAvChatComponentEventHandler
    {
    public:
       /**
        * @brief 收到邀请
        * @param invitor 邀请者的用户id
        * @param userIDs 通话里的用户id列表
        * @param isFromGroup  是否为多人通话
        * @param type  呼叫类型 {@see AVCHAT_CALL_TYPE}
        * @return void
        */
        virtual void onInvited(const std::string& invitor, 
            std::vector<std::string> userIDs, 
            bool isFromGroup, 
            int type)
        {};

        /**
         * @brief 收到用户接受
         * @param userId 用户id
         * @return void
         */
        virtual void onUserAccept(const std::string& userId) {}
        
        /**
         * @brief 收到用户拒绝
         * @param userId  用户id
         * @return void
         */
        virtual void onUserReject(const std::string& userId) {}
        
        /**
         * @brief 收到用户加入
         * @param userId 用户id
         * @return void
         */
        virtual void onUserEnter(const std::string& userId) {}

        /**
         * @brief 收到用户离开
         * @param userId 用户id
         * @return void
         */
        virtual void onUserLeave(const std::string& userId) {}

        /**
         * @brief 收到用户断开连接
         * @param userId 用户id
         * @return void
         */
        virtual void onUserDisconnect(const std::string& userId) {}

        /**
         * @brief 收到用户忙碌
         * @param userId 用户id
         * @return void
         */
        virtual void onUserBusy(const std::string& userId) {}

        /**
         * @brief 收到用户取消
         * @param userId 用户id
         * @return void
         */
        virtual void onUserCancel(const std::string& userId) {}

        /**
         * @brief 收到G2断开连接
         * @param reason 断开的原因 {@see nertc::NERtcErrorCode}
         * @return void
         */
        virtual void onDisconnect(int reason) {}

        /**
         * @brief 收到呼叫超时
         * @return void
         */
        virtual void onCallingTimeOut() {}

        /**
         * @brief 收到视频切换成音频
         * @return void
         */
        virtual void OnVideoToAudio() {}

        /**
         * @brief 收到呼叫结束
         * @return void
         */
        virtual void onCallEnd() {}

        /**
         * @brief 收到G2错误
         * @param errCode G2错误码 {@see nertc::NERtcErrorCode}
         * @return void
         */
        virtual void onError(int errCode) {}

        /**
         * @brief 收到其他端接受
         * @return void
         */
        virtual void onOtherClientAccept() {}
        
        /**
         * @brief 收到其他端拒绝
         * @return void
         */
        virtual void onOtherClientReject() {}

        /**
         * @brief 收到用户网络质量
         * @param mapNetworkQuality 用户的网络质量列表
         * @return void
         */
        virtual void onUserNetworkQuality(std::map<uint64_t, nertc::NERtcNetworkQualityType> mapNetworkQuality) {}

        /**
         * @brief 收到用户视频开关状态
         * @param userId 用户id
         * @param available 视频是开关状态，ture开，false关
         * @return void
         */
        virtual void onCameraAvailable(const std::string& userId, bool available) {}

        /**
         * @brief 收到用户音频开关状态
         * @param userId 用户id
         * @param available 视频是开关状态，ture开，false关
         * @return void
         */
        virtual void onAudioAvailable(const std::string& userId, bool available) {}

        /**
         * @brief 本地视频状态改变
         * @param isEnable 视频状态，ture开，false关
         * @return void
         */
        virtual void onLocalCameraStateChanged(bool isEnable) {}

        /**
         * @brief 音频音量改变
         * @param value 音量值
         * @param isRecord 是否为麦克风，true麦克风，false扬声器
         * @return void 
         */
        virtual void onAudioVolumeChanged(unsigned char value, bool isRecord) {}
    };

    /**
     * 组件操作回调（返回调用组件接口的错误码）
     */
    using AvChatComponentOptCb = std::function<void(int errCode)>;

    /**
     * 获取token函数
     */
    using GetTokenServiceFunc = std::function<void(int64_t uid, std::function<void(const std::string& token)> onGetToken)>;

    /**
     * @brief 组件实现类
     */
    class AvChatComponent : public nbase::SupportWeakCallback, public nertc::IRtcEngineEventHandlerEx, public nertc::IRtcMediaStatsObserver
    {
        /**
         * 呼叫的状态
         */
        enum ComponentStatus {
            idle = 0,   /**< 空闲 */
            calling,    /**< 呼叫中 */
            called,     /**< 被邀请 */
            inCall,     /**< 在通道 */
        };
    public:
        /**
         * @brief 构造函数
         */
        AvChatComponent();

        /**
         * @brief 析构函数
         */
        ~AvChatComponent();

        /**
         * @brief 创建内部资源
         * @param key appkey
         * @param useRtcSafeMode 是否使用安全模式，默认true使用，false不使用
         * @return void 
         */
        void setupAppKey(const std::string& key, bool useRtcSafeMode = true);

        /**
         * @brief 释放内部资源
         * @return void 
         */
        void release();
        
        /**
         * @brief 登录
         * @param account 账号
         * @param token token
         * @param cb 登录结果回调
         * @return void
         */
        void login(const std::string& account, const std::string& token, AvChatComponentOptCb cb);

        /**
         * @brief 登出
         * @param cb 登出结构回调
         * @return void
         */
        void logout(AvChatComponentOptCb cb);

        /**
         * @brief 设置本地视频画布
         * @param canvas 画布
         * @return void
         */
        void setupLocalView(nertc::NERtcVideoCanvas* canvas);

        /**
         * @brief 设置远端视频画布
         * @param canvas 画布
         * @param userId 用户id
         * @return void
         */
        void setupRemoteView(nertc::NERtcVideoCanvas* canvas, const std::string& userId);

        /**
         * @brief 切换视频
         * @return void
         */
        void switchCamera();

        /**
         * @brief 打开本地视频
         * @param enable 是否打开本地视频，true打开，false关闭
         * @return void 
         */
        void enableLocalVideo(bool enable);

        /**
         * @brief 打开本地音频
         * @param mute 是否打开本地音频，true打开，false关闭
         * @return void
         */
        void muteLocalAudio(bool mute);

        /**
         * @brief 静音本地音频
         * @param enable 是否静音本地音频，true不静音，false静音
         * @return void
         */
        void enableAudioPlayout(bool enable);

        /**
         * @brief 呼叫
         * @param userId 对方用户id 
         * @param type 呼叫类型 {@see AVCHAT_CALL_TYPE}
         * @param cb 结果回调
         * @return void
         */
        void call(const std::string& userId, AVCHAT_CALL_TYPE type, AvChatComponentOptCb cb);

        /**
         * @brief 接受
         * @param cb 结果回调
         * @return void
         */
        void accept(AvChatComponentOptCb cb);

        /**
         * @brief 拒绝
         * @param cb 结果回调
         * @return void
         */
        void reject(AvChatComponentOptCb cb);

        /**
         * @brief 挂断
         * @param cb 结果回调
         * @return void
         */
        void hangup(AvChatComponentOptCb cb);

        /**
         * @brief 取消
         * @param cb 结果回调
         * @return void
         */
        void cancel(AvChatComponentOptCb cb);

        /**
         * @brief 离开
         * @param cb 结果回调
         * @return void
         */
        void leave(AvChatComponentOptCb cb);

        /**
         * @brief 注册事件监听器
         * @param compEventHandler 事件监听器
         * @return void
         */
        void regEventHandler(std::shared_ptr<IAvChatComponentEventHandler> compEventHandler);
        
        /**
         * @brief 本地预览
         * @param start 是否开始本地预览，true开始，false停止
         * @return void
         */
        void startVideoPreview(bool start = true);

        /**
         * @brief 切换呼叫类型
         * @param user_id 用户id
         * @param call_type 呼叫类型 {@see AVCHAT_CALL_TYPE}
         * @return void
         */
        void switchCallType(std::string user_id, int call_type);

        /**
         * @brief 开始音频设备回路测试
         * @param interval 回调时间间隔，单位为毫秒
         * @return void
         */
        void startAudioDeviceLoopbackTest(int interval);

        /**
         * @brief 停止音频设备回路测试
         * @return void
         */
        void stopAudioDeviceLoopbackTest();

        /**
         * @brief 请求token
         * @param uid 用户id
         * @return void
         */
        void requestTokenValue(int64_t uid);

        /**
         * @brief 设置视频画质质量
         * @param type 画质类型
         * @return void
         */
        void setVideoQuality(nertc::NERtcVideoProfileType type);

        /**
         * @brief 打开用户音频
         * @param user_id 用户id
         * @param bOpen 是否打开用户音频，true打开，false关闭
         * @return void
         */
        void setAudioMute(std::string user_id, bool bOpen);

        /**
         * @brief 获取频道信息
         * @return nim::SignalingCreateResParam
         */
        nim::SignalingCreateResParam getCreatedChannelInfo() {return createdChannelInfo_;}

        /**
         * @brief 呼叫等待超时
         * @return void
         */
        void onWaitingTimeout();

        /**
         * @brief 设置麦克风音量
         * @param value 音量，取值范围为[0, 400]
         * @return int
         * @retval 0 成功
         * @retval 非0 失败
         */
        int setRecordDeviceVolume(int value);

        /**
         * @brief 设置扬声器音量
         * @param value 音量，取值范围为[0, 400]
         * @return int
         * @retval 0 成功
         * @retval 非0 失败
         */
        int setPlayoutDeviceVolume(int value);

        /**
         * @brief 获取音量
         * @param isRecord 是否为麦克风，true是麦克风，false是扬声器
         * @return uint32_t 范围为[0, 255]
         */
        uint32_t getAudioVolumn(bool isRecord);

        /**
         * @brief 获取本地设备列表
         * @param recordDevicesNames 麦克风设备名列表 
         * @param recordDevicesIds 麦克风设备id列表
         * @param playoutDevicesNames 扬声器设备名列表
         * @param playoutDevicesIds 扬声器设备id列表
         * @param videoDeviceNames 视频设备名列表
         * @param videoDeviceIds 视频设备id列表
         * @return void
         */
        void getLocalDeviceList(
            std::vector<std::wstring>* recordDevicesNames,
            std::vector<std::wstring>* recordDevicesIds,
            std::vector<std::wstring>* playoutDevicesNames,
            std::vector<std::wstring>* playoutDevicesIds,
            std::vector<std::wstring>* videoDeviceNames,
            std::vector<std::wstring>* videoDeviceIds);
        
        /**
         * @brief 设置当前视频设备
         * @param id 视频设备id
         * @return void
         */
        void setVideoDevice(const std::wstring& id);

        /**
         * @brief 获取当前视频设备id
         * @return std::wstring 
         */
        std::wstring getVideoDevice();

        /**
         * @brief 设置音频设备
         * @param id 音频设备id
         * @param isRecord 是否为麦克风，true是麦克风，false是扬声器
         * @return void
         */
        void setAudioDevice(const std::wstring& id, bool isRecord);

        /**
         * @brief 获取音频设备
         * @param isRecord 是否为麦克风，true是麦克风，false是扬声器
         * @return std::wstring 
         */
        std::wstring getAudioDevice(bool isRecord);

        /**
         * @brief 获取G2引擎
         * @return nertc::IRtcEngineEx* 
         */
        nertc::IRtcEngineEx* getRtcEngine() { return rtcEngine_/*rtcEngine_.get()*/; }

        /**
         * @brief 设置token服务
         * @attention 在线上环境中，token的获取需要放到您的应用服务端完成，然后由服务器通过安全通道把token传递给客户端，
         * Demo中使用的URL仅仅是demoserver，不要在您的应用中使用，详细请参考: http://dev.netease.im/docs?doc=server
         * @param getTokenService token服务
         * @return void
         */
        void setTokenService(GetTokenServiceFunc getTokenService){ getTokenService_ = getTokenService;}
        
    protected:
        void signalingCreateCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        void signalingJoinCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb, const std::string& channelId);
        void signalingInviteCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        // 被叫方调用accept的结果的回调
        void signalingAcceptCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        void signalingRejectCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        void signalingCloseCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        void signalingLeaveCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
        void signalingControlCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param);

        void handleInvited(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleControl(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleAccepted(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleOtherClientAccepted(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleOtherClientRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleJoin(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleLeave(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleClose(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void handleCancelInvite(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void signalingNotifyCb(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void signalingMutilClientSyncCb(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
        void signalingOfflineNotifyCb(std::list<std::shared_ptr<nim::SignalingNotifyInfo>> notifyInfo);

        //G2事件回调
        virtual void onJoinChannel(nertc::channel_id_t cid, nertc::uid_t uid, nertc::NERtcErrorCode result, uint64_t elapsed) override;
        virtual void onUserJoined(nertc::uid_t uid, const char* user_name) override;
        virtual void onUserLeft(nertc::uid_t uid, nertc::NERtcSessionLeaveReason reason) override;
        virtual void onUserAudioStart(nertc::uid_t uid) override;
        virtual void onUserAudioStop(nertc::uid_t uid) override;
        virtual void onUserVideoStart(nertc::uid_t uid, nertc::NERtcVideoProfileType max_profile) override;
        virtual void onUserVideoStop(nertc::uid_t uid) override;
        virtual void onDisconnect(nertc::NERtcErrorCode reason) override;

        //G2 MediaStatsObserver回调
        //该回调描述每个用户在通话中的网络状态，每 2 秒触发一次，只上报状态有变更的成员。
        virtual void onNetworkQuality(const nertc::NERtcNetworkQualityInfo *infos, unsigned int user_count)override;
    private:
        void startDialWaitingTimer();
        void closeChannelInternal(const std::string& channelId, AvChatComponentOptCb cb);
        void updateChannelMembers(const nim::SignalingJoinResParam* res);
        void handleNetCallMsg(nim_comp::NIMNetCallStatus why);

        GetTokenServiceFunc getTokenService_;
        std::string getAccid(int64_t uid);
        std::string appKey_;
        nertc::IRtcEngineEx* rtcEngine_;
        //std::string currentChannelId;
        std::weak_ptr<IAvChatComponentEventHandler> compEventHandler_;
        AvChatComponentOptCb optCb_;
        std::string senderAccid;
        std::string toAccid;
        std::map<std::string, int64_t>    channelMembers_;
        nim::SignalingNotifyInfoInvite    invitedInfo_;
        nim::SignalingInviteParam        invitingInfo_;
        nim::SignalingCreateResParam    createdChannelInfo_;
        nbase::WeakCallbackFlag            calling_timeout_timer_;
        ComponentStatus                    status_;
        std::string                        joined_channel_id_;
        int64_t                            to_account_id_;
        std::string                        from_account_id_;
        std::string                        stoken_;
        std::string                        version_;        /**< 对方版本 */
        std::string                        channelName_;    /**< 房间频道名称 */
        int callType;
        bool isCameraOpen;
        bool timeOutHurryUp;
        bool isMasterInvited;        /**< 主叫方标记 */
        bool isUseRtcSafeMode;
    };
}

#endif  // !AVCHAT_COMPONENT_H_
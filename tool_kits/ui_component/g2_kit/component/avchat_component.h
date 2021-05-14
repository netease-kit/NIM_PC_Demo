#pragma once
//G2易用性组件
//2020.11.03
//Martin

#include "src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "nertc_sdk/nertc_engine.h"
#include "nertc_sdk/nertc_engine_ex.h"
#include "nertc_sdk/nertc_audio_device_manager.h"
#include "nertc_sdk/nertc_video_device_manager.h"
#include "ui_kit/util/net_call_helper.h"

namespace nim_comp
{
#define RTC_COMPONENT_VER "1.2.0"

	/*与 NIMSignalingType 同值*/
	enum AVCHAT_CALL_TYPE {
		kAvChatAudio = 1,		/**< 音频类型 */
		kAvChatVideo = 2,		/**< 视频类型 */
		kAvChatCustom= 3,		/**< 自定义 */
	};

	enum AVCHAT_MODULE_TYPE
	{
		kAvChatP2P = 0,
		kAvChatMulti
	};
	//组件事件回调（通话过程发生状态变化时回调）
	class IAvChatComponentEventHandler
	{
		
	public:
		virtual void onInvited(const std::string& invitor, 
			std::vector<std::string> userIDs, 
			bool isFromGroup, 
			int type) 
		{
		
		};

		virtual void onUserAccept(const std::string& userId) {}
		virtual void onUserReject(const std::string& userId) {}
		virtual void onUserEnter(const std::string& userId) {}
		virtual void onUserLeave(const std::string& userId) {}
		virtual void onUserDisconnect(const std::string& userId) {}
		virtual void onUserBusy(const std::string& userId) {}
		virtual void onUserCancel(const std::string& userId) {}

		virtual void onDisconnect(int reason) {}
		virtual void onCallingTimeOut() {}
		virtual void OnVideoToAudio() {}
		virtual void onCallEnd() {}
		virtual void onError(int errCode) {}

		virtual void onOtherClientAccept() {}
		virtual void onOtherClientReject() {}
		virtual void onUserNetworkQuality(std::map<uint64_t, nertc::NERtcNetworkQualityType>) {}

		virtual void onCameraAvailable(const std::string& userId, bool available) {}
		virtual void onAudioAvailable(const std::string& userId, bool available) {}
		virtual void onLocalCameraStateChanged(bool isEnable) {}
		virtual void onAudioVolumeChanged(unsigned char value, bool isRecord) {}
	};
	//组件操作回调（返回调用组件接口的错误码）
	using AvChatComponentOptCb = std::function<void(int errCode)>;
	//获取token函数
	using GetTokenServiceFunc = std::function<void(int64_t uid, std::function<void(const std::string& token)> onGetToken)>;

	class AvChatComponent:public nbase::SupportWeakCallback, public nertc::IRtcEngineEventHandlerEx, public nertc::IRtcMediaStatsObserver
	{
		enum ComponentStatus {
			//主动方状态，[1,99]为主动方状态专有(P后缀,positive)，[101, ~]为被动方专有状态（N后缀,negative）
			idle = 0,
			calling,
			called,
			inCall,
		};
	public:
		AvChatComponent();
		~AvChatComponent();
		void release();
		//useRtcSafeMode默认使用安全模式
		void setupAppKey(const std::string& key, bool useRtcSafeMode = true);
		void login(const std::string& account, const std::string& token, AvChatComponentOptCb cb);
		void logout(AvChatComponentOptCb cb);
		void setupLocalView(nertc::NERtcVideoCanvas* canvas);
		void setupRemoteView(nertc::NERtcVideoCanvas* canvas, const std::string& userId);
		void switchCamera();
		void enableLocalVideo(bool enable);
		void muteLocalAudio(bool mute);
		void enableAudioPlayout(bool enable);
		void call(const std::string& userId, AVCHAT_CALL_TYPE type, AvChatComponentOptCb cb);
		void accept(AvChatComponentOptCb cb);
		void reject(AvChatComponentOptCb cb);
		void hangup(AvChatComponentOptCb cb);
		void cancel(AvChatComponentOptCb cb);
		void leave(AvChatComponentOptCb cb);
		void regEventHandler(std::shared_ptr<IAvChatComponentEventHandler> compEventHandler);
		
		void startVideoPreview(bool start = true);
		void switchCallType(std::string user_id, int call_type);
		void startAudioDeviceLoopbackTest(int interval);
		void stopAudioDeviceLoopbackTest();
		void requestTokenValue(int64_t uid);
		void setVideoQuality(nertc::NERtcVideoProfileType type);
		void setAudioMute(std::string user_id, bool bMute);
		nim::SignalingCreateResParam getCreatedChannelInfo() {return createdChannelInfo_;}

		void onWaitingTimeout();
		int setRecordDeviceVolume(int value);
		int setPlayoutDeviceVolume(int value);
		uint32_t getAudioVolumn(bool isRecord);
		void getLocalDeviceList(
			std::vector<std::wstring>* recordDevicesNames,
			std::vector<std::wstring>* recordDevicesIds,
			std::vector<std::wstring>* playoutDevicesNames,
			std::vector<std::wstring>* playoutDevicesIds,
			std::vector<std::wstring>* videoDeviceNames,
			std::vector<std::wstring>* videoDeviceIds);
		void setVideoDevice(const std::wstring& id);
		std::wstring getAudioDevice(bool isRecord);
		std::wstring getVideoDevice();
		void setAudioDevice(const std::wstring& id, bool isRecord);
		nertc::IRtcEngineEx* getRtcEngine() { return rtcEngine_/*rtcEngine_.get()*/; }
		//在线上环境中，token的获取需要放到您的应用服务端完成，然后由服务器通过安全通道把token传递给客户端
		//Demo中使用的URL仅仅是demoserver，不要在您的应用中使用
		//详细请参考: http://dev.netease.im/docs?doc=server
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
		std::map<std::string, int64_t>	channelMembers_;
		nim::SignalingNotifyInfoInvite	invitedInfo_;
		nim::SignalingInviteParam		invitingInfo_;
		nim::SignalingCreateResParam	createdChannelInfo_;
		nbase::WeakCallbackFlag			calling_timeout_timer_;
		ComponentStatus					status_;
		std::string						joined_channel_id_;
		int64_t							to_account_id_;
		std::string						from_account_id_;
		std::string						stoken_;
		std::string						version_;		/**< 对方版本 */
		std::string						channelName_;	/**< 房间频道名称 */
		int callType;
		bool isCameraOpen;
		bool timeOutHurryUp;
		bool isMasterInvited;		//主叫方标记
		bool isUseRtcSafeMode;
	};

}
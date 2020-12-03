#pragma once
//G2易用性组件
//2020.11.03
//Martin

#include "src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "nertc_sdk/nertc_engine.h"
#include "nertc_sdk/nertc_engine_ex.h"
#include "nertc_sdk/nertc_audio_device_manager.h"
#include "nertc_sdk/nertc_video_device_manager.h"

namespace nim_comp
{
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
		virtual void onUserBusy(const std::string& userId) {}
		virtual void onUserCancel(const std::string& userId) {}

		virtual void onCallingTimeOut() {}
		virtual void onCallEnd() {}
		virtual void onError(int errCode) {}

		virtual void onCameraAvailable(const std::string& userId, bool available) {}
		virtual void onAudioAvailable(const std::string& userId, bool available) {}
		virtual void onLocalCameraStateChanged(bool isEnable) {}
		virtual void onAudioVolumeChanged(unsigned char value, bool isRecord) {}
	};
	//组件操作回调（返回调用组件接口的错误码）
	using AvChatComponentOptCb = std::function<void(int errCode)>;

	class AvChatComponent:public nbase::SupportWeakCallback, public nertc::IRtcEngineEventHandlerEx
	{
		enum ComponentStatus {
			//主动方状态，[1,99]为主动方状态专有(P后缀,positive)，[101, ~]为被动方专有状态（N后缀,negative）
			kAvChatComponentNone = 0,
			kAvChatComponentCalling_P,
			kAvChatComponentInviting_P,
			kAvChatComponentAccepted_P,
			kAvChatComponentRejected_P,
			kAvChatComponentEntered_P,
			kAvChatComponentLeaved_P,
			kAvChatComponentBusy_P,
			kAvChatComponentCancle_P,
			kAvChatComponentClose_P,

			kAvChatComponentInvited_N = 100,
			kAvChatComponentAccepted_N,
			kAvChatComponentRejected_N,
			kAvChatComponentEntered_N,
			kAvChatComponentClose_N,
		};
	public:
		AvChatComponent();
		~AvChatComponent();
		void release();
		void setupAppKey(const std::string& key);
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
		void startAudioDeviceLoopbackTest(int interval);
		void stopAudioDeviceLoopbackTest();
		void setVideoQuality(nertc::NERtcVideoProfileType type);
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
		nertc::IRtcEngineEx* getRtcEngine() { return rtcEngine_/*rtcEngine_.get()*/; };
	protected:
		void signalingCreateCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void signalingJoinCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb, const std::string& channelId);
		void signalingInviteCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void signalingAcceptCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void signalingRejectCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void signalingCloseCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void signalingLeaveCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb);
		void handleInvited(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		
		void handleAccepted(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void handleRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void handleJoin(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void handleLeave(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void handleClose(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void handleCancelInvite(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);
		void signalingNotifyCb(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo);

		//G2事件回调
		virtual void onJoinChannel(nertc::channel_id_t cid, nertc::uid_t uid, nertc::NERtcErrorCode result, uint64_t elapsed) override;
		virtual void onUserJoined(nertc::uid_t uid, const char* user_name) override;
		virtual void onUserLeft(nertc::uid_t uid, nertc::NERtcSessionLeaveReason reason) override;
		virtual void onUserAudioStart(nertc::uid_t uid) override;
		virtual void onUserAudioStop(nertc::uid_t uid) override;
		virtual void onUserVideoStart(nertc::uid_t uid, nertc::NERtcVideoProfileType max_profile) override;
		virtual void onUserVideoStop(nertc::uid_t uid) override;
	private:
		void startDialWaitingTimer();
		void closeChannelInternal(const std::string& channelId, AvChatComponentOptCb cb);
		void updateChannelMembers(const nim::SignalingJoinResParam* res);
		std::string getAccid(int64_t uid);
		std::string appKey_;
		nertc::IRtcEngineEx* rtcEngine_;
		//std::string currentChannelId;
		std::weak_ptr<IAvChatComponentEventHandler> compEventHandler_;
		std::string senderAccid;
		std::string toAccid;
		std::map<std::string, int64_t>	channelMembers_;
		nim::SignalingNotifyInfoInvite	invitedInfo_;
		nim::SignalingInviteParam		invitingInfo_;
		nim::SignalingCreateResParam	createdChannelInfo_;
		nbase::WeakCallbackFlag			calling_timeout_timer_;
		ComponentStatus					status_;
		std::string						joined_channel_id_;
		int callType;
		bool isCameraOpen;
	};

}
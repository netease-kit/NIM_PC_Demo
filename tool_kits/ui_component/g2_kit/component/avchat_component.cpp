#include "pch.h"
#include "avchat_component.h"
#include "nim_sdk/src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "avchat_component_def.h"
#include "base/win32/path_util.h"
#include "wrapper/avchat_business_wrapper.h"

namespace nim_comp
{
	using Signaling = nim::Signaling;
	using namespace nim;
	using namespace std::placeholders;

	const int iCallingTimeoutSeconds = 45;
	bool parseCustomInfo(const std::string& str, bool& isFromGroup, std::vector<std::string>& members, std::string& version, std::string& channelName);
	int64_t getUid(const std::list<SignalingMemberInfo>& list, const std::string& accid);
	// 0相等，1大于，-1小于
	int versionCompare(const std::string& v1, const std::string& v2);

	AvChatComponent::AvChatComponent()
	{
		isCameraOpen = true;
		timeOutHurryUp = false;
		isMasterInvited = false;
		isUseRtcSafeMode = false;
	}
	AvChatComponent::~AvChatComponent()
	{
		if (rtcEngine_)
		{
			rtcEngine_->release();
			destroyNERtcEngine((void*&)rtcEngine_);
			rtcEngine_ = nullptr;
		}
	}

	void AvChatComponent::release()
	{
		if (rtcEngine_)
		{
			QLOG_APP(L"AvChatComponent release");
			rtcEngine_->release();
			destroyNERtcEngine((void*&)rtcEngine_);
			rtcEngine_ = nullptr;
		}
	}

	void AvChatComponent::setupAppKey(const std::string& key, bool useRtcSafeMode)
	{
		appKey_ = key;
		isUseRtcSafeMode = useRtcSafeMode;
		//创建并初始化engine；
		assert(!rtcEngine_);
		//rtcEngine_.reset((nertc::IRtcEngineEx*)createNERtcEngine());
		rtcEngine_ = (nertc::IRtcEngineEx*)createNERtcEngine();
		assert(rtcEngine_);

		std::string logPath  = nbase::UTF16ToUTF8(nbase::win32::GetLocalAppDataDir().append(AVCHAT_LOG_DIR));

		nertc::NERtcEngineContext context;
		context.app_key = appKey_.c_str();
		context.event_handler = this;
		context.log_dir_path = logPath.c_str();//AVCHAT_LOG_DIR;
		context.log_level = nertc::kNERtcLogLevelDebug;
		context.log_file_max_size_KBytes = 0;
		//context.log_dir_path = ""
		int ret = rtcEngine_->initialize(context);
		assert(ret == 0);
		ret = rtcEngine_->setAudioProfile(nertc::kNERtcAudioProfileStandardExtend, nertc::kNERtcAudioScenarioSpeech);
		if (0 != ret) {
			QLOG_APP(L"setAudioProfile failed!");
		}
		Signaling::RegOnlineNotifyCb(nbase::Bind(&AvChatComponent::signalingNotifyCb, this, std::placeholders::_1));
		Signaling::RegMutilClientSyncNotifyCb(nbase::Bind(&AvChatComponent::signalingMutilClientSyncCb, this, std::placeholders::_1));
		Signaling::RegOfflineNotifyCb(nbase::Bind(&AvChatComponent::signalingOfflineNotifyCb, this, std::placeholders::_1));
	}

	int AvChatComponent::setRecordDeviceVolume(int value)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		if (audio_device_manager)
		{
			int ret = audio_device_manager->adjustRecordingSignalVolume(value);
			if (ret == 0)
			{
				compEventHandler_.lock()->onAudioVolumeChanged(value, true);
			}
			return ret;
		}
		return 1;
	}
	int AvChatComponent::setPlayoutDeviceVolume(int value)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		if (audio_device_manager)
		{
			int ret = audio_device_manager->adjustPlaybackSignalVolume(value);
			if (ret == 0)
			{
				QLOG_APP(L"setPlayoutDeviceVolume seccess");
				compEventHandler_.lock()->onAudioVolumeChanged(value, false);
			}
			return ret;
		}
		return 1;
	}
	uint32_t AvChatComponent::getAudioVolumn(bool isRecord)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		uint32_t volume = 0;
		if (audio_device_manager)
		{
			isRecord ? audio_device_manager->getRecordDeviceVolume(&volume) 
				: audio_device_manager->getPlayoutDeviceVolume(&volume);
		}
		return volume;
	}
	void AvChatComponent::getLocalDeviceList(
		std::vector<std::wstring>* recordDevicesNames, 
		std::vector<std::wstring>* recordDevicesIds, 
		std::vector<std::wstring>* playoutDevicesNames, 
		std::vector<std::wstring>* playoutDevicesIds, 
		std::vector<std::wstring>* videoDeviceNames, 
		std::vector<std::wstring>* videoDeviceIds )
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		nertc::IVideoDeviceManager* video_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		rtcEngine_->queryInterface(nertc::kNERtcIIDVideoDeviceManager, (void**)&video_device_manager);

		if (audio_device_manager)
		{
			nertc::IDeviceCollection* audio_record_collection = audio_device_manager->enumerateRecordDevices();
			for (int i=0; i<audio_record_collection->getCount(); i++)
			{
				char device_name[kNERtcMaxDeviceNameLength]{ 0 };
				char device_id[kNERtcMaxDeviceIDLength]{ 0 };
				int ret = audio_record_collection->getDevice(i, device_name, device_id);
				if (recordDevicesNames)
					recordDevicesNames->emplace_back(nbase::UTF8ToUTF16(device_name));
				if (recordDevicesIds)
					recordDevicesIds->emplace_back(nbase::UTF8ToUTF16(device_id));
			}

			nertc::IDeviceCollection* audio_playout_collection = audio_device_manager->enumeratePlayoutDevices();
			for (int i = 0; i <audio_playout_collection->getCount(); i++)
			{
				char device_name[kNERtcMaxDeviceNameLength]{ 0 };
				char device_id[kNERtcMaxDeviceIDLength]{ 0 };
				int ret = audio_playout_collection->getDevice(i, device_name, device_id);
				if (playoutDevicesNames)
					playoutDevicesNames->emplace_back(nbase::UTF8ToUTF16(device_name));
				if (playoutDevicesIds)
					playoutDevicesIds->emplace_back(nbase::UTF8ToUTF16(device_id));
			}
		}
		if (video_device_manager)
		{
			nertc::IDeviceCollection* video_collection = video_device_manager->enumerateCaptureDevices();
			for (int i = 0; i < video_collection->getCount(); i++)
			{
				char device_name[kNERtcMaxDeviceNameLength]{ 0 };
				char device_id[kNERtcMaxDeviceIDLength]{ 0 };
				int ret = video_collection->getDevice(i, device_name, device_id);
				if (videoDeviceNames)
					videoDeviceNames->emplace_back(nbase::UTF8ToUTF16(device_name));
				if (videoDeviceIds)
					videoDeviceIds->emplace_back(nbase::UTF8ToUTF16(device_id));
			}
		}
	}
	void AvChatComponent::setVideoDevice(const std::wstring& id)
	{
		nertc::IVideoDeviceManager* video_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDVideoDeviceManager, (void**)&video_device_manager);
		if (!video_device_manager)
			return;
		video_device_manager->setDevice(nbase::UTF16ToUTF8(id).c_str());
	}

	std::wstring AvChatComponent::getAudioDevice(bool isRecord)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		if (!audio_device_manager)
			return L"";

		char device_id[kNERtcMaxDeviceIDLength] = { 0 };
		isRecord ? audio_device_manager ->getRecordDevice(device_id)
			: audio_device_manager->getPlayoutDevice(device_id);

		return nbase::UTF8ToUTF16(device_id);
	}
	std::wstring AvChatComponent::getVideoDevice()
	{
		nertc::IVideoDeviceManager* video_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDVideoDeviceManager, (void**)&video_device_manager);
		if (!video_device_manager)
			return L"";

		char device_id[kNERtcMaxDeviceIDLength] = { 0 };
		video_device_manager->getDevice(device_id);

		return nbase::UTF8ToUTF16(device_id);
	}

	void AvChatComponent::setAudioDevice(const std::wstring& id, bool isRecord)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);

		if (!audio_device_manager)
			return;

		isRecord ? audio_device_manager->setRecordDevice(nbase::UTF16ToUTF8(id).c_str())
			: audio_device_manager->setPlayoutDevice(nbase::UTF16ToUTF8(id).c_str());
	}

	//登录登出使用IM SDK
	void AvChatComponent::login(const std::string& account, const std::string& token, AvChatComponentOptCb cb)
	{

	}
	void AvChatComponent::logout(AvChatComponentOptCb cb)
	{

	}

	// 呼叫方首先发送INVITE，扩展字段携带自身版本号(version)及动态channelName，即<channelId> | 0 | <uid>，并直接预加载token。
	// 0代表1v1，uid为信令房间返回的用户uid；1代表group呼叫，uid传群组teamId
	void AvChatComponent::call(const std::string& userId, AVCHAT_CALL_TYPE type, AvChatComponentOptCb cb)
	{
		channelMembers_.clear();
		version_.clear();
		channelName_.clear();
		nim::SignalingCreateParam createParam;
		createParam.channel_type_ = (nim::NIMSignalingType)type;
		toAccid = userId;
		callType = type;
		optCb_ = cb;
		senderAccid = nim::Client::GetCurrentUserAccount();
		invitedInfo_ = SignalingNotifyInfoInvite();
		createdChannelInfo_ = SignalingCreateResParam();
		isMasterInvited = true; //主叫方标记true

		//1,创建channel
		auto createCb = nbase::Bind(&AvChatComponent::signalingCreateCb, this,  _1, _2, cb);
		Signaling::SignalingCreate(createParam, createCb);
		status_ = calling;
		startDialWaitingTimer();
		rtcEngine_->stopVideoPreview();
		stopAudioDeviceLoopbackTest();
	}
	void AvChatComponent::onWaitingTimeout()
	{
		if (status_ == calling)
		{
			closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, nullptr);
			compEventHandler_.lock()->onCallingTimeOut();
		}
		handleNetCallMsg(nim_comp::kNIMNetCallStatusTimeout);
	}
	void AvChatComponent::startDialWaitingTimer()
	{
		calling_timeout_timer_.Cancel();
		StdClosure timer_cb = nbase::Bind([this]() {
			if (status_ == calling)
			{
				//closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, nullptr);
				timeOutHurryUp = true;
				compEventHandler_.lock()->onCallingTimeOut();
				handleNetCallMsg(nim_comp::kNIMNetCallStatusTimeout);
			}
			});
		timer_cb = calling_timeout_timer_.ToWeakCallback(timer_cb);
		nbase::ThreadManager::PostDelayedTask(timer_cb, nbase::TimeDelta::FromSeconds(iCallingTimeoutSeconds));
	}

	// 被叫方发送ACCEPT，并携带自己版本号(version)
	void AvChatComponent::accept(AvChatComponentOptCb cb)
	{
		//信令accept（自动join）
		SignalingAcceptParam param;
		param.account_id_ = invitedInfo_.from_account_id_;
		param.auto_join_ = true;
		param.channel_id_ = invitedInfo_.channel_info_.channel_id_;
		param.request_id_ = invitedInfo_.request_id_;
		param.offline_enabled_ = true;
		param.uid_ = 0;

		Json::Value values;
		//TODO PC暂不实现多人通话，故此处不处理channel中的其他人的信息
		//单人通话不传kAvChatChannelMembers
		Json::Reader().parse("[]", values[kAvChatChannelMembers]);
		//values[kAvChatCallType] = (int)kAvChatP2P;
		values[kAvChatCallVersion] = RTC_COMPONENT_VER;
		Json::FastWriter fw;
		param.accept_custom_info_ = fw.write(values);

		//int ret = rtcEngine_->joinChannel("", param.channel_id_.c_str(), 0);
		auto acceptCb = nbase::Bind(&AvChatComponent::signalingAcceptCb, this, _1, _2, cb);

		QLOG_APP(L"accept, version: {0}") << RTC_COMPONENT_VER;
		Signaling::Accept(param, acceptCb);
	}

	void AvChatComponent::reject(AvChatComponentOptCb cb)
	{
		//信令reject
		SignalingRejectParam param;
		param.account_id_ = invitedInfo_.from_account_id_;
		param.channel_id_ = invitedInfo_.channel_info_.channel_id_;
		param.request_id_ = invitedInfo_.request_id_;
		param.offline_enabled_ = true;

		auto rejectCb = nbase::Bind(&AvChatComponent::signalingRejectCb, this, _1, _2, cb);
		Signaling::Reject(param, rejectCb);

		invitedInfo_ = SignalingNotifyInfoInvite();
		handleNetCallMsg(nim_comp::kNIMNetCallStatusRejected);
	}
	void AvChatComponent::hangup(AvChatComponentOptCb cb)
	{
		rtcEngine_->leaveChannel();
		if (status_ == idle) 
		{
			QLOG_APP(L"The AvChatComponent status is idle, discard hangup operation");
			return;
		}
		if (status_ == calling && isMasterInvited)
		{
			SignalingCancelInviteParam param;
			param.channel_id_ = invitingInfo_.channel_id_;
			param.account_id_ = invitingInfo_.account_id_;
			param.request_id_ = invitingInfo_.request_id_;
			param.offline_enabled_ = invitingInfo_.offline_enabled_;

			Signaling::CancelInvite(param, nullptr);
		}


		if (timeOutHurryUp) { //来自超时的自动挂断,不需要发送NetCallMsg
			timeOutHurryUp = false;
		}
		else {
			handleNetCallMsg(nim_comp::kNIMNetCallStatusCanceled);
		}

		//主动方调用挂断
		if (isMasterInvited)
		{
			closeChannelInternal(invitingInfo_.channel_id_, cb);
		} //被动方调用挂断
		else
		{
			closeChannelInternal(invitedInfo_.channel_info_.channel_id_, cb);
		}
	}

	//主动方取消呼叫
	void AvChatComponent::cancel(AvChatComponentOptCb cb)
	{
		if (isMasterInvited && status_ == calling)
		{
			{
				SignalingCancelInviteParam param;
				param.channel_id_ = invitingInfo_.channel_id_;
				param.account_id_ = invitingInfo_.account_id_;
				param.request_id_ = invitingInfo_.request_id_;
				param.offline_enabled_ = invitingInfo_.offline_enabled_;

				Signaling::CancelInvite(param, nullptr);
			}
			rtcEngine_->leaveChannel();
			closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, cb);
		}
		else
		{
			QLOG_ERR(L"cancel error");
			if (cb)
				cb(0);
		}
	}
	void AvChatComponent::leave(AvChatComponentOptCb cb)
	{
		rtcEngine_->leaveChannel();
		if (!joined_channel_id_.empty())
		{
			nim::SignalingLeaveParam param;
			param.channel_id_ = joined_channel_id_;
			param.offline_enabled_ = true;
			Signaling::Leave(param, nbase::Bind(&AvChatComponent::signalingLeaveCb, this, _1, _2, cb));
		}
		else
		{
			QLOG_ERR(L"leave error: no joined channel exist.");
			if (cb)
				cb(0);
		}
	}
	void AvChatComponent::setupLocalView(nertc::NERtcVideoCanvas* canvas)
	{
		assert(rtcEngine_ && canvas);
		int ret = rtcEngine_->setupLocalVideoCanvas(canvas);
		QLOG_APP(L"setupLocalView ret: {0}") << ret;
	}
	void AvChatComponent::setupRemoteView(nertc::NERtcVideoCanvas* canvas, const std::string& userId)
	{
		assert(rtcEngine_);
		int64_t uid = channelMembers_[userId];
		int ret = rtcEngine_->setupRemoteVideoCanvas(uid, canvas);
		QLOG_APP(L"setupLocalView ret: {0}") << ret;
		//ret = rtcEngine_->subscribeRemoteVideoStream(uid, nertc::kNERtcRemoteVideoStreamTypeHigh, true);

		QLOG_APP(L"subscribeRemoteVideoStream ret: {0}") << ret;
	}

	void AvChatComponent::switchCamera()
	{
		//PC暂不实现摄像头切换
		assert(false, "swtich camera is not supported on PC");
	}
	void AvChatComponent::enableLocalVideo(bool enable)
	{
		assert(rtcEngine_);
		int ret = rtcEngine_->enableLocalVideo(enable);
		QLOG_APP(L"enableLocalVideo ret: {0}") << ret;
	}
	//音频输入设备静音
	void AvChatComponent::muteLocalAudio(bool mute)
	{
		assert(rtcEngine_); 
		if (mute)
		{
			QLOG_APP(L"muteLocalAudio ");
		}
		int ret = rtcEngine_->enableLocalAudio(!mute);
		QLOG_APP(L"enableLocalAudio ret: {0}") << ret;
	}
	void AvChatComponent::enableAudioPlayout(bool enable)
	{
		rtcEngine_->muteLocalAudioStream(!enable);
	}

	void AvChatComponent::regEventHandler(std::shared_ptr<IAvChatComponentEventHandler> compEventHandler)
	{
		compEventHandler_.reset();
		compEventHandler_ = compEventHandler;
	}

	void AvChatComponent::startVideoPreview(bool start/* = true*/)
	{
		if (start)
			rtcEngine_->enableLocalVideo(true);
		start ? rtcEngine_->startVideoPreview() : rtcEngine_->stopVideoPreview();
	}

	void AvChatComponent::switchCallType(std::string user_id, int call_type)
	{
		if (rtcEngine_ != nullptr)
		{
			int64_t uid = channelMembers_[user_id];
			int ret = rtcEngine_->subscribeRemoteVideoStream(uid, nertc::kNERtcRemoteVideoStreamTypeHigh, false);
			//ret = rtcEngine_->subscribeRemoteAudioStream(uid, true);

			QLOG_APP(L"subscribeRemoteVideoStream ret: {0}") << ret;

			ret = rtcEngine_->muteLocalVideoStream(true);
			//ret = rtcEngine_->subscribeRemoteAudioStream(uid, true);

			QLOG_APP(L"enableVideoToAudio ret: {0}") << ret;

			Json::Value values;
			values["cid"] = 2; //cid = 2表示控制信令，表示触发被叫方视频转音频
			values["type"] = kAvChatAudio; ///***音频频道* /AUDIO(1), 视频频道VIDEO(2) */
			nim::SignalingControlParam controlParam;
			controlParam.channel_id_ = joined_channel_id_.empty()? AvChatBusinessWrapper::getChannelId(): joined_channel_id_;
			controlParam.account_id_ = user_id;
			controlParam.custom_info_ = values.toStyledString();

			auto controlCb = nbase::Bind(&AvChatComponent::signalingControlCb,
				this,
				std::placeholders::_1,
				std::placeholders::_2);
			//控制信令
			Signaling::Control(controlParam, controlCb);

		}
	}

	void AvChatComponent::startAudioDeviceLoopbackTest(int interval)
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);

		if (audio_device_manager)
		{
			audio_device_manager->startAudioDeviceLoopbackTest(interval);
		}
	}

	void AvChatComponent::stopAudioDeviceLoopbackTest()
	{
		nertc::IAudioDeviceManager* audio_device_manager = nullptr;
		rtcEngine_->queryInterface(nertc::kNERtcIIDAudioDeviceManager, (void**)&audio_device_manager);
		if (audio_device_manager)
		{
			audio_device_manager->stopAudioDeviceLoopbackTest();
		}
	}

	void AvChatComponent::requestTokenValue(int64_t uid)
	{
		stoken_ = "xyz";
		if (isUseRtcSafeMode) {
			//int64_t uid;
			//uid = channelMembers_[senderAccid];
			getTokenService_(uid, [=](const std::string token) {
				stoken_ = token;
			});
		}
	}

	void AvChatComponent::setVideoQuality(nertc::NERtcVideoProfileType type)
	{
		nertc::NERtcVideoConfig config;
		config.max_profile = type;
		config.framerate = nertc::kNERtcVideoFramerateFps_15;
		config.crop_mode_ = nertc::kNERtcVideoCropModeDefault;
		rtcEngine_->setVideoConfig(config);
	}

	void AvChatComponent::setAudioMute(std::string user_id, bool bMute) {
		if (rtcEngine_) {
			int64_t uid = channelMembers_[user_id];
			rtcEngine_->subscribeRemoteAudioStream(uid, bMute);
		}
	}

	void AvChatComponent::closeChannelInternal(const std::string& channelId, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"closeChannelInternal， channelId: {0}")<< channelId;
		SignalingCloseParam param;
		param.channel_id_ = channelId;
		param.offline_enabled_ = true;
		auto closeCb = nbase::Bind(&AvChatComponent::signalingCloseCb, this, _1, _2, cb);
		Signaling::SignalingClose(param, closeCb);
		isMasterInvited = false;
	}
	void AvChatComponent::signalingInviteCb(int errCode, std::shared_ptr<SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		//4,invite完毕后，call过程结束，调用cb返回结果
		QLOG_APP(L"signalingInviteCb error: errCode:{0}") << errCode;
		if (errCode != 200)
		{
			//closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, cb);
		}
		status_ = calling;
		
		if (cb) 
			cb(errCode);
	}
	void AvChatComponent::signalingAcceptCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		SignalingAcceptResParam* res = (SignalingAcceptResParam*)res_param.get();
		QLOG_APP(L"signalingAcceptCb, errCOde: {0}") << errCode;

		if (errCode == 200)
		{
			updateChannelMembers(res);
			auto uid = getUid(res->info_.members_, invitedInfo_.to_account_id_);
			//int ret = rtcEngine_->joinChannel("", res->info_.channel_info_.channel_id_.c_str(), uid);
			
			to_account_id_ = uid;
			joined_channel_id_ = res->info_.channel_info_.channel_id_;
			status_ = inCall;
			if (versionCompare(version_, "1.1.0") < 0)
			{
				requestTokenValue(uid);
			}
			else {
				requestTokenValue(uid);
				while ("xyz" == stoken_)
				{
					std::this_thread::yield();
				}
				int type = res->info_.channel_info_.channel_type_;
				if (type == kAvChatAudio) {
					enableLocalVideo(false);
				}
				else if (type == kAvChatVideo) {
					enableLocalVideo(true);
				}
				std::string strToken = "";
				if (isUseRtcSafeMode) strToken = stoken_;
				QLOG_APP(L"handleControl: strToken = {0}") << strToken;
				int ret = rtcEngine_->joinChannel(strToken.c_str(), channelName_.c_str(), uid);
				if (ret != 0)
				{
					QLOG_ERR(L"nertc join channel failed: {0}") << ret;
					if (cb)
						cb(errCode);
					return;
				}
			}
		}

		if (cb)
			cb(errCode);
	}

	void AvChatComponent::signalingRejectCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingAcceptCb, errCOde: {0}") << errCode;
		if (cb) 
			cb(errCode);

		status_ = idle;
	}
	void AvChatComponent::signalingCloseCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingCloseCb: errCode: {0}") << errCode;
		if (errCode == 200)
		{
			createdChannelInfo_ = SignalingCreateResParam();
			joined_channel_id_.clear();
		}
		else
		{
			QLOG_ERR(L"signalingCloseCb: errCode: {0}") << errCode;
		}
		status_ = idle;
		if (cb)
			cb(errCode);
	}
	void AvChatComponent::signalingLeaveCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingLeaveCb errCode: {0}") << errCode;
		if (errCode == 200)
		{
			joined_channel_id_.clear();
			status_ = idle;
		}
		if (cb)
			cb(errCode);
	}
	void AvChatComponent::updateChannelMembers(const SignalingJoinResParam* res)
	{
		for (auto memInfo : res->info_.members_)
		{
			channelMembers_[memInfo.account_id_] = memInfo.uid_;
		}
	}
	std::string AvChatComponent::getAccid(int64_t uid)
	{
		for (auto it : channelMembers_)
		{
			if (it.second == uid)
				return it.first;
		}
		QLOG_ERR(L"Get accid failed, uid: {0}") << uid;
		return "";
	}
	void AvChatComponent::signalingJoinCb(int errCode, std::shared_ptr<SignalingResParam> res_param, AvChatComponentOptCb cb, const std::string& channelId)
	{
		if (errCode != 200)
		{
			QLOG_ERR(L"SignalingOptCallback error: errCode:{0}") << errCode;
			closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, nullptr);
			if (cb) cb(errCode);
			return;
		}
		updateChannelMembers((SignalingJoinResParam*)res_param.get());

		int64_t uid;
		uid = channelMembers_[senderAccid];
		requestTokenValue(uid);
		Json::Value values;
		//TODO PC暂不实现多人通话，故此处不处理channel中的其他人的信息
		//单人通话不传kAvChatChannelMembers
		Json::Reader().parse("[]", values[kAvChatChannelMembers]);
		values[kAvChatCallType] = (int)kAvChatP2P;
		values[kAvChatCallVersion] = RTC_COMPONENT_VER;
		channelName_ = std::string(channelId).append("|").append("0").append("|").append(std::to_string(uid));
		values[kAvChatCallChannelName] = channelName_;
		Json::FastWriter fw;
		SignalingInviteParam inviteParam;
		inviteParam.account_id_ = toAccid;
		inviteParam.channel_id_ = channelId;
		inviteParam.request_id_ = channelId;
		inviteParam.custom_info_ = fw.write(values);

		auto inviteCb = nbase::Bind(&AvChatComponent::signalingInviteCb, this, _1, _2, cb);
		QLOG_APP(L"Signaling::Invite, callType: {0}, version: {1}, channelName: {2}") << (int)kAvChatP2P << RTC_COMPONENT_VER << channelName_;
		//3,信令 invite
		Signaling::Invite(inviteParam, inviteCb);
		status_ = calling;
		invitingInfo_ = inviteParam;
	}

	void AvChatComponent::signalingCreateCb(int errCode, std::shared_ptr<SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		if (errCode != 200)
		{
			QLOG_ERR(L"SignalingOptCallback error: errCode:{0}") << errCode;
			if (cb) cb(errCode);
			return;
		}
		assert(res_param);

		SignalingCreateResParam* res = (SignalingCreateResParam*)res_param.get();
		createdChannelInfo_ = *res;

		nim::SignalingJoinParam joinParam;
		joinParam.channel_id_ = res->channel_info_.channel_id_;
		joinParam.uid_ = 0;//0，服务器会自动分配
		joinParam.offline_enabled_ = true;

		auto joinCb = nbase::Bind(&AvChatComponent::signalingJoinCb,
							this,
							std::placeholders::_1, 
							std::placeholders::_2,
							cb,
							res->channel_info_.channel_id_);
		//2.信令Join
		Signaling::Join(joinParam, joinCb);
	}

	void AvChatComponent::signalingControlCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param)
	{
		if (errCode != 200)
		{
			QLOG_ERR(L"SignalingOptCallback error: errCode:{0}") << errCode;
			if (optCb_) optCb_(errCode);
			return;
		}
	}

	void AvChatComponent::handleControl(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		Json::Value values;
		Json::Reader reader;
		std::string info = notifyInfo->custom_info_;
		int type = notifyInfo->channel_info_.channel_type_;
		if (!reader.parse(info, values) || !values.isObject())
		{
			QLOG_ERR(L"parse custom info failed: {0}");
			return;
		}

		if (values["cid"].isInt()) {
			int value = values["cid"].asInt();
			//std::string sValue = values["cid"].asString();
			if (value == 1) {
				if (notifyInfo) {
					while ("xyz" == stoken_)
					{
						std::this_thread::yield();
					}
					if (type == kAvChatAudio) {
						enableLocalVideo(false);
					}
					else if (type == kAvChatVideo) {
						enableLocalVideo(true);
					}
					std::string strToken = "";
					if (isUseRtcSafeMode) strToken = stoken_;
					QLOG_APP(L"handleControl: strToken: {0}") << strToken;
					int ret = rtcEngine_->joinChannel(strToken.c_str(), notifyInfo->channel_info_.channel_id_.c_str(), to_account_id_);
					if (ret != 0)
					{
						QLOG_ERR(L"nertc join channel failed: {0}") << ret;
						//if (cb) cb(ret);
						return;
					}
				}
			}
			if (value == 2) {
				compEventHandler_.lock()->OnVideoToAudio(); //被叫方切换成音频模式
			}
		}
	}

	// 被叫收到INVITE，先获取token，并判断版本号。如果是老版本发起呼叫，并等待控制信令，否则直接加入channelName，而不是原来的channelId。
	void AvChatComponent::handleInvited(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		//抛出onInvite事件，更新UI、响应用户操作
		if (compEventHandler_.expired())
			return;

		bool isFromGroup = false;
		std::vector<std::string> members;
		version_.clear();
		channelName_.clear();
		if (!parseCustomInfo(notifyInfo->custom_info_, isFromGroup, members, version_, channelName_))
		{
			assert(false);
			return;
		}
		QLOG_APP(L"handleInvited, from_account_id: {0}, version: {1}, channelName: {2}") << notifyInfo->from_account_id_ << version_ << channelName_;
		SignalingNotifyInfoInvite* inviteInfo = (SignalingNotifyInfoInvite*)notifyInfo.get();

		//忙线处理
		if (status_ != idle || isFromGroup)
		{
			//信令reject
			SignalingRejectParam param;
			param.account_id_ = inviteInfo->from_account_id_;
			param.channel_id_ = inviteInfo->channel_info_.channel_id_;
			param.request_id_ = inviteInfo->request_id_;
			param.custom_info_ = "601";
			param.offline_enabled_ = true;

			Signaling::Reject(param, [isFromGroup](int errCode, std::shared_ptr<nim::SignalingResParam> res_param) {
				if (!isFromGroup) {
					QLOG_APP(L"handle busy, Signaling::Reject return: {0}") << errCode;
				}
				else {
					QLOG_APP(L"handle isFromGroup is true, Signaling::Reject return: {0}") << errCode;
				}
				});
			//忙线方(被叫方)发送话单
			SendNetCallMsg(inviteInfo->from_account_id_,
				param.channel_id_,
				inviteInfo->channel_info_.channel_type_,
				isFromGroup ? (int)nim_comp::kNIMNetCallStatusRejected : (int)nim_comp::kNIMNetCallStatusBusy,
				std::vector<std::string>{inviteInfo->from_account_id_, nim::Client::GetCurrentUserAccount()},
				std::vector<int>{0, 0}
			);
			return;
		}

		//将收到邀请后的频道信息拷贝到内部，供accept使用
		invitedInfo_ = *inviteInfo;
		status_ = called;
		int type = inviteInfo->channel_info_.channel_type_;
		compEventHandler_.lock()->onInvited(notifyInfo->from_account_id_,
			members,
			isFromGroup,
			AVCHAT_CALL_TYPE(type)
		);
	}

	void AvChatComponent::handleOtherClientAccepted(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		//被叫方的通话邀请在其他端被接听，通知上层关闭界面、不关channel
		compEventHandler_.lock()->onOtherClientAccept();
		status_ = idle;
	}

	// 呼叫方收到accept之后，发起方自身 RTC Join。成功后服务端会开始进行通话计时
	// 呼叫方收到ACCEPT，进入channelName而不是原来的channelId，加入成功后判断被叫方版本。如果是老版本则发送CONTROL
	void AvChatComponent::handleAccepted(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoAccept* acceptedInfo = (SignalingNotifyInfoAccept*)notifyInfo.get();
		//SignalingNotifyInfoAccept tempacceptedInfo = *acceptedInfo;
		QLOG_APP(L"handleAccepted, from_account_id_: {0}, senderAccid: {1}") << acceptedInfo->from_account_id_ << senderAccid;
		if (acceptedInfo->to_account_id_ != senderAccid)
			return;
	
		bool isFromGroup = false;
		std::vector<std::string> members;
		version_.clear();
		if (!acceptedInfo->custom_info_.empty())
		{
			std::string channelNameTmp;
			if (!parseCustomInfo(acceptedInfo->custom_info_, isFromGroup, members, version_, channelNameTmp))
			{
				assert(false);
				return;
			}
		}
		QLOG_APP(L"handleAccepted, version_: {0}") << version_;

		requestTokenValue(channelMembers_[senderAccid]);
		while ("xyz" == stoken_)
		{
			std::this_thread::yield();
		}

		if (callType == kAvChatAudio) {
			enableLocalVideo(false);
		}
		else if (callType == kAvChatVideo) {
			enableLocalVideo(true);
		}
		//auto selfUid = nim::Client::GetCurrentUserAccount();
		std::string strToken = "";
		if (isUseRtcSafeMode) strToken = stoken_;
		QLOG_APP(L"handleAccepted: strToken: {0}") << strToken;
		int ret = rtcEngine_->joinChannel(strToken.c_str(), versionCompare(version_, "1.1.0") >= 0 ? channelName_.c_str() : acceptedInfo->channel_info_.channel_id_.c_str(), channelMembers_[senderAccid]);

		//rtcEngine_->enableLocalAudio(true);
		//rtcEngine_->(true);
		status_ = inCall;
		compEventHandler_.lock()->onUserAccept(acceptedInfo->from_account_id_);
		from_account_id_ = acceptedInfo->from_account_id_;

		if (ret != 0)
		{
			QLOG_ERR(L"nertc join channel failed: {0}") << ret;
			//if (cb) cb(ret);
			return;
		}
	}
	
	//处理异常情况下的话单
	void AvChatComponent::handleNetCallMsg(nim_comp::NIMNetCallStatus why)
	{
		if (status_ == inCall) return; //对于已经建立连接通话之后的挂断，不需要发送话单，由服务器发送
		if (isMasterInvited) {
			std::string channel_id = AvChatBusinessWrapper::getChannelId();
			std::string session_id = toAccid;
			bool is_video_mode_ = callType == AVCHAT_CALL_TYPE::kAvChatVideo ? true : false;
			SendNetCallMsg(session_id,
				channel_id,
				is_video_mode_ ? 2 : 1,
				(int)why,
				std::vector<std::string>{session_id, nim::Client::GetCurrentUserAccount()},
				std::vector<int>{0, 0}
			);
			
		}
	}

	void AvChatComponent::handleOtherClientRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		//主叫方的通话邀请在其他端被拒接，通知上层关闭界面、不关channel
		compEventHandler_.lock()->onOtherClientReject();
		status_ = idle;
	}

	void AvChatComponent::handleRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoReject* rejectedInfo = (SignalingNotifyInfoReject*)notifyInfo.get();
		QLOG_APP(L"handleRejected, from_account_id_: {0}") << rejectedInfo->from_account_id_;
		if (rejectedInfo->to_account_id_ != senderAccid)
			return;

		status_ = idle;
		//被叫方忙线会自动reject、塞入601作为标记，并发送忙线话单，主叫方无需发送话单，也无需调用hangup（hangup时会发送话单）
		if (rejectedInfo->custom_info_ == "601")
		{
			//上层处理onUserBusy事件时
			compEventHandler_.lock()->onUserBusy(rejectedInfo->from_account_id_);
			closeChannelInternal(rejectedInfo->channel_info_.channel_id_, nullptr);
		}
		else
			compEventHandler_.lock()->onUserReject(rejectedInfo->from_account_id_);
	}

	//频道中有成员加入，用户维护成员列表
	void AvChatComponent::handleJoin(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoJoin* joinInfo = (SignalingNotifyInfoJoin*)notifyInfo.get();
		QLOG_APP(L"handleJoin: accid: {0}, uid: {1}") << joinInfo->member_.account_id_ << joinInfo->member_.uid_;
		channelMembers_[joinInfo->member_.account_id_] = joinInfo->member_.uid_;

		//有自身以外的人加入频道
		if (joinInfo->member_.account_id_ != senderAccid)
		{
			//status_ = calling;
			QLOG_APP(L"handleJoin, onUserEnter, userID: {0}") << joinInfo->member_.account_id_;
			compEventHandler_.lock()->onUserEnter(joinInfo->member_.account_id_);

			if (isMasterInvited && versionCompare(version_, "1.1.0") < 0) {
				Json::Value values;
				values["cid"] = 1;  //cid = 1表示控制信令，调整call流程，修复话单主叫被叫方顺序不对bug
				values["type"] = 0;
				nim::SignalingControlParam controlParam;
				controlParam.channel_id_ = joinInfo->channel_info_.channel_id_;
				controlParam.account_id_ = joinInfo->member_.account_id_;
				controlParam.custom_info_ = values.toStyledString();

				auto controlCb = nbase::Bind(&AvChatComponent::signalingControlCb,
					this,
					std::placeholders::_1,
					std::placeholders::_2);
				//发送控制信令，告知对方进行rtcJoin
				Signaling::Control(controlParam, controlCb);
			}
		}
	}

	void AvChatComponent::handleLeave(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoLeave* leaveInfo = (SignalingNotifyInfoLeave*)notifyInfo.get(); 
		QLOG_APP(L"handleLeave, from_account_id_: {0}, senderAccid: {1}") << leaveInfo->from_account_id_ << senderAccid;

		compEventHandler_.lock()->onUserLeave(leaveInfo->from_account_id_);
	}
	void AvChatComponent::handleClose(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoClose* closeInfo = (SignalingNotifyInfoClose*)notifyInfo.get();
		QLOG_APP(L"handleClose, from_account_id_: {0}, senderAccid: {1}") << closeInfo->from_account_id_ << senderAccid;
		
		status_ = idle;
		compEventHandler_.lock()->onCallEnd();
		
	}
	void AvChatComponent::handleCancelInvite(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoCancelInvite* cancelInfo = (SignalingNotifyInfoCancelInvite*)notifyInfo.get();
		
		compEventHandler_.lock()->onUserCancel(cancelInfo->from_account_id_);
		status_ = idle;
	}
	void AvChatComponent::signalingNotifyCb(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		switch (notifyInfo->event_type_)
		{
		case nim::kNIMSignalingEventTypeInvite:
			handleInvited(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeAccept:
			handleAccepted(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeJoin:
			handleJoin(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeReject:
			handleRejected(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeCancelInvite:
			handleCancelInvite(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeLeave:
			handleLeave(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeClose:
			handleClose(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeCtrl:
			handleControl(notifyInfo);
			break;
		default:
			break;
		}
	}

	void AvChatComponent::signalingMutilClientSyncCb(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		QLOG_APP(L"MutilClientSyncCb");
		switch (notifyInfo->event_type_)
		{
		case nim::kNIMSignalingEventTypeAccept:
			handleOtherClientAccepted(notifyInfo);
			break;
		case nim::kNIMSignalingEventTypeReject:
			handleOtherClientRejected(notifyInfo);
			break;
		default:
			QLOG_APP(L"MutilClientSyncCb event_type: {0}")<< notifyInfo->event_type_;
			break;
		}
	}
	void AvChatComponent::signalingOfflineNotifyCb(std::list<std::shared_ptr<nim::SignalingNotifyInfo>> notifyInfo)
	{
		std::list<std::shared_ptr<SignalingNotifyInfo>> notifyInfoList = notifyInfo;
		std::shared_ptr <nim::SignalingNotifyInfo> inviteInfo = nullptr;
		std::set<std::string> cancelSet;
		for (std::shared_ptr<SignalingNotifyInfo> info : notifyInfoList) {
			if (info != nullptr) {
				if (info->event_type_ == kNIMSignalingEventTypeInvite) {
					if (!inviteInfo || info->timestamp_ > inviteInfo->timestamp_) {
						inviteInfo = info;
					}
				}
				else if (info->event_type_ == kNIMSignalingEventTypeCancelInvite) {
					nim::SignalingNotifyInfoCancelInvite* cancelInfo = (nim::SignalingNotifyInfoCancelInvite*)(&(*info));
					cancelSet.insert(cancelInfo->request_id_);
				}
			}
		
		}
	
		nim::SignalingNotifyInfoInvite* lastInviteInfo = (nim::SignalingNotifyInfoInvite*)(&(*inviteInfo));
		if (lastInviteInfo != nullptr) {
			int nCount = cancelSet.count(lastInviteInfo->request_id_);
			//BOOL isValid = (inviteInfo != nullptr) && !inviteInfo->channel_info_.invalid_ && (cancelSet.find(lastInviteInfo->request_id_) == cancelSet.end());
			BOOL isValid = (inviteInfo != nullptr) && (cancelSet.find(lastInviteInfo->request_id_) == cancelSet.end());
			if (isValid) {
				handleInvited(inviteInfo);
			}
		}
	}
	///////////////////////////////G2事件///////////////////////////////
	void AvChatComponent::onJoinChannel(nertc::channel_id_t cid, nertc::uid_t uid, nertc::NERtcErrorCode result, uint64_t elapsed) {
		QLOG_APP(L"onJoinChannel");
		//rtcEngine_->enableLocalAudio(true);
	}
	void AvChatComponent::onUserJoined(nertc::uid_t uid, const char* user_name)
	{
		QLOG_APP(L"onUserJoined");
		int ret = rtcEngine_->subscribeRemoteVideoStream(uid, nertc::kNERtcRemoteVideoStreamTypeHigh, true);
		//ret = rtcEngine_->subscribeRemoteAudioStream(uid, true);

		QLOG_APP(L"subscribeRemoteVideoStream ret: {0}") << ret;
		
		//对方rtc Join之后 订阅视频流
	}
	void AvChatComponent::onUserLeft(nertc::uid_t uid, nertc::NERtcSessionLeaveReason reason)
	{
		if (reason != 4) {
			compEventHandler_.lock()->onUserLeave(getAccid(uid));
		}
		else {
			compEventHandler_.lock()->onUserDisconnect(getAccid(uid));
		}
	}
	void AvChatComponent::onUserAudioStart(nertc::uid_t uid)
	{
		compEventHandler_.lock()->onAudioAvailable(getAccid(uid), true);
	}
	void AvChatComponent::onUserAudioStop(nertc::uid_t uid)
	{
		compEventHandler_.lock()->onAudioAvailable(getAccid(uid), false);
	}

	void AvChatComponent::onUserVideoStart(nertc::uid_t uid, nertc::NERtcVideoProfileType max_profile)
	{
		int ret = rtcEngine_->subscribeRemoteVideoStream(uid, nertc::kNERtcRemoteVideoStreamTypeHigh, true);
		compEventHandler_.lock()->onCameraAvailable(getAccid(uid), true);
	}
	void AvChatComponent::onUserVideoStop(nertc::uid_t uid)
	{
		compEventHandler_.lock()->onCameraAvailable(getAccid(uid), false);
	}
	void AvChatComponent::onDisconnect(nertc::NERtcErrorCode reason) {
		compEventHandler_.lock()->onDisconnect(reason);
	}

	void AvChatComponent::onNetworkQuality(const nertc::NERtcNetworkQualityInfo *infos, unsigned int user_count)
	{
		std::map<uint64_t, nertc::NERtcNetworkQualityType> network_quality;
		for (int i = 0; i < user_count; i++)
		{
			network_quality[infos[i].uid] = infos[i].tx_quality;
		}
		compEventHandler_.lock()->onUserNetworkQuality(network_quality);
	}
	///////////////////////////////////////////内部方法////////////////////////////////////////
	//Signaling::SignalingNotifyCallback 
	int64_t getUid(const std::list<SignalingMemberInfo>& list, const std::string& accid)
	{
		for (auto it : list)
		{
			if (it.account_id_ == accid)
				return it.uid_;
		}
		assert(false);
		return 0;
	}

	bool parseCustomInfo(const std::string& str, bool& isFromGroup, std::vector<std::string>& members, std::string& version, std::string& channelName)
	{
		version.clear();
		channelName.clear();

		Json::Value values;
		Json::Reader reader;
		if (!reader.parse(str, values) || !values.isObject())
		{
			QLOG_ERR(L"parse custom info failed: {0}") << str;
			return false;
		}
		
		isFromGroup = ((AVCHAT_MODULE_TYPE)values[kAvChatCallType].asBool() == kAvChatMulti);
		if (values[kAvChatChannelMembers].isArray())
		{
			for (int i = 0; i < values[kAvChatChannelMembers].size(); i++)
			{
				members.push_back(values[kAvChatChannelMembers][i].asString());
			}
		}

		if (values.isMember(kAvChatCallVersion))
		{
			version = values[kAvChatCallVersion].asString();
		}

		if (values.isMember(kAvChatCallChannelName))
		{
			channelName = values[kAvChatCallChannelName].asString();
		}
		
		return true;
	}

	int versionCompare(const std::string& version1, const std::string& version2)
	{
		char* p1 = (char*)version1.data();
		char* p2 = (char*)version2.data();
		int v1 = 0, v2 = 0;
		while (*p1 || *p2) {
			v1 = v2 = 0;
			if (*p1) v1 = atoi(p1);
			if (*p2) v2 = atoi(p2);
			if (v1 > v2) return 1;
			if (v1 < v2) return -1;
			while (*p1 && *p1 != '.') ++p1;
			if (*p1 == '.') ++p1;
			while (*p2 && *p2 != '.') ++p2;
			if (*p2 == '.') ++p2;
		}
		return 0;
	}
}
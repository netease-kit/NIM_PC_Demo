#include "pch.h"
#include "avchat_component.h"
#include "nim_sdk/src/cpp_sdk/nim/api/nim_cpp_signaling.h"
#include "avchat_component_def.h"
#include "base/win32/path_util.h"

namespace nim_comp
{
	using Signaling = nim::Signaling;
	using namespace nim;
	using namespace std::placeholders;

	const int iCallingTimeoutSeconds = 45;
	bool parseCustomInfo(const std::string& str, bool& isFromGroup, std::vector<std::string>& members);
	int64_t getUid(const std::list<SignalingMemberInfo>& list, const std::string& accid);

	AvChatComponent::AvChatComponent()
	{
		isCameraOpen = true;
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

	void AvChatComponent::setupAppKey(const std::string& key)
	{
		appKey_ = key;
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

		Signaling::RegOnlineNotifyCb(nbase::Bind(&AvChatComponent::signalingNotifyCb, this, std::placeholders::_1));
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

	void AvChatComponent::call(const std::string& userId, AVCHAT_CALL_TYPE type, AvChatComponentOptCb cb)
	{
		channelMembers_.clear();
		nim::SignalingCreateParam createParam;
		createParam.channel_type_ = (nim::NIMSignalingType)type;
		toAccid = userId;
		callType = type;
		senderAccid = nim::Client::GetCurrentUserAccount();
		invitedInfo_ = SignalingNotifyInfoInvite();
		createdChannelInfo_ = SignalingCreateResParam();

		//1,创建channel
		auto createCb = nbase::Bind(&AvChatComponent::signalingCreateCb, this,  _1, _2, cb);
		Signaling::SignalingCreate(createParam, createCb);
		status_ = kAvChatComponentCalling_P;
		//startDialWaitingTimer();
		rtcEngine_->stopVideoPreview();
		stopAudioDeviceLoopbackTest();
	}
	void AvChatComponent::onWaitingTimeout()
	{
		if (status_ == kAvChatComponentCalling_P || status_ == kAvChatComponentInviting_P)
		{
			closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, nullptr);
			compEventHandler_.lock()->onCallingTimeOut();
		}
	}
	void AvChatComponent::startDialWaitingTimer()
	{
		calling_timeout_timer_.Cancel();
		StdClosure timer_cb = nbase::Bind(&AvChatComponent::onWaitingTimeout, this);
		timer_cb = calling_timeout_timer_.ToWeakCallback(timer_cb);
		nbase::ThreadManager::PostDelayedTask(timer_cb, nbase::TimeDelta::FromSeconds(iCallingTimeoutSeconds));
	}
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

		//int ret = rtcEngine_->joinChannel("", param.channel_id_.c_str(), 0);
		auto acceptCb = nbase::Bind(&AvChatComponent::signalingAcceptCb, this, _1, _2, cb);
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
	}
	void AvChatComponent::hangup(AvChatComponentOptCb cb)
	{
		if (status_ == kAvChatComponentInviting_P)
		{
			SignalingCancelInviteParam param;
			param.channel_id_ = invitingInfo_.channel_id_;
			param.account_id_ = invitingInfo_.account_id_;
			param.request_id_ = invitingInfo_.request_id_;
			param.offline_enabled_ = invitingInfo_.offline_enabled_;

			Signaling::CancelInvite(param, nullptr);
		}

		rtcEngine_->leaveChannel();

		//主动方调用挂断
		if (int(status_) >= int(kAvChatComponentInviting_P) && int(status_) < int(kAvChatComponentClose_P))
		{
			closeChannelInternal(invitingInfo_.channel_id_, cb);
		} //被动方调用挂断
		else if(int(status_) >= int(kAvChatComponentAccepted_N) && int(status_) < int(kAvChatComponentClose_N))
		{
			closeChannelInternal(invitedInfo_.channel_info_.channel_id_, cb);
		}
		else 
		{
			QLOG_ERR(L"hangup error");
			if (cb) cb(0);
		}
	}

	//主动方取消呼叫
	void AvChatComponent::cancel(AvChatComponentOptCb cb)
	{
		if (int(status_) >= int(kAvChatComponentCalling_P) && int(status_) < int(kAvChatComponentClose_P))
		{
			if (status_ == kAvChatComponentInviting_P)
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

	void AvChatComponent::setVideoQuality(nertc::NERtcVideoProfileType type)
	{
		nertc::NERtcVideoConfig config;
		config.max_profile = type;
		config.crop_mode_ = nertc::kNERtcVideoCropModeDefault;
		rtcEngine_->setVideoConfig(config);
	}
	void AvChatComponent::closeChannelInternal(const std::string& channelId, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"closeChannelInternal， channelId: {0}")<< channelId;
		SignalingCloseParam param;
		param.channel_id_ = channelId;
		param.offline_enabled_ = true;
		auto closeCb = nbase::Bind(&AvChatComponent::signalingCloseCb, this, _1, _2, cb);
		Signaling::SignalingClose(param, closeCb);

	}
	void AvChatComponent::signalingInviteCb(int errCode, std::shared_ptr<SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		//4,invite完毕后，call过程结束，调用cb返回结果
		QLOG_APP(L"signalingInviteCb error: errCode:{0}") << errCode;
		if (errCode != 200)
		{
			closeChannelInternal(createdChannelInfo_.channel_info_.channel_id_, cb);
		}
		status_ = kAvChatComponentInviting_P;
		
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
			int ret = rtcEngine_->joinChannel("", res->info_.channel_info_.channel_id_.c_str(), uid);
			
			joined_channel_id_ = res->info_.channel_info_.channel_id_;
			status_ = kAvChatComponentAccepted_N;
		}

		if (cb)
			cb(errCode);
	}

	void AvChatComponent::signalingRejectCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingAcceptCb, errCOde: {0}") << errCode;
		if (cb) 
			cb(errCode);

		status_ = kAvChatComponentRejected_N;
	}
	void AvChatComponent::signalingCloseCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingCloseCb: errCode: {0}") << errCode;
		if (errCode == 200)
		{
			createdChannelInfo_ = SignalingCreateResParam();
			joined_channel_id_.clear();
			status_ = kAvChatComponentNone;
		}
		else
		{
			QLOG_ERR(L"signalingCloseCb: errCode: {0}") << errCode;
		}
		if (cb)
			cb(errCode);
	}
	void AvChatComponent::signalingLeaveCb(int errCode, std::shared_ptr<nim::SignalingResParam> res_param, AvChatComponentOptCb cb)
	{
		QLOG_APP(L"signalingLeaveCb errCode: {0}") << errCode;
		if (errCode == 200)
		{
			joined_channel_id_.clear();
			status_ = kAvChatComponentNone;
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

		Json::Value values;
		//TODO PC暂不实现多人通话，故此处不处理channel中的其他人的信息
		//单人通话不传kAvChatChannelMembers
		Json::Reader().parse("[]", values[kAvChatChannelMembers]);
		values[kAvChatCallType] = (int)kAvChatP2P;
		Json::FastWriter fw;
		SignalingInviteParam inviteParam;
		inviteParam.account_id_ = toAccid;
		inviteParam.channel_id_ = channelId;
		inviteParam.request_id_ = channelId;
		inviteParam.custom_info_ = fw.write(values);

		auto inviteCb = nbase::Bind(&AvChatComponent::signalingInviteCb, this, _1, _2, cb);
		//3,信令 invite
		Signaling::Invite(inviteParam, inviteCb);
		status_ = kAvChatComponentInviting_P;
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

	void AvChatComponent::handleInvited(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		//抛出onInvite事件，更新UI、响应用户操作
		if (compEventHandler_.expired())
			return;

		bool isFromGroup;
		std::vector<std::string> members;
		if (!parseCustomInfo(notifyInfo->custom_info_, isFromGroup, members))
		{
			assert(false);
			return;
		}

		//将收到邀请后的频道信息拷贝到内部，供accept使用
		SignalingNotifyInfoInvite* inviteInfo = (SignalingNotifyInfoInvite*)notifyInfo.get();
		invitedInfo_ = *inviteInfo;
		status_ = kAvChatComponentInvited_N;
		int type = inviteInfo->channel_info_.channel_type_;
		compEventHandler_.lock()->onInvited(notifyInfo->from_account_id_,
			members,
			isFromGroup,
			AVCHAT_CALL_TYPE(type)
		);
	}

	//4.待受邀方accept之后，发起方自身 RTC Join。成功后服务端会开始进行通话计时
	void AvChatComponent::handleAccepted(std::shared_ptr<SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoAccept* acceptedInfo = (SignalingNotifyInfoAccept*)notifyInfo.get();

		QLOG_APP(L"handleAccepted, from_account_id_: {0}, senderAccid: {1}") << acceptedInfo->from_account_id_ << senderAccid;
		if (acceptedInfo->to_account_id_ != senderAccid)
			return;

		//auto selfUid = nim::Client::GetCurrentUserAccount();
		int ret = rtcEngine_->joinChannel("", acceptedInfo->channel_info_.channel_id_.c_str()
			, channelMembers_[senderAccid]);

		//rtcEngine_->enableLocalAudio(true);
		//rtcEngine_->(true);
		status_ = kAvChatComponentAccepted_P;
		compEventHandler_.lock()->onUserAccept(acceptedInfo->from_account_id_);

		if (ret != 0)
		{
			QLOG_ERR(L"nertc join channel failed: {0}") << ret;
			//if (cb) cb(ret);
			return;
		}
	}
	
	void AvChatComponent::handleRejected(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoReject* rejectedInfo = (SignalingNotifyInfoReject*)notifyInfo.get();
		QLOG_APP(L"handleRejected, from_account_id_: {0}") << rejectedInfo->from_account_id_;
		if (rejectedInfo->to_account_id_ != senderAccid)
			return;

		status_ = kAvChatComponentRejected_P;
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
			status_ = kAvChatComponentEntered_P;
			QLOG_APP(L"handleJoin, onUserEnter, userID: {0}") << joinInfo->member_.account_id_;
			compEventHandler_.lock()->onUserEnter(joinInfo->member_.account_id_);
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
		
		status_ = kAvChatComponentClose_P;
		compEventHandler_.lock()->onCallEnd();
		
	}
	void AvChatComponent::handleCancelInvite(std::shared_ptr<nim::SignalingNotifyInfo> notifyInfo)
	{
		SignalingNotifyInfoCancelInvite* cancelInfo = (SignalingNotifyInfoCancelInvite*)notifyInfo.get();
		
		compEventHandler_.lock()->onUserCancel(cancelInfo->from_account_id_);
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
		default:
			break;
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
		compEventHandler_.lock()->onUserLeave(getAccid(uid));
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

	bool parseCustomInfo(const std::string& str, bool& isFromGroup, std::vector<std::string>& members)
	{
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
		
		return true;
	}

}
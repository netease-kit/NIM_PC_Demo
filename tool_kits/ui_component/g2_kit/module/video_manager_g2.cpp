
#include "pch.h"
#include "video_manager_g2.h"
#include "g2_kit/component/avchat_component.h"
#include "g2_kit/component/avchat_component_def.h"
#include "g2_kit/gui/video_setting_g2.h"
#include "src/cpp_sdk/nim/api/nim_cpp_client.h"

namespace nim_comp
{
	//VideoManagerG2	VideoManagerG2::g_instace_;

	std::vector<std::string> VideoManagerG2::GetSupportedActionsList()
	{
		return
		{
			kAvChatRelease,
			kAvChatSetupAppKey,
			kAvChatCall,
			kAvChatLogin,
			kAvChatLogout,
			kAvChatAccept,
			kAvChatReject,
			kAvChatHangup,
			kAvChatCancel,
			kAvChatLeave,
			kAvChatSetupLocalView,
			kAvChatSetupRemoteView,
			kAvChatEnableLocalVideo,
			kAvChatMuteLocalAudio,
			kAvChatEnableAudioPlayout,
			kAvChatSwitchCamera,

			kAvChatSetRecordVolume,
			kAvChatSetPlayoutVolume,
			kAvChatSetAudioDevice,
			kAvChatSetVideoDevice,
			kAvChatStartVideoPreview,
			kAvChatStartAudioDeviceLoopbackTest,
			kAvChatStopAudioDeviceLoopbackTest,
			kAvChatSetVideoQuality,
		};
	}
	VideoManagerG2::VideoManagerG2():init_flag_(false), is_video_enable_(true), video_quality_type_(3), is_audio_input_enable_(true),is_audio_output_enable_(true)
	{
		action_dispatcher_.reset(new VideoActionDispatcher());

		action_dispatcher_->AddActionProc(kAvChatRelease, &AvChatBusinessWrapper::release);
		action_dispatcher_->AddActionProc(kAvChatSetupAppKey, &AvChatBusinessWrapper::setupAppKey);
		action_dispatcher_->AddActionProc(kAvChatCall, &AvChatBusinessWrapper::call);
		action_dispatcher_->AddActionProc(kAvChatLogin, &AvChatBusinessWrapper::login);
		action_dispatcher_->AddActionProc(kAvChatLogout, &AvChatBusinessWrapper::logout);
		action_dispatcher_->AddActionProc(kAvChatAccept, &AvChatBusinessWrapper::accept);
		action_dispatcher_->AddActionProc(kAvChatReject, &AvChatBusinessWrapper::reject);
		action_dispatcher_->AddActionProc(kAvChatHangup, &AvChatBusinessWrapper::hangup);
		action_dispatcher_->AddActionProc(kAvChatCancel, &AvChatBusinessWrapper::cancel);
		action_dispatcher_->AddActionProc(kAvChatLeave, &AvChatBusinessWrapper::leave);
		action_dispatcher_->AddActionProc(kAvChatSetupLocalView, &AvChatBusinessWrapper::setupLocalView);
		action_dispatcher_->AddActionProc(kAvChatSetupRemoteView, &AvChatBusinessWrapper::setupRemoteView);
		action_dispatcher_->AddActionProc(kAvChatEnableLocalVideo, &AvChatBusinessWrapper::enableLocalVideo);
		action_dispatcher_->AddActionProc(kAvChatMuteLocalAudio, &AvChatBusinessWrapper::muteLocalAudio);
		action_dispatcher_->AddActionProc(kAvChatEnableAudioPlayout, &AvChatBusinessWrapper::enableAudioPlayout);
		action_dispatcher_->AddActionProc(kAvChatSwitchCamera, &AvChatBusinessWrapper::switchCamera);
		
		action_dispatcher_->AddActionProc(kAvChatSetRecordVolume, &AvChatBusinessWrapper::setRecordDeviceVolume);
		action_dispatcher_->AddActionProc(kAvChatSetPlayoutVolume, &AvChatBusinessWrapper::setPlayoutDeviceVolume);
		action_dispatcher_->AddActionProc(kAvChatSetAudioDevice, &AvChatBusinessWrapper::setAudioDevice);
		action_dispatcher_->AddActionProc(kAvChatSetVideoDevice, &AvChatBusinessWrapper::setVideoDevice);
		action_dispatcher_->AddActionProc(kAvChatStartVideoPreview, &AvChatBusinessWrapper::startVideoPreview);
		action_dispatcher_->AddActionProc(kAvChatStartAudioDeviceLoopbackTest, &AvChatBusinessWrapper::startAudioDeviceLoopbackTest);
		action_dispatcher_->AddActionProc(kAvChatStopAudioDeviceLoopbackTest, &AvChatBusinessWrapper::stopAudioDeviceLoopbackTest);
		action_dispatcher_->AddActionProc(kAvChatSetVideoQuality, &AvChatBusinessWrapper::setVideoQuality);

		nbase::BusinessManager::GetInstance()->RegActions(GetSupportedActionsList(),
			nbase::ActionDispatcher(action_dispatcher_)
		);

		SubSingleNotifyForUI(kAvChatOnCalling, &VideoManagerG2::OnCallingCb);
		SubSingleNotifyForUI(kAvChatOnInvite, &VideoManagerG2::OnInvitedCb);

		SubSingleNotifyForUI(kAvChatOnUserAccept, &VideoManagerG2::OnUserAcceptCb);
		SubSingleNotifyForUI(kAvChatOnUserReject, &VideoManagerG2::OnUserRejectCb);
		SubSingleNotifyForUI(kAvChatOnUserEnter, &VideoManagerG2::OnUserEnterCb);
		SubSingleNotifyForUI(kAvChatOnUserLeave, &VideoManagerG2::OnUserLeaveCb);
		SubSingleNotifyForUI(kAvChatOnUserBusy, &VideoManagerG2::OnUserBusyCb);
		SubSingleNotifyForUI(kAvChatOnUserCancel, &VideoManagerG2::OnUserCancelCb);

		SubSingleNotifyForUI(kAvChatOnCallingTimeOut, &VideoManagerG2::OnCallingTimeOutCb);
		SubSingleNotifyForUI(kAvChatOnCallEnd, &VideoManagerG2::OnCallEndCb);
		SubSingleNotifyForUI(kAvChatOnError, &VideoManagerG2::OnErrorCb);

		SubSingleNotifyForUI(kAvChatOnCameraAvailable, &VideoManagerG2::OnCameraAvailableCb);
		SubSingleNotifyForUI(kAvChatOnAudioAvailable, &VideoManagerG2::OnAudioAvailableCb);
		SubSingleNotifyForUI(kAvChatOnLocalCameraStateChanged, &VideoManagerG2::OnLocalCameraStateChanged);
		SubSingleNotifyForUI(kAvChatOnAudioVolumChanged, &VideoManagerG2::OnAudioVolumeChanged);

		//设置appkey，初始化nertc引擎
		std::string appkey = app_sdk::AppSDKInterface::GetAppKey();
		nbase::BatpPack bp;
		bp.head_.action_name_ = kAvChatSetupAppKey;
		bp.body_.param_ = appkey;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
	}

	void VideoManagerG2::UnInit(StdClosure cb)
	{
		nbase::BatpPack bp;
		bp.head_.action_name_ = kAvChatRelease;
		nbase::BusinessManager::GetInstance()->Request(bp, [cb](const nbase::BatpPack& response) {
			if (cb)
				cb();
		});
	}
	VideoManagerG2::~VideoManagerG2()
	{
		if (setting_form_ == NULL || !IsWindow(setting_form_flag_.expired() ? NULL : setting_form_->GetHWND()))
		{
			setting_form_.release();
		}
	}

	//客户端只处理异常情况下的话单
	void VideoManagerG2::handleNetCallMsg(nim_comp::NIMNetCallStatus why)
	{
		//return;
		std::string channel_id = AvChatBusinessWrapper::getChannelId();
		std::string session_id = video_form_->GetSessionId();
		SendNetCallMsg(session_id,
			channel_id,
			is_video_mode_ ? 2 : 1,
			(int)why,
			std::vector<std::string>{session_id, nim::Client::GetCurrentUserAccount()},
			std::vector<int>{0,0}
		);
	}

	void VideoManagerG2::SubSingleNotifyForUI(const std::string& notify_name, void(VideoManagerG2::* mem_func)(const nbase::BatpPack& response))
	{
		auto cb = nbase::Bind([this,mem_func](const nbase::BatpPack& response) {
			StdClosure closure = nbase::Bind(
				[this, mem_func, response]() {
					(this->*mem_func)(response);
				});

			Post2UI(closure);

			}, std::placeholders::_1);

		std::vector<std::string> vec;
		vec.emplace_back(notify_name);
		//auto cb = nbase::Bind(&class_name::notify_name##Cb, this, std::placeholders::_1);	
		nbase::BusinessManager::GetInstance()->SubNotify(vec, cb);
	}

	bool VideoManagerG2::ShowVideoChatForm(const std::string& session_id, bool video, bool isSender/* = true*/)
	{
		SetDeviceOnce();

		video_form_.release();
		video_form_.reset(new VideoFormG2(session_id));
		video_form_flag_ = video_form_->GetWeakFlag();
		video_form_->setVideoMode(video);
		video_form_->Create(NULL, L"", WS_OVERLAPPEDWINDOW, 0, false, ui::UiRect());
		video_form_->CenterWindow();
		video_form_->ShowWindow();

		is_video_mode_ = video;
		is_sender_ = isSender;

		if (isSender)
		{
			nbase::BatpPack bp;
			AvChatParams params;
			params.userId = session_id;// nim::Client::GetCurrentUserAccount();
			params.callType = is_video_mode_ ? kAvChatVideo : kAvChatAudio;
			bp.head_.action_name_ = kAvChatCall;
			bp.body_.param_ = params;
			nbase::BusinessManager::GetInstance()->Request(bp, nullptr);

			video_form_->ShowStatusPage(VideoFormG2::SP_DIAL);
			video_form_->SwitchStatus(VideoFormG2::STATUS_CALLING);
			video_form_->StartDialWaitingTimer();
		}
		else
		{
			video_form_->AdjustWindowSize(is_video_mode_);
			video_form_->CheckTitle();
			video_form_->ShowStatusPage(VideoFormG2::SP_CONFIRM);
			video_form_->SwitchStatus(VideoFormG2::STATUS_INVITING);
		}

		return true;
	}
	bool VideoManagerG2::IsTalking()
	{
		if (video_form_ != NULL && !video_form_flag_.expired())
			return video_form_->IsStart();
		return false;
	}
	bool VideoManagerG2::ShowVideoSetting(bool video/* = true*/)
	{
		SetDeviceOnce();

		bool ret = setting_form_flag_.expired();
		if (setting_form_ != NULL && !setting_form_flag_.expired())
		{
			setting_form_->ShowWindow(true, true);
			setting_form_->ShowPage(video);
			SetForegroundWindow(setting_form_->GetHWND());
			return false;
		}
		else
		{
			setting_form_.release();
			setting_form_.reset(new VideoSettingFormG2());
			setting_form_flag_ = setting_form_->GetWeakFlag();
			bool ret = setting_form_flag_.expired();
			setting_form_->Create(NULL, L"", WS_OVERLAPPEDWINDOW, 0);
			setting_form_->CenterWindow();
			setting_form_->ShowWindow();
			setting_form_->ShowPage(video);
		}//*/
		return true;
	}

	// capture true 标识设置麦克风音量，false 标识设置播放音量
	void VideoManagerG2::SetAudioVolumn(unsigned char volumn, bool capture)
	{
		AvChatParams params;
		params.volume = volumn;
		nbase::BatpPack bp;
		bp.head_.action_name_ = capture ? kAvChatSetRecordVolume : kAvChatSetPlayoutVolume;
		bp.body_.param_ = params;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);

// 		if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
// 		{
// 			multi_video_form_->InitVolumnSetting();
// 		}
	}
	unsigned char VideoManagerG2::GetAudioVolumn(bool capture)
	{
		return AvChatBusinessWrapper::getAudioVolumn(capture);
// 		int ret = 255;// 
// 		if (setting_form_ && !setting_form_flag_.expired())
// 		{
// 			int output = setting_form_->output_volumn_slider_->GetValue();
// 			int input = setting_form_->input_volumn_slider_->GetValue();
// 			capture ? ret = input : ret = output;
// 		}
// 		return ret;
	}
	bool VideoManagerG2::IsAudioEnable(bool isRecord) 
	{
		if (isRecord)
		{
			return is_audio_input_enable_;
		}
		//auto ret = AvChatBusinessWrapper::isAudioEnable(isRecord);
		return is_audio_output_enable_;
	}

	bool VideoManagerG2::IsVideoEnable() 
	{ 
		return is_video_enable_;
		//return AvChatBusinessWrapper::isVideoEnable();
	}

	std::vector<G2_MEDIA_DEVICE_INFO> VideoManagerG2::GetDeviceInfo(nim::NIMDeviceType type, int* curDeviceNo/* = nullptr*/)
	{
		std::vector<std::wstring> names;
		std::vector<std::wstring> ids;
		std::vector<G2_MEDIA_DEVICE_INFO> info;

		std::wstring curDeviceId;
		switch (type)
		{
		case nim::kNIMDeviceTypeAudioIn:
			AvChatBusinessWrapper::getLocalDeviceList(&names, &ids, nullptr, nullptr, nullptr, nullptr);
			curDeviceId = GetCurAudioDeviceId(true);
			break;
		case nim::kNIMDeviceTypeAudioOut:
			AvChatBusinessWrapper::getLocalDeviceList(nullptr, nullptr, &names, &ids, nullptr, nullptr);
			curDeviceId = GetCurAudioDeviceId(false);
			break;
		case nim::kNIMDeviceTypeAudioOutChat:
			AvChatBusinessWrapper::getLocalDeviceList(nullptr, nullptr, &names, &ids, nullptr, nullptr);
			curDeviceId = GetCurAudioDeviceId(false);
			break;
		case nim::kNIMDeviceTypeVideo:
			AvChatBusinessWrapper::getLocalDeviceList(nullptr, nullptr, nullptr, nullptr, &names, &ids);
			curDeviceId = GetCurVideoDeviceId();
			break;
		case nim::kNIMDeviceTypeSoundcardCapturer:
			break;
		case nim::kNIMDeviceTypeAudioHook:
			break;
		default:
			break;
		}

		for (int i = 0; i < names.size(); i++)
		{
			info.emplace_back(G2_MEDIA_DEVICE_INFO{ names[i], ids[i] });
			if (curDeviceNo && !curDeviceId.empty() && curDeviceId == ids[i])
				*curDeviceNo = i;
		}	

		return info;
	}

	void VideoManagerG2::EnableVideoDevice(bool enable)
	{
		AvChatParams param;
		nbase::BatpPack bp;

		bp.head_.action_name_ = kAvChatEnableLocalVideo;
		param.cameraAvailable = enable;
		bp.body_.param_ = param;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
		is_video_enable_ = enable;
	}
	void VideoManagerG2::EnableAudioDevice(bool enable, bool isRecord)
	{
		AvChatParams param;
		nbase::BatpPack bp;

		if (isRecord)
		{
			bp.head_.action_name_ = kAvChatMuteLocalAudio;
			param.muteAudio = !enable;
			bp.body_.param_ = param;
			nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
			is_audio_input_enable_ = enable;
		}
		else
		{
			bp.head_.action_name_ = kAvChatEnableAudioPlayout;
			param.audioAvailable = enable;
			bp.body_.param_ = param;
			nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
			is_audio_output_enable_ = enable;
		}
	}

	/**
	 * 通话之前统一设置一次deviceId、enbale一次设备
	 */
	void VideoManagerG2::SetDeviceOnce()
	{
		static std::atomic_bool exec = false;
		if (exec)
			return;

		std::vector<std::wstring> ids;
		AvChatBusinessWrapper::getLocalDeviceList(nullptr, &ids, nullptr, nullptr, nullptr, nullptr);
		if (ids.size() > 0)
			SetAudioDevice(ids[0], true);
		ids.clear();

		AvChatBusinessWrapper::getLocalDeviceList(nullptr, nullptr, nullptr, &ids, nullptr, nullptr);
		if (ids.size() > 0)
			SetAudioDevice(ids[0], false);
		ids.clear();

		AvChatBusinessWrapper::getLocalDeviceList(nullptr, nullptr, nullptr, nullptr, nullptr, &ids);
		if (ids.size() > 0)
			SetVideoDevice(ids[0]);
		ids.clear();

		EnableAudioDevice(true, true);
		EnableAudioDevice(true, false);
		EnableVideoDevice(true);
		exec = true;
	}
	void VideoManagerG2::SetVideoDevice(const std::wstring& id)
	{
		std::wstring deviceId = GetCurVideoDeviceId();
		if (id == deviceId)
			return;
		AvChatParams param;
		nbase::BatpPack bp;

		bp.head_.action_name_ = kAvChatSetVideoDevice;
		param.deviceId = id;
		bp.body_.param_ = param;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
		//cur_video_device_id_ = id;
	}
	std::wstring VideoManagerG2::GetCurAudioDeviceId(bool isRecord)
	{
		return AvChatBusinessWrapper::getAudioDevice(isRecord);
		//return isRecord ? cur_audio_input_device_id_ : cur_audio_output_device_id_;
	}
	std::wstring VideoManagerG2::GetCurVideoDeviceId()
	{
		return AvChatBusinessWrapper::getVideoDevice();
		//return cur_video_device_id_;
	}
	void VideoManagerG2::SetAudioDevice(const std::wstring& id, bool isRecord)
	{
		if (id == GetCurAudioDeviceId(isRecord))
			return;

		AvChatParams param;
		nbase::BatpPack bp;

		bp.head_.action_name_ = kAvChatSetAudioDevice;
		param.deviceId = id;
		param.isRecordDevice = isRecord;
		bp.body_.param_ = param;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
		//isRecord ? (cur_audio_input_device_id_  = id) : (cur_audio_output_device_id_ = id);
	}
	void VideoManagerG2::SetRemoteCanvas(const std::string& userId)
	{
		if (!is_video_mode_)
			return;
		auto canvasDataCb = [](
			nertc::uid_t uid,   /**< uid */
			void* data,         /**< 数据指针 */
			uint32_t type,      /**< NERtcVideoType */
			uint32_t width,     /**< 宽度 */
			uint32_t height,    /**< 高度 */
			uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
			uint32_t offset[4], /**< 每类数据偏移 */
			uint32_t stride[4], /**< 每类数据步进 */
			uint32_t rotation,  /**< NERtcVideoRotation */
			void* user_data     /**< 用户透传数据 */) {

				auto video_frame = VideoManagerG2::GetInstance()->GetVideoForm();
				if (video_frame)
					video_frame->CaptureRemoteVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
		};

		nbase::BatpPack bp_canvas;
		bp_canvas.head_.action_name_ = kAvChatSetupRemoteView;
		AvChatParams canvas_params;
		canvas_params.window = NULL;
		canvas_params.dataCb = canvasDataCb;
		canvas_params.userId = userId;

		bp_canvas.body_.param_ = canvas_params;
		nbase::BusinessManager::GetInstance()->Request(bp_canvas, nullptr);
	}
	void VideoManagerG2::SetLocalCanvas()
	{
		if (!is_video_mode_)
			return;

		auto canvasDataCb = [](
			nertc::uid_t uid,   /**< uid */
			void* data,         /**< 数据指针 */
			uint32_t type,      /**< NERtcVideoType */
			uint32_t width,     /**< 宽度 */
			uint32_t height,    /**< 高度 */
			uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
			uint32_t offset[4], /**< 每类数据偏移 */
			uint32_t stride[4], /**< 每类数据步进 */
			uint32_t rotation,  /**< NERtcVideoRotation */
			void* user_data     /**< 用户透传数据 */) {

				auto video_frame = VideoManagerG2::GetInstance()->GetVideoForm();
				if(video_frame)
					video_frame->CaptureLocalVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
		};
		//设置本地画布
		nbase::BatpPack bp_canvas;
		bp_canvas.head_.action_name_ = kAvChatSetupLocalView;
		AvChatParams canvas_params;
		canvas_params.window = NULL;
		canvas_params.dataCb = canvasDataCb;

		bp_canvas.body_.param_ = canvas_params;
		nbase::BusinessManager::GetInstance()->Request(bp_canvas, nullptr);
		//video_form_->ShowCanvas();
	}

	void VideoManagerG2::AcceptInvite()
	{
		auto acceptCb = [this](int errCode)->void {
			if (errCode == 200)
			{
				video_form_->ShowStatusPage(VideoFormG2::SP_VIDEO);
				//video_form_->ShowCanvas();
				SetLocalCanvas();
				SetRemoteCanvas(video_form_->GetSessionId());
				video_form_->SwitchStatus(VideoFormG2::STATUS_TALKING);
				video_form_->OnComeIn(0);
			}
			else
			{
				video_form_->EnterEndCallPage(VideoFormG2::END_CALL_REJECT);
			}
		};
		nbase::BatpPack bp;
		AvChatParams params;
		params.optCb = nbase::Bind(acceptCb, std::placeholders::_1);
		bp.head_.action_name_ = kAvChatAccept;
		bp.body_.param_ = params;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
	}

	void VideoManagerG2::OnCallingCb(const nbase::BatpPack& response)
	{
		QLOG_APP(L"OnCallingCb thread id:{0}");
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		
		video_form_->AdjustWindowSize(is_video_mode_);
		video_form_->CheckTitle();
		video_form_->ShowStatusPage(VideoFormG2::SP_DIAL);
		video_form_->SwitchStatus(VideoFormG2::STATUS_WAITING);
		if (setting_form_ && !setting_form_flag_.expired())
		{
			setting_form_->StopVideoPreview();
			setting_form_->StopAudioPreview();
		}
	}

	void VideoManagerG2::OnInvitedCb(const nbase::BatpPack& response)
	{
		QLOG_APP(L"OnInvitedCb thread id:{0}") << nbase::ThreadManager::CurrentThread()->GetManagedThreadId();
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		//SetAudioStatusFromUI();
		//TODO弹出accept界面
		ShowVideoChatForm(params.invitorAccid, (AVCHAT_CALL_TYPE)params.callType == kAvChatVideo, false);
		if (setting_form_ && !setting_form_flag_.expired())
		{
			setting_form_->StopVideoPreview();
			setting_form_->StopAudioPreview();
		}
	}

	void VideoManagerG2::OnUserAcceptCb(const nbase::BatpPack& response)
	{
		QLOG_APP(L"OnUserAcceptCb");
		//SetAudioStatusFromUI();
		video_form_->ShowStatusPage(VideoFormG2::SP_VIDEO);
		SetLocalCanvas();
		SetRemoteCanvas(video_form_->GetSessionId());
		video_form_->SwitchStatus(VideoFormG2::STATUS_TALKING);

		//video_form_->ShowCanvas();
	}
	void VideoManagerG2::OnUserRejectCb(const nbase::BatpPack& response)
	{
		video_form_->EnterEndCallPage(VideoFormG2::END_CALL_REJECT);
	}
	
	//摄像头开关、画质
	//麦克风开关、音量
	//扬声器开关、音量
	void VideoManagerG2::SetDeviceStatusFromSetting()
	{
		EnableVideoDevice(is_video_enable_);
		SetVideoQuality(video_quality_type_);
		EnableAudioDevice(is_audio_input_enable_, true);
		EnableAudioDevice(is_audio_output_enable_, false);

		if (is_audio_input_enable_)
			SetAudioVolumn(GetAudioVolumn(true), true);
		
		if (is_audio_output_enable_)
			SetAudioVolumn(GetAudioVolumn(false), false);	
	}

	void VideoManagerG2::OnUserEnterCb(const nbase::BatpPack& response)
	{
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		
		//对方进入频道设置远端画布
		if (params.userId == video_form_->GetSessionId())
		{
			SetDeviceStatusFromSetting();
			SetRemoteCanvas(params.userId);
			video_form_->OnComeIn(0);
			QLOG_APP(L"userEnter channel: {0}") << params.userId;
			if (!is_sender_) {
				video_form_->ShowStatusPage(VideoFormG2::SP_VIDEO);
				SetLocalCanvas();
				video_form_->SwitchStatus(VideoFormG2::STATUS_TALKING);
			}
		}
		else if (params.userId == nim::Client::GetCurrentUserAccount())
		{
			video_form_->OnComeIn(0);
		}
		else
		{
			assert(false, "wrong channel member");
		}
	}
	void VideoManagerG2::OnUserLeaveCb(const nbase::BatpPack& response)
	{
		video_form_->EnterEndCallPage(VideoFormG2::END_CALL_BE_HANGUP);
	}
	void VideoManagerG2::OnUserBusyCb(const nbase::BatpPack& response)
	{
		video_form_->EnterEndCallPage(VideoFormG2::END_CALL_BUSYING);
	}
	void VideoManagerG2::OnUserCancelCb(const nbase::BatpPack& response)
	{
		video_form_->EnterEndCallPage(VideoFormG2::END_CALL_BE_HANGUP);

	}

	void VideoManagerG2::OnCallingTimeOutCb(const nbase::BatpPack& response)
	{
		std::string channelId = AvChatBusinessWrapper::getChannelId();

	}
	void VideoManagerG2::OnCallEndCb(const nbase::BatpPack& response)
	{
		if (video_form_)
			video_form_->EnterEndCallPage(VideoFormG2::END_CALL_BE_HANGUP);
	}
	void VideoManagerG2::OnErrorCb(const nbase::BatpPack& response)
	{
		if (video_form_)
			video_form_->EnterEndCallPage(VideoFormG2::END_CALL_CLOSE);
	}

	void VideoManagerG2::OnCameraAvailableCb(const nbase::BatpPack& response)
	{
		QLOG_APP(L"");
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		//只处理P2P视频
		video_form_->OnPeerCameraChanged(params.userId, params.cameraAvailable);
	}
	void VideoManagerG2::OnAudioAvailableCb(const nbase::BatpPack& response)
	{

	}
	void VideoManagerG2::OnLocalCameraStateChanged(const nbase::BatpPack& response)
	{
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		bool isEnable = params.cameraAvailable;

	}

	void VideoManagerG2::OnAudioVolumeChanged(const nbase::BatpPack& response)
	{
		return;
		AvChatParams params = nbase::BatpParamCast<AvChatParams>(response.body_.param_);
		bool isRecord = params.isRecordDevice;

		if (setting_form_ != NULL && !setting_form_flag_.expired())
		{
			setting_form_->AudioVolumnChange();
		}
		if (video_form_ != NULL && !video_form_flag_.expired())
		{
			video_form_->InitVolumnSetting();
		}
	}
	void VideoManagerG2::StartAudioPreview(bool start/* = true*/)
	{
		nbase::BatpPack bp;
		bp.head_.action_name_ = start ? kAvChatStartAudioDeviceLoopbackTest : kAvChatStopAudioDeviceLoopbackTest;
		AvChatParams params;
		params.interval = 0;
		bp.body_.param_ = params;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
	}

	void VideoManagerG2::SetVideoQuality(int type)
	{
		nbase::BatpPack bp;
		bp.head_.action_name_ = kAvChatSetVideoQuality;
		AvChatParams params;
		params.videoQuality = type;
		bp.body_.param_ = params;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
		video_quality_type_ = type;
	}

	void VideoManagerG2::StartVideoPreview(bool start/* = true*/)
	{
		nbase::BatpPack bp;
		bp.head_.action_name_ = kAvChatStartVideoPreview;
		AvChatParams params;
		params.startPreview = start;
		bp.body_.param_ = params;

		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);
	}
	VideoSettingFormG2* VideoManagerG2::GetVideoSettingForm()
	{
		if (setting_form_ && !setting_form_flag_.expired())
			return setting_form_.get();
		return nullptr;
	}
	VideoFormG2* VideoManagerG2::GetVideoForm()
	{
		if (video_form_ && !video_form_flag_.expired())
			return video_form_.get();
		return nullptr;
	}
}
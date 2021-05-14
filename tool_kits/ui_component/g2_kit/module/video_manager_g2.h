#pragma once

#include "g2_kit/gui/video_form_g2.h"
#include "shared/business_action_gateway/batp/batp.h"
#include "shared/business_action_gateway/business_action_gateway/action_dispatcher.h"
#include "shared/business_action_gateway/business_action_gateway/business_action_gateway_imp.h"
#include "g2_kit/component/wrapper/avchat_business_wrapper.h"
#include "tool_kits/ui_component/ui_kit/util/net_call_helper.h"
#include "g2_kit/gui/video_setting_g2.h"
#include "g2_kit/module/g2_device_def.h"

/** @class VideoManagerG2
  * @brief 音视频管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2020.11.02
  * 1，负责注册AvCharComponent组件支持的接口
  * 2，负责与服务器交互，将accid映射成userId
  */

namespace nim_comp
{
	typedef std::function<void(int code)>  ConnectCallback;
	typedef std::function<void(std::string uid, bool join_type)>  PeopleChangeCallback;
	typedef std::function<void(int code)>  MultiRoomCreatedCallback;

	using ActionProc = void(*)(const nbase::BatpPack&);
	void QueryToken(int64_t uid, std::function<void(const std::string& token)> onGetToken);


	class VideoActionDispatcher : public nbase::IActionDispatcher
		, public std::enable_shared_from_this<VideoActionDispatcher>
	{
	public:
		virtual void DispatchActionRequest(const nbase::BatpPack& request) override
		{
			auto it_proc_function = actionproc_list_.find(request.head_.action_name_);
			if (it_proc_function != actionproc_list_.end())
			{
				(*it_proc_function->second)(request);
			}
		}
		void AddActionProc(const std::string& action_name, ActionProc proc) {
			actionproc_list_[action_name] = proc;
		}
	private:
		std::map<std::string, ActionProc> actionproc_list_;
	};

	class VideoManagerG2 : public nbase::SupportWeakCallback
	{		
	public:
		SINGLETON_DEFINE(VideoManagerG2);
		//static VideoManagerG2* GetInstance() { return &g_instace_; };

		VideoManagerG2();
		void Init() {};
		void UnInit(StdClosure cb);
		~VideoManagerG2();
		void handleNetCallMsg(nim_comp::NIMNetCallStatus why);
		/**
		* 显示一个音视频窗口
		* @param[in] id 对方用户id
		* @param[in] video 是否包含视频功能
		* @param[in] isSender 是否是发起方
		* @return bool true 成功，false 失败
		*/
		bool ShowVideoChatForm(const std::string& id, bool video, bool isSender = true);

		/**
	     * 关闭所有窗口
	     * @return void 无返回值
	     */
		void DestroyVideoChatForm();

		//是否正在通话中，通话中无法进行本地预览以及音频测试
		bool IsTalking();
		bool ShowVideoSetting(bool video = true);
		void SetAudioVolumn(unsigned char volumn, bool capture);
		bool IsAudioEnable(bool isRecord);
		bool IsVideoEnable();

		unsigned char GetAudioVolumn(bool capture);
		std::vector<G2_MEDIA_DEVICE_INFO> GetDeviceInfo(nim::NIMDeviceType type, int* curDeviceNo = nullptr);

		void EnableVideoDevice(bool anable);
		void EnableAudioDevice(bool anable, bool isRecord);
		void SetVideoDevice(const std::wstring& id);
		void SetDeviceOnce();
		std::wstring GetCurVideoDeviceId();
		std::wstring GetCurAudioDeviceId(bool isRecord);
		void SetAudioDevice(const std::wstring& id, bool isRecord);
		void SetRemoteCanvas(const std::string& userId);
		void SetLocalCanvas();
		void SetDeviceStatusFromSetting();
		std::vector<std::string> GetSupportedActionsList();

		/**
		* 订阅感兴趣的事件，用于更新界面状态
		* @return void	无返回值
		*/
		void SubSingleNotifyForUI(const std::string& notify_name, void(VideoManagerG2::* mem_func)(const nbase::BatpPack& response));

		void OnCallingCb(const nbase::BatpPack& response);
		void OnInvitedCb(const nbase::BatpPack& response);
		//void EnableLocalAudio(bool enable);
		//void EnableLocalCamera(bool enable);
		void AcceptInvite();
		void OnUserAcceptCb(const nbase::BatpPack& response);
		void OnUserRejectCb(const nbase::BatpPack& response);
		void OnUserEnterCb(const nbase::BatpPack& response);
		void OnUserLeaveCb(const nbase::BatpPack& response);
		void OnUserDisconnectCb(const nbase::BatpPack& response);
		void OnUserBusyCb(const nbase::BatpPack& response);
		void OnUserCancelCb(const nbase::BatpPack& response);

		void OnCallingTimeOutCb(const nbase::BatpPack& response);
		void OnVideoToAudioCb(const nbase::BatpPack& response);
		void OnCallEndCb(const nbase::BatpPack& response);
		void OnErrorCb(const nbase::BatpPack& response);

		void onOtherClientAcceptCb(const nbase::BatpPack& response);
		void onOtherClientRejectCb(const nbase::BatpPack& response);
		void onUserNetworkQualityCb(const nbase::BatpPack& response);

		void OnCameraAvailableCb(const nbase::BatpPack& response);
		void OnAudioAvailableCb(const nbase::BatpPack& response);
		void OnLocalCameraStateChanged(const nbase::BatpPack& response);
		void OnAudioVolumeChanged(const nbase::BatpPack& response);
		void StartVideoPreview(bool start = true);
		void StartAudioPreview(bool start = true);
		void SetVideoQuality(int tytpe);
		void EnableVideoToAudio(std::string	session_id_, int call_type);
		void SetAudioMute(std::string	session_id_, bool bMute);
		VideoSettingFormG2* GetVideoSettingForm();
		VideoFormG2* GetVideoForm();
		//nbase::BusinessActionGatewayImp			gateway_impl_;
	private:
		std::shared_ptr<VideoActionDispatcher>	action_dispatcher_;
		std::unique_ptr<VideoSettingFormG2>		setting_form_;
		//std::weak_ptr<VideoSettingFormG2>		weak_setting_form_;
		std::unique_ptr<VideoFormG2>			video_form_;
		std::weak_ptr<nbase::WeakFlag>			video_form_flag_;
		std::weak_ptr<nbase::WeakFlag>			setting_form_flag_;
		std::atomic_bool						init_flag_;
		//static VideoManagerG2					g_instace_;
		bool									is_video_mode_;
		bool									is_video_enable_;
		bool									is_audio_input_enable_;
		bool									is_audio_output_enable_;
		bool									is_sender_;
		int										video_quality_type_;
// 		std::wstring							cur_video_device_id_;
// 		std::wstring							cur_audio_input_device_id_;
// 		std::wstring							cur_audio_output_device_id_;
	};

#define ADD_ACTION_PROC(func_name) AddActionProc(#func_name, &AvChatBusinessWrapper::func_name)
}
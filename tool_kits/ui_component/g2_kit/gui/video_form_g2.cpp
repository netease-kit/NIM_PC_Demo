
#include "pch.h"
#include "video_form_g2.h"
#include "video_canvas_g2.h"
#include "shared/closure.h"
#include "ui_kit/util/net_call_helper.h"
#include "g2_kit/component/avchat_component_def.h"
#include "g2_kit/module/video_manager_g2.h"
#include "av_kit/module/video/video_manager.h"
#include "g2_kit/component/wrapper/avchat_business_wrapper.h"


//#include <sys/timeb.h>
//#include "shared/tool.h"
//#include "base/time/time.h"
//#include "module/video_manager_g2.h"
//#include "./helper/smooth.h"
//#include "./helper/yuv_image.h"
//#include "./helper/colorbalance.h"

namespace nim_comp
{
	using namespace ui;
	const LPCTSTR VideoFormG2::kClassName = L"VideoFormG2";
	const int kG2DialTimeOut = 45;

	VideoFormG2::VideoFormG2(const std::string& session_id):session_id_(session_id)
	{
		mic_in_open_ = true;
		speaker_out_open_ = true;
		status_ = STATUS_NONE;
		channel_id_ = 0;
		screen_is_other_ = true;
		current_video_mode_ = false;
		custom_video_mode_ = false;
		is_self_ = true;

		end_call_ = END_CALL_NONE;

		send_camera_notify_ = false;
		is_mode_changing_ = false;

		camera_is_open_ = true;
		camera_is_open_other_ = true;

		need_change_form_size_ = true;

		is_max_window_ = false;
		mp4_recording_ = false;
		is_start_ = false;
	}
	VideoFormG2::~VideoFormG2()
	{

	}
	std::string VideoFormG2::GetSessionId()
	{
		return session_id_;
	}
	/*bool VideoFormG2::InitVideo()
	{
		//std::string def_video;
		//int no = 0;
		//VideoManager::GetInstance()->GetDefaultDevicePath(no, def_video, nim::kNIMDeviceTypeVideo);
		//if (def_video.empty())
		//{
		//	return false;
		//}
		//VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, def_video, kDeviceSessionTypeChat);
		//return true;
	}

	void VideoFormG2::FreeVideo()
	{
		//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeChat);
	}
	*/
	std::wstring VideoFormG2::GetSkinFolder()
	{
		return L"video";
	}

	/* 与G1版本共用界面*/
	std::wstring VideoFormG2::GetSkinFile()
	{
		return L"videoform_g2.xml";
	}
	HWND VideoFormG2::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, bool isLayeredWindow, const UiRect& rc)
	{
		return Window::Create(hwndParent, pstrName, dwStyle, dwExStyle, false, rc);
	}
	void VideoFormG2::InitWindow()
	{
		m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&VideoFormG2::Notify, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&VideoFormG2::OnClicked, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&VideoFormG2::OnSelect, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&VideoFormG2::OnSelect, this, std::placeholders::_1));

		status_page_ = (Box*)FindControl(L"status_page");
		video_page_ = (Box*)FindControl(L"video_page");
		volumn_page_ = (Box*)FindControl(L"volumn_page");
		confirm_page_ = (Box*)FindControl(L"confirm_page");

		headicon_btn_ = (Button*)FindControl(L"headicon");
		friend_label_ = (Label*)FindControl(L"friend_name");
		status_label_ = (Label*)FindControl(L"chat_status");

		video_ctrl_screen_ = (BitmapControlG2*)FindControl(L"photo_screen");
		//video_ctrl_screen_->SetAccount(session_id_);
		video_ctrl_preview_ = (BitmapControlG2*)FindControl(L"photo_preview");
		video_ctrl_preview_->SetAutoSize(true);
		//video_ctrl_preview_->SetAccount(session_id_);
		
		time_tick_label_ = (Label*)FindControl(L"time_tick");
		camera_open_label_ = (Label*)FindControl(L"camera_opening");
		camera_closed_label_ = (Label*)FindControl(L"camera_closed");

		swap_photo_btn_ = (Button*)FindControl(L"swap_photo");

		video_border_ctrl_ = FindControl(L"video_border");
		Box* box_of_preview = (Box*)FindControl(L"box_video_preview");
		ASSERT(box_of_preview);
		box_of_preview->AttachBubbledEvent(kEventMouseEnter, nbase::Bind(&VideoFormG2::OnMouseMessage, this, std::placeholders::_1));
		box_of_preview->AttachBubbledEvent(kEventMouseLeave, nbase::Bind(&VideoFormG2::OnMouseMessage, this, std::placeholders::_1));

		camera_page_tip_ = FindControl(L"camera_page_tip");
		camera_page_tip2_ = FindControl(L"camera_page_tip2");

		//暂不支持通话过程中的音视频切换
		//into_video_btn_ = (Button*)FindControl(L"into_video");
		into_audio_btn_ = (Button*)FindControl(L"into_audio");
		stop_chat_btn_ = (Button*)FindControl(L"stop_chat");

		speaker_btn_ = (Button*)FindControl(L"speaker");
		microphone_btn_ = (Button*)FindControl(L"microphone");
		camera_checkbox_ = (CheckBox*)FindControl(L"camera");
		start_record_btn_ = (Button*)FindControl(L"record_start");
		stop_record_btn_ = (Button*)FindControl(L"record_stop");
		face_open_btn_ = (Button*)FindControl(L"face_open");
		face_close_btn_ = (Button*)FindControl(L"face_close");
		
		input_volumn_slider_ = (Slider*)FindControl(L"input_volumn");
		vbox_of_input_ = (VBox*)FindControl(L"vbox_of_input_volumn");
		place_ctrl_1_ = FindControl(L"place_ctrl_1");

		output_volumn_slider_ = (Slider*)FindControl(L"output_volumn");
		vbox_of_output_ = (VBox*)FindControl(L"vbox_of_output_volumn");
		place_ctrl_2_ = FindControl(L"place_ctrl_2");
		
		output_volumn_slider_->AttachKillFocus(nbase::Bind(&VideoFormG2::OnSpeakerVolumnKillFocus, this, std::placeholders::_1));
		input_volumn_slider_->AttachKillFocus(nbase::Bind(&VideoFormG2::OnMicrophoneVolumnKillFocus, this, std::placeholders::_1));

		refuse_btn_ = (Button*)FindControl(L"refuse");
		complete_btn_ = (Button*)FindControl(L"complete");

		netstat_label_ = (Label*)FindControl(L"netstat");
		hbox_camera_fail_ = (HBox*)FindControl(L"hbox_camera_fail");
		vbox_video_audio_tip_ = (Box*)FindControl(L"vbox_video_audio_tip");

		record_tip_box_ = (Box*)FindControl(L"record_tip_box");
		record_tip_label0_ = (Label*)FindControl(L"record_tip0");
		record_tip_label1_ = (Label*)FindControl(L"record_tip1");
		record_tip_label2_ = (Label*)FindControl(L"record_tip2");
		record_tip_label3_ = (Label*)FindControl(L"record_tip3");
		recording_tip_label_ = (Label*)FindControl(L"recording_tip");
		/**/
		AllowWindowMaxsize(false);

		CheckHeadIcon();
		CheckFriendName();

		InitSetting();

		
// 		microphone_btn_->SetVisible(true);
// 		speaker_btn_->SetVisible(true);
// 		camera_checkbox_->SetVisible(true);

// 		microphone_btn_->SetEnabled(false);
// 		speaker_btn_->SetEnabled(false);
// 		camera_checkbox_->SetEnabled(false);
		/**/
		into_audio_btn_->SetVisible(current_video_mode_);
		voip_ring_.Init(this->GetHWND());
		
	}

	bool VideoFormG2::Notify(ui::EventArgs* msg)
	{
		std::wstring name = msg->pSender->GetName();
		if (msg->Type == ui::kEventValueChange)
		{
			if (name == L"output_volumn")
			{
				int v = output_volumn_slider_->GetValue();
				VideoManagerG2::GetInstance()->SetAudioVolumn(v, false);
			}
			else if (name == L"input_volumn")
			{
				int v = input_volumn_slider_->GetValue();

				//VideoManagerG2::GetInstance()->SetAudioInputAutoVolumn(false);
				VideoManagerG2::GetInstance()->SetAudioVolumn(v, true);
			}
		}
		return true;
	}
	LRESULT VideoFormG2::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_SIZE)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				// Nothing to do
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (is_max_window_)
					OnMaxWindow(false);
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				OnMaxWindow(true);
			}
		}
		else if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
		{
			if (end_call_ == END_CALL_NONE)
			{
				BeforeClose();
				return 0;
			}
			else
			{
				return 0;
			}
		}
		//else if(uMsg == WM_MSG_DESTROY_WND)
		//{
		//	OnAutoCloseWnd();
		//	return 0;
		//}
		voip_ring_.Notify(uMsg, wParam, lParam);

		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void VideoFormG2::OnMaxWindow(bool _max)
	{
		if (is_max_window_ == _max)
			return;
		is_max_window_ = _max;

		Button* max_btn = static_cast<Button*>(FindControl(L"maxbtn"));
		Button* restore_btn = static_cast<Button*>(FindControl(L"restorebtn"));
		if (_max)
		{
			max_btn->SetVisible(false);
			restore_btn->SetVisible(true);
		}
		else
		{
			max_btn->SetVisible(true);
			restore_btn->SetVisible(false);
		}
	}
	void VideoFormG2::OnFinalMessage(HWND hWnd)
	{

	}

	void VideoFormG2::InitVolumnSetting()
	{
		//麦克风
		//BYTE vol_in = nim::VChat::GetAudioVolumn(true);
		BYTE vol_in = VideoManagerG2::GetInstance()->GetAudioVolumn(true);
		if (vol_in == 0)
		{
			microphone_btn_->SetClass(L"Microphone2");
		}
		else
		{
			microphone_btn_->SetClass(L"Microphone1");
		}

		//音量
		//BYTE vol_out = nim::VChat::GetAudioVolumn(false);
		BYTE vol_out = VideoManagerG2::GetInstance()->GetAudioVolumn(false);
		if (vol_out == 0)
		{
			speaker_btn_->SetClass(L"Speaker2");
		}
		else
		{
			speaker_btn_->SetClass(L"Speaker1");
		}

		//VideoManagerG2::GetInstance()->SetAudioVolumn(255, false);
		//VideoManager::GetInstance()->SetAudioInputAutoVolumn(255);
	}

	void VideoFormG2::ClearBitmapControl(bool mine)
	{
		BitmapControlG2* view = NULL;
		Control* tip = NULL;
		if (mine)
		{
			if (screen_is_other_)
			{
				view = video_ctrl_preview_;
				tip = camera_page_tip2_;
			}
			else
			{
				view = video_ctrl_screen_;
				tip = camera_page_tip_;
			}
			tip->SetVisible(!VideoManagerG2::GetInstance()->IsVideoEnable());
		}
		else
		{
			if (screen_is_other_)
			{
				view = video_ctrl_screen_;
				tip = camera_page_tip_;
			}
			else
			{
				view = video_ctrl_preview_;
				tip = camera_page_tip2_;
			}
			tip->SetVisible(!camera_is_open_other_);
		}
		view->Clear();
		
	}

	void VideoFormG2::ChangeToAudio()
	{
		//IVideoChatService* service = GetVideoChatService();
		//PTR_VOID(service);

		//FreeVideo();
		VideoManagerG2::GetInstance()->EnableVideoToAudio(session_id_, kVideoToAudio);
		IntoAudio();
		//into_video_btn_->SetVisible(true);

		//VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlVideoToAudio);
	}

	bool VideoFormG2::OnClicked(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"stop_chat")
		{
			if (status_ != STATUS_TALKING)
			{
				//VideoManagerG2::GetInstance()->handleNetCallMsg(nim_comp::kNIMNetCallStatusCanceled);
			}
			EnterEndCallPage(END_CALL_HANGUP);
		}
		else if (name == L"into_video")
		{
			//ChangeToVideo();
			//VideoManagerG2::GetInstance()->SetAudioMute(session_id_, true); 静音远端音频功能
		}
		else if (name == L"swap_photo")
		{
			screen_is_other_ = !screen_is_other_;
			bool is_preview_enable = video_ctrl_preview_->IsEnableCaptureVideoFrame();
			bool is_screen_enable = video_ctrl_screen_->IsEnableCaptureVideoFrame();
			video_ctrl_screen_->EnableCaptureVideoFrame(is_preview_enable);
			video_ctrl_preview_->EnableCaptureVideoFrame(is_screen_enable);
			ClearBitmapControl(true);
			ClearBitmapControl(false);
			if (screen_is_other_ && !camera_is_open_other_)
				camera_closed_label_->SetVisible(true);
			else
				camera_closed_label_->SetVisible(false);
		}
		else if (name == L"into_audio")
		{
			ChangeToAudio();
		}
		else if (name == L"go_setting")
		{
			//VideoManager::GetInstance()->ShowVideoSetting();
		}
		else if (name == L"microphone")
		{
			int v = VideoManagerG2::GetInstance()->GetAudioVolumn(true);
			//int v = nim::VChat::GetAudioVolumn(true);
			input_volumn_slider_->SetValue(v);

			vbox_of_input_->SetVisible(true);

			input_volumn_slider_->SetFocus();
		}
		else if (name == L"speaker")
		{
			//int v = nim::VChat::GetAudioVolumn(false);
			int v = VideoManagerG2::GetInstance()->GetAudioVolumn(false);
			output_volumn_slider_->SetValue(v);

			vbox_of_output_->SetVisible(true);

			output_volumn_slider_->SetFocus();
			
		}

		if (name == L"refuse")
		{
			EnterEndCallPage(END_CALL_REJECT);
			//VideoManagerG2::GetInstance()->handleNetCallMsg(nim_comp::kNIMNetCallStatusRejected);
		}
		else if (name == L"complete")
		{
			refuse_btn_->SetEnabled(false);
			complete_btn_->SetEnabled(false);

			ShowStatusPage(SP_DIAL);
			SwitchStatus(STATUS_CONNECTING);

			VideoManagerG2::GetInstance()->AcceptInvite();
			/*bool ret = VideoManager::GetInstance()->VChatCalleeAck(channel_id_, nim_comp::kVChatAskAccept, session_id_);
			if (ret)
			{
				is_start_ = true;
				StdClosure closure = nbase::Bind(&VideoForm::OnConnectTimeOut, this);
				closure = connect_timeout_timer_.ToWeakCallback(closure);
				nbase::ThreadManager::PostDelayedTask(closure, nbase::TimeDelta::FromSeconds(kConnectTimeOut));
			}
			else
			{
				EnterEndCallPage(END_CALL_STARTFAIL);
			}*/
		}else if (name == L"close_btn")
		{
			if (end_call_ == END_CALL_NONE)
			{
				BeforeClose();
			}
		}
		/*
		else if (name == L"record_start")
		{
			// 要保存的文件名
			if (record_select_form_ == NULL || record_select_form_flag_.expired())
			{
				record_select_form_.release();
				record_select_form_.reset(new RecordSelectForm);
				record_select_form_->Create(NULL, RecordSelectForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
				record_select_form_flag_ = record_select_form_->GetWeakFlag();
			}
			record_select_form_->CenterWindow();
			record_select_form_->ShowWindow();
			record_select_form_->SetSelFileCb(current_video_mode_, nbase::Bind(&VideoForm::OnRecordSelFileCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		}
		else if (name == L"record_stop")
		{
			nim::VChat::StopRecord("", nbase::Bind(&VideoForm::StopRecordCb, this, true, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
			nim::VChat::StopRecord(session_id_, nbase::Bind(&VideoForm::StopRecordCb, this, true, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
			nim::VChat::StopAudioRecord(nbase::Bind(&VideoForm::StopRecordCb, this, false, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		}
		else if (name == L"rotate")
		{
			nim::VChat::SetRotateRemoteVideo(!nim::VChat::IsRotateRemoteVideo());
		}
		else if (name == L"face_open")
		{
			SetCustomVideoMode(true);
		}
		else if (name == L"face_close")
		{
			SetCustomVideoMode(false);
		}*/
		return false;
	}

	bool VideoFormG2::OnSelect(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"camera")
		{
			camera_is_open_ = camera_checkbox_->IsSelected();
			if (camera_is_open_)
			{
				VideoManagerG2::GetInstance()->EnableVideoDevice(true);
				screen_is_other_ ? video_ctrl_preview_->EnableCaptureVideoFrame(true)
					: video_ctrl_screen_->EnableCaptureVideoFrame(true);
				screen_is_other_ ? camera_page_tip2_->SetVisible(false)
					: camera_page_tip_->SetVisible(false);
			}
			else
			{
				VideoManagerG2::GetInstance()->EnableVideoDevice(false);
				screen_is_other_ ? video_ctrl_preview_->EnableCaptureVideoFrame(false)
					: video_ctrl_screen_->EnableCaptureVideoFrame(false);
				ClearBitmapControl(true);
			}
			InitSetting();
		}
		return false;
	}

	bool VideoFormG2::OnMouseMessage(ui::EventArgs* arg)
	{
		ui::EventType type = arg->Type;
		if (type == kEventMouseEnter)
		{
			video_border_ctrl_->SetBorderColor(L"border_alpha_blue");
			swap_photo_btn_->SetVisible(true);
		}
		else if (type == kEventMouseLeave)
		{
			video_border_ctrl_->SetBorderColor(L"border_alpha_white");
			swap_photo_btn_->SetVisible(false);
		}
		return true;
	}

	bool VideoFormG2::OnSpeakerVolumnKillFocus(::ui::EventArgs* arg)
	{
		vbox_of_output_->SetVisible(false);
		return true;
	}
	bool VideoFormG2::OnMicrophoneVolumnKillFocus(::ui::EventArgs* arg)
	{
		vbox_of_input_->SetVisible(false);
		return true;
	}

	void VideoFormG2::AllowWindowMaxsize(bool allow)
	{
		Button* button = (Button*)FindControl(L"maxbtn");
		ASSERT(button);
		if (allow)
		{
			button->SetEnabled(true);

			this->SetSizeBox(UiRect(4, 4, 4, 4));
		}
		else
		{
			button->SetEnabled(false);

			this->SetSizeBox(UiRect(0, 0, 0, 0));
		}
	}
	void VideoFormG2::CheckHeadIcon()
	{
		nim::UserNameCard info;
		UserService::GetInstance()->GetUserInfo(session_id_, info);
		std::wstring photo = PhotoService::GetInstance()->GetUserPhoto(info.GetAccId());
		headicon_btn_->SetBkImage(photo);
	}
	void VideoFormG2::CheckFriendName()
	{
		friend_label_->SetText(UserService::GetInstance()->GetUserName(session_id_));
	}
	int VideoFormG2::CheckVideoInputDevice()
	{
		std::string device;
		int no = 0;
		auto info = VideoManagerG2::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeVideo);
		if (!info.empty())
		{
			return (VideoManagerG2::GetInstance()->IsVideoEnable() ? 0 : 1);
		}
		else
			return 2;
	}
	void VideoFormG2::InitSetting()
	{
		InitVolumnSetting();

		if (current_video_mode_)
		{
			int rs = CheckVideoInputDevice();
			//int rs = 1; 
			if (rs == 2)
			{
				camera_checkbox_->SetClass(L"Camera3");
				camera_checkbox_->SetToolTipText(L"");
				camera_checkbox_->SetEnabled(false);
			}
			else
			{
				camera_checkbox_->Selected(rs == 0, false);
				if (rs == 0)
				{
					camera_checkbox_->SetClass(L"Camera1");
					camera_checkbox_->SetToolTipText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_CLOSE_CAMERA"));
				}
				else
				{
					camera_checkbox_->SetClass(L"Camera2");
					camera_checkbox_->SetToolTipText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_OPEN_CAMERA"));
				}
				if (status_ == STATUS_TALKING)
					camera_checkbox_->SetEnabled(true);
			}
			camera_is_open_ = VideoManagerG2::GetInstance()->IsVideoEnable();
			camera_checkbox_->Selected(camera_is_open_, false);
			camera_checkbox_->SetVisible(true);

			//美颜功能暂不支持
			face_open_btn_->SetVisible(false);
			face_close_btn_->SetVisible(false);
		}
		else
		{
			camera_checkbox_->SetVisible(false);
			face_open_btn_->SetVisible(false);
			face_close_btn_->SetVisible(false);
		}
	}
	void VideoFormG2::OnTick()
	{
		long sec = 0;
		auto getTickTime = [this](long& sec){
			uint32_t tm = nbase::Time::Now().ToTimeT();
			sec = tm - this->time_stamp_;
			if (sec < 0)
				sec = 0;

			std::wstring time;
			int h = sec / 3600, m = (sec % 3600) / 60, s = sec % 60;
			if (h > 0)
				nbase::StringPrintf(time, L"%02d:%02d:%02d", h, m, s);
			else
				nbase::StringPrintf(time, L"%02d:%02d", m, s);

			return time;
		};
		std::wstring time = getTickTime(sec);

		time_tick_label_->SetText(time);

		status_label_->SetText(time);
	}

	void VideoFormG2::OnComeIn(uint64_t id)
	{
		//uint64_t v = StringToUint64t(session_id_);
		//if( v != id )
		//	return;
		//start_record_btn_->SetVisible(true);
		if (current_video_mode_)
		{
			IntoVideo();
		}
		else
		{
			IntoAudio();
		}

		time_stamp_ = nbase::Time::Now().ToTimeT();
		if (current_video_mode_)
			time_tick_label_->SetText(L"00:00");
		else
			status_label_->SetText(L"00:00");

		StdClosure closure = nbase::Bind(&VideoFormG2::OnTick, this);
		auto weak_callback = tick_weakflag_.ToWeakCallback(closure);
		nbase::ThreadManager::PostRepeatedTask(weak_callback, nbase::TimeDelta::FromSeconds(1));

		SwitchStatus(STATUS_TALKING);

		if (current_video_mode_)
		{
			into_audio_btn_->SetEnabled(true);

			AllowWindowMaxsize(true);
		}
	}
	void VideoFormG2::BeforeClose()
	{
		std::wstring content_id, button_id;
		content_id = (current_video_mode_ ? L"STRID_VIDEO_QUIT_QUERY_VIDEO" : L"STRID_VIDEO_QUIT_QUERY_AUDIO");
		button_id = (current_video_mode_ ? L"STRID_VIDEO_QUIT_RETURN_VIDEO" : L"STRID_VIDEO_QUIT_RETURN_AUDIO");

		MsgboxCallback mb = nbase::Bind(&VideoFormG2::OnQuitMsgBox, this, std::placeholders::_1);
		ShowMsgBox(m_hWnd, mb, content_id, true, L"STRING_TIPS", true, L"STRID_VIDEO_QUIT_YES", true, button_id, true);
	}
	void VideoFormG2::OnQuitMsgBox(MsgBoxRet ret)
	{
		if (ret == MB_YES)
		{
			EnterEndCallPage(END_CALL_CLOSE);
		}
	}

	void VideoFormG2::CaptureLocalVideoFrame(uint64_t uid,   /**< uid */
		void* data,         /**< 数据指针 */
		uint32_t type,      /**< NERtcVideoType */
		uint32_t width,     /**< 宽度 */
		uint32_t height,    /**< 高度 */
		uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
		uint32_t offset[4], /**< 每类数据偏移 */
		uint32_t stride[4], /**< 每类数据步进 */
		uint32_t rotation,  /**< NERtcVideoRotation */
		void* user_data     /**< 用户透传数据 */)
	{
		//抛到UI线程(YUV数据拷贝有问题)
		//size_t data_size = width * height * 3 / 2;
		//void* new_data = new char[data_size];
		//memcpy(new_data, data, data_size);
		//auto closure = nbase::Bind([=]() {
		//	//new_data由CaptureVideoFrame释放
		//	//video_ctrl_preview_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
		//	if (screen_is_other_)
		//	{
		//		video_ctrl_preview_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
		//	}
		//	else
		//	{
		//		video_ctrl_screen_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
		//
		//	}
		//});
		//nbase::ThreadManager::PostTask(kThreadUI, closure);
		if (screen_is_other_ && video_ctrl_preview_)
		{
			if (video_ctrl_preview_)
				video_ctrl_preview_->CaptureVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
		}
		else
		{
			if (video_ctrl_screen_)
				video_ctrl_screen_->CaptureVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);

		}
	}

	void VideoFormG2::CaptureRemoteVideoFrame(uint64_t uid,   /**< uid */
		void* data,         /**< 数据指针 */
		uint32_t type,      /**< NERtcVideoType */
		uint32_t width,     /**< 宽度 */
		uint32_t height,    /**< 高度 */
		uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
		uint32_t offset[4], /**< 每类数据偏移 */
		uint32_t stride[4], /**< 每类数据步进 */
		uint32_t rotation,  /**< NERtcVideoRotation */
		void* user_data     /**< 用户透传数据 */)
	{
// 		size_t data_size = width * height * 1.5;
// 		void* new_data = new char[data_size];
// 		memcpy(new_data, data, data_size);
// 		auto closure = nbase::Bind([=]() {
// 			//new_data由CaptureVideoFrame释放
// 			//video_ctrl_screen_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
// 			if (screen_is_other_)
// 			{
// 				video_ctrl_screen_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
// 			}
// 			else
// 			{
// 				video_ctrl_preview_->CaptureVideoFrame(uid, new_data, type, width, height, count, offset, stride, rotation, user_data);
// 			}
// 		});
// 		nbase::ThreadManager::PostTask(kThreadUI, closure);

		if (!firstFrame_)
		{
			firstFrame_ = true;
			QLOG_APP(L"CaptureRemoteVideoFrame firstFrame, uid: {0}") << uid;
		}

		if (screen_is_other_)
		{
			if(video_ctrl_screen_)
				video_ctrl_screen_->CaptureVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
		}
		else
		{
			if (video_ctrl_preview_)
				video_ctrl_preview_->CaptureVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
		}
	}
	void VideoFormG2::OnPeerCameraChanged(const std::string& userId, bool avaiable)
	{
		if (userId != session_id_)
			return;
		if (camera_is_open_other_ == avaiable)
			return;

		camera_is_open_other_ = avaiable;
		if (avaiable)
		{
			screen_is_other_ ? video_ctrl_screen_->EnableCaptureVideoFrame(true)
				: video_ctrl_preview_->EnableCaptureVideoFrame(true);
			screen_is_other_ ? camera_page_tip_->SetVisible(false)
				: camera_page_tip2_->SetVisible(false);

			camera_closed_label_->SetVisible(false);
		}
		else
		{
			screen_is_other_ ? video_ctrl_screen_->EnableCaptureVideoFrame(false)
				: video_ctrl_preview_->EnableCaptureVideoFrame(false);
			ClearBitmapControl(false);
		}
	}

	void VideoFormG2::EnterEndCallPage(EndCallEnum why)
	{
		is_start_ = false;
		if (end_call_ != END_CALL_NONE)
		{
			return;
		}

		dial_timeout_timer_.Cancel();
		nbase::BatpPack bp;
		AvChatParams params;
		switch (why)
		{
		case END_CALL_REJECT:
			bp.head_.action_name_ = kAvChatReject;
			break;
		case END_CALL_BUSYING:
		case END_CALL_OTHER_CLIENT_ACCEPT://在其他端接听，只退出当前界面，不挂断通话
		case END_CALL_OTHER_CLIENT_REJECT:
			bp.head_.action_name_ = "NoAction";
			break;
		default:
			bp.head_.action_name_ = kAvChatHangup;
		}

		bp.body_.param_ = params;
		nbase::BusinessManager::GetInstance()->Request(bp, nullptr);

		Control* min = FindControl(L"minbtn");
		ASSERT(min);
		min->SetEnabled(false);

		Control* max = FindControl(L"maxbtn");
		ASSERT(max);
		max->SetEnabled(false);

		Control* restore = FindControl(L"restorebtn");
		ASSERT(restore);
		restore->SetEnabled(false);

		Control* close = FindControl(L"close_btn");
		ASSERT(close);
		close->SetEnabled(false);
		Label* end_call_tip = (Label*)FindControl(L"end_call_tip");
		ASSERT(end_call_tip);

		Label* end_call_why = (Label*)FindControl(L"end_call_why");
		ASSERT(end_call_why);

		Box* end_call_page = (Box*)FindControl(L"end_call_page");
		ASSERT(end_call_page);

		bool need_save_close = false; //万一铃声播放有问题，那么窗口岂不是无法关闭了，所以加入安全措施，6s后自动关闭

		end_call_ = why;
		switch (why)
		{
		case VideoFormG2::END_CALL_NONE:
		case VideoFormG2::END_CALL_CLOSE:
		case VideoFormG2::END_CALL_HANGUP:
		case VideoFormG2::END_CALL_BE_HANGUP:
		case VideoFormG2::END_CALL_SYNC_ACCEPT:
		case VideoFormG2::END_CALL_SYNC_REFUSE:
		case VideoFormG2::END_CALL_BAD_MICROPHONE:
		case VideoFormG2::END_CALL_VERSION:
		case VideoFormG2::END_CALL_CONNECTION:
		case VideoFormG2::END_CALL_STARTFAIL:
		case VideoFormG2::END_CALL_OTHER_CLIENT_ACCEPT:
		case VideoFormG2::END_CALL_OTHER_CLIENT_REJECT:
		{
			ASSERT(why != END_CALL_NONE);
			if (why == END_CALL_BE_HANGUP)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_BE_HANGUP");
			else if (why == END_CALL_SYNC_ACCEPT)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_SYNC_ACCEPT");
			else if (why == END_CALL_SYNC_REFUSE)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_SYNC_REFUSE");
			else if (why == END_CALL_STARTFAIL)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_STARTFAIL");
			else if(why == END_CALL_OTHER_CLIENT_ACCEPT)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_OTHER_CLIENT_ACCEPT");
			else if(why == END_CALL_OTHER_CLIENT_REJECT)
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_OTHER_CLIENT_REJECT");
			else
				end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_HANGUP");

			if (why == END_CALL_BAD_MICROPHONE)
			{
				end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_MICROPHONE");
				end_call_why->SetVisible(true);
			}
			else if (why == END_CALL_VERSION)
			{
				end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_VERSION");
				end_call_why->SetVisible(true);
			}
			else if (why == END_CALL_CONNECTION)
			{
				end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_CONNECTION");
				end_call_why->SetVisible(true);
			}

			StdClosure closure = nbase::Bind(&VideoFormG2::OnAutoCloseWnd, this);
			nbase::ThreadManager::PostDelayedTask(kThreadUI, ToWeakCallback(closure), nbase::TimeDelta::FromSeconds(3));
			break;
		}
		case VideoFormG2::END_CALL_RESPONSE:
		case VideoFormG2::END_CALL_BUSYING:
		case VideoFormG2::END_CALL_REJECT:
		{
			need_save_close = true;
			end_call_tip->SetTextId(L"STRID_VIDEO_END_CALL_HANGUP");

			//Ring::RingPlayCallback cb = nbase::Bind( &VideoForm::OnCloseRingEnd, this, std::placeholders::_1 );
			if (why == END_CALL_RESPONSE)
				voip_ring_.Play(RING_VOIP_NORESPONSE);
			else if (why == END_CALL_BUSYING)
			{
				voip_ring_.Play(RING_VOIP_PEERBUSY);
				end_call_tip->SetTextId(L"STRID_SESSION_NETCALL_MSG_BUSY");
			}
			else if (why == END_CALL_REJECT)
				voip_ring_.Play(RING_VOIP_PEERREJECT);
			break;
		}
		default:
			break;
		}

		end_call_page->SetVisible(true);

		QLOG_APP(L"EnterEndCallPage {0}:{1}") << why << end_call_tip->GetText();

		if (need_save_close)
		{
			auto closure = nbase::Bind(&VideoFormG2::OnAutoCloseWnd, this);
			nbase::ThreadManager::PostDelayedTask(kThreadUI, ToWeakCallback(closure), nbase::TimeDelta::FromSeconds(6));
		}


	}
	void VideoFormG2::OnAutoCloseWnd()
	{
		DirectQuit();
	}
	void VideoFormG2::DirectQuit()
	{
		::DestroyWindow(this->GetHWND());
	}
	void VideoFormG2::AdjustWindowSize(bool video)
	{
		int FORM_CX = 0, FORM_CY = 0;
		if (video)
		{
			FORM_CX = 510;
			FORM_CY = 410;

			AllowWindowMaxsize(status_ == STATUS_TALKING);
		}
		else
		{
			FORM_CX = 290;
			FORM_CY = 410;

			AllowWindowMaxsize(false);
		}
		this->SetMinInfo(FORM_CX, FORM_CY);

		if (IsZoomed(m_hWnd))
		{
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		}

		UiRect dest(0, 0, FORM_CX, FORM_CY);
		this->SetPos(dest, true, SWP_NOMOVE);
	}

	void VideoFormG2::IntoVideo()
	{
		current_video_mode_ = true;
		need_change_form_size_ = true;

		screen_is_other_ = true;
		//ClearBitmapControl(true);
		//ClearBitmapControl(false);

		AdjustWindowSize(true);

		CheckTitle();
		ShowStatusPage(SP_VIDEO);

		InitSetting();

		//into_video_btn_->SetVisible(false);

		into_audio_btn_->SetVisible(true);
		into_audio_btn_->SetEnabled(false);
	}

	void VideoFormG2::IntoAudio()
	{
		//VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
		//VideoManagerG2::GetInstance()->EnableVideoToAudio();
		current_video_mode_ = false;

		AdjustWindowSize(false);

		CheckTitle();
		ShowStatusPage(SP_DIAL);

		InitSetting();

		into_audio_btn_->SetVisible(false);
		//into_video_btn_->SetVisible(false);
	}

	void VideoFormG2::CheckTitle()
	{
		std::wstring str;
		if (current_video_mode_)
			str = L"STRID_VIDEO_TITLE_VIDEO";
		else
			str = L"STRID_VIDEO_TITLE_AUDIO";
		str = MutiLanSupport::GetInstance()->GetStringViaID(str);

		Label* caption = (Label*)FindControl(L"caption");
		caption->SetText(str);

		::SetWindowText(this->GetHWND(), str.c_str());
	}
	void VideoFormG2::setVideoMode(bool mode)
	{
		current_video_mode_ = mode;
	}
	void VideoFormG2::PaintVideo()
	{
		if (video_ctrl_screen_)
			video_ctrl_screen_->Refresh();
		
		if (video_ctrl_preview_)
			video_ctrl_preview_->Refresh();
	}

	void VideoFormG2::StartDialWaitingTimer()
	{
		dial_timeout_timer_.Cancel();
		StdClosure timer_cb = nbase::Bind([this]() {
			if (status_ == STATUS_WAITING)
			{
				SwitchStatus(STATUS_NO_RESPONSE);
				VideoManagerG2::GetInstance()->handleNetCallMsg(nim_comp::kNIMNetCallStatusTimeout);
			}
		});
		timer_cb = dial_timeout_timer_.ToWeakCallback(timer_cb);
		nbase::ThreadManager::PostDelayedTask(timer_cb, nbase::TimeDelta::FromSeconds(kG2DialTimeOut));
	}

	void VideoFormG2::StartPaintVideo()
	{
		paint_video_timer_.Cancel();
		StdClosure task = nbase::Bind(&VideoFormG2::PaintVideo, this);
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, paint_video_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(70));
	}

	void VideoFormG2::ShowStatusPage(StatusPage sp)
	{
		is_start_ = true;
		ui::MutiLanSupport* lang = MutiLanSupport::GetInstance();

		status_page_->SetVisible(false);
		confirm_page_->SetVisible(false);
		
		video_page_->SetVisible(false);
		volumn_page_->SetVisible(false);
		
		time_tick_label_->SetVisible(false);

		camera_open_label_->SetVisible(false);
		camera_closed_label_->SetVisible(false);

		netstat_label_->SetVisible(false);
		hbox_camera_fail_->SetVisible(false);
		
		if (sp == SP_DIAL)
		{
			status_page_->SetVisible(true);
			volumn_page_->SetVisible(true);
		}
		else if (sp == SP_CONFIRM)
		{
			status_page_->SetVisible(true);
			confirm_page_->SetVisible(true);

			if (current_video_mode_)
			{
				complete_btn_->SetClass(L"CompleteVideo");
				refuse_btn_->SetClass(L"RefuseVideo");

				status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_VIDEO"));
				complete_btn_->SetText(lang->GetStringViaID(L"STRING_ACCEPT"));
			}
			else
			{
				complete_btn_->SetClass(L"CompleteAudio");
				refuse_btn_->SetClass(L"RefuseAudio");

				status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_AUDIO"));
				complete_btn_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_ACCEPT_LISTEN"));
			}
		}
		else if (sp == SP_VIDEO)
		{
			time_tick_label_->SetVisible(status_ == STATUS_TALKING);

			video_page_->SetVisible(true);
			volumn_page_->SetVisible(true);
			video_ctrl_screen_->SetVisible(true);
			video_ctrl_preview_->SetVisible(true);
			StartPaintVideo();
		}
		else
		{
			ASSERT(0); 
		}
	}

	void VideoFormG2::SwitchStatus(StatusEnum status)
	{
		status_ = status;

		ui::MutiLanSupport* lang = MutiLanSupport::GetInstance();

		switch (status)
		{
		case STATUS_CALLING:
		{
			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_CALLING"));
			camera_checkbox_->SetVisible(current_video_mode_);
		}
		break;
		case STATUS_WAITING:
		{
			if (current_video_mode_)
				voip_ring_.Play(RING_VOIP_RING, true);
			else
			{
				//Ring::RingPlayCallback cb = nbase::Bind( &VideoForm::OnRepeatRingSend, this, std::placeholders::_1 );
				voip_ring_.Play(RING_VOIP_CONNTETING);
			}

			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_WAITING"));
		}
		break;
		case STATUS_TALKING:
		{
			voip_ring_.Stop();

			microphone_btn_->SetEnabled(true);
			speaker_btn_->SetEnabled(true);

			microphone_btn_->SetVisible(true);
			speaker_btn_->SetVisible(true);

			into_audio_btn_->SetVisible(true);
			into_audio_btn_->SetEnabled(true);
			if (current_video_mode_)
			{
				time_tick_label_->SetVisible(true);
			}
			else {
				into_audio_btn_->SetVisible(false); //如果是语音呼叫，则不用显示
			}
		}
		break;
		case STATUS_INVITING:
		{
			//status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_WAITING"));
			voip_ring_.Play(RING_VOIP_RING, true);
		}
		break;
		case STATUS_CONNECTING:
		{
			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_CONNECTING"));

			camera_checkbox_->SetVisible(current_video_mode_);
		}
		break;
		case STATUS_NO_RESPONSE:
		case STATUS_BUSYING:
		case STATUS_REJECT:
		{
			if (status == STATUS_NO_RESPONSE)
			{
				EnterEndCallPage(END_CALL_RESPONSE);
				//SendNetCallMsg(session_id_, );
			}
			else if (status == STATUS_BUSYING)
			{
				EnterEndCallPage(END_CALL_BUSYING);
			}
			else if (status == STATUS_REJECT)
			{
				EnterEndCallPage(END_CALL_REJECT);
			}
		}
		break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	ui::Control* VideoFormG2::CreateControl(const std::wstring& pstrClass)
	{
		
		if (pstrClass == _T("BitmapControlG2"))
		{
			return new ui::BitmapControlG2(this);
		}
		return nullptr;
	}
}
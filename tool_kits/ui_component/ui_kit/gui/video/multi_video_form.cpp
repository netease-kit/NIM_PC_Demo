#include "multi_video_form.h"
#include "module/video/video_manager.h"
#include "module/session/ring.h"
#include "gui/contact_select_form/contact_select_form.h"

using namespace ui;
namespace nim_comp
{
	const LPCTSTR MultiVideoChatForm::kClassName = L"MultiVideoChatForm";
	bool MultiVideoChatForm::InitVideo()
	{
		std::string def_video;
		int no = 0;
		VideoManager::GetInstance()->GetDefaultDevicePath(no, def_video, nim::kNIMDeviceTypeVideo);
		if (def_video.empty())
		{
			return false;
		}
		VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, def_video, kDeviceSessionTypeChat);
		return true;
	}

	bool MultiVideoChatForm::InitAudio()
	{
		std::string def_audio;
		int no = 0;
		VideoManager::GetInstance()->GetDefaultDevicePath(no, def_audio, nim::kNIMDeviceTypeAudioOutChat);
		VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOutChat, def_audio, kDeviceSessionTypeChat);
		VideoManager::GetInstance()->GetDefaultDevicePath(no, def_audio, nim::kNIMDeviceTypeAudioIn);
		VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioIn, def_audio, kDeviceSessionTypeChat);
		if (def_audio.empty())
		{
			return false;
		}
		return true;
	}

	MultiVideoChatForm::MultiVideoChatForm(std::string room_name, std::string session_id, int64_t channel_id, bool video,bool creator,std::string creator_id /*= ""*/)
		:room_name_(room_name)
		, session_id_(session_id)
		, channel_id_(channel_id_)
		, current_video_mode_(video)
		, creator_(creator)
		, creator_id_(creator_id)
	{
		is_max_window_ = false;
		camera_is_open_ = false;
		current_status_ = kConnected;

		//current_video_mode_ = false;
		invited_members_.clear();
		talking_members_.clear();
		VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChating);
	}

	MultiVideoChatForm::~MultiVideoChatForm()
	{
	}

	std::wstring MultiVideoChatForm::GetSkinFolder()
	{
		return L"video";
	}

	std::wstring MultiVideoChatForm::GetSkinFile()
	{
		return L"multi_video_chat.xml";
	}

	void MultiVideoChatForm::OnMaxWindow(bool _max)
	{
		if (is_max_window_ == _max)
			return;
		is_max_window_ = _max;

// 		Button* max_btn = static_cast<Button*>(FindControl(L"maxbtn"));
// 		Button* restore_btn = static_cast<Button*>(FindControl(L"restorebtn"));
// 		if (_max)
// 		{
// 			max_btn->SetVisible(false);
// 			restore_btn->SetVisible(true);
// 		}
// 		else
// 		{
// 			max_btn->SetVisible(true);
// 			restore_btn->SetVisible(false);
// 		}
	}

	LRESULT MultiVideoChatForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			BeforeClose();
			return 0;
		}
		voip_ring_.Notify(uMsg, wParam, lParam);
		return __super::HandleMessage(uMsg, wParam, lParam);
	}

	void MultiVideoChatForm::InitWindow()
	{
		SetTaskbarTitle(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_TASK_TITLE"));
		m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MultiVideoChatForm::Notify, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MultiVideoChatForm::OnClicked, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&MultiVideoChatForm::OnSelect, this, std::placeholders::_1));
		m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&MultiVideoChatForm::OnUnSelect, this, std::placeholders::_1));
		btn_microphone_ = (Button*)FindControl(L"microphone");
		btn_speaker_ = (Button*)FindControl(L"speaker");
		btn_win_top_ = (Button*)FindControl(L"win_top");
		cb_camera_ = (CheckBox*)FindControl(L"camera");
		input_volumn_slider_ = (Slider*)FindControl(L"input_volumn");
		vbox_of_input_ = (VBox*)FindControl(L"vbox_of_input_volumn");
		place_ctrl_1_ = FindControl(L"place_ctrl_1");
		hbox_camera_fail_ = (HBox*)FindControl(L"hbox_camera_fail");
		output_volumn_slider_ = (Slider*)FindControl(L"output_volumn");
		vbox_of_output_ = (VBox*)FindControl(L"vbox_of_output_volumn");
		place_ctrl_2_ = FindControl(L"place_ctrl_2");
		output_volumn_slider_->AttachKillFocus(nbase::Bind(&MultiVideoChatForm::OnSpeakerVolumnKillFocus, this, std::placeholders::_1));
		input_volumn_slider_->AttachKillFocus(nbase::Bind(&MultiVideoChatForm::OnMicrophoneVolumnKillFocus, this, std::placeholders::_1));
		camera_page_tip_ = (ui::Control*)FindControl(L"camera_page_tip");
		lb_ctrl_screen_ = (ui::Label*)FindControl(L"talk_name");
		video_ctrl_screen_ = (BitmapControl*)FindControl(L"photo_screen");
		big_wnd_member_volume_ = (ui::Control*)FindControl(L"member_volume");
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		video_ctrl_screen_->SetAccount(current_user_id);
		nim::UserNameCard user_info;
		UserService::GetInstance()->GetUserInfo(current_user_id, user_info);

		lb_ctrl_screen_->SetText(OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName())));
		big_wnd_info_.uid = current_user_id;
		big_wnd_info_.video = current_video_mode_;
		big_wnd_info_.status = kConnected;
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				Box* box_of_preview = (Box*)FindControl(nbase::StringPrintf(L"box_video_preview_%d_%d", (i + 1), (j + 1)));
				ASSERT(box_of_preview);
				box_of_preview->AttachBubbledEvent(kEventMouseEnter, nbase::Bind(&MultiVideoChatForm::OnMouseMessage, this, std::placeholders::_1));
				box_of_preview->AttachBubbledEvent(kEventMouseLeave, nbase::Bind(&MultiVideoChatForm::OnMouseMessage, this, std::placeholders::_1));
				swap_photo_btn_[i][j] = (Button*)FindControl(nbase::StringPrintf(L"swap_photo_%d_%d", (i + 1), (j + 1)));
				swap_photo_btn_[i][j]->AttachMouseEnter(nbase::Bind(&MultiVideoChatForm::OnMouseMessage, this, std::placeholders::_1));
				video_border_ctrl_[i][j] = FindControl(nbase::StringPrintf(L"video_border_%d_%d", (i + 1),(j + 1)));
				video_ctrl_preview_[i][j] = (BitmapControl*)FindControl(nbase::StringPrintf(L"photo_preview_%d_%d", (i + 1), (j + 1)));
				video_ctrl_preview_[i][j]->SetAutoSize(true);
				video_ctrl_preview_[i][j]->SetAccount("");
				lb_member_name_[i][j]=(ui::Label*)FindControl(nbase::StringPrintf(L"talk_name_%d_%d", (i + 1), (j + 1)));
				lb_member_status_prompt_[i][j]= (ui::Label*)FindControl(nbase::StringPrintf(L"member_prompt_%d_%d", (i + 1), (j + 1)));
				small_camera_page_tip_[i][j] = (ui::Control*)FindControl(nbase::StringPrintf(L"camera_page_tip_%d_%d",(i + 1), (j + 1)));
				member_volume_[i][j] = (ui::Control*)FindControl(nbase::StringPrintf(L"member_volume_%d_%d", i + 1, j + 1));
			}
		}
	
		InitSetting();
		//InitVideo();
		InitAudio();
		paint_video_timer_.Cancel();
		StdClosure task = nbase::Bind(&MultiVideoChatForm::PaintVideo, this);
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, paint_video_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(70));

		start_call_timer_.Cancel();
		StdClosure start_call_task = nbase::Bind(&MultiVideoChatForm::EndVChat, this);
		nbase::ThreadManager::PostDelayedTask(start_call_timer_.ToWeakCallback(start_call_task), nbase::TimeDelta::FromSeconds(45));
		voip_ring_.Init(this->GetHWND());
		voip_ring_.Play(RING_VOIP_RING, true);

		if (current_video_mode_)
		{
			InitVideo();
		}

// 		start_video_timer_.Cancel();
// 		StdClosure start_camera_task = nbase::Bind(&MultiVideoChatForm::StartCameraDevice, this);
// 		nbase::ThreadManager::PostDelayedTask(start_video_timer_.ToWeakCallback(start_camera_task), nbase::TimeDelta::FromSeconds(2));
	}

	void MultiVideoChatForm::EndVChat()
	{
		voip_ring_.Stop();
		EnterEndCallPage(END_CALL_CLOSE);
	}



	bool MultiVideoChatForm::OnMouseMessage(ui::EventArgs* arg)
	{
		ui::EventType type = arg->Type;
		std::wstring name = arg->pSender->GetName();
		int i = -1, j = -1;
		if (((int)name.find(L"1_1"))>0)
		{
			i = 0;
			j = 0;
		}
		else if (((int)name.find(L"1_2"))>0)
		{
			i = 0;
			j = 1;
		}
		else if (((int)name.find(L"1_3"))>0)
		{
			i = 0;
			j = 2;
		}
		else if (((int)name.find(L"1_4"))>0)
		{
			i = 0;
			j = 3;
		}
		else if (((int)name.find(L"2_1"))>0)
		{
			i = 1;
			j = 0;
		}
		else if (((int)name.find(L"2_2"))>0)
		{
			i = 1;
			j = 1;
		}
		else if (((int)name.find(L"2_3"))>0)
		{
			i = 1;
			j = 2;
		}
		else if (((int)name.find(L"2_4"))>0)
		{
			i = 1;
			j = 3;
		}

		//QLOG_APP(L"mouse enter {0} {1} name:{2}") << i << j << name;
		if (type == kEventMouseEnter&&i>=0)
		{
			//QLOG_APP(L"mouse enter {0} {1} true") << i << j;
			if (video_uids[i][j].status == kConnected)
			{
				swap_photo_btn_[i][j]->SetVisible(true);
			}
			//video_border_ctrl_[i][j]->SetBorderColor(L"border_alpha_blue");
			
		}
		else if (type == kEventMouseLeave&&i>=0)
		{
			//video_border_ctrl_[i][j]->SetBorderColor(L"border_alpha_white");
			//QLOG_APP(L"mouse leave {0} {1} false") << i << j;
			if (video_uids[i][j].status == kConnected)
			{
				swap_photo_btn_[i][j]->SetVisible(false);
			}
		}
		return true;
	}

	bool MultiVideoChatForm::OnMicrophoneVolumnKillFocus(ui::EventArgs* arg)
	{
		vbox_of_input_->SetVisible(false);
		return false;
	}

	bool MultiVideoChatForm::OnSpeakerVolumnKillFocus(ui::EventArgs* arg)
	{
		vbox_of_output_->SetVisible(false);
		return false;
	}

	int MultiVideoChatForm::CheckVideoInputDevice()
	{
		std::string device;
		int no = 0;
		VideoManager::GetInstance()->GetDefaultDevicePath(no, device, nim::kNIMDeviceTypeVideo);
		if (!device.empty())
		{
			return (camera_is_open_ ? 0 : 1);
		}
		else
			return 2;
	}

	void MultiVideoChatForm::InitSetting()
	{
		InitVolumnSetting();
		//摄像头
		InitCbCameraSetting();
	}

	void MultiVideoChatForm::InitCbCameraSetting()
	{
		int rs = CheckVideoInputDevice();
		if (rs == 2)
		{
			cb_camera_->SetClass(L"Camera3");
			cb_camera_->SetToolTipText(L"");
			cb_camera_->SetEnabled(false);
		}
		else
		{
			cb_camera_->Selected(rs == 0, false);
			if (rs == 0)
			{
				cb_camera_->SetClass(L"Camera1");
				cb_camera_->SetToolTipText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_CLOSE_CAMERA"));
			}
			else
			{
				cb_camera_->SetClass(L"Camera2");
				cb_camera_->SetToolTipText(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_VIDEO_OPEN_CAMERA"));
			}
		}
		cb_camera_->SetVisible(true);
	}

	void MultiVideoChatForm::InitVolumnSetting()
	{
		//麦克风
		BYTE vol_in = nim::VChat::GetAudioVolumn(true);
		if (vol_in == 0)
		{
			btn_microphone_->SetClass(L"Microphone2");
		}
		else
		{
			btn_microphone_->SetClass(L"Microphone1");
		}

		//音量
		BYTE vol_out = nim::VChat::GetAudioVolumn(false);
		if (vol_out == 0)
		{
			btn_speaker_->SetClass(L"Speaker2");
		}
		else
		{
			btn_speaker_->SetClass(L"Speaker1");
		}

		btn_win_top_->SetClass(L"WinTop1");
	}

	void MultiVideoChatForm::FreeVideo()
	{
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeChat);
	}

	void MultiVideoChatForm::FreeAudio()
	{
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeChat);
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeChat);
	}

	void MultiVideoChatForm::OnFinalMessage(HWND hWnd)
	{
		FreeVideo();
		FreeAudio();
		VideoManager::GetInstance()->EndChat(session_id_);
		VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
		nbase::NAutoLock lock(&data_lock_);
		__super::OnFinalMessage(hWnd);
	}

	ui::Control* MultiVideoChatForm::CreateControl(const std::wstring& pstrClass)
	{
		if (pstrClass == _T("BitmapControl"))
		{
			return new ui::BitmapControl(&nim_comp::VideoManager::GetInstance()->video_frame_mng_);
		}
		return NULL;
	}

	bool MultiVideoChatForm::Notify(ui::EventArgs* msg)
	{
		std::wstring name = msg->pSender->GetName();
		if (msg->Type == ui::kEventValueChange)
		{
			if (name == L"output_volumn")
			{
				//VideoManager* vm = VideoManager::GetInstance();

				int v = output_volumn_slider_->GetValue();
				VideoManager::GetInstance()->SetAudioVolumn(v, false);
			}
			else if (name == L"input_volumn")
			{
				//VideoManager* vm = VideoManager::GetInstance( );

				int v = input_volumn_slider_->GetValue();

				VideoManager::GetInstance()->SetAudioInputAutoVolumn(false);
				VideoManager::GetInstance()->SetAudioVolumn(v, true);
			}
		}
		return true;
	}

	void MultiVideoChatForm::EnterEndCallPage(MultiEndCallEnum why)
	{
		Control* min = FindControl(L"minbtn");
		ASSERT(min);
		min->SetEnabled(false);

		//Control* max = FindControl(L"maxbtn");
		//ASSERT(max);
		//max->SetEnabled(false);

		//Control* restore = FindControl(L"restorebtn");
		//ASSERT(restore);
		//restore->SetEnabled(false);

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

		switch (why)
		{
		case END_CALL_CLOSE:
		case END_CALL_HANGUP:
		case END_CALL_CONNECTION:
		{
		   if (why == END_CALL_CONNECTION)
			{
				end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_CONNECTION");
				end_call_why->SetVisible(true);
			}
			end_call_tip->SetTextId(L"STRID_MULTIVIDEOCHATFORM_END_PROMPT");

			current_status_ = kDisConnect;

			StdClosure closure = nbase::Bind(&MultiVideoChatForm::OnAutoCloseWnd, this);
			nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(3));
			break;
		}

		default:
			break;
		}

		end_call_page->SetVisible(true);

		QLOG_APP(L"EnterEndCallPage {0}:{1}") << why << end_call_tip->GetText();

		if (need_save_close)
		{
			auto closure = nbase::Bind(&MultiVideoChatForm::OnAutoCloseWnd, this);
			nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(6));
		}
	}

	void MultiVideoChatForm::OnAutoCloseWnd()
	{
		DirectQuit();
	}

	void MultiVideoChatForm::DirectQuit()
	{
		DestroyWindow(this->GetHWND());
	}

	void MultiVideoChatForm::BeforeClose()
	{
		std::wstring content_id, button_id;
		content_id = L"STRID_VIDEO_QUIT_QUERY_VIDEO";
		button_id = L"STRID_VIDEO_QUIT_RETURN_VIDEO";

		MsgboxCallback mb = nbase::Bind(&MultiVideoChatForm::OnQuitMsgBox, this, std::placeholders::_1);
		ShowMsgBox(m_hWnd, mb, content_id, true, L"STRING_TIPS", true, L"STRID_VIDEO_QUIT_YES", true, button_id, true);
	}

	void MultiVideoChatForm::OnQuitMsgBox(MsgBoxRet ret)
	{
		if (ret == MB_YES)
		{
			EnterEndCallPage(END_CALL_CLOSE);
		}
	}

	void MultiVideoChatForm::SelectedCompleted(const std::list<UTF8String>& friend_list, const std::list<UTF8String>& team_list, bool delete_enable)
	{
		
		UserService* user_service = UserService::GetInstance();
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		nim::UserNameCard user_info;
		Json::FastWriter fw;
		Json::Value value;
		//Json::Value array_value(Json::arrayValue);
		int index = 0;
		std::set<std::string> inviting_member;
		user_service->GetUserInfo(current_user_id, user_info);
		std::wstring team_name = nbase::UTF8ToUTF16(user_info.GetName());
		value["members"].append(current_user_id);
		//array_value[index] = current_user_id;
		//index++;
		for each (auto friend_info in talking_members_)
		{
			if (!team_name.empty())
			{
				team_name.append(L"、");
			}
			user_service->GetUserInfo(friend_info, user_info);
			team_name.append(nbase::UTF8ToUTF16(user_info.GetName()));
			//array_value[index] = friend_info;
			//index++;
			value["members"].append(friend_info);
		}

		for each (auto friend_info in friend_list)
		{
			if (talking_members_.count(friend_info) == 0)
				inviting_member.insert(friend_info);
		}
		//TeamService* team_service = TeamService::GetInstance();
		//std::wstring team_name = team_service->GetTeamName(session_id_);
		
		value["teamId"] = session_id_;
		value["channelId"] = channel_id_;
		value["teamName"] = nbase::UTF16ToUTF8(team_name);
		value["roomName"] = room_name_;
		value["id"] = kMsgMultiVchatVideo;
		value["creatorId"] = creator_id_;
		// = array_value;
		std::string json_value = fw.write(value);
		//发送自定义消息给用户

		nim::SysMessage msg;
		msg.sender_accid_ = current_user_id;
		msg.client_msg_id_ = QString::GetGUID();
		msg.attach_ = json_value;
		msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

		for each(auto member_info in inviting_member)
		{
			msg.receiver_accid_ = member_info;
			SetInvitingMember(inviting_member);
			QLOG_APP(L"send MsgMultiVchatVideo: msg-{0}") << msg.ToJsonString();
			nim::SystemMsg::SendCustomNotificationMsg(msg.ToJsonString());
		}
		
	}

	int32_t MultiVideoChatForm::GetVolumeStep(uint32_t volume)
	{
		int32_t volume_step = 0;

		{
			volume /= 40;
			while (volume > 0)
			{
				volume_step++;
				volume /= 4;
			}
			if (volume_step > 3)
			{
				volume_step = 3;
			}
		}
		return volume_step;
	}

	void MultiVideoChatForm::RefreshMemberAudioStatus()
	{
		nbase::NAutoLock lock(&data_lock_);
		if (current_status_ != kConnected)
			return;
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				if (!video_uids[i][j].uid.empty())
				{
					int32_t volume_step = GetVolumeStep(video_uids[i][j].audio_volumn);
					std::wstring volume_png = nbase::StringPrintf(L"audio_volume_%d.png", volume_step);
					member_volume_[i][j]->SetVisible(volume_step > 0);
					if (member_volume_[i][j]->IsVisible())
					{
						member_volume_[i][j]->SetBkImage(volume_png);
					}
				}
			}
		}

		if (!big_wnd_info_.uid.empty())
		{
			int32_t volume_step = GetVolumeStep(big_wnd_info_.audio_volumn);
			std::wstring volume_png = nbase::StringPrintf(L"audio_volume_%d.png", volume_step);

			big_wnd_member_volume_->SetVisible(volume_step > 0);
			if (big_wnd_member_volume_->IsVisible())
			{
				big_wnd_member_volume_->SetBkImage(volume_png);
			}
		}
	}


	void MultiVideoChatForm::OnVChatVolumeCallback(std::map<std::string, int32_t> map_volume)
	{
		nbase::NAutoLock lock(&data_lock_);
		if (current_status_ != kConnected)
			return;
		if (!big_wnd_info_.uid.empty())
		{
			auto &cur_it = map_volume.find(big_wnd_info_.uid);
			if (cur_it != map_volume.end())
			{
				big_wnd_info_.audio_volumn = cur_it->second;
				map_volume.erase(cur_it);
			}
		}

		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				if (!video_uids[i][j].uid.empty())
				{
					auto &cur_it = map_volume.find(video_uids[i][j].uid);
					if (cur_it != map_volume.end())
					{
						video_uids[i][j].audio_volumn = cur_it->second;
						map_volume.erase(cur_it);
					}
					else
					{
						video_uids[i][j].audio_volumn = 0;
					}
				}
			}
		}
		StdClosure closure = [=]() 
		{
			RefreshMemberAudioStatus();
		};
		Post2UI(closure);
	}

	void  MultiVideoChatForm::ExChangeBitmapControl(int bitmap_i_index,int bitmap_j_index)
	{
		nbase::NAutoLock lock(&data_lock_);
		nim::UserNameCard user_info;
		std::string big_wnd_uid = video_ctrl_screen_->GetAccount();
		video_ctrl_preview_[bitmap_i_index][bitmap_j_index]->SetAccount(big_wnd_uid);
		std::wstring show_text = L"";
		if (!UserService::GetInstance()->GetUserInfo(big_wnd_uid, user_info))
		{
			members_info_error_.insert(big_wnd_uid);
			QLOG_APP(L"get user info {0} failed") << video_uids[bitmap_i_index][bitmap_j_index].uid;
		}

		//show_text = nbase::StringPrintf("%s-%s", user_info.GetName().c_str(), big_wnd_uid.c_str());
		show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
		lb_member_name_[bitmap_i_index][bitmap_j_index]->SetText(show_text);
		video_ctrl_screen_->SetAccount(video_uids[bitmap_i_index][bitmap_j_index].uid);
		QLOG_APP(L"ExChangeBitmapControl big:uid-{0}-video-{1} small:uid-{2}-video-{3}") << big_wnd_uid << big_wnd_info_.video << video_uids[bitmap_i_index][bitmap_j_index].uid << video_uids[bitmap_i_index][bitmap_j_index].video;

		if (!UserService::GetInstance()->GetUserInfo(video_uids[bitmap_i_index][bitmap_j_index].uid, user_info))
		{
			members_info_error_.insert(video_uids[bitmap_i_index][bitmap_j_index].uid);
			QLOG_APP(L"get user info {0} failed") << video_uids[bitmap_i_index][bitmap_j_index].uid;
		}
		//show_text = nbase::StringPrintf("%s-%s", user_info.GetName().c_str(), video_uids[bitmap_i_index][bitmap_j_index].uid.c_str());
		show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
		lb_ctrl_screen_->SetText(show_text);
		VideoUidsInfo temp_info = video_uids[bitmap_i_index][bitmap_j_index];
		video_uids[bitmap_i_index][bitmap_j_index] = big_wnd_info_;
		big_wnd_info_ = temp_info;
		if (video_uids[bitmap_i_index][bitmap_j_index].video)
		{
			small_camera_page_tip_[bitmap_i_index][bitmap_j_index]->SetVisible(false);
			video_ctrl_preview_[bitmap_i_index][bitmap_j_index]->SetBkImage(L"");
		}
		else
		{
			std::wstring image = PhotoService::GetInstance()->GetUserPhoto(video_uids[bitmap_i_index][bitmap_j_index].uid, false);
			video_ctrl_preview_[bitmap_i_index][bitmap_j_index]->SetBkImage(image);
			//camera_page_tip[bitmap_i_index][bitmap_j_index]->SetBkImage(image);
			//camera_page_tip[bitmap_i_index][bitmap_j_index]->SetVisible(true);
		}
		QLOG_APP(L"ExChangeBitmapControl video{0}") << big_wnd_info_.video;
		camera_page_tip_->SetVisible(!big_wnd_info_.video);
		

	}

	void  MultiVideoChatForm::SetTopMast(bool forever)
	{
		ToTopMost(this->m_hWnd, forever);
		std::wstring text_id = forever ? L"STRING_MULTIVIDEOCHATFORM_CANCEL_FRONT_PROMPT" : L"STRING_MULTIVIDEOCHATFORM_FRONT_PROMPT";
		btn_win_top_->SetToolTipTextId(text_id);
		std::wstring control_class = forever ? L"WinTop2" : L"WinTop1";
		btn_win_top_->SetClass(control_class);
	}

	bool MultiVideoChatForm::OnClicked(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"end_chat")
		{
			EnterEndCallPage(END_CALL_HANGUP);
		}
		else if (name == L"add_member")
		{
			//弹出群成员选择框
			//群视频人员选择界面
			if (talking_members_.size() >= MULTIVCHATMEMBERCOUNT)
			{
				ShowLimitNotify(false);
			}
			else
			{
				std::string wnd_id = session_id_;
				ContactSelectForm *contact_select_form = (ContactSelectForm *)WindowsManager::GetInstance()->GetWindow(ContactSelectForm::kClassName, nbase::UTF8ToUTF16(wnd_id));
				if (!contact_select_form)
				{
					std::list<UTF8String> exnclude_ids;
					contact_select_form = new ContactSelectForm(wnd_id, exnclude_ids, nbase::Bind(&MultiVideoChatForm::SelectedCompleted, this, std::placeholders::_1, std::placeholders::_2, false), false, true);
					contact_select_form->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
					contact_select_form->CenterWindow();
				}
				contact_select_form->ActiveWindow();
			}
		
		}
		else if (name == L"microphone")
		{
			int v = nim::VChat::GetAudioVolumn(true);
			input_volumn_slider_->SetValue(v);
			vbox_of_input_->SetVisible(true);
			input_volumn_slider_->SetFocus();
		}
		else if (name == L"speaker")
		{
			int v = nim::VChat::GetAudioVolumn(false);
			output_volumn_slider_->SetValue(v);
			vbox_of_output_->SetVisible(true);
			output_volumn_slider_->SetFocus();
		}
		else if (name == L"win_top")
		{
			static bool forever = false;
			forever = !forever;
			SetTopMast(forever);
			
		}
		else if (name == L"close_btn")
		{
			BeforeClose();
		}
		else if (name == L"swap_photo_1_1")
		{
			if (video_uids[0][0].status == kConnected)
				ExChangeBitmapControl(0, 0);
		}
		else if (name == L"swap_photo_1_2") 
		{
			if (video_uids[0][1].status == kConnected)
				ExChangeBitmapControl(0, 1);
		}
		else if (name == L"swap_photo_1_3")
		{
			if (video_uids[0][2].status == kConnected)
				ExChangeBitmapControl(0, 2);
		}
		else if (name == L"swap_photo_1_4")
		{
			if (video_uids[0][3].status == kConnected)
				ExChangeBitmapControl(0, 3);
		}
		else if (name == L"swap_photo_2_1")
		{
			if (video_uids[1][0].status == kConnected)
				ExChangeBitmapControl(1, 0);
		}
		else if (name == L"swap_photo_2_2")
		{
			if (video_uids[1][1].status == kConnected)
				ExChangeBitmapControl(1, 1);
		}
		else if (name == L"swap_photo_2_3")
		{
			if (video_uids[1][2].status == kConnected)
				ExChangeBitmapControl(1, 2);
		}
		else if (name == L"swap_photo_2_4")
		{
			if (video_uids[1][3].status == kConnected)
				ExChangeBitmapControl(1, 3);
		}
		else if (name == L"go_setting")
		{
			VideoManager::GetInstance()->ShowVideoSetting();
		}
		return true;
	}

	bool MultiVideoChatForm::OnUnSelect(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"camera")
		{
			//bool select = cb_camera_->IsSelected();
			//if (!select)
			{
				camera_is_open_ = false;
				VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeChat, true);
				current_video_mode_ = false;
				VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
				SendNotifyToMember(session_id_, true);
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				AdjustVideoBitmap(current_user_id, false, false, kConnected);
				//ClearBitmapControl(video_ctrl_screen_); todo
				hbox_camera_fail_->SetVisible(false);
			}
			//camera_is_open_ = select;
			InitCbCameraSetting();
		}
		return false;
	}

	bool MultiVideoChatForm::OnSelect(ui::EventArgs* arg)
	{
		std::wstring name = arg->pSender->GetName();
		if (name == L"camera")
		{
			bool select = cb_camera_->IsSelected();
			if (select)
			{
				if (CheckVChatMembers() >= MULTIVCHATVIDEOCOUNT)
				{
					ShowLimitNotify();
					camera_is_open_ = false;
					//InitSetting();
				}
				else
				{
					std::string device;
					int no = 0;
					VideoManager::GetInstance()->GetDefaultDevicePath(no, device, nim::kNIMDeviceTypeVideo);
					if (!device.empty())
					{
						VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, device, kDeviceSessionTypeChat);
					}
					else
					{
						cb_camera_->SetEnabled(false);
						current_video_mode_ = false;
						VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
						SendNotifyToMember(session_id_, true);
					}
				}
			}
			//camera_is_open_ = select;
			//InitSetting();
			InitCbCameraSetting();
		}
		return false;
	}



	void MultiVideoChatForm::OnVideoInDeviceChange(uint32_t status)
	{

		if (current_status_==kConnected)
		{
			if (status != nim::kNIMDeviceStatusNoChange)
			{
				InitSetting();
			}
			if (status & nim::kNIMDeviceStatusStart)
			{
				hbox_camera_fail_->SetVisible(false);
				camera_is_open_ = true;
				cb_camera_->SetEnabled();
				cb_camera_->Selected(true, false);
				AdjustVideoBitmap(LoginManager::GetInstance()->GetAccount(), false, true, kConnected);
				current_video_mode_ = true;
				VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeVideo);
				SendNotifyToMember(session_id_, true);
			}
			else if (status & nim::kNIMDeviceStatusEnd)
			{
				camera_is_open_ = false;
				cb_camera_->Selected(false, false);
				AdjustVideoBitmap(LoginManager::GetInstance()->GetAccount(), false, false, kConnected);
				current_video_mode_ = false;
				VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
				SendNotifyToMember(session_id_, true);
			}
			else
				return;
		}
	}

	void MultiVideoChatForm::OnAudioInDeviceChange(uint32_t status)
	{
		if (current_status_ == kConnected)
		{
			if (status != nim::kNIMDeviceStatusNoChange)
			{
				InitVolumnSetting();
			}
			if (status & nim::kNIMDeviceStatusStart)
			{

			}
			else if (status & nim::kNIMDeviceStatusEnd)
			{
				
			}
			else
				return;
		}
	}

	void MultiVideoChatForm::OnVideoDeviceStartCallback(bool result)
	{
		StdClosure closure = [=]()
		{
			cb_camera_->SetEnabled(true);
			camera_is_open_ = result;
			if (!result)
			{
				current_video_mode_ = false;
				VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
				SendNotifyToMember(session_id_, true);
				hbox_camera_fail_->SetVisible(true);
			}
			else
			{
				current_video_mode_ = true;
				VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeVideo);
				SendNotifyToMember(session_id_, true);
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				AdjustVideoBitmap(current_user_id, false, true, kConnected);
				
			}
			InitCbCameraSetting();
		};
		Post2UI(closure);
	}

	void MultiVideoChatForm::OnAudioDeviceStartCallback(bool result)
	{
		if (result)
		{
			//jitter_buffer_.reset(new Jitter);
			//jitter_buffer_->init(1);
		}
		else
		{
			//#ifndef DEBUG
			//		EnterEndCallPage( END_CALL_BAD_MICROPHONE );
			//#endif
		}
	}

	void MultiVideoChatForm::StartCameraDevice()
	{
		int count = 0;
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				if (!(video_uids[i][j].uid.empty()) && video_uids[i][j].video)
				{
					count++;
				}
			}
		}
		QLOG_APP(L"StartCameraDevice count:{0}") << count;
		if (count < 4)
		{
			InitVideo();
		}
		else
		{
			QLOG_ERR(L"StartCameraDevice count:{0}") << count;
		}
	}

	void MultiVideoChatForm::EndCallTimer(std::string uid)
	{
		for (int i = 0;i < 2;i++)
		{
			for (int j = 0;j < 4;j++)
			{
				if (video_uids[i][j].uid == uid)
				{
					QLOG_APP(L"Start Call Timer{0}") << uid;
					video_uids[i][j].call_timer_.Cancel();
				}
			}
		}
	}

	//启动呼叫倒计时
	void MultiVideoChatForm::StartCallTimer(VideoUidsInfo& uid_info)
	{
		uid_info.call_timer_.Cancel();
		StdClosure closure = [=]()
		{
			if (uid_info.status == kWaiting)
			{
				QLOG_APP(L"Start Call Timer{0}") << uid_info.uid;
				AdjustVideoBitmap(uid_info.uid, true, false, kRefuse);
				nbase::NAutoLock lock(&invited_members_lock_);
				invited_members_.erase(uid_info.uid);
			}
		};
		nbase::ThreadManager::PostDelayedTask(kThreadUI, uid_info.call_timer_.ToWeakCallback(closure), nbase::TimeDelta::FromSeconds(45));
	}


	void MultiVideoChatForm::PaintVideo()
	{
		//current_video_mode_ = true;
		//if (current_video_mode_)
		{
			bool screen_is_other_ = false;
			video_ctrl_screen_->SetVideoFrameMng(&nim_comp::VideoManager::GetInstance()->video_frame_mng_);
			for (int i = 0;i < 2;i++)
			{
				for (int j = 0;j < 4;j++)
					video_ctrl_preview_[i][j]->SetVideoFrameMng(&nim_comp::VideoManager::GetInstance()->video_frame_mng_);
			}
			UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
			screen_is_other_ = !(big_wnd_info_.uid == current_user_id);
			if (video_ctrl_screen_->Refresh(this, !screen_is_other_, !screen_is_other_))
			{

			}
			if (!big_wnd_info_.video)
			{
				video_ctrl_screen_->Clear();
			}

			for (int i = 0;i < 2;i++)
			{
				for (int j = 0;j < 4;j++)
				{
					screen_is_other_ = !(video_uids[i][j].uid == current_user_id);
					if (video_ctrl_preview_[i][j]->Refresh(this, !screen_is_other_, !screen_is_other_))
					{

					}

					if (!video_uids[i][j].video)
					{
						video_ctrl_preview_[i][j]->Clear();
					}
				}
			}

			if (!big_wnd_info_.video)
			{
				video_ctrl_screen_->Clear();
			}
		}
	}

	void MultiVideoChatForm::AdjustCreatorVideoBitmap(std::string uid)
	{
		QLOG_APP(L"AdjustCreatorVideoBitmap uid-{0} creator-{1}") << uid << creator_id_;
		if (uid == creator_id_)
		{
			for (int i = 0;i<2;i++)
				for (int j = 0;j<4;j++)
				{
					if (uid == video_uids[i][j].uid)
					{
						QLOG_APP(L"exchange creator i:{0} j:{1}") << i << j;
						ExChangeBitmapControl(i, j);
					}
				}
		}
	}

	void MultiVideoChatForm::OnPeopleJoin(std::string uid)
	{
		//people status:0,json:{"uid":"leewp04"}
		//QLOG_APP(L"people status:{0},json:{1}") << code << json;
		//发送消息5
		QLOG_APP(L"people join:uid-{0} creator_id-{1}") << uid << creator_id_;
		bool video = false;
		start_call_timer_.Cancel();
		SendNotifyToMember(uid,false);
		RefreshTalkingMembers(uid, false);
		EndCallTimer(uid);
		voip_ring_.Stop();
		if (cache_members_info_.count(uid) > 0)
		{
			video = cache_members_info_[uid];
			cache_members_info_.erase(uid);
		}
		AdjustVideoBitmap(uid, false, video, kConnected);
		AdjustCreatorVideoBitmap(uid);
	}

	void MultiVideoChatForm::OnPeopleLeave(std::string uid)
	{
		nbase::NAutoLock lock(&invited_members_lock_);
		QLOG_APP(L"people leave:{0}") << uid;
		AdjustVideoBitmap(uid, true, false,kDisConnect);
		RefreshTalkingMembers(uid, true);
		invited_members_.erase(uid);
	}

	void MultiVideoChatForm::SendNotifyToMember(std::string uid,bool team)
	{
		Json::FastWriter fw;
		int index = 0;
		UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
		Json::Value value;
		value["teamId"] = session_id_;
		value["channelId"] = channel_id_;
		value["roomName"] = room_name_;
		value["video"] = current_video_mode_;
		value["id"] = team ? kMsgMultiVchatTeamChangeMode : kMsgMultiVchatP2PChangeMode;
		std::string json_value = fw.write(value);
		//发送自定义消息给用户
		nim::SysMessage temp_msg;
		temp_msg.sender_accid_ = current_user_id;
		temp_msg.client_msg_id_ = QString::GetGUID();
		temp_msg.attach_ = json_value;
		temp_msg.type_ = team ? nim::kNIMSysMsgTypeCustomTeamMsg : nim::kNIMSysMsgTypeCustomP2PMsg;
		temp_msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
		temp_msg.receiver_accid_ = team ? session_id_ : uid;
		std::string msg_info = temp_msg.ToJsonString();
		QLOG_APP(L"send MsgMultiVchatTeamChangeMode or MsgMultiVchatP2PChangeMode:{0}") << msg_info;
		nim::SystemMsg::SendCustomNotificationMsg(msg_info);
	}

	void MultiVideoChatForm::OnSetShowBitmap(std::string uid, bool video,bool refuse)
	{
		VchatMemberStatus status = kConnected;
		bool remove = false;
		if (refuse)
		{
			remove = true;
			status = kRefuse;
		}
		QLOG_APP(L"On SetShowBitmap uid:{0},video:{1},refuse:{2}") << uid << video << refuse;
		AdjustVideoBitmap(uid, remove, video, status);
	}

	void MultiVideoChatForm::ShowLimitNotify(bool video/*=true*/)
	{
		std::wstring content_id;
		content_id = L"STRING_MULTIVIDEOCHATFORM_VIDEO_LIMIT_PROMPT";
		if (!video)
		{
			content_id = L"STRING_MULTIVIDEOCHATFORM_LIMIT_PROMPT";
		}
		MsgboxCallback mb = [](MsgBoxRet ret) {

		};
		ShowMsgBox(m_hWnd, mb, content_id, true, L"STRING_MULTIVIDEOCHATFORM_TITLE_PROMPT", true, L"STRING_OK");
	}


	int MultiVideoChatForm::GetVideoCount()
	{
		int video_count = 0;
		int chating_count = 0;
		int invited_videos = 0;
		nbase::NAutoLock lock(&invited_members_lock_);
		nbase::NAutoLock lock_2(&data_lock_);
		std::map<std::string, bool>::iterator it = invited_members_.begin();
		int repeat_count = 0;
		for (int temp_i = 0;temp_i < 2;temp_i++)
		{
			for (int temp_j = 0;temp_j < 4;temp_j++)
			{
				if ((!video_uids[temp_i][temp_j].uid.empty())&&video_uids[temp_i][temp_j].video == true)
				{
					QLOG_APP(L"vchat  member uid:{0},video{1}") << video_uids[temp_i][temp_j].uid << video_uids[temp_i][temp_j].video;
					chating_count++;
					if (invited_members_.count(video_uids[temp_i][temp_j].uid) > 0)
					{
						repeat_count++;
					}
				}
			}
		}
		if ((!big_wnd_info_.uid.empty()) && big_wnd_info_.video == true)
		{
			chating_count++;
			if (invited_members_.count((big_wnd_info_.uid)) > 0)
			{
				repeat_count++;
			}
		}
	
		for (;it != invited_members_.end();it++)
		{
			if (it->second == true)
			{
				QLOG_APP(L"invited  member uid:{0},video{1}") << it->first << it->second;
				invited_videos++;
			}
		}

		video_count = chating_count + invited_videos - repeat_count;
		QLOG_APP(L"get video count:{0} chating:{1},invited_videos:{2},repeat_count:{3}") << video_count << chating_count << invited_videos << repeat_count;
		return video_count;
	}

	int MultiVideoChatForm::CheckVChatMembers()
	{
		nbase::NAutoLock lock(&data_lock_);
		int vchat_members = 0;
		for (int temp_i = 0;temp_i < 2;temp_i++)
		{
			for (int temp_j = 0;temp_j < 4;temp_j++)
			{
				if (video_uids[temp_i][temp_j].video == true)
				{
					QLOG_APP(L"vchat  member uid:{0},video{1}") << video_uids[temp_i][temp_j].uid << video_uids[temp_i][temp_j].video;
					vchat_members++;
				}
			}
		}
		if (big_wnd_info_.video == true)
		{
			vchat_members++;
		}
		QLOG_APP(L"Check VChat Members:{0}") << vchat_members;
		return vchat_members;
	}

	void MultiVideoChatForm::AdjustVideoBitmap(std::string uid, bool remove, bool video, VchatMemberStatus status)
	{
		nbase::NAutoLock lock(&data_lock_);
		bool find = false;
		int find_i_index = -1;
		int find_j_index = -1;
		int find_unnormal_i_index = -1;
		int find_unnormal_j_index = -1;
		int find_waiting_i_index = -1;
		int find_waiting_j_index = -1;
		bool local = false;
		if (big_wnd_info_.uid == uid)
		{
			if (remove)
			{
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				uid = current_user_id;
				local = true;
			}
			else
			{
				big_wnd_info_.video = video;
				camera_page_tip_->SetVisible(!video);
				return;
			}
		}

		for (int temp_i = 0;temp_i < 2;temp_i++)
		{
			for (int temp_j = 0;temp_j < 4;temp_j++)
			{
				//找到第一个的位置
				if (video_uids[temp_i][temp_j].uid.empty())
				{
					if (find_i_index == -1)
						find_i_index = temp_i;
					if (find_j_index == -1)
						find_j_index = temp_j;
				}
				else if (video_uids[temp_i][temp_j].status == kRefuse || video_uids[temp_i][temp_j].status == kDisConnect)
				{
					if (find_unnormal_i_index == -1)
						find_unnormal_i_index = temp_i;
					if (find_unnormal_j_index == -1)
						find_unnormal_j_index = temp_j;
				}
				else if (video_uids[temp_i][temp_j].status == kWaiting)
				{
					if (find_waiting_i_index == -1)
						find_waiting_i_index = temp_i;
					if (find_waiting_j_index == -1)
						find_waiting_j_index = temp_j;
				}

				if (video_uids[temp_i][temp_j].uid == uid)
				{
					find = true;
					find_i_index = temp_i;
					find_j_index = temp_j;
					break;
				}
			}
		}

		if (remove)
		{
			if (find)
			{
				int temp_i = find_i_index, temp_j = find_j_index;
				std::wstring show_text = L"";
				if (local)
				{
					//大窗口显示正在聊天的人
					VideoUidsInfo temp_info = big_wnd_info_;
					nim::UserNameCard user_info;
					if (!UserService::GetInstance()->GetUserInfo(video_uids[temp_i][temp_j].uid, user_info))
					{
						members_info_error_.insert(video_uids[temp_i][temp_j].uid);
						QLOG_APP(L"get user info error {0}") << video_uids[temp_i][temp_j].uid;

					}
					show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
					lb_ctrl_screen_->SetText(show_text);
					video_ctrl_screen_->SetAccount(video_uids[temp_i][temp_j].uid);
					big_wnd_info_ = video_uids[temp_i][temp_j];
					video_uids[temp_i][temp_j] = temp_info;
				}
				nim::UserNameCard user_info;
				if (!UserService::GetInstance()->GetUserInfo(video_uids[temp_i][temp_j].uid, user_info))
				{
					members_info_error_.insert(video_uids[temp_i][temp_j].uid);
					QLOG_APP(L"get user info error {0}") << video_uids[temp_i][temp_j].uid;
				}
				show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
				lb_member_name_[temp_i][temp_j]->SetText(show_text);
				video_ctrl_preview_[temp_i][temp_j]->SetAccount(video_uids[temp_i][temp_j].uid);
				video_ctrl_preview_[temp_i][temp_j]->SetVisible(true);
				video_uids[temp_i][temp_j].video = false;
				video_uids[temp_i][temp_j].audio_volumn = 0;
				video_uids[temp_i][temp_j].status = status;
				//std::wstring image = PhotoService::GetInstance()->GetUserPhoto(uid, false); 头像感觉不合理，用默认图片
				//std::wstring image = L"video_close_small.png";
				//video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(image);
				std::wstring image = L"video_close_small.png";
				video_ctrl_preview_[temp_i][temp_j]->SetBkImage(L"");
				small_camera_page_tip_[temp_i][temp_j]->SetBkImage(image);
				small_camera_page_tip_[temp_i][temp_j]->SetVisible(true);
				swap_photo_btn_[temp_i][temp_j]->SetVisible(false);
				if (status == kDisConnect)
				{
					lb_member_status_prompt_[temp_i][temp_j]->SetTextId(L"STRING_MULTIVIDEOCHATFORM_HANGUP");
					lb_member_status_prompt_[temp_i][temp_j]->SetVisible(true);
				}
				else if (status == kRefuse)
				{
					lb_member_status_prompt_[temp_i][temp_j]->SetTextId(L"STRING_MULTIVIDEOCHATFORM_REFUSE");
					lb_member_status_prompt_[temp_i][temp_j]->SetVisible(true);
				}
				camera_page_tip_->SetVisible(!big_wnd_info_.video);
			}
		}
		else
		{
			if (find_i_index == -1)
			{
				if (find_unnormal_i_index != -1)
					find_i_index = find_unnormal_i_index;
				else
					find_i_index = find_waiting_i_index;
			}

			if (find_j_index == -1)
			{
				if (find_unnormal_j_index != -1)
					find_j_index = find_unnormal_j_index;
				else
					find_j_index = find_waiting_j_index;
			}
			//if (find_j_index == -1) find_j_index = find_unnormal_j_index;
			if (find_i_index >= 0 && find_j_index >= 0)
			{
				nim::UserNameCard user_info;
				if (!UserService::GetInstance()->GetUserInfo(uid, user_info))
				{
					QLOG_APP(L"user service get user info error {0}") << uid;
					members_info_error_.insert(uid);
				}
				std::string show_text = user_info.GetName();
				lb_member_name_[find_i_index][find_j_index]->SetUTF8Text(show_text);
				video_ctrl_preview_[find_i_index][find_j_index]->SetAccount(uid);
				video_uids[find_i_index][find_j_index].uid = uid;
				video_uids[find_i_index][find_j_index].video = video;
				video_uids[find_i_index][find_j_index].status = status;
				video_ctrl_preview_[find_i_index][find_j_index]->SetVisible(true);
				
				if (!video)
				{
					if (status != kWaiting)
					{
						std::wstring image = PhotoService::GetInstance()->GetUserPhoto(uid, false);
						small_camera_page_tip_[find_i_index][find_j_index]->SetBkImage(L"");
						video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(image);
					}
					else
					{
						video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(L"");
						std::wstring image = L"video_close_small.png";
						small_camera_page_tip_[find_i_index][find_j_index]->SetBkImage(image);
					}
				}
				else
				{
					video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(L"");
				}
				small_camera_page_tip_[find_i_index][find_j_index]->SetVisible(!video);
				//if (!video)
				//{
					//std::wstring image = PhotoService::GetInstance()->GetUserPhoto(uid, false);
					//std::wstring image = L"video_close_small.png";
					//video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(image);
					
				//}
				if (status == kWaiting)
				{
					lb_member_status_prompt_[find_i_index][find_j_index]->SetTextId(L"STRING_MULTIVIDEOCHATFORM_WAITING");
					lb_member_status_prompt_[find_i_index][find_j_index]->SetVisible(true);
					StartCallTimer(video_uids[find_i_index][find_j_index]);
				}
				else
				{
					lb_member_status_prompt_[find_i_index][find_j_index]->SetVisible(false);
				}
			}
			
		}
		QLOG_APP(L"uid:{0},remove{1},video:{2},status:{3} i:{4} j:{5}") << uid << remove << video << status << find_i_index << find_j_index;
		AdjustWindowSize();
	}

	void MultiVideoChatForm::AdjustVideoBitmap2(std::string uid, bool remove, bool video, VchatMemberStatus status)
	{
		nbase::NAutoLock lock(&data_lock_);
		QLOG_APP(L"uid:{0},remove{1},video:{2},status:{3}") << uid << remove << video << status;
		bool find = false;
		int find_i_index = -1;
		int find_j_index = -1;
		bool local = false;
		if (big_wnd_info_.uid == uid)
		{
			if (remove)
			{
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				uid = current_user_id;
				local = true;
			}
			else
			{
				big_wnd_info_.video = video;
				return;
			}
		}
		
		for (int temp_i = 0;temp_i < 2;temp_i++)
		{
			for (int temp_j = 0;temp_j < 4;temp_j++)
			{
				if (video_uids[temp_i][temp_j].uid.empty())
				{
					if (find_i_index == -1)
						find_i_index = temp_i;
					if (find_j_index == -1)
						find_j_index = temp_j;
				}
				
				if (video_uids[temp_i][temp_j].uid == uid)
				{
					find = true;
					find_i_index = temp_i;
					find_j_index = temp_j;
					break;
				}
			}
		}

		if (remove)
		{
			if (find)
			{
				std::wstring show_text = L"";
				int temp_i = find_i_index, temp_j = find_j_index;
				if (local)
				{
					nim::UserNameCard user_info;
					UserService::GetInstance()->GetUserInfo(video_uids[temp_i][temp_j].uid, user_info);
					show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
					lb_ctrl_screen_->SetText(show_text);
					video_ctrl_screen_->SetAccount(video_uids[temp_i][temp_j].uid);
					big_wnd_info_ = video_uids[temp_i][temp_j];
				}
				lb_member_name_[temp_i][temp_j]->SetUTF8Text("");
				video_ctrl_preview_[temp_i][temp_j]->SetAccount("");
				video_ctrl_preview_[temp_i][temp_j]->SetVisible(false);
				video_uids[temp_i][temp_j].uid = "";
				video_uids[temp_i][temp_j].video = false;
				video_uids[temp_i][temp_j].audio_volumn = 0;

				//后续的往前移
				int i = temp_i, j = temp_j;
				temp_j++;
				if (temp_j == 4)
				{
					temp_i++;
					temp_j = 0;
				}
				while (temp_i < 2 && temp_j < 4)
				{
					if (!video_uids[temp_i][temp_j].uid.empty())
					{
						nim::UserNameCard user_info;
						UserService::GetInstance()->GetUserInfo(video_uids[i][j].uid, user_info);
						show_text = OnHandlerUserName(nbase::UTF8ToUTF16(user_info.GetName()));
						lb_member_name_[i][j]->SetText(show_text);
						video_uids[i][j] = video_uids[temp_i][temp_j];
						video_ctrl_preview_[i][j]->SetAccount(video_uids[i][j].uid);
						video_ctrl_preview_[i][j]->SetVisible(true);
						video_uids[temp_i][temp_j].uid = "";
						video_ctrl_preview_[temp_i][temp_j]->SetAccount("");
						video_ctrl_preview_[temp_i][temp_j]->SetVisible(false);
						video_uids[temp_i][temp_j].uid = "";
						video_uids[temp_i][temp_j].video = false;
						video_uids[temp_i][temp_j].audio_volumn = 0;
						i = temp_i;
						j = temp_j;
					}

					temp_j++;
					if (temp_j == 4)
					{
						temp_i++;
						temp_j = 0;
					}
				}
			}
		}
		else
		{
			lb_member_name_[find_i_index][find_j_index]->SetUTF8Text(uid);
			video_ctrl_preview_[find_i_index][find_j_index]->SetAccount(uid);
			video_uids[find_i_index][find_j_index].uid = uid;
			video_uids[find_i_index][find_j_index].video = video;
			video_ctrl_preview_[find_i_index][find_j_index]->SetVisible(true);
			small_camera_page_tip_[find_i_index][find_j_index]->SetVisible(!video);
			//if (!video)
			//{
				//std::wstring image = PhotoService::GetInstance()->GetUserPhoto(uid, false);
				//std::wstring image = L"video_close_small.png";
				//video_ctrl_preview_[find_i_index][find_j_index]->SetBkImage(image);
				//camera_page_tip[find_i_index][find_j_index]->SetVisible(true);
			//}
			//else
			//{
			//	camera_page_tip[find_i_index][find_j_index]->SetVisible(false);
			//}
		}

		AdjustWindowSize();
	}


	std::map<std::string,bool> MultiVideoChatForm::GetInvitedMember()
	{
		return invited_members_;
	}

	void MultiVideoChatForm::SetInvitedMember(std::string Member,bool video)
	{
		nbase::NAutoLock lock(&invited_members_lock_);
		invited_members_[Member] = video;
	}


	void MultiVideoChatForm::SetInvitingMember(std::set<std::string> inviting_members)
	{
		auto closure = [=]()
		{
			std::set<std::string>::iterator it = inviting_members.begin();
			for (;it != inviting_members.end();it++)
			{
				QLOG_APP(L"Set Inviting Member");
				AdjustVideoBitmap(*it, false, false, kWaiting);
			}
		};
		Post2UI(closure);
	}

	void MultiVideoChatForm::AdjustWindowSize()
	{
		int member_count = 0;
		for (int temp_i = 0;temp_i < 2;temp_i++)
		{
			for (int temp_j = 0;temp_j < 4;temp_j++)
			{
				if (!video_uids[temp_i][temp_j].uid.empty())
				{
					member_count++;
				}
			}
			int FORM_CX = 0, FORM_CY = 0;
			if (member_count > 4)
			{
				FORM_CX = 802;
				FORM_CY = 472;
			}
			else
			{
				FORM_CX = 672;
				FORM_CY = 472;
			}
			this->SetMinInfo(FORM_CX, FORM_CY);

			if (IsZoomed(m_hWnd))
			{
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			}

			UiRect dest(0, 0, FORM_CX, FORM_CY);
			this->SetPos(dest, true, SWP_NOMOVE);
		}
	}

	void MultiVideoChatForm::RefreshTalkingMembers(std::string uid, bool remove)
	{
		if (remove)
		{
			talking_members_.erase(uid);
		}
		else
		{
			talking_members_.insert(uid);
		}
	}

	void MultiVideoChatForm::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
	{
		nbase::NAutoLock lock(&data_lock_);
		for (const auto &info : uinfos)
		{
			if (members_info_error_.count(info.GetAccId())>0)
			{
				std::string show_text = "";
				if (big_wnd_info_.uid == info.GetAccId())
				{
					show_text = nbase::StringPrintf("%s-%s", info.GetName().c_str(), big_wnd_info_.uid.c_str());
					lb_ctrl_screen_->SetUTF8Text(show_text);
					members_info_error_.erase(big_wnd_info_.uid);
					continue;
				}

				for (int i = 0;i < 2;i++)
				{
					for (int j = 0;j < 4;j++)
					{
						if (video_uids[i][j].uid == info.GetAccId())
						{
							show_text = nbase::StringPrintf("%s-%s", info.GetName().c_str(), big_wnd_info_.uid.c_str());
							lb_member_name_[i][j]->SetUTF8Text(show_text);
							members_info_error_.erase(info.GetAccId());
							continue;
						}
					}
				}

			}
		}
	}


	std::wstring MultiVideoChatForm::OnHandlerUserName(std::wstring user_name)
	{
		if (user_name.length() > 6)
		{
			user_name = (user_name.substr(0, 6)).append(L"...");
		}
		return user_name;
	}

}
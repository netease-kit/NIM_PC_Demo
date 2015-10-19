#include "video_form.h"
#include "util/windows_manager.h"
#include "module/video/video_manager.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR VideoForm::kClassName = L"VideoForm";

const wchar_t kVoipConnecting[] = L"voip_connecting.mp3";
const wchar_t kVoipNoResponse[] = L"voip_no_response.mp3";
const wchar_t kVoipPeerBusy[]	= L"voip_peer_busy.mp3";
const wchar_t kVoipPeerReject[] = L"voip_peer_reject.mp3";
const wchar_t kVoipRing[]		= L"voip_ring.mp3";

VideoForm::VideoForm(std::string session_id) : session_id_(session_id)
{
	mic_in_open_	  = true;
	speaker_out_open_ = true;
	status_			 = STATUS_NONE;
	channel_id_		 = 0;
	screen_is_other_ = true;
	current_video_mode_ = false;
	is_self_ = true;

	end_call_ = END_CALL_NONE;

	send_camera_notify_ = false;
	is_mode_changing_   = false;

	camera_is_open_ = true;
	camera_is_open_other_ = true;

	need_change_form_size_ = true;

	is_max_window_ = false;
}

VideoForm::~VideoForm()
{
}

::ui::UILIB_RESOURCETYPE VideoForm::GetResourceType() const
{
	return ui::UILIB_FILE; 
}

std::wstring VideoForm::GetZIPFileName() const
{
	return (L"videoform.zip");
}

std::wstring VideoForm::GetSkinFolder()
{
	return L"video";
}

std::wstring VideoForm::GetSkinFile()
{
	return L"videoform.xml";
}

ui::Control* VideoForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BitmapControl"))
	{
		return new ui::CBitmapControl();
	}
	return NULL;
}

LRESULT VideoForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_SIZE)
	{
		if(wParam == SIZE_MINIMIZED) 
		{
			// Nothing to do
		} 
		else if(wParam == SIZE_RESTORED) 
		{
			if(is_max_window_)
				OnMaxWindow(false);
		}
		else if(wParam == SIZE_MAXIMIZED) 
		{
			OnMaxWindow(true);
		}
	}
	else if( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
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

void VideoForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&VideoForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&VideoForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&VideoForm::OnSelect, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&VideoForm::OnSelect, this, std::placeholders::_1));

	status_page_ = (Box*) FindControl(L"status_page");
	video_page_  = (Box*) FindControl(L"video_page");
	volumn_page_ = (Box*) FindControl(L"volumn_page");
	confirm_page_ = (Box*) FindControl(L"confirm_page");

	headicon_btn_   = (Button*) FindControl(L"headicon");
	friend_label_   = (Label*) FindControl(L"friend_name");
	status_label_   = (Label*) FindControl(L"chat_status");

	video_ctrl_screen_  = (CBitmapControl*) FindControl(L"photo_screen");
	video_ctrl_preview_ = (CBitmapControl*) FindControl(L"photo_preview");
	video_ctrl_preview_->SetAutoSize(true);

	time_tick_label_ = (Label*) FindControl( L"time_tick" );
	camera_open_label_ = (Label*) FindControl( L"camera_opening" );
	camera_closed_label_ = (Label*) FindControl( L"camera_closed" );

	swap_photo_btn_ = (Button*) FindControl( L"swap_photo" );

	video_border_ctrl_ = FindControl( L"video_border" );
	Box* box_of_preview = (Box*) FindControl( L"box_video_preview" );
	ASSERT( box_of_preview );
	box_of_preview->AttachBubbledEvent(kEventMouseEnter, nbase::Bind(&VideoForm::OnMouseMessage, this, std::placeholders::_1));
	box_of_preview->AttachBubbledEvent(kEventMouseLeave, nbase::Bind(&VideoForm::OnMouseMessage, this, std::placeholders::_1));

	camera_page_tip_ = FindControl( L"camera_page_tip" );
	camera_page_tip2_ = FindControl( L"camera_page_tip2" );

	into_video_btn_ = (Button*) FindControl(L"into_video");
	into_audio_btn_	= (Button*) FindControl(L"into_audio");
	stop_chat_btn_  = (Button*) FindControl(L"stop_chat");

	speaker_btn_	 = (Button*) FindControl(L"speaker");
	microphone_btn_  = (Button*) FindControl(L"microphone");
	camera_checkbox_ = (CheckBox*) FindControl(L"camera");

	input_volumn_slider_ = (Slider*) FindControl( L"input_volumn" );
	vbox_of_input_ = (VBox*) FindControl( L"vbox_of_input_volumn" );
	place_ctrl_1_ = FindControl( L"place_ctrl_1" );

	output_volumn_slider_ = (Slider*) FindControl(L"output_volumn");
	vbox_of_output_ = (VBox*) FindControl( L"vbox_of_output_volumn" );
	place_ctrl_2_ = FindControl( L"place_ctrl_2" );

	output_volumn_slider_->AttachKillFocus(nbase::Bind(&VideoForm::OnSpeakerVolumnKillFocus, this, std::placeholders::_1));
	input_volumn_slider_->AttachKillFocus(nbase::Bind(&VideoForm::OnMicrophoneVolumnKillFocus, this, std::placeholders::_1));

	refuse_btn_ = (Button*) FindControl(L"refuse");
	complete_btn_ = (Button*) FindControl(L"complete");

	netstat_label_ = (Label*) FindControl( L"netstat" );
	hbox_camera_fail_ = (HBox*) FindControl( L"hbox_camera_fail" );
	vbox_video_audio_tip_ = (Box*) FindControl( L"vbox_video_audio_tip" );

	AllowWindowMaxsize( false );

	CheckHeadIcon();
	CheckFriendName();

	InitSetting();

	microphone_btn_->SetVisible( true );
	speaker_btn_->SetVisible( true );
	camera_checkbox_->SetVisible( false );

	microphone_btn_->SetEnabled( false );
	speaker_btn_->SetEnabled( false );
	camera_checkbox_->SetEnabled( false );

	voip_ring_.Init(this->GetHWND());
}

void VideoForm::OnFinalMessage(HWND hWnd)
{
	PrepareQuit();

	FreeVideo();
	FreeAudio();

	VideoManager::GetInstance()->EndChat();

	__super::OnFinalMessage(hWnd);
}

void VideoForm::ShowStatusPage( StatusPage sp )
{
	ui::MutiLanSupport* lang = MutiLanSupport::GetInstance();

	status_page_->SetVisible(false);
	confirm_page_->SetVisible(false);
	video_page_->SetVisible(false);
	volumn_page_->SetVisible(false);

	time_tick_label_->SetVisible( false );

	camera_open_label_->SetVisible( false );
	camera_closed_label_->SetVisible( false );

	netstat_label_->SetVisible( false );
	hbox_camera_fail_->SetVisible( false );

	if(sp == SP_DIAL)
	{
		status_page_->SetVisible(true);
		volumn_page_->SetVisible(true);
	}
	else if(sp == SP_CONFIRM)
	{
		status_page_->SetVisible(true);
		confirm_page_->SetVisible(true);

		if(current_video_mode_)
		{
			complete_btn_->SetClass( L"CompleteVideo" );
			refuse_btn_->SetClass( L"RefuseVideo" );

			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_VIDEO"));
			complete_btn_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_ACCEPT_VIDEO"));
		}
		else
		{
			complete_btn_->SetClass( L"CompleteAudio" );
			refuse_btn_->SetClass( L"RefuseAudio" );

			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_AUDIO"));
			complete_btn_->SetText(lang->GetStringViaID(L"STRID_VIDEO_INVITE_ACCEPT_LISTEN"));
		}
	}
	else if(sp == SP_VIDEO)
	{
		time_tick_label_->SetVisible(status_ == STATUS_TALKING);

		video_page_->SetVisible(true);
		volumn_page_->SetVisible(true);
	}
	else
	{
		ASSERT(0);
	}
}

void VideoForm::AdjustWindowSize( bool video )
{
	int FORM_CX = 0, FORM_CY = 0;
	if( video )
	{
		FORM_CX = 500;
		FORM_CY = 410;

		AllowWindowMaxsize( status_ == STATUS_TALKING );
	}
	else
	{
		FORM_CX = 280;
		FORM_CY = 410;

		AllowWindowMaxsize( false );
	}
	this->SetMinInfo( FORM_CX, FORM_CY );

	if( IsZoomed( m_hWnd ) )
	{
		SendMessage( WM_SYSCOMMAND, SC_RESTORE, 0 );
	}

	UiRect dest( 0, 0, FORM_CX, FORM_CY );
	this->SetPos( dest, SWP_NOMOVE );
}

void VideoForm::AllowWindowMaxsize( bool allow )
{
	Button* button = (Button*) FindControl( L"maxbtn" );
	ASSERT( button );
	if( allow )
	{
		button->SetEnabled( true );

		this->SetSizeBox( UiRect( 4, 4, 4, 4 ) );
	}
	else
	{
		button->SetEnabled( false );

		this->SetSizeBox( UiRect( 0, 0, 0, 0 ) );
	}
}

void VideoForm::OnAutoCloseWnd()
{
	DirectQuit();
}

void VideoForm::OnCloseRingEnd( const int32_t mode )
{
	//if( mode == MCI_MODE_STOP )
	//{
	//	end_call_timer_.Cancel();
	//	//OnAutoCloseWnd();
	//	::PostMessage(m_hWnd, WM_MSG_DESTROY_WND, 0, 0);
	//}
}

void VideoForm::EnterEndCallPage( EndCallEnum why )
{
	Control* min = FindControl( L"minbtn" );
	ASSERT( min );
	min->SetEnabled( false );

	Control* max = FindControl( L"maxbtn" );
	ASSERT( max );
	max->SetEnabled( false );

	Control* restore = FindControl( L"restorebtn" );
	ASSERT( restore );
	restore->SetEnabled( false );

	Control* close = FindControl( L"close_btn" );
	ASSERT( close );
	close->SetEnabled( false );

	Label* end_call_tip = (Label*) FindControl( L"end_call_tip" );
	ASSERT( end_call_tip );

	Label* end_call_why = (Label*) FindControl( L"end_call_why" );
	ASSERT( end_call_why );

	Box* end_call_page = (Box*) FindControl( L"end_call_page" );
	ASSERT( end_call_page );

	bool need_save_close = false; //万一铃声播放有问题，那么窗口岂不是无法关闭了，所以加入安全措施，6s后自动关闭

	end_call_ = why;
	switch( why )
	{
	case VideoForm::END_CALL_NONE:
	case VideoForm::END_CALL_CLOSE:
	case VideoForm::END_CALL_HANGUP:
	case VideoForm::END_CALL_BE_HANGUP:
	case VideoForm::END_CALL_SYNC_ACCEPT:
	case VideoForm::END_CALL_SYNC_REFUSE:
	case VideoForm::END_CALL_BAD_MICROPHONE:
	case VideoForm::END_CALL_VERSION:
	case VideoForm::END_CALL_CONNECTION:
	{
		ASSERT( why != END_CALL_NONE );
		if( why == END_CALL_BE_HANGUP )
		{
			//VideoMission* miss = (VideoMission*) tool::WindowsManager::GetInstance()->GetWindow( VideoMission::kClassName, VideoMission::kClassName );
			//if( miss )
			//	::DestroyWindow( miss->GetHWND() );
			end_call_tip->SetTextId( L"STRID_VIDEO_END_CALL_BE_HANGUP" );
		}
		else if( why == END_CALL_SYNC_ACCEPT )
			end_call_tip->SetTextId( L"STRID_VIDEO_END_CALL_SYNC_ACCEPT" );
		else if( why == END_CALL_SYNC_REFUSE )
			end_call_tip->SetTextId( L"STRID_VIDEO_END_CALL_SYNC_REFUSE" );
		else
			end_call_tip->SetTextId( L"STRID_VIDEO_END_CALL_HANGUP" );

		if( why == END_CALL_BAD_MICROPHONE )
		{
			end_call_why->SetTextId( L"STRID_VIDEO_END_CALL_MICROPHONE" );
			end_call_why->SetVisible( true );
		}
		else if(why == END_CALL_VERSION)
		{
			end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_VERSION");
			end_call_why->SetVisible(true);
		}
		else if(why == END_CALL_CONNECTION)
		{
			end_call_why->SetTextId(L"STRID_VIDEO_END_CALL_CONNECTION");
			end_call_why->SetVisible(true);
		}

		StdClosure closure = nbase::Bind(&VideoForm::OnAutoCloseWnd, this);
		nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(3));
		break;
	}
	case VideoForm::END_CALL_RESPONSE:
	case VideoForm::END_CALL_BUSYING:
	case VideoForm::END_CALL_REJECT:
	{
		need_save_close = true;
		end_call_tip->SetTextId( L"STRID_VIDEO_END_CALL_HANGUP" );

		//Ring::RingPlayCallback cb = nbase::Bind( &VideoForm::OnCloseRingEnd, this, std::placeholders::_1 );
		if( why == END_CALL_RESPONSE )
			voip_ring_.Play( RING_VOIP_NORESPONSE);
		else if( why == END_CALL_BUSYING )
			voip_ring_.Play( RING_VOIP_PEERBUSY );
		else if( why == END_CALL_REJECT )
			voip_ring_.Play( RING_VOIP_PEERREJECT );
		break;
	}
	default:
		break;
	}

	end_call_page->SetVisible(true);

	QLOG_APP(L"EnterEndCallPage {0}:{1}") << why << end_call_tip->GetText();

	if( need_save_close )
	{
		auto closure = nbase::Bind( &VideoForm::OnAutoCloseWnd, this );
		nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(6));
	}
}

void VideoForm::BeforeClose()
{
	std::wstring content, button;
	content = (current_video_mode_ ? L"是否要退出当前视频" : L"是否要退出当前通话");
	button = (current_video_mode_ ? L"返回视频" : L"返回通话");
	
	MsgboxCallback mb = nbase::Bind(&VideoForm::OnQuitMsgBox, this, std::placeholders::_1);
	ShowMsgBox(m_hWnd, content, mb, L"提示", L"退出", button);
}

void VideoForm::OnQuitMsgBox( MsgBoxRet ret )
{
	if( ret == MB_YES )
	{
		EnterEndCallPage( END_CALL_CLOSE );
	}
}

void VideoForm::DirectQuit()
{
	DestroyWindow(this->GetHWND());
}

bool VideoForm::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == ui::kEventValueChange)
	{
		if(name == L"output_volumn")
		{
			//VideoManager* vm = VideoManager::GetInstance();

			int v = output_volumn_slider_->GetValue();
			VideoManager::GetInstance()->SetAudioVolumn(v, false);
		}
		else if(name == L"input_volumn")
		{
			//VideoManager* vm = VideoManager::GetInstance( );

			int v = input_volumn_slider_->GetValue();

			VideoManager::GetInstance()->SetAudioInputAutoVolumn(false);
			VideoManager::GetInstance()->SetAudioVolumn(v, true);
		}
	}
	return true;
}

bool VideoForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"stop_chat")
	{
		EnterEndCallPage( END_CALL_HANGUP );
	}
	else if(name == L"into_video")
	{
		ChangeToVideo();
	}
	else if(name == L"swap_photo")
	{
		screen_is_other_ = !screen_is_other_;
		ClearBitmapControl( true );
		ClearBitmapControl( false );

		if( screen_is_other_ && !camera_is_open_other_ )
			camera_closed_label_->SetVisible( true );
		else
			camera_closed_label_->SetVisible( false );
	}
	else if(name == L"into_audio")
	{
		ChangeToAudio();
	}
	else if( name == L"go_setting" )
	{
		//VideoSettingForm* form = tool::WindowsManager::SingletonShow<VideoSettingForm>( VideoSettingForm::kClassName );
		//form->ShowPage( true );
		VideoManager::GetInstance()->ShowVideoSetting();
	}
	else if(name == L"microphone")
	{
		int v = nim::VChat::GetAudioVolumn(true);
		input_volumn_slider_->SetValue(v);

		vbox_of_input_->SetVisible( true );

		input_volumn_slider_->SetFocus();
	}
	else if(name == L"speaker")
	{
		int v = nim::VChat::GetAudioVolumn(false);
		output_volumn_slider_->SetValue(v);

		vbox_of_output_->SetVisible( true );

		output_volumn_slider_->SetFocus();
	}
	else if(name == L"refuse")
	{
		EnterEndCallPage( END_CALL_HANGUP );
	}
	else if(name == L"complete")
	{
		refuse_btn_->SetEnabled(false);
		complete_btn_->SetEnabled(false);

		ShowStatusPage(SP_DIAL);
		SwitchStatus(STATUS_CONNECTING);

		VideoManager::GetInstance()->VChatCalleeAck(channel_id_, true);

		StdClosure closure = nbase::Bind(&VideoForm::OnConnectTimeOut, this);
		closure = connect_timeout_timer_.ToWeakCallback(closure);
		nbase::ThreadManager::PostDelayedTask(closure, nbase::TimeDelta::FromSeconds(kConnectTimeOut));
	}
	else if (name == L"close_btn")
	{
		if (end_call_ == END_CALL_NONE)
		{
			BeforeClose();
		}
	}
	return false;
}

bool VideoForm::OnSelect( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	if(name == L"camera")
	{
		bool select = camera_checkbox_->IsSelected();
		if( select )
		{
			std::string device;
			int no = 0;
			VideoManager::GetInstance()->GetDefaultDevicePath(no, device, nim::kNIMDeviceTypeVideo);
			if( !device.empty() )
			{
				VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, device, kDeviceSessionTypeChat);
			}
			else
			{
				camera_checkbox_->SetEnabled( false );
			}
		}
		else
		{
			camera_is_open_ = false;
			VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeChat, true);

			ClearBitmapControl( true );
			hbox_camera_fail_->SetVisible( false );
		}
		camera_is_open_ = select;
		InitSetting();
	}
	return false;
}

bool VideoForm::OnMouseMessage( ui::EventArgs* arg )
{
	ui::EventType type = arg->Type;
	if( type == kEventMouseEnter )
	{
		video_border_ctrl_->SetBorderColor( L"border_alpha_blue" );
		swap_photo_btn_->SetVisible( true );
	}
	else if( type == kEventMouseLeave )
	{
		video_border_ctrl_->SetBorderColor( L"border_alpha_white" );
		swap_photo_btn_->SetVisible( false );
	}
	return true;
}

void VideoForm::OnMaxWindow( bool _max )
{
	if(is_max_window_ == _max)
		return;
	is_max_window_ = _max;

	Button* max_btn = static_cast<Button*>(FindControl(L"maxbtn"));
	Button* restore_btn = static_cast<Button*>(FindControl(L"restorebtn"));
	if(_max)
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

bool VideoForm::OnMicrophoneVolumnKillFocus(ui::EventArgs* arg)
{
	vbox_of_input_->SetVisible( false );
	return false;
}

bool VideoForm::OnSpeakerVolumnKillFocus(ui::EventArgs* arg)
{
	vbox_of_output_->SetVisible( false );
	return false;
}
void VideoForm::OnAudioInDeviceChange(uint32_t status)
{
	if (status_ == STATUS_TALKING)
	{
		if (status != nim::kNIMDeviceStatusNoChange)
		{
			InitSetting();
		}
		if (status & nim::kNIMDeviceStatusStart)
		{
			SendControlCommand(true, true);
		}
		else if (status & nim::kNIMDeviceStatusEnd)
		{
			SendControlCommand(true, false);
		}
		else
			return;
	}
}

void VideoForm::OnVideoInDeviceChange(uint32_t status)
{
	if (current_video_mode_ && status_ == STATUS_TALKING)
	{
		if (status != nim::kNIMDeviceStatusNoChange)
		{
			InitSetting();
		}
		if (status & nim::kNIMDeviceStatusStart)
		{
			hbox_camera_fail_->SetVisible(false);

			if (send_camera_notify_ && !is_mode_changing_)
				SendControlCommand(false, true);
			else
				send_camera_notify_ = true;

			camera_is_open_ = true;
			camera_checkbox_->SetEnabled();
			camera_checkbox_->Selected(true, false);
		}
		else if (status & nim::kNIMDeviceStatusEnd)
		{
			send_camera_notify_ = true;

			if (!is_mode_changing_)
				SendControlCommand(false, false);
			camera_is_open_ = false;
			//if (!is_mode_changing_ && !camera_is_open_other_) //对方摄像头已经关闭了
			//	ChangeToAudio();
			camera_checkbox_->Selected(false, false);
		}
		else
			return;
	}

	if (current_video_mode_)
	{
		if (status & nim::kNIMDeviceStatusEnd)
		{
			ClearBitmapControl(true);
		}
	}
}

//void VideoForm::ShowPosition( WindowEx* session )
//{
//	if( IsIconic( session->GetHWND() ) )
//	{
//		this->CenterWindow();
//	}
//	else
//	{
//		POINT pt = { 0, 0 };
//		pt = GetBestPosition( session, this );
//
//		UiRect rect( pt.x, pt.y, 0, 0 );
//		this->SetPos( rect, SWP_NOSIZE );
//	}
//	ToTopMost(m_hWnd, false);
//}

void VideoForm::StartCalling( bool video_mode )
{
	current_video_mode_ = video_mode;
	is_self_ = true;

	AdjustWindowSize( current_video_mode_ );

	CheckTitle();
	ShowStatusPage(SP_DIAL);
	SwitchStatus(STATUS_CALLING);
	StartChat();
}

void VideoForm::StartReceiving(uint64_t channel_id, bool video)
{
	current_video_mode_ = video;
	channel_id_ = channel_id;
	is_self_ = false;

	AdjustWindowSize( current_video_mode_ );

	CheckTitle();
	ShowStatusPage(SP_CONFIRM);
	SwitchStatus(STATUS_INVITING);

	this->ActiveWindow();

	StdClosure closure = nbase::Bind( &VideoForm::OnAnswerTimeOut, this );
	closure = answer_timeout_timer_.ToWeakCallback(closure);
	nbase::ThreadManager::PostDelayedTask(closure, nbase::TimeDelta::FromSeconds(kAnswerTimeOut));
}

void VideoForm::PrepareQuit()
{
	if(status_ == STATUS_WAITING || status_ == STATUS_BUSYING)
	{		
		//if(status_ == STATUS_WAITING )
		//	Hangup();

		ShowVideochatMsg(current_video_mode_, kVideoChatMsgStateHangup1, 0, session_id_, is_self_, 0, false, true);
	}
	else if(status_ == STATUS_REJECT)
	{
		ShowVideochatMsg( current_video_mode_, kVideoChatMsgStateHangup2, 0, session_id_, is_self_, 0, false, true );
	}
	else if( status_ == STATUS_NO_RESPONSE || status_ == STATUS_INVITING || status_ == STATUS_CONNECTING )
	{
		if( status_ == STATUS_INVITING || status_ == STATUS_CONNECTING )
		{
			if( status_ == STATUS_INVITING )
			{
				//IVideoChatService* service = GetVideoChatService();
				//ASSERT( service );
				//if( service != NULL )
				//{
				//	uint8_t chat_type = (current_video_mode_ ? nbiz::kTagChatTypeVideo : nbiz::kTagChatTypeAudio);
				//	service->Invoke_VideoChatReject( session_id_, chat_type, channel_id_, nbiz::IAsynCallback() );
				//}
				VideoManager::GetInstance()->VChatCalleeAck(channel_id_, false);
			}
			//Hangup();
		}

		ShowVideochatMsg( current_video_mode_, kVideoChatMsgStateHangup3, 0, session_id_, is_self_, 0, false, true );
	}
	else if(status_ == STATUS_TALKING)
	{
		//Hangup();

		EndSession();

		uint32_t tm = nbase::Time::Now().ToTimeT();
		uint32_t sec = tm - time_stamp_;
		if(sec < 0)
			sec = 0;

		ShowVideochatMsg( current_video_mode_, kVideoChatMsgStateSuccess, sec, session_id_, is_self_, 0, false, true );
	}
}

void VideoForm::OnRepeatRingSend( const int32_t mode )
{
	if( mode == MCI_MODE_STOP )
	{
		voip_ring_.Play(RING_VOIP_RING, true);
	}
}

void VideoForm::SwitchStatus( StatusEnum status )
{
	status_ = status;

	ui::MutiLanSupport* lang = MutiLanSupport::GetInstance();

	switch(status)
	{
	case STATUS_CALLING:
		{
			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_CALLING"));

			camera_checkbox_->SetVisible( current_video_mode_ );
		}
		break;
	case STATUS_WAITING:
		{
			if( current_video_mode_ )
				voip_ring_.Play(RING_VOIP_RING, true);
			else
			{
				//Ring::RingPlayCallback cb = nbase::Bind( &VideoForm::OnRepeatRingSend, this, std::placeholders::_1 );
				voip_ring_.Play( RING_VOIP_CONNTETING );
			}

			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_WAITING"));
		}
		break;
	case STATUS_TALKING:
		{
			voip_ring_.Stop();

			microphone_btn_->SetEnabled( true );
			speaker_btn_->SetEnabled( true );

			if(current_video_mode_)
			{
				time_tick_label_->SetVisible(true);
			}
		}
		break;
	case STATUS_INVITING:
		{
			voip_ring_.Play(RING_VOIP_RING, true);
		}
		break;
	case STATUS_CONNECTING:
		{
			status_label_->SetText(lang->GetStringViaID(L"STRID_VIDEO_CONNECTING"));

			camera_checkbox_->SetVisible( current_video_mode_ );
		}
		break;
	case STATUS_NO_RESPONSE:
	case STATUS_BUSYING:
	case STATUS_REJECT:
		{
			if(status == STATUS_NO_RESPONSE)
			{
				EnterEndCallPage( END_CALL_RESPONSE );
			}
			else if(status == STATUS_BUSYING)
			{
				EnterEndCallPage( END_CALL_BUSYING );
			}
			else if(status == STATUS_REJECT)
			{
				EnterEndCallPage( END_CALL_REJECT );
			}
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}

void VideoForm::IntoVideo()
{
	current_video_mode_ = true;
	need_change_form_size_ = true;

	screen_is_other_ = true;
	ClearBitmapControl( true );
	ClearBitmapControl( false );

	AdjustWindowSize( true );

	CheckTitle();
	ShowStatusPage(SP_VIDEO);

	InitSetting();
	
	into_video_btn_->SetVisible( false );

	into_audio_btn_->SetVisible( true );
	into_audio_btn_->SetEnabled( false );
}

void VideoForm::IntoAudio()
{
	VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeAudio);
	current_video_mode_ = false;

	AdjustWindowSize( false );

	CheckTitle();
	ShowStatusPage(SP_DIAL);

	InitSetting();

	into_audio_btn_->SetVisible( false );
	into_video_btn_->SetVisible( false );
}

void VideoForm::CheckTitle()
{
	std::wstring str;
	if(current_video_mode_)
		str = L"STRID_VIDEO_TITLE_VIDEO";
	else
		str = L"STRID_VIDEO_TITLE_AUDIO";
	str = MutiLanSupport::GetInstance()->GetStringViaID(str);

	Label* caption = (Label*) FindControl(L"caption");
	caption->SetText(str);

	::SetWindowText(this->GetHWND(), str.c_str());
}

void VideoForm::CheckHeadIcon()
{
	UserInfo info;
	UserService::GetInstance()->GetUserInfo(session_id_, info);
	std::wstring photo = UserService::GetInstance()->GetUserPhoto(info.account);
	headicon_btn_->SetBkImage(photo);
}

void VideoForm::CheckFriendName()
{
	UserInfo info;
	UserService::GetInstance()->GetUserInfo(session_id_, info);
	friend_label_->SetUTF8Text(info.name);
}

bool VideoForm::CheckAudioInputDevice()
{
	//VideoManager* vm = VideoManager::GetInstance();

	//std::wstring device;
	//int no = 0;
	//bool has_microphone = vm->GetDefaultAudioInput( no, device );

	//bool open = vm->GetAudioOpenInput();

	//return has_microphone && open;
	return true;
}

bool VideoForm::CheckAudioOutputDevice()
{
	//VideoManager* vm = VideoManager::GetInstance();

	//std::wstring device;
	//int no = 0;
	//bool has_speaker = vm->GetDefaultAudioOutput(no, device);

	//bool open = vm->GetAudioOpenOutput();

	//return has_speaker && open;
	return true;
}

int VideoForm::CheckVideoInputDevice()
{
	std::string device;
	int no = 0;
	VideoManager::GetInstance()->GetDefaultDevicePath(no, device, nim::kNIMDeviceTypeVideo);
	if(!device.empty())
	{
		return (camera_is_open_ ? 0 : 1);
	}
	else
		return 2;
}
void VideoForm::InitSetting()
{
	InitVolumnSetting();
	//摄像头
	if (current_video_mode_)
	{
		int rs = CheckVideoInputDevice();
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
				camera_checkbox_->SetToolTipText(L"关闭摄像头");
			}
			else
			{
				camera_checkbox_->SetClass(L"Camera2");
				camera_checkbox_->SetToolTipText(L"打开摄像头");
			}
			if (status_ == STATUS_TALKING)
				camera_checkbox_->SetEnabled(true);
		}
		camera_checkbox_->SetVisible(true);
	}
	else
	{
		camera_checkbox_->SetVisible(false);
	}
}

void VideoForm::InitVolumnSetting()
{
	//麦克风
	BYTE vol_in = nim::VChat::GetAudioVolumn(true);
	if( vol_in == 0 )
	{
		microphone_btn_->SetClass( L"Microphone2" );
	}
	else
	{
		microphone_btn_->SetClass( L"Microphone1" );
	}

	//音量
	BYTE vol_out = nim::VChat::GetAudioVolumn(false);
	if( vol_out == 0 )
	{
		speaker_btn_->SetClass( L"Speaker2" );
	}
	else
	{
		speaker_btn_->SetClass( L"Speaker1" );
	}
}

void VideoForm::ClearBitmapControl( bool mine )
{
	CBitmapControl* view = NULL;
	Control*		tip  = NULL;
	if( mine )
	{
		if( screen_is_other_ )
		{
			view = video_ctrl_preview_;
			tip = camera_page_tip2_;
		}
		else
		{
			view = video_ctrl_screen_;
			tip = camera_page_tip_;
		}
	}
	else
	{
		if( screen_is_other_ )
		{
			view = video_ctrl_screen_;
			tip = camera_page_tip_;
		}
		else
		{
			view = video_ctrl_preview_;
			tip = camera_page_tip2_;
		}
	}
	view->Clear();
	tip->SetVisible( true );
}


void VideoForm::StartTalking( /*const nbiz::VideoChatStartCallbackParam& msg*/ )
{
	current_video_mode_ = true;/*(msg.type_ == nbiz::kTagChatTypeVideo)*/;
	is_self_ = false;

	channel_id_;// = msg.channel_id_;

	if( current_video_mode_ )
		need_change_form_size_ = true;

	AdjustWindowSize( current_video_mode_ );

	CheckTitle();
	ShowStatusPage(SP_DIAL);
	SwitchStatus(STATUS_CONNECTING);
}
}
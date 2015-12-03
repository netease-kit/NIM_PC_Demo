#include "video_form.h"
#include <sys/timeb.h>
#include "shared/tool.h"
#include "util/windows_manager.h"
#include "base/time/time.h"
#include "module/video/video_manager.h"

namespace nim_comp
{
using namespace ui;
bool VideoForm::InitVideo()
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

void VideoForm::FreeVideo()
{
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeChat);
}

bool VideoForm::InitAudio()
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

void VideoForm::FreeAudio()
{
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeChat);
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeChat);
}

void VideoForm::OnVideoData( const std::string &data, ui::CSize size, uint64_t timestamp)
{
	// 本机摄像头捕捉画面
	if(current_video_mode_ && camera_is_open_)
	{
		if( screen_is_other_ )
		{
			camera_page_tip2_->SetVisible( false );
			video_ctrl_preview_->Refresh( m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, true );
		}
		else
		{
			camera_page_tip_->SetVisible( false );
			video_ctrl_screen_->Refresh( m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, true );
		}
	}
}

void VideoForm::OnRecVideo( const std::string &data, ui::CSize size, uint64_t timestamp)
{
	// 接收的画面
	if (current_video_mode_ && data.size() > 0 && camera_is_open_other_)
	{
		camera_open_label_->SetVisible( false );
		camera_closed_label_->SetVisible( false );

		if( need_change_form_size_ )
		{
			need_change_form_size_ = false;

			int min_width = 500, min_height = 400;
			if( size.cx > min_width )
			{
				min_width = size.cx;
				if( size.cy > min_height - 34 ) //标题栏的高度
					min_height = size.cy + 34;

				if( IsZoomed( m_hWnd ) )
				{
					SendMessage( WM_SYSCOMMAND, SC_RESTORE, 0 );
				}

				ui::UiRect dest( 0, 0, min_width, min_height );
				this->SetPos( dest, SWP_NOMOVE );
			}
			else if (size.cy > min_height - 34 )
			{
				min_height = size.cy + 34;
				if (size.cx > min_width)
				{
					min_width = size.cx;
				}
				if (IsZoomed(m_hWnd))
				{
					SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				}
				ui::UiRect dest(0, 0, min_width, min_height);
				this->SetPos(dest, SWP_NOMOVE);
			}
		}

		if( screen_is_other_ )
		{
			camera_page_tip_->SetVisible(false);
			video_ctrl_screen_->Refresh(m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, false);
		}
		else
		{
			camera_page_tip2_->SetVisible(false);
			video_ctrl_preview_->Refresh(m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, false);
		}
	}
}

void VideoForm::StartChat()
{
	bool ret = VideoManager::GetInstance()->StartChat((current_video_mode_ ? nim::kNIMVideoChatModeVideo : nim::kNIMVideoChatModeAudio), session_id_);
	if (ret)
	{
		StartDialWaitingTimer();
	}
	else
	{
		ChatStartCallback(false, 0);
	}
}
void VideoForm::StartDialWaitingTimer()
{
	dial_timeout_timer_.Cancel();
	StdClosure timer_cb = nbase::Bind(&VideoForm::OnDialTimeOut, this);
	timer_cb = dial_timeout_timer_.ToWeakCallback(timer_cb);
	nbase::ThreadManager::PostDelayedTask(timer_cb, nbase::TimeDelta::FromSeconds(kDialTimeOut));
}

void VideoForm::ChatStartCallback(bool success, int64_t channel_id)
{
	if(success)
	{
		channel_id_ = channel_id;
		SwitchStatus(STATUS_WAITING);
		StartDialWaitingTimer();
	}
	else
	{
		QLOG_ERR(L"发起{0}聊天失败") << (current_video_mode_ ? L"视频" : L"语音");
		//if(param.code_ == 2)
		//	EnterEndCallPage( END_CALL_VERSION );
		//else
			EnterEndCallPage( END_CALL_HANGUP );
	}
}

void VideoForm::ChangeToVideo()
{
	//IVideoChatService* service = GetVideoChatService();
	//PTR_VOID(service);

	is_mode_changing_ = true;

	IntoVideo();
	camera_open_label_->SetVisible( true );
	InitVideo();

	VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlAudioToVideo);
}

void VideoForm::ChangeToAudio()
{
	//IVideoChatService* service = GetVideoChatService();
	//PTR_VOID(service);

	FreeVideo();
	IntoAudio();
	into_video_btn_->SetVisible( true );

	VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlVideoToAudio);
}

void VideoForm::SendControlCommand(bool audio, bool open)
{
	//IVideoChatService* service = GetVideoChatService();
	//PTR_VOID(service);

	if (audio)
	{
		VideoManager::GetInstance()->VChatControl(channel_id_, open ? nim::kNIMTagControlOpenAudio : nim::kNIMTagControlCloseAudio);
	}
	else
	{
		VideoManager::GetInstance()->VChatControl(channel_id_, open ? nim::kNIMTagControlOpenVideo : nim::kNIMTagControlCloseVideo);
	}
}

void VideoForm::OnControlModeChange(int64_t channel_id, nim::NIMVChatControlType type)
{
	if (channel_id != channel_id_)
	{
		return;
	}
	switch (type)
	{
	case nim::kNIMTagControlOpenAudio:
	{
		wprintf_s(L"-----提示，对方打开了麦克风\n");
	}
	break;
	case nim::kNIMTagControlCloseAudio:
	{
		wprintf_s( L"-----提示，对方关闭了麦克风\n" );
	}
	break;
	case nim::kNIMTagControlOpenVideo:
	{
		wprintf_s(L"-----提示，对方打开了摄像头\n");
		camera_is_open_other_ = true;
		camera_closed_label_->SetVisible(false);
	}
	break;
	case nim::kNIMTagControlCloseVideo:
	{
		camera_is_open_other_ = false;
		camera_closed_label_->SetVisible(screen_is_other_);

		ClearBitmapControl(false);

		if (!is_mode_changing_ && !camera_is_open_)
		{
			ChangeToAudio();

			ShowVideoTip(L"STRID_VIDEO_TIP_AUTO_CHANGE_TO_AUDIO");
		}
	}
	break;
	case nim::kNIMTagControlAudioToVideo:
	{
		if (::IsIconic(m_hWnd))
		{
			FLASHWINFO flash_info =
			{
				sizeof(FLASHWINFO),
				m_hWnd,
				FLASHW_TRAY,
				3,
				0
			};
			::FlashWindowEx(&flash_info);
		}
		//VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlAgreeAudioToVideo);

		MsgboxCallback cb = nbase::Bind(&VideoForm::OnMissionCallback, this, std::placeholders::_1);
		ShowMsgBox(m_hWnd, L"对方邀请你开启视频聊天", cb);
	}
	break;
	case nim::kNIMTagControlAgreeAudioToVideo:
	{
		camera_is_open_other_ = true;  //开启视频聊天或从语音转视频，默认对方摄像头是打开的
		camera_open_label_->SetVisible(false);

		is_mode_changing_ = false;

		into_audio_btn_->SetEnabled(true);
		VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeVideo);
	}
	break;
	case nim::kNIMTagControlRejectAudioToVideo:
	{
		is_mode_changing_ = false;
		FreeVideo();
		IntoAudio();
		into_video_btn_->SetVisible(true);

		ShowVideoTip(L"STRID_VIDEO_TIP_REJECT_A_TO_V");
	}
	break;
	case nim::kNIMTagControlVideoToAudio:
	{
		FreeVideo();
		IntoAudio();
		into_video_btn_->SetVisible(true);
	}
	break;
	case nim::kNIMTagControlBusyLine:
	{
		wprintf_s(L"---------------对方占线\n");
		SwitchStatus(STATUS_BUSYING);
	}
	break;
	case nim::kNIMTagControlCamaraNotAvailable:
	{
		wprintf_s( L"-----对方摄像头不可用\n" );
	}
	break;
	case nim::kNIMTagControlReceiveStartNotifyFeedback:
	{

	}
	break;
	}
}

//void VideoForm::OnTimeout( const nbiz::VideoChatHangupOnTimeoutCallbackParam& msg )
//{
//	if(msg.channel_id_ != channel_id_)
//		return;
//
//	if( status_ == STATUS_TALKING )
//		return;
//
//	SwitchStatus(STATUS_NO_RESPONSE);
//}

void VideoForm::OnRejected(int64_t channel_id)
{
	if (channel_id != channel_id_)
		return;

	SwitchStatus(STATUS_REJECT);
}

void VideoForm::OnVideochatSyncNotify(bool accept, int64_t channel_id)
{
	if (channel_id == channel_id_)
	{
		status_ = STATUS_NONE;
		if (accept)
			EnterEndCallPage(END_CALL_SYNC_ACCEPT);
		else
			EnterEndCallPage(END_CALL_SYNC_REFUSE);
	}
}

std::wstring VideoForm::GetTickTime( long &sec )
{
	uint32_t tm = nbase::Time::Now().ToTimeT();
	sec = tm - time_stamp_;
	if(sec < 0)
		sec = 0;

	std::wstring time;
	int h = sec / 3600, m = (sec % 3600) / 60, s = sec % 60;
	if(h > 0)
		nbase::StringPrintf(time, L"%02d:%02d:%02d", h, m, s);
	else
		nbase::StringPrintf(time, L"%02d:%02d", m, s);

	return time;
}

void VideoForm::OnTick()
{
	long sec = 0;
	std::wstring time = GetTickTime(sec);

	time_tick_label_->SetText(time);

	status_label_->SetText(time);

	//if( !time_weak_flag_.expired() )
	//{
	//	time_notify_(sec);
	//}
}

//上面是app服务器，下面是数据服务器

void VideoForm::OnComeIn( uint64_t id )
{
	//uint64_t v = StringToUint64t(session_id_);
	//if( v != id )
	//	return;

	if( current_video_mode_ )
	{
		IntoVideo();
		InitVideo();
		VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeVideo);
	}
	else
	{
		into_video_btn_->SetVisible( true );
	}

	time_stamp_ = nbase::Time::Now().ToTimeT();
	if(current_video_mode_)
		time_tick_label_->SetText(L"00:00");
	else
		status_label_->SetText(L"00:00");

	StdClosure closure = nbase::Bind(&VideoForm::OnTick, this);
	auto weak_callback = tick_weakflag_.ToWeakCallback(closure);
	nbase::ThreadManager::PostRepeatedTask(weak_callback, nbase::TimeDelta::FromSeconds(1));

	SwitchStatus(STATUS_TALKING);

	if(current_video_mode_)
	{
		into_audio_btn_->SetEnabled(true);

		AllowWindowMaxsize(true);
	}
}

void VideoForm::OnVideoDeviceStartCallback( bool result )
{
	camera_checkbox_->SetEnabled( true );

	if( !is_mode_changing_ )
		into_audio_btn_->SetEnabled( true );

	camera_is_open_ = result;
	ClearBitmapControl( true );

	if( !result )
	{
		hbox_camera_fail_->SetVisible( true );
	}
}

void VideoForm::OnComeOut( uint64_t id )
{
	//uint64_t v = StringToUint64t(session_id_);
	//if( v == id )
	{
		EnterEndCallPage( END_CALL_BE_HANGUP );
	}
}

void VideoForm::OnHangup(uint64_t channel_id)
{
	if (channel_id != (uint64_t)channel_id_)
		return;

	EnterEndCallPage(END_CALL_BE_HANGUP);
}

void VideoForm::EndSession()
{
	if(status_ == STATUS_TALKING) //避免重复处理
	{
		tick_weakflag_.Cancel();

		//jitter_buffer_.reset(NULL);
	}
}

void VideoForm::OnNetstatChange(uint16_t stat)
{
	if( current_video_mode_ )
		netstat_label_->SetVisible(stat >= nim::kNIMVideoChatSessionNetStatBad);
	else
		netstat_label_->SetVisible( false );
}

void VideoForm::OnLogin( bool success )
{
	if(success)
	{
		InitAudio();
		{
			if(is_self_)
			{
				StartDialWaitingTimer();
			}
		}
		//else
		//{
		//	EnterEndCallPage( END_CALL_BAD_MICROPHONE );
		//}
	}
	else
	{
		EnterEndCallPage( END_CALL_CONNECTION );
	}
}

void VideoForm::OnAudioDeviceStartCallback( bool result )
{
	if(result)
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

void VideoForm::OnDialTimeOut()
{
	if(status_ == STATUS_WAITING)
	{
		SwitchStatus(STATUS_NO_RESPONSE);
	}
}

void VideoForm::OnAnswerTimeOut()
{
	if(status_ == STATUS_INVITING)
	{
		EnterEndCallPage( END_CALL_HANGUP );
	}
}

void VideoForm::OnConnectTimeOut()
{
	if(status_ == STATUS_CONNECTING)
	{
		//LOG_APP("connect voip server timeout");
		EnterEndCallPage( END_CALL_CONNECTION );
	}
}


void VideoForm::ShowVideoTip( const std::wstring stringID )
{
	ui::Label* label = (ui::Label*) vbox_video_audio_tip_->FindSubControl( L"label_of_tip" );
	PTR_VOID( label );

	std::wstring text = ui::MutiLanSupport::GetInstance()->GetStringViaID( stringID );
	label->SetText( text );

	vbox_video_audio_tip_->SetVisible( true );
	StdClosure closure = nbase::Bind(&VideoForm::OnVideoAudioTip, this);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, closure, nbase::TimeDelta::FromSeconds(2));
}

void VideoForm::OnVideoAudioTip()
{
	vbox_video_audio_tip_->SetVisible( false );
}


void VideoForm::OnMissionCallback(MsgBoxRet ret)
{
	is_mode_changing_ = false;

	VideoManager::GetInstance()->VChatControl(channel_id_, ret == MB_YES ? nim::kNIMTagControlAgreeAudioToVideo : nim::kNIMTagControlRejectAudioToVideo);

	if (ret == MB_YES)
	{
		IntoVideo();
		VideoManager::GetInstance()->SetVoipMode(nim::kNIMVideoChatModeVideo);
		camera_is_open_other_ = true;

		send_camera_notify_ = false;
		if (InitVideo())
		{

		}
		else
		{
			into_audio_btn_->SetEnabled(true);
			VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlCamaraNotAvailable);
		}
	}
}


//显示保存消息
void ShowVideochatMsg( bool is_video_type, VideoForm::VideoChatMsgStateType type, long sec, UTF8String uid, bool is_self, uint32_t time, bool is_offline, bool open_sess )
{
	//Json::Value root;
	//root[kSessionVideoChatState] = type;
	//root[kSessionVideoChatTime] = sec;
	//Json::FastWriter fast_writer;
	//UTF8String json = fast_writer.write(root);

	//UTF8String my_id = GetIYixinCore()->GetUid();
	//UTF8String sender = (is_self ? my_id : uid);
	//UTF8String receiver = (is_self ? uid : my_id);

	//nbiz::SessionMsgType content_type = (is_video_type ? kSessionMsgTypeVideoChatMessage : kSessionMsgTypeAudioChatMessage);

	//uint32_t msg_time = 0;
	//if( time == 0 )
	//	msg_time = msg_helper::GetNowTimestamp();
	//else
	//	msg_time = time;

	//nbiz::Property msg;
	//msg.put_string(kSessionTagClientMsgID, GetUUID());
	//msg.put_string(kSessionTagSender,  sender);
	//msg.put_string(kSessionTagReceiver, receiver);
	//msg.put_string(kSessionTagBody, json);
	//msg.put_int32(kSessionTagContentType, content_type);
	//msg.put_uint32(kSessionTagTime, msg_time);

	//msg_helper::SaveMsgLog( msg, uid, kSessionTypeDouble, kMsgLogStatusUnread, "", true );

	//std::string msg_body = msg_helper::GetMsgTextShowByData(content_type, json);
	//SessionManager::GetInstance()->UpdateOneLocalMsg(kSessionTypeDouble, uid, msg_body, msg_time/*, is_video_type ? SessionIconVideo : SessionIconAudio*/);

	//if( open_sess )
	//{
	//	SessionManager::GetInstance()->CreateSession(uid, kSessionTypeDouble);
	//}

	//if( is_offline )
	//	SessionManager::GetInstance()->CreateSession( msg, false, true, false, false );
	//else
	//	SessionManager::GetInstance()->CreateSession( msg, true, false, false, false );
}
}
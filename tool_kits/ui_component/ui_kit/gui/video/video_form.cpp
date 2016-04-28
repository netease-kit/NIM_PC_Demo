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

//void VideoForm::OnVideoData( const std::string &data, ui::CSize size, uint64_t timestamp)
//{
//	// 本机摄像头捕捉画面
//	if(current_video_mode_ && camera_is_open_)
//	{
//		if( screen_is_other_ )
//		{
//			camera_page_tip2_->SetVisible( false );
//			video_ctrl_preview_->Refresh( m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, true );
//		}
//		else
//		{
//			camera_page_tip_->SetVisible( false );
//			video_ctrl_screen_->Refresh( m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, true );
//		}
//	}
//}
//
//void VideoForm::OnRecVideo( const std::string &data, ui::CSize size, uint64_t timestamp)
//{
//	// 接收的画面
//	if (current_video_mode_ && data.size() > 0 && camera_is_open_other_)
//	{
//		camera_open_label_->SetVisible( false );
//		camera_closed_label_->SetVisible( false );
//
//		if( need_change_form_size_ )
//		{
//			need_change_form_size_ = false;
//
//			int min_width = 500, min_height = 400;
//			if( size.cx > min_width )
//			{
//				min_width = size.cx;
//				if( size.cy > min_height - 34 ) //标题栏的高度
//					min_height = size.cy + 34;
//
//				if( IsZoomed( m_hWnd ) )
//				{
//					SendMessage( WM_SYSCOMMAND, SC_RESTORE, 0 );
//				}
//
//				ui::UiRect dest( 0, 0, min_width, min_height );
//				this->SetPos( dest, SWP_NOMOVE );
//			}
//			else if (size.cy > min_height - 34 )
//			{
//				min_height = size.cy + 34;
//				if (size.cx > min_width)
//				{
//					min_width = size.cx;
//				}
//				if (IsZoomed(m_hWnd))
//				{
//					SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
//				}
//				ui::UiRect dest(0, 0, min_width, min_height);
//				this->SetPos(dest, SWP_NOMOVE);
//			}
//		}
//
//		if( screen_is_other_ )
//		{
//			camera_page_tip_->SetVisible(false);
//			video_ctrl_screen_->Refresh(m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, false);
//		}
//		else
//		{
//			camera_page_tip2_->SetVisible(false);
//			video_ctrl_preview_->Refresh(m_hWnd, (BYTE*)data.c_str(), data.size(), size.cx, size.cy, false);
//		}
//	}
//}

void VideoForm::StartChat()
{
	bool ret = VideoManager::GetInstance()->StartChat((current_video_mode_ ? nim::kNIMVideoChatModeVideo : nim::kNIMVideoChatModeAudio), "", "", session_id_);
	if (ret)
	{
		is_start_ = true;
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
		EnterEndCallPage(END_CALL_STARTFAIL);
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
		if (is_mode_changing_ || current_video_mode_)
		{
			StdClosure closure = nbase::Bind(&VideoForm::OnMissionCallback, this, MB_YES);
			nbase::ThreadManager::PostTask(kThreadUI, closure);
		}
		else
		{
			MsgboxCallback cb = nbase::Bind(&VideoForm::OnMissionCallback, this, std::placeholders::_1);
			ShowMsgBox(m_hWnd, L"对方邀请你开启视频聊天", cb);
		}

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
	case nim::kNIMTagControlMp4StartRecord:
	{
		ShowRecordTip(L"对方开始录制");
	}
	break;
	case nim::kNIMTagControlMp4StopRecord:
	{
		ShowRecordTip(L"对方结束录制");
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
	start_record_btn_->SetVisible(true);
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
		paint_video_timer_.Cancel();
		auto task = nbase::Bind(&VideoForm::PaintVideo, this);
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, task, nbase::TimeDelta::FromMilliseconds(70));
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
void VideoForm::OnRecordMp4SelFileCb(BOOL ret, std::wstring path)
{
	if (ret)
	{
		if (current_video_mode_)
		{
			ShowRecordTip(L"仅录制你的声音和图像");
		} 
		else
		{
			ShowRecordTip(L"仅录制你说话的内容");
		}
		nim::VChat::StartRecord(nbase::UTF16ToUTF8(path), std::bind(&VideoForm::StartRecordCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	}
}
//开始录制回调
void VideoForm::StartRecordCb(bool ret, int code, const std::string& file, __int64 time)
{
	if (ret)
	{
		start_record_btn_->SetVisible(false);
		stop_record_btn_->SetVisible(true);
		mp4_recording_ = true;
		if (!record_tip_box_->IsVisible())
		{
			ShowRecordTip();
		}
		VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlMp4StartRecord);
		StdClosure task = nbase::Bind(&VideoForm::CheckRecordDiskSpace, this, nbase::UTF8ToUTF16(file));
		auto weak_task = record_check_disk_space_timer_.ToWeakCallback(task);
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, weak_task, nbase::TimeDelta::FromSeconds(5));
	}
	else
	{

	}
}
//结束录制回调，不需要处理，在结束的通知里处理即可
void VideoForm::StopRecordCb(bool ret, int code, const std::string& file, __int64 time)
{

}
//录制开始通知，正式开始录制数据
void VideoForm::OnStartRecord(const std::string& file, __int64 time)
{

}
//录制结束通知
void VideoForm::OnStopRecord(int code, const std::string& file, __int64 time)
{
	record_check_disk_space_timer_.Cancel();
	record_tip_label0_->SetVisible(false);
	VideoManager::GetInstance()->VChatControl(channel_id_, nim::kNIMTagControlMp4StopRecord);
	start_record_btn_->SetVisible(true);
	stop_record_btn_->SetVisible(false);
	mp4_recording_ = false;
	std::wstring tip;
	std::wstring tip1 = L"录制已结束，录制文件已保存至"; 
	std::wstring path = nbase::UTF8ToUTF16(file);
	if (code == nim::kNIMVChatMp4RecordVideoSizeError)
	{
		tip = L"由于摄像头更换";
	}
	else if (code == nim::kNIMVChatMp4RecordOutDiskSpace)
	{
		tip = L"你的硬盘存储不足";
	}
	ShowRecordTip(tip, tip1, path);
}
void VideoForm::CheckRecordDiskSpace(const std::wstring& file)
{
	std::wstring path = file;
	double ret = 0;
	nbase::StringReplaceAll(L"\\", L"/", path);
	if (path.size() >= 3 && path.substr(1, 2) == L":/")
	{
		std::wstring root_path = path.substr(0, 3);
		DWORD dwSectorsPerCluster = 0;//每簇中扇区数  
		DWORD dwBytesPerSector = 0;//每扇区中字节数  
		DWORD dwFreeClusters = 0;//剩余簇数  
		DWORD dwTotalClusters = 0;//总簇数  
		if (GetDiskFreeSpace(root_path.c_str(), &dwSectorsPerCluster, &dwBytesPerSector,
			&dwFreeClusters, &dwTotalClusters))
		{
			double dd = dwSectorsPerCluster*dwBytesPerSector / (1024.*1024.);
			ret = dwFreeClusters*dd;//该磁盘剩余容量总大小  
		}
	}
	if (ret < 100)
	{
		record_tip_label0_->SetText(L"你的硬盘存储已不足100M");
		record_tip_label0_->SetVisible(true);
	}
	else
	{
		record_tip_label0_->SetVisible(false);
	}
}
void VideoForm::PaintVideo()
{
	if (current_video_mode_)
	{
		bool show_screen = false;
		bool show_preview = false;
		if (screen_is_other_)
		{
			show_screen = camera_is_open_other_;
			show_preview = camera_is_open_;
		}
		else
		{
			show_screen = camera_is_open_;
			show_preview = camera_is_open_other_;
		}
		if (show_screen && video_ctrl_screen_->Refresh(this, !screen_is_other_, !screen_is_other_))
		{
			camera_page_tip_->SetVisible(false);
			if (screen_is_other_)
			{
				camera_open_label_->SetVisible(false);
				camera_closed_label_->SetVisible(false);
			}
		}
		if (show_preview && video_ctrl_preview_->Refresh(this, screen_is_other_, screen_is_other_))
		{
			camera_page_tip2_->SetVisible(false);
			if (!screen_is_other_)
			{
				camera_open_label_->SetVisible(false);
				camera_closed_label_->SetVisible(false);
			}
		}
	}
}

}
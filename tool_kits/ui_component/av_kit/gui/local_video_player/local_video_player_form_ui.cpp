#include "local_video_player_form.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#ifdef SUPPORTLOCALPLAYER
const LPTSTR LocalVideoPlayerForm::kClassName = L"LocalVideoPlayerForm";
unsigned int LocalVideoPlayerForm::InstanceNum = 0;

using namespace ui;

LocalVideoPlayerForm::LocalVideoPlayerForm(const std::function<void()>& close_cb, const std::wstring& path) : close_cb_(close_cb),path_(path)
{
	LocalVideoPlayerForm::InstanceNum++;
}

LocalVideoPlayerForm::~LocalVideoPlayerForm()
{

}
ui::Control* LocalVideoPlayerForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BitmapControl"))
	{
		return new ui::VideoPlayerBitmapControl();
	}
	return NULL;
}

std::wstring LocalVideoPlayerForm::GetSkinFolder()
{
	return L"local_video_player";
}

std::wstring LocalVideoPlayerForm::GetSkinFile()
{
	return L"local_video_player.xml";
}

ui::UILIB_RESOURCETYPE LocalVideoPlayerForm::GetResourceType() const
{
	return ui::UILIB_RESOURCETYPE::UILIB_FILE;
}

std::wstring LocalVideoPlayerForm::GetZIPFileName() const
{
	return L"";
}

std::wstring LocalVideoPlayerForm::GetWindowClassName() const
{
	return kClassName;
}



UINT LocalVideoPlayerForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LocalVideoPlayerForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&LocalVideoPlayerForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&LocalVideoPlayerForm::OnClicked, this, std::placeholders::_1));

	volume_box_ = (ui::VBox*)FindControl(L"volume_box");
	canvas_ = (ui::VideoPlayerBitmapControl*)FindControl(L"video_display");
	play_pause_box_ = (ui::TabBox*)FindControl(L"start_or_pause_box");
	btn_start_ = (ui::Button*)FindControl(L"start");
	playing_progress_ = (ui::Slider*)FindControl(L"playing_progress");	
	label_duration_ = (ui::Label*)FindControl(L"duration");
	btn_volume_ = (ui::Button*)FindControl(L"volume_btn");
	btn_mute_ = (ui::Button*)FindControl(L"volume_mute_btn");
	if (!path_.empty())
		nbase::ThreadManager::PostTask(ToWeakCallback([this](){
		StartPlay();
	}));	
}
LRESULT LocalVideoPlayerForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
		{
			if (is_max_window_)
				OnMaxWindow(false);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			if (!is_max_window_)
				OnMaxWindow(true);
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}
LRESULT LocalVideoPlayerForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (close_cb_ != nullptr)
		close_cb_();
	auto post_close_cb = ToWeakCallback([this]()
	{
		while (playing_)
			Sleep(100);
		::DestroyWindow(m_hWnd); //确保播放器实例销毁之后再真正关闭窗口
		LocalVideoPlayerForm::InstanceNum--;		
	});

	ShowWindow(false);
	if (playing_)
	{
		Shutdown();
		nbase::ThreadManager::PostDelayedTask(post_close_cb, nbase::TimeDelta::FromSeconds(1)); //等待shudown完成再关闭窗口
	}
	else
		post_close_cb(); //直接关闭窗口
	bHandled = true;
	return 0;
}

bool LocalVideoPlayerForm::Notify(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (msg->Type == ui::kEventValueChange)
	{
		if (name == L"playing_progress")
		{
			if (nelp_handle_)
			{
				double frac = playing_progress_->GetValue() / playing_progress_->GetMaxValue();
				long long seek_pos = (long long)(total_duration_ * frac);
				Nelp_SeekTo(nelp_handle_, seek_pos);
			}
		}		
	}
	else if (msg->Type == ui::kEventKillFocus)
	{
		if (name == L"volume_slider")
		{
			volume_box_->SetVisible(false);
		}
	}
	return true;
}
bool LocalVideoPlayerForm::OnClicked(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"volume_btn")
	{
		muted_ = true;
		Nelp_SetMute(nelp_handle_, true);
		btn_volume_->SetVisible(false);
		btn_mute_->SetVisible(true);
	}
	else if (name == L"volume_mute_btn")
	{
		muted_ = false;
		Nelp_SetMute(nelp_handle_, false);
		btn_volume_->SetVisible(true);
		btn_mute_->SetVisible(false);
	}
	else if (name == L"start" )
	{
		ChangePlayerState(PlayerStateStarting);
		if (!StartPlay()) //这里只处理失败情况，收到NELP_MSG_PREPARED消息才算播放成功
			ChangePlayerState(PlayerStateError);
	}
	else if (name == L"pause")
	{
		if (playing_)
		{
			Nelp_Pause(nelp_handle_);
			ChangePlayerState(PlayerStatePaused);
		}
	}
	else if (name == L"resume")
	{
		if (playing_)
		{
			Nelp_Resume(nelp_handle_);
			ChangePlayerState(PlayerStatePlaying);
		}
	}
	return false;
}
void LocalVideoPlayerForm::OnEsc(BOOL &bHandled)
{
	Close();
}

void LocalVideoPlayerForm::OnMaxWindow(bool _max)
{
	if (is_max_window_ == _max)
		return;
	is_max_window_ = _max;
	if (_max)
	{
		FindControl(L"maxbtn")->SetVisible(false);
		FindControl(L"restorebtn")->SetVisible(true);
	}
	else
	{
		FindControl(L"maxbtn")->SetVisible(true);
		FindControl(L"restorebtn")->SetVisible(false);
	}
}


void LocalVideoPlayerForm::ChangePlayerState(PlayerState new_state)
{
	state_ = new_state;
	switch (new_state)
	{
	case PlayerStateStarting:
	{
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		break;
	}
	case PlayerStatePlaying: //Nelp_Start成功或Nelp_Resume之后
	{		
		play_pause_box_->SelectItem(L"pause"); //点播显示暂停按钮	
		btn_start_->SetEnabled(false); //disable开始按钮
		playing_progress_->SetEnabled(true);
		playing_progress_->SetVisible(true);
	
		if (!refresh_time_flag_.HasUsed())
		{
			auto refresh_time_cb = refresh_time_flag_.ToWeakCallback([this]()
			{
				RefreshPlayTime(); //开始刷新播放时间
			});
			nbase::ThreadManager::PostRepeatedTask(refresh_time_cb, nbase::TimeDelta::FromMilliseconds(500));
		}
		break;
	}
	case PlayerStatePaused:
	{
		play_pause_box_->SelectItem(L"resume"); //显示继续播放按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		break;
	}
	case PlayerStateStopping:
	{
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		break;
	}
	case PlayerStateCompleted:
	case PlayerStateStopped:
	case PlayerStateError:
	{		
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		refresh_time_flag_.Cancel(); //取消刷新时间任务
		btn_start_->SetEnabled(true); //disable开始按钮
		playing_progress_->SetEnabled(false);
		if (new_state != PlayerStateCompleted)
		{
			playing_progress_->SetVisible(false); //隐藏进度条
			playing_progress_->SetValue(0);
			label_duration_->SetText(L""); //重置播放时间
			canvas_->Clear();
		}
		break;
	}
	}
}
#endif
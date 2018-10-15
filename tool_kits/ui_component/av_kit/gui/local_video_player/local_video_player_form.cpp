#include "local_video_player_form.h"
#ifdef SUPPORTLOCALPLAYER

using namespace ui;

std::map<_HNLPSERVICE, LocalVideoPlayerForm*> LocalVideoPlayerForm::InstFormMap;
void LocalVideoPlayerForm::Play(const std::wstring& path)
{	
	if (path_.compare(path) == 0)
	{
		if (!playing_)
			StartPlay();		
	}		
	else
	{
		path_ = path;	
		StartPlay();
	}
}
bool LocalVideoPlayerForm::SamePath(const std::wstring& path) const
{
	return path.compare(path_) == 0;
}
bool LocalVideoPlayerForm::StartPlay()
{
	if (nelp_handle_)
		Shutdown();

	NELP_RET ret = NELP_OK;
	ret = Nelp_Create(nullptr, &nelp_handle_);
	if (ret != NELP_OK)
		return false;
	InstFormMap[nelp_handle_] = this;
	ST_NELP_PARAM param = {0};
	std::string url = nbase::UTF16ToUTF8(path_);
	param.paPlayUrl = (char*)url.c_str();
	param.enBufferStrategy = EN_NELP_ANTI_JITTER;
	try
	{
		if (Nelp_InitParam(nelp_handle_, &param) != NELP_OK) throw(false);			
		Nelp_RegisterMessageCB(nelp_handle_, LocalVideoPlayerForm::PlayerMessageCB);
		Nelp_RegisterGetVideoFrameCB(nelp_handle_, EN_ARGB8888, LocalVideoPlayerForm::VideoFrameCB);
		Nelp_SetVolume(nelp_handle_, volume_);
		Nelp_SetMute(nelp_handle_, muted_);
		Nelp_SetPlaybackTimeout(nelp_handle_, -1);
		if (Nelp_PrepareToPlay(nelp_handle_) != NELP_OK) throw(false);
		throw(true);
	}	
	catch (bool ret)
	{
		if (!ret)
			Shutdown();
		return ret;
	}
	return true;
}
void LocalVideoPlayerForm::Shutdown()
{
	Nelp_Stop(nelp_handle_);
	Nelp_Shutdown(nelp_handle_);
	InstFormMap.erase(nelp_handle_);
	nelp_handle_ = NULL;
	playing_ = false;
}

void FormatTime(long long seconds_in, int &hour, int &minute, int &second)
{
	second = seconds_in % 60;
	seconds_in /= 60;
	minute = seconds_in % 60;
	seconds_in /= 60;
	hour = seconds_in % 60;
}

void LocalVideoPlayerForm::RefreshPlayTime()
{
	long long current_duration = Nelp_GetCurrentPlaybackTime(nelp_handle_);
	if (current_duration < 0) current_duration = total_duration_;
	int cur_h = 0, cur_m = 0, cur_s = 0;
	FormatTime(current_duration, cur_h, cur_m, cur_s);
	if (total_duration_ > 0)
	{
		int max_value = playing_progress_->GetMaxValue();
		playing_progress_->SetValue(current_duration * 1.0 / total_duration_ * max_value);
		int total_h = 0, total_m = 0, total_s = 0;
		FormatTime(total_duration_, total_h, total_m, total_s);
		label_duration_->SetText(nbase::StringPrintf(L"%02d:%02d:%02d / %02d:%02d:%02d", cur_h, cur_m, cur_s, total_h, total_m, total_s));
	}
	else
		label_duration_->SetText(nbase::StringPrintf(L"%02d:%02d:%02d", cur_h, cur_m, cur_s));
}

void LocalVideoPlayerForm::PlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg)
{
	nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(&LocalVideoPlayerForm::UIPlayerMessageCB, hNLPService, msg)); //抛到UI线程执行
}

void LocalVideoPlayerForm::UIPlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg)
{
	auto iter = InstFormMap.find(hNLPService);
	if (iter == InstFormMap.end())
		return;

	NELP_RET ret = NELP_OK;
	LocalVideoPlayerForm* main_form = iter->second;
	switch (msg.iWhat)
	{
	case NELP_MSG_ERROR: //开始播放失败或者播放过程中出错
		main_form->Shutdown();
		main_form->ChangePlayerState(PlayerStateError);
		break;
	case NELP_MSG_PREPARED: //预处理完成，才执行Nelp_Start
		if (Nelp_Start(main_form->nelp_handle_) == NELP_OK) //开始播放成功
		{
			main_form->playing_ = true; //修改标志
			main_form->total_duration_ = Nelp_GetDuration(main_form->nelp_handle_); //获取总时长
			main_form->ChangePlayerState(PlayerStatePlaying);
		}
		break;
	case NELP_MSG_COMPLETED:
		main_form->Shutdown();
		main_form->ChangePlayerState(PlayerStateCompleted);
		main_form->RefreshPlayTime();		
		break;
	case NELP_MSG_VIDEO_SIZE_CHANGED:
	{		
		if (main_form->canvas_ != nullptr)
		{
			if (main_form->canvas_->GetFixedHeight() != msg.iArg2 || main_form->canvas_->GetFixedWidth() != msg.iArg1)
			{
				main_form->canvas_->SetFixedWidth(msg.iArg1);
				main_form->canvas_->SetFixedHeight(msg.iArg2);
				nbase::ThreadManager::PostTask(ThreadId::kThreadUI, main_form->ToWeakCallback([main_form](){
					main_form->CenterWindow();
				}));
			}			
		}
		break;
	}
	case NELP_MSG_VIDEO_RENDERING_START:
		break;
	case NELP_MSG_AUDIO_RENDERING_START:
		break;
	case NELP_MSG_BUFFERING_START:
		break;
	case NELP_MSG_BUFFERING_END:
		break;
	case NELP_MSG_SEEK_COMPLETED:
		break;
	case NELP_MSG_PLAYBACK_STATE_CHANGED:
		break;
	case NELP_MSG_AUDIO_DEVICE_OPEN_FAILED:
		break;
	case NELP_MSG_VIDEO_PARSE_ERROR:
		break;
	default:
		break;
	}
}

//VideoFrameCB是在主线程执行的
void LocalVideoPlayerForm::VideoFrameCB(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame)
{
	auto iter = InstFormMap.find(hNLPService);
	if (iter == InstFormMap.end())
		return;
	LocalVideoPlayerForm* main_form = iter->second;
	if (frame && frame->enMFormat == EN_ARGB8888)
		main_form->canvas_->Update((char*)frame->puaUsrData, frame->iWidth, frame->iHeight);
}
#endif
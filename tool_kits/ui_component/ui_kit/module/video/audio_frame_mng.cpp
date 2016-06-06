
#include "audio_frame_mng.h"
#include <sys/timeb.h>

namespace nim_comp
{
AudioFrameMng::AudioFrameMng()
{
}

AudioFrameMng::~AudioFrameMng()
{
}
void AudioFrameMng::Clear()
{
	nbase::NAutoLock auto_lock(&lock_);
	capture_audio_data_.clear();
	capture_time_ = 0;
	recv_audio_data_.clear();
	recv_time_ = 0;
	rate_rec_ = 16000;
	rate_capture_ = 16000;
}
void AudioFrameMng::AddAudioFrame(bool capture, int64_t time, const char* data, int size, const std::string& json)
{
	nbase::NAutoLock auto_lock(&lock_);
	timeb time_now;
	ftime(&time_now); // √Î ˝
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // ∫¡√Î

	Json::Value valus;
	Json::Reader reader;
	int rate = 16000;
	if (reader.parse(json, valus))
	{
		rate = (nim::NIMVideoSubType)valus[nim::kNIMDeviceSampleRate].asInt();
	}
	if (capture)
	{
		if (rate_capture_ != rate || cur_timestamp > capture_time_ + 1000 || capture_audio_data_.size() > 6400)
		{
			capture_audio_data_.clear();
		}
		rate_capture_ = rate;
		capture_time_ = cur_timestamp;
		capture_audio_data_.append(data, size);
	} 
	else
	{
		if (rate_rec_ != rate || cur_timestamp > recv_time_ + 1000 || recv_audio_data_.size() > 6400)
		{
			recv_audio_data_.clear();
		}
		rate_rec_ = rate;
		recv_time_ = cur_timestamp;
		recv_audio_data_.append(data, size);
	}
}
void AudioFrameMng::GetAudioFrame(bool capture, std::string& out_data, int& rate)
{
	nbase::NAutoLock auto_lock(&lock_);
	timeb time_now;
	ftime(&time_now); // √Î ˝
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // ∫¡√Î

	if (capture)
	{
		out_data = capture_audio_data_;
		capture_audio_data_.clear();
		rate = rate_capture_;
	}
	else
	{
		out_data = recv_audio_data_;
		recv_audio_data_.clear();
		rate = rate_rec_;
	}
}
}

#pragma once

#include "base/synchronization/lock.h"
#include "base/base_types.h"
#include <string>

namespace nim_comp
{
class AudioFrameMng
{
public:
	AudioFrameMng();
	~AudioFrameMng();

	void Clear();
	void AddAudioFrame(bool capture, int64_t time, const char* data, int size, const std::string& json);
	void GetAudioFrame(bool capture, std::string& out_data, int& rate);
private:
	nbase::NLock  lock_;
	std::string capture_audio_data_;
	int64_t capture_time_ = 0;
	std::string recv_audio_data_;
	int64_t recv_time_ = 0;
	int32_t rate_rec_;
	int32_t rate_capture_;
};
}

#pragma once

#include "base/synchronization/lock.h"
#include "base/base_types.h"
#include <string>

namespace nim_comp
{
/** @class AudioFrameMng
  * @brief 音频帧管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class AudioFrameMng
{
public:
	AudioFrameMng();
	~AudioFrameMng();

	/**
	* 清理保存的音频帧数据
	* @return void	无返回值
	*/
	void Clear();

	/**
	* 添加一个音频帧数据
	* @param[in] capture 是否为录制音频的帧
	* @param[in] time 时间戳
	* @param[in] data 帧数据
	* @param[in] size 帧数据大小
	* @param[in] json 包含采样频率的json串
	* @return void	无返回值
	*/
	void AddAudioFrame(bool capture, int64_t time, const char* data, int size, const std::string& json);

	/**
	* 获取一个音频帧数据
	* @param[in] capture 是否为录制音频的帧
	* @param[out] out_data 帧数据
	* @param[out] rate 采样频率
	* @return void	无返回值
	*/
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
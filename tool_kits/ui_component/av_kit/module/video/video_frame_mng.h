
#pragma once

#include "base/synchronization/lock.h"
#include "base/base_types.h"
#include <string>
#include <map>

namespace nim_comp
{
/** @struct PicRegion
  * @brief 一块颜色数据区的描述，便于参数传递
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
struct PicRegion
{
	PicRegion()
	{
		pdata_ = NULL;
		//subtype_ = nim::kNIMVideoSubTypeARGB;
		size_max_ = 0;
		size_ = 0;
	}

	~PicRegion()
	{
		Clear();
	}

	/**
	* 清理保存的颜色数据
	* @return void	无返回值
	*/
	void Clear()
	{
		if (pdata_)
		{
			delete[] pdata_;
			pdata_ = NULL;
		}
		size_max_ = 0;
		size_ = 0;
	}

	/**
	* 重置颜色数据
	* @param[in] time 时间戳
	* @param[in] data 帧数据
	* @param[in] size 帧数据大小
	* @param[in] width 视频宽度
	* @param[in] height 视频高度
	* @return int 返回传入的size值
	*/
	int ResetData(uint64_t time, const char* data, int size, unsigned int width, unsigned int height/*, nim::NIMVideoSubType subtype*/)
	{
		if (size > size_max_)
		{
			if (pdata_)
			{
				delete[] pdata_;
				pdata_ = NULL;
			}
			pdata_ = new char[size];
			size_max_ = size;
		}
		width_ = width;
		height_ = height;
		timestamp_ = time;
		//subtype_ = subtype;
		size_ = size;
		memcpy(pdata_, data, size);
		return size;
	}
	
	//nim::NIMVideoSubType subtype_;
	char*		pdata_;         //颜色数据首地址
	int			size_max_;
	int			size_;
	long        width_;         //像素宽度
	long        height_;        //像素高度
	uint64_t	timestamp_;     //时间戳（毫秒）
};

/** @class VideoFrameMng
  * @brief 视频帧管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class VideoFrameMng
{
public:
	enum FrameType
	{
		Ft_I420 = 0,
		Ft_ARGB,
		Ft_ARGB_r,
	};
	VideoFrameMng();
	~VideoFrameMng();

	/**
	* 清理保存的视频帧数据
	* @return void	无返回值
	*/
	void Clear();

	/**
	* 添加一个视频帧数据
	* @param[in] capture 是否为录制音频的帧
	* @param[in] time 时间戳
	* @param[in] data 帧数据
	* @param[in] size 帧数据大小
	* @param[in] width 视频宽度
	* @param[in] height 视频高度
	* @param[in] json 包含发送者用户id的json串
	* @param[in] frame_type 帧类型
	* @return void	无返回值
	*/
	void AddVideoFrame(bool capture, int64_t time, const char* data, int size, int width, int height, const std::string& json, FrameType frame_type = Ft_ARGB_r);

	/**
	* 获取某个用户发来的一个视频帧数据
	* @param[in] account 用户id
	* @param[out] time 时间戳
	* @param[out] out_data 帧数据
	* @param[out] width 视频宽度
	* @param[out] height 视频高度
	* @param[in] mirror 是否需要翻转图像
	* @param[in] argb_or_yuv 是否需要ARGB格式数据
	* @return void	无返回值
	*/
	bool GetVideoFrame(std::string account, int64_t& time, char* out_data, int& width, int& height, bool mirror = false, bool argb_or_yuv = true);
private:
	nbase::NLock  lock_;
	PicRegion capture_video_pic_;
	std::map<std::string, PicRegion*> recv_video_pic_list_;
};
}
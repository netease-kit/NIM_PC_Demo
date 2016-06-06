
#pragma once

#include "base/synchronization/lock.h"
#include "base/base_types.h"
#include <string>
#include <map>

namespace nim_comp
{
struct PicRegion //一块颜色数据区的描述，便于参数传递
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

	void Clear();
	void AddVideoFrame(bool capture, int64_t time, const char* data, int size, int width, int height, const std::string& json, FrameType frame_type = Ft_ARGB_r);
	bool GetVideoFrame(std::string account, int64_t& time, char* out_data, int& width, int& height, bool mirror = false, bool argb_or_yuv = true);
private:
	nbase::NLock  lock_;
	PicRegion capture_video_pic_;
	std::map<std::string, PicRegion*> recv_video_pic_list_;
};
}
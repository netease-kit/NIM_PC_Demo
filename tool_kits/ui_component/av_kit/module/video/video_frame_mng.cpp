
#include "video_frame_mng.h"
#include "libyuv.h"
#include <sys/timeb.h>

namespace nim_comp
{
VideoFrameMng::VideoFrameMng()
{
}

VideoFrameMng::~VideoFrameMng()
{
	Clear();
}
void VideoFrameMng::Clear()
{
	nbase::NAutoLock auto_lock(&lock_);
	capture_video_pic_.Clear();
	for (auto it : recv_video_pic_list_)
	{
		delete it.second;
	}
	recv_video_pic_list_.clear();
}
void VideoFrameMng::AddVideoFrame(bool capture, int64_t time, const char* data, int size, int width, int height, const std::string& json, FrameType frame_type)
{
	Json::Value valus;
	Json::Reader reader;
	std::string account;
	if (reader.parse(json, valus))
	{
		account = valus[nim::kNIMDeviceDataAccount].asString();
	}
	if (!capture && account.empty())
	{
		return;
	}
	nbase::NAutoLock auto_lock(&lock_);
	//nim::NIMVideoSubType subtype = nim::kNIMVideoSubTypeI420;
	timeb time_now;
	ftime(&time_now); // 秒数
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒
	const char* src_buffer = data;
	std::string ret_data;
	if (frame_type != Ft_I420)
	{
		int byte_width = width * 4;
		width -= width % 2;
		height -= height % 2;
		int wxh = width * height;
		ret_data.append(wxh * 3 / 2, (char)0);
		uint8_t* des_y = (uint8_t*)ret_data.c_str();
		uint8_t* des_u = des_y + wxh;
		uint8_t* des_v = des_u + wxh / 4;
		const uint8_t* src_argb = (const uint8_t*)data;
		if (frame_type == Ft_ARGB_r)
		{
			src_argb = (const uint8_t*)data + size - byte_width;
			byte_width = -byte_width;
		}
		libyuv::ARGBToI420(src_argb, byte_width,
			des_y, width,
			des_u, width / 2,
			des_v, width / 2,
			width, height);
		src_buffer = ret_data.c_str();
		size = wxh * 3 / 2;
	}
	if (capture)
	{
		capture_video_pic_.ResetData(cur_timestamp, src_buffer, size, width, height/*, subtype*/);
	}
	else
	{
		auto it = recv_video_pic_list_.find(account);
		if (it != recv_video_pic_list_.end())
		{
			it->second->ResetData(cur_timestamp, src_buffer, size, width, height/*, subtype*/);
		} 
		else
		{
			PicRegion* pic_info = new PicRegion;
			pic_info->ResetData(cur_timestamp, src_buffer, size, width, height/*, subtype*/);
			recv_video_pic_list_[account] = pic_info;
		}
	}
}
bool VideoFrameMng::GetVideoFrame(std::string account, int64_t& time, char* out_data, int& width, int& height, bool mirror, bool argb_or_yuv)
{
	nbase::NAutoLock auto_lock(&lock_);
	timeb time_now;
	ftime(&time_now); // 秒数
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒
	PicRegion* pic_info = nullptr;
	if (account.empty())
	{
		pic_info = &capture_video_pic_;
	}
	else
	{
		auto it = recv_video_pic_list_.find(account);
		if (it != recv_video_pic_list_.end())
		{
			pic_info = it->second;
		}
	}
	if (pic_info && pic_info->pdata_ && time < pic_info->timestamp_ && cur_timestamp - 1000 < pic_info->timestamp_)
	{
		time = pic_info->timestamp_;
		int src_w = pic_info->width_;
		int src_h = pic_info->height_;
		//等比
		if (width <= 0 || height <= 0)
		{
			width = src_w;
			height = src_h;
		}
		else if (src_h * width > src_w * height)
		{
			width = src_w * height / src_h;
		}
		else
		{
			height = src_h * width / src_w;
		}
		width -= width % 2;
		height -= height % 2;

		std::string ret_data;
		if (width != src_w || height != src_h)
		{
			ret_data.append(width * height * 3 / 2, (char)0);
			uint8_t* src_y = (uint8_t*)pic_info->pdata_;
			uint8_t* src_u = src_y + src_w * src_h;
			uint8_t* src_v = src_u + src_w * src_h / 4;
			uint8_t* des_y = (uint8_t*)ret_data.c_str();
			uint8_t* des_u = des_y + width * height;
			uint8_t* des_v = des_u + width * height / 4;
			libyuv::FilterMode filter_mode = libyuv::kFilterBox;
			libyuv::I420Scale(src_y, src_w,
				src_u, src_w / 2,
				src_v, src_w / 2,
				src_w, src_h,
				des_y, width,
				des_u, width / 2,
				des_v, width / 2,
				width, height,
				filter_mode);
		}
		else
		{
			ret_data.append(pic_info->pdata_, pic_info->size_);
		}
		if (mirror)
		{
			std::string data_src_temp = ret_data;
			uint8_t* src_y = (uint8_t*)data_src_temp.c_str();
			uint8_t* src_u = src_y + width * height;
			uint8_t* src_v = src_u + width * height / 4;
			uint8_t* des_y = (uint8_t*)ret_data.c_str();
			uint8_t* des_u = des_y + width * height;
			uint8_t* des_v = des_u + width * height / 4;
			libyuv::I420Mirror(src_y, width,
				src_u, width / 2,
				src_v, width / 2,
				des_y, width,
				des_u, width / 2,
				des_v, width / 2,
				width, height);
		}
		if (argb_or_yuv)
		{
			uint8_t* des_y = (uint8_t*)ret_data.c_str();
			uint8_t* des_u = des_y + width * height;
			uint8_t* des_v = des_u + width * height / 4;
			libyuv::I420ToARGB(
				des_y, width,
				des_u, width / 2,
				des_v, width / 2,
				(uint8_t*)out_data, width * 4,
				width, height);
		} 
		else
		{
			memcpy(out_data, ret_data.c_str(), ret_data.size());
		}
		return true;
	}
	return false;
}
}
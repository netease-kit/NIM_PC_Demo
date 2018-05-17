//
// Author:gq 均方差磨皮
// Date: 2016/7/29
//
#pragma once
#include <functional>
#include <algorithm>
#include <WinDef.h>

namespace nim_comp
{
	typedef struct smooth_info_s
	{
		uint64_t *matrix_;
		uint64_t *matrix_sqr_;
		uint32_t *skin_mask_;
		uint32_t width_;
		uint32_t height_;
	}
	smooth_info_t;

	smooth_info_t *smooth_init(uint8_t *in_i420, uint32_t width, uint32_t height);
	//磨皮的程度 sigema 
	//半径radius > 0 ? radius : std::max(info->width_, info->height_) * 0.02
	//磨皮后图像混合时 透明度，255代表磨皮其余全部替换原图，0代表不适用磨皮数据
	void smooth_processing(smooth_info_t *info, uint8_t *in_y, float sigema, int radius, uint8_t alpha);
	void smooth_clear(smooth_info_t **info);

	void smooth_process(uint8_t *i420, uint32_t width, uint32_t height, int sigema_level, int radius, uint8_t alpha);
}
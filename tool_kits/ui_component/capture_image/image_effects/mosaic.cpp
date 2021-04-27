#include "mosaic.h"
#include <stdlib.h>

#define COLOR_WEIGHT	3
#define RGB_GET(r) (r * 85.0 / RAND_MAX)
#define MIX_CLR(a, r) ((a * COLOR_WEIGHT + RGB_GET(r)) / (COLOR_WEIGHT + 1))

namespace ImageEffects
{
	void GetBlockColorMean(unsigned long &color, int real_width, int dest_width, int dest_height, long* data_start, int step)
	{
		//算均值
		unsigned long B_t = 0;
		unsigned long G_t = 0;
		unsigned long R_t = 0;
		unsigned long count = 0;
		unsigned char* point_clr;
		for (int i = 0; i < dest_height; i += step)
		{
			long* data_w_start = data_start + i * real_width;
			for (int j = 0; j < dest_width; j += step)
			{
				long* data_w = data_w_start + j;
				point_clr = (unsigned char*)data_w;
				B_t += *point_clr++;
				G_t += *point_clr++;
				R_t += *point_clr++;
				count++;
			}
		}
		B_t /= count;
		G_t /= count;
		R_t /= count;
		point_clr = (unsigned char*)&color;
		*point_clr++ = B_t;
		*point_clr++ = G_t;
		*point_clr++ = R_t;
	}
	void TransBlockColor(unsigned long color, int real_width, int dest_width, int dest_height, long* data_start)
	{
		//修改
		unsigned char* point_clr_dest = (unsigned char*)&color;
		for (int i = 0; i < dest_height; i++)
		{
			long* data_w_start = data_start + i * real_width;
			for (int j = 0; j < dest_width; j++)
			{
				long* data_w = data_w_start + j;
				unsigned char* point_clr = (unsigned char*)data_w;
				*point_clr++ = point_clr_dest[0];
				*point_clr++ = point_clr_dest[1];
				*point_clr++ = point_clr_dest[2];
				*point_clr = 0;
			}
		}
	}
	void ChangeRandomColor(unsigned long &color, int pt_num)
	{
		unsigned char* point_clr = (unsigned char*)&color;
		srand(pt_num);
		if (rand() % 256 > 0x7f)
		{
			srand(rand());
		}
		int r1 = rand();
		int r2 = rand();
		int r3 = rand();
		point_clr[0] = MIX_CLR(point_clr[0], r1);
		point_clr[1] = MIX_CLR(point_clr[1], r2);
		point_clr[2] = MIX_CLR(point_clr[2], r3);
	}
	void MosaicTransBlock(int real_width, int dest_width, int dest_height, long* data_start, int step)
	{
		//算均值
		unsigned long color = 0;
		GetBlockColorMean(color, real_width, dest_width, dest_height, data_start, step);
		//修改
		TransBlockColor(color, real_width, dest_width, dest_height, data_start);
	}
	void MosaicTransBlock(unsigned long color, int real_width, int dest_width, int dest_height, long* data_start, int pt_num)
	{
		ChangeRandomColor(color, pt_num);
		//修改
		TransBlockColor(color, real_width, dest_width, dest_height, data_start);
	}
	void MosaicTrans(	int real_width,		//图像数据宽度
						int real_height, 	//图像数据高度
						int left,			//马赛克处理区域
						int top,			//马赛克处理区域
						int right,			//马赛克处理区域
						int bottom,			//马赛克处理区域
						long* data,		//图像数据指针，ARGB，PC数据为从下往上的
						int mosaic_w,		//马赛克宽度
						int mosaic_h,		//马赛克高度
						bool random)		//加入随机
	{
		if (top < 0)
			top = 0;
		if (left < 0)
			left = 0;
		if (right >= real_width)
			right = real_width - 1;
		if (bottom >= real_height)
			bottom = real_height - 1;
		int block_w = 0;
		int block_h = 0;
		int step = (mosaic_w + mosaic_h) > 20 ? 2 : 1;
		unsigned long color = 0x7f7f7f;
		if (random)
		{
			GetBlockColorMean(color, real_width, right - left + 1, bottom - top + 1, &data[(real_height - bottom - 1)*real_width + left], step);
		}
		for (int i = top; i <= bottom; i += block_h)
		{
			block_h = mosaic_h - i % mosaic_h;
			if (block_h > bottom - i + 1)
			{
				block_h = bottom - i + 1;
			}
			for (int j = left; j <= right; j += block_w)
			{
				block_w = mosaic_w - j % mosaic_w;
				if (block_w > right - j + 1)
				{
					block_w = right - j + 1;
				}
				int pt_num = (real_height - i - block_h)*real_width + j;
				if (random)
				{
					MosaicTransBlock(color, real_width, block_w, block_h, &data[pt_num], pt_num);
				} 
				else
				{
					MosaicTransBlock(real_width, block_w, block_h, &data[pt_num], step);
				}
			}
		}
	}
}
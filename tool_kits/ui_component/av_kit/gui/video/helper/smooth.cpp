#include "smooth.h"
#include <math.h>
//#include"imagebright.h"
namespace nim_comp
{
	void smooth_init_integral(smooth_info_t *info, const uint8_t *in_y)
	{
		uint32_t width = info->width_;
		uint32_t height = info->height_;
		uint64_t *column_sum = new uint64_t[width];
		uint64_t *column_sum_sqr = new uint64_t[width];

		column_sum[0] = in_y[0];
		column_sum_sqr[0] = in_y[0] * in_y[0];

		info->matrix_[0] = column_sum[0];
		info->matrix_sqr_[0] = column_sum_sqr[0];

		for (int i = 1; i < width; i++) {

			column_sum[i] = in_y[i];
			column_sum_sqr[i] = in_y[i] * in_y[i];

			info->matrix_[i] = column_sum[i];
			info->matrix_[i] += info->matrix_[i - 1];
			info->matrix_sqr_[i] = column_sum_sqr[i];
			info->matrix_sqr_[i] += info->matrix_sqr_[i - 1];
		}
		for (int i = 1; i < height; i++) {
			int offset = i * width;

			column_sum[0] += in_y[offset];
			column_sum_sqr[0] += in_y[offset] * in_y[offset];

			info->matrix_[offset] = column_sum[0];
			info->matrix_sqr_[offset] = column_sum_sqr[0];
			// other columns  
			for (int j = 1; j < width; j++) {
				column_sum[j] += in_y[offset + j];
				column_sum_sqr[j] += in_y[offset + j] * in_y[offset + j];

				info->matrix_[offset + j] = info->matrix_[offset + j - 1] + column_sum[j];
				info->matrix_sqr_[offset + j] = info->matrix_sqr_[offset + j - 1] + column_sum_sqr[j];
			}
		}
		delete[] column_sum;
		delete[] column_sum_sqr;
	}
	void smooth_init_skin_mask(smooth_info_t *info, uint8_t *in_i420)
	{
		int wxh = info->width_ * info->height_;
		for (int i = 0; i < info->height_; i++) {
			int offset = i*info->width_;
			int offset2 = (i / 2)*(info->width_ / 2);
			for (int j = 0; j < info->width_; j++) {
				int src_y = (uint8_t)in_i420[offset];
				int src_u = (uint8_t)in_i420[wxh + offset2];
				int src_v = (uint8_t)in_i420[wxh * 5 / 4 + offset2];
				if ((src_u > 77 && src_u < 127) && (src_v > 133 && src_v < 173))
				{
					info->skin_mask_[offset] = 255;
					//in_i420[offset] = 255 - (255 - src_y)*0.9;
				}
				else
				{
					info->skin_mask_[offset] = 0;
				}
				offset++;
				if (j % 2 == 0)
				{
					offset2++;
				}
			}
		}
	}

	smooth_info_t *smooth_init(uint8_t *in_i420, uint32_t width, uint32_t height)
	{
		smooth_info_t *info = new smooth_info_t;
		info->matrix_ = new uint64_t[width*height];
		info->matrix_sqr_ = new uint64_t[width*height];
		info->skin_mask_ = new uint32_t[width*height];
		info->width_ = width;
		info->height_ = height;
		smooth_init_skin_mask(info, in_i420);
		smooth_init_integral(info, in_i420);
		return info;
	}
	void smooth_processing(smooth_info_t *info, uint8_t *in_y, float sigema, int radius, uint8_t alpha)
	{
		radius = radius > 0 ? radius : max(info->width_, info->height_) * 0.02;

		for (int i = 1; i < info->height_; i++) {
			for (int j = 1; j < info->width_; j++) {
				int offset = i * info->width_ + j;
				if (info->skin_mask_[offset] > 0) {
					int i_max = i + radius >= info->height_ - 1 ? info->height_ - 1 : i + radius;
					int j_max = j + radius >= info->width_ - 1 ? info->width_ - 1 : j + radius;
					int i_min = i - radius <= 1 ? 1 : i - radius;
					int j_min = j - radius <= 1 ? 1 : j - radius;

					int squar = (i_max - i_min + 1)*(j_max - j_min + 1);
					int i4 = i_max*info->width_ + j_max;
					int i3 = (i_min - 1)*info->width_ + (j_min - 1);
					int i2 = i_max*info->width_ + (j_min - 1);
					int i1 = (i_min - 1)*info->width_ + j_max;

					float m = (info->matrix_[i4]
						+ info->matrix_[i3]
						- info->matrix_[i2]
						- info->matrix_[i1]) / squar;

					float v = (info->matrix_sqr_[i4]
						+ info->matrix_sqr_[i3]
						- info->matrix_sqr_[i2]
						- info->matrix_sqr_[i1]) / squar - m*m;
					float k = v / (v + sigema);
					int32_t temp = m - k * m + k * in_y[offset];
					temp = in_y[offset] * (255 - alpha) + temp * alpha;
					temp /= 255;
					in_y[offset] = temp > 255 ? 255 : temp;
				}
			}
		}
	}
	void smooth_clear(smooth_info_t **info)
	{
		if (*info)
		{
			delete[](*info)->matrix_;
			delete[](*info)->matrix_sqr_;
			delete[](*info)->skin_mask_;
			delete (*info);
			*info = nullptr;
		}
	}
	
	void smooth_process(uint8_t *i420, uint32_t width, uint32_t height, int sigema_level, int radius, uint8_t alpha)
	{
	
		smooth_info_t *info = smooth_init(i420, width, height);
		smooth_processing(info, i420, sigema_level*sigema_level * 5 + 10, radius, alpha);
		smooth_clear(&info);
	}

}
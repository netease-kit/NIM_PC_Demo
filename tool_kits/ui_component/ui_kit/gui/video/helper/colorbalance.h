#ifndef COLORBALANCE_H
#define COLORBALANCE_H
namespace nim_comp
{
	unsigned char *colorbalance_rgb_u8(unsigned char *rgb, size_t size, size_t nb_min, size_t nb_max,size_t step);
	unsigned char *colorbalance_yuv_u8(unsigned char *yuv, size_t size, size_t nb_min, size_t nb_max);
	float *colorbalance_irgb_f32(float *rgb, size_t size, size_t nb_min, size_t nb_max);
	unsigned char *balance_u8(unsigned char *data, size_t size, size_t nb_min, size_t nb_max,size_t offset,size_t step);
	float *balance_f32(float *data, size_t size, size_t nb_min, size_t nb_max);
	float *idiv_f32(float *a, const float *b, size_t size);
	float *imul_f32(float *a, const float *b, size_t size);
	float *imuls_f32(float *a, const float b, size_t size);
	float *iadd_f32(float *a, const float *b, size_t size);
}
#endif

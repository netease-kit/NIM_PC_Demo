
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>

/* ensure consistency */
#include "colorbalance.h"


namespace nim_comp
{
	/**
	* @brief get the min/max of an unsigned char array
	*
	* @param data input array
	* @param size array size
	* @param ptr_min, ptr_max pointers to the returned values, ignored if NULL
	*/
	static void minmax_u8(const unsigned char *data, size_t size,
		unsigned char *ptr_min, unsigned char *ptr_max,size_t offset,size_t step)
	{
		unsigned char min, max;
		size_t i;
		size_t index = 0;
		/* compute min and max */
		min = data[0+offset];
		max = data[0+offset];
		for (i = 1; i < size; i++) 
		{
			index = i*step + offset;
			
			if (data[index] < min)
				min = data[index];
			if (data[index] > max)
				max = data[index];
		}

		/* save min and max to the returned pointers if available */
		if (NULL != ptr_min)
			*ptr_min = min;
		if (NULL != ptr_max)
			*ptr_max = max;
		return;
	}

	/**
	* @brief get the min/max of a float array
	*
	* @param data input array
	* @param size array size
	* @param ptr_min, ptr_max pointers to the returned values, ignored if NULL
	*/
	static void minmax_f32(const float *data, size_t size,
		float *ptr_min, float *ptr_max)
	{
		float min, max;
		size_t i;

		/* compute min and max */
		min = data[0];
		max = data[0];
		for (i = 1; i < size; i++) {
			if (data[i] < min)
				min = data[i];
			if (data[i] > max)
				max = data[i];
		}

		/* save min and max to the returned pointers if available */
		if (NULL != ptr_min)
			*ptr_min = min;
		if (NULL != ptr_max)
			*ptr_max = max;
		return;
	}

	/**
	* @brief get quantiles from an unsigned char array such that a given
	* number of pixels is out of this interval
	*
	* This function computes min (resp. max) such that the number of
	* pixels < min (resp. > max) is inferior or equal to nb_min
	* (resp. nb_max). It uses an histogram algorithm.
	*
	* @param data input/output
	* @param size data array size
	* @param nb_min, nb_max number of pixels to flatten
	* @param ptr_min, ptr_max computed min/max output, ignored if NULL
	*/
	static void quantiles_u8(const unsigned char *data, size_t size,
		size_t nb_min, size_t nb_max,
		unsigned char *ptr_min, unsigned char *ptr_max,size_t offset,size_t step)
	{
		/*
		* the histogram must hold all possible "unsigned char" values,
		* including 0
		*/
		size_t h_size = UCHAR_MAX + 1;
		size_t histo[UCHAR_MAX + 1];
		size_t i;

		/* make a cumulative histogram
		* 构建直方图
		*/
		memset(histo, 0x00, h_size * sizeof(size_t));
		for (i = 0; i < size; i++)
			histo[(size_t)data[i*step+offset]] += 1;
		for (i = 1; i < h_size; i++)
			histo[i] += histo[i - 1];

		/* get the new min/max */

		if (NULL != ptr_min) {
			/* simple forward traversal of the cumulative histogram */
			/* search the first value > nb_min */
			i = 0;
			while (i < h_size && histo[i] <= nb_min)
				i++;
			/* the corresponding histogram value is the current cell position */
			*ptr_min = (unsigned char)i;
		}

		if (NULL != ptr_max) {
			/* simple backward traversal of the cumulative histogram */
			/* search the first value <= size - nb_max */
			i = h_size - 1;
			/* i is unsigned, we check i<h_size instead of i>=0 */
			while (i < h_size && histo[i] >(size - nb_max))
				i--;
			/*
			* if we are not at the end of the histogram,
			* get to the next cell,
			* the last (backward) value > size - nb_max
			*/
			if (i < h_size - 1)
				i++;
			*ptr_max = (unsigned char)i;
		}
		return;
	}

	/**
	* @brief float comparison
	*
	* IEEE754 floats can be compared as integers. Not *converted* to
	* integers, but *read* as integers while maintaining an order.
	* cf. http://www.cygnus-software.com/papers/comparingfloats/Comparing%20floating%20point%20numbers.htm#_Toc135149455
	*/
	static int cmp_f32(const void *a, const void *b)
	{
		if (*(const int *)a > *(const int *)b)
			return 1;
		if (*(const int *)a < *(const int *)b)
			return -1;
		return 0;
	}

	/**
	* @brief get quantiles from a float array such that a given
	* number of pixels is out of this interval
	*
	* This function computes min (resp. max) such that the number of
	* pixels < min (resp. > max) is inferior or equal to nb_min
	* (resp. nb_max). It uses a sorting algorithm.
	*
	* @param data input/output
	* @param size data array size
	* @param nb_min, nb_max number of pixels to flatten
	* @param ptr_min, ptr_max computed min/max output, ignored if NULL
	*
	* @todo instead of sorting the whole array (expensive), select
	* pertinent values with a 128 bins histogram then sort the bins
	* around the good bin
	*/
	static void quantiles_f32(const float *data, size_t size,
		size_t nb_min, size_t nb_max,
		float *ptr_min, float *ptr_max)
	{
		float *data_tmp;

		data_tmp = (float *)malloc(size * sizeof(float));

		/* copy the data and sort */
		memcpy(data_tmp, data, size * sizeof(float));
		qsort(data_tmp, size, sizeof(float), &cmp_f32);

		/* get the min/max */
		if (NULL != ptr_min)
			*ptr_min = data_tmp[nb_min];
		if (NULL != ptr_max)
			*ptr_max = data_tmp[size - 1 - nb_max];

		free(data_tmp);
		return;
	}

	/**
	* @brief rescale an unsigned char array
	*
	* This function operates in-place. It rescales the data by a bounded
	* affine function such that min becomes 0 and max becomes UCHAR_MAX.
	*
	* @param data input/output array
	* @param size array size
	* @param min, max the minimum and maximum of the input array
	*
	* @return data
	*/
	static unsigned char *rescale_u8(unsigned char *data, size_t size,
		unsigned char min, unsigned char max,size_t offset,size_t step)
	{
		size_t i;

		if (max <= min)
			for (i = 0; i < size; i++)
				data[i*step+offset] = UCHAR_MAX / 2;
		else {
			/* build a normalization table */
			unsigned char norm[UCHAR_MAX + 1];
			for (i = 0; i < min; i++)
				norm[i] = 0;
			for (i = min; i < max; i++)
				/*
				* we can't store and reuse UCHAR_MAX / (max - min) because
				*     105 * 255 / 126.            -> 212.5, rounded to 213
				*     105 * (double) (255 / 126.) -> 212.4999, rounded to 212
				*/
				norm[i] = (unsigned char)((i - min) * UCHAR_MAX
				/ (double)(max - min) + .5);
			for (i = max; i < UCHAR_MAX + 1; i++)
				norm[i] = UCHAR_MAX;
			/* use the normalization table to transform the data */
			for (i = 0; i < size; i++)
				data[i*step+offset] = norm[(size_t)data[i*step+offset]];
		}
		return data;
	}

	/**
	* @brief rescale a float array
	*
	* This function operates in-place. It rescales the data by a bounded
	* affine function such that min becomes 0 and max becomes 1.
	* Warnings similar to the ones mentioned in rescale_u8() apply about
	* the risks of rounding errors.
	*
	* @param data input/output array
	* @param size array size
	* @param min, max the minimum and maximum of the input array
	*
	* @return data
	*/
	static float *rescale_f32(float *data, size_t size, float min, float max)
	{
		size_t i;

		if (max <= min)
			for (i = 0; i < size; i++)
				data[i] = .5;
		else
			for (i = 0; i < size; i++)
				data[i] = (min > data[i] ? 0. :
				(max < data[i] ? 1. : (data[i] - min) / (max - min)));

		return data;
	}

	/**
	* @brief normalize an unsigned char array
	*
	* This function operates in-place. It computes the minimum and
	* maximum values of the data, and rescales the data to
	* [0-UCHAR_MAX], with optionally flattening some extremal pixels.
	*
	* @param data input/output array
	* @param size array size
	* @param nb_min, nb_max number extremal pixels flattened
	*
	* @return data
	*/
	unsigned char *balance_u8(unsigned char *data, size_t size,
		size_t nb_min, size_t nb_max,size_t offset,size_t step)
	{
		unsigned char min, max;

		/* sanity checks */
		if (NULL == data) {
			fprintf(stderr, "a pointer is NULL and should not be so\n");
			abort();
		}
		if (nb_min + nb_max >= size) {
			nb_min = (size - 1) / 2;
			nb_max = (size - 1) / 2;
			fprintf(stderr, "the number of pixels to flatten is too large\n");
			fprintf(stderr, "using (size - 1) / 2\n");
		}

		/* get the min/max */
		if (0 != nb_min || 0 != nb_max)
			quantiles_u8(data, size, nb_min, nb_max, &min, &max,offset,step);
		else
			minmax_u8(data, size, &min, &max, offset, step);

		/* rescale 
		*/
		(void)rescale_u8(data, size, min, max, offset, step);

		return data;
	}

	/**
	* @brief normalize a float array
	*
	* This function operates in-place. It computes the minimum and
	* maximum values of the data, and rescales the data to
	* [0-1], with optionally flattening some extremal pixels.
	*
	* @param data input/output array
	* @param size array size
	* @param nb_min, nb_max number extremal pixels flattened
	*
	* @return data
	*/
	float *balance_f32(float *data, size_t size, size_t nb_min, size_t nb_max)
	{
		float min, max;

		/* sanity checks */
		if (NULL == data) {
			fprintf(stderr, "a pointer is NULL and should not be so\n");
			abort();
		}
		if (nb_min + nb_max >= size) {
			nb_min = (size - 1) / 2;
			nb_max = (size - 1) / 2;
			fprintf(stderr, "the number of pixels to flatten is too large\n");
			fprintf(stderr, "using (size - 1) / 2\n");
		}

		/* get the min/max */
		if (0 != nb_min || 0 != nb_max)
			quantiles_f32(data, size, nb_min, nb_max, &min, &max);
		else
			minmax_f32(data, size, &min, &max);

		/* rescale */
		(void)rescale_f32(data, size, min, max);

		return data;
	}

	

	/**
	* @brief simplest color balance on RGB channels
	*
	* The input image is normalized by affine transformation on each RGB
	* channel, saturating a percentage of the pixels at the beginning and
	* end of the color space on each channel.
	*/
	unsigned char *colorbalance_rgb_u8(unsigned char *rgb, size_t size,
		size_t nb_min, size_t nb_max,size_t step)
	{
		(void)balance_u8(rgb, size, nb_min, nb_max,0,step); 
		(void)balance_u8(rgb, size, nb_min, nb_max,1,step);
		(void)balance_u8(rgb, size, nb_min, nb_max,2,step);
		return rgb;
	}

	unsigned char *colorbalance_yuv_u8(unsigned char *yuv, size_t size, size_t nb_min, size_t nb_max)
	{
		(void)balance_u8(yuv, size, nb_min, nb_max,0,1); //对y值取平衡,对亮度进行调整
		return yuv;
	}

	/** @brief max of A and B */
#define MAX(A,B) (((A) >= (B)) ? (A) : (B))

	/** @brief max of A, B, and C */
#define MAX3(A,B,C) (((A) >= (B)) ? MAX(A,C) : MAX(B,C))

	/**
	* @brief simplest color balance based on the I axis applied to the
	* RGB channels, bounded
	*
	* The input image is normalized by affine transformation on the I
	* axis, saturating a percentage of the pixels at the beginning and
	* end of the axis. This transformation is linearly applied to the R,
	* G and B channels. The RGB cube is not stable by this operation, so
	* some projections towards (0,0,0) on the RGB cube will be performed
	* if needed.
	*/
	float *colorbalance_irgb_f32(float *rgb, size_t size,
		size_t nb_min, size_t nb_max)
	{
		float *irgb, *inorm;        /* intensity scale */
		double s, m;
		size_t i;
		/** @todo compute I=R+G+B instead of (R+G+B)/3 to save a division */
		irgb = (float *)malloc(size * sizeof(float));
		for (i = 0; i < size; i++)
			irgb[i] = (rgb[i] + rgb[i + size] + rgb[i + 2 * size]) / 3.;
		/* copy and normalize I */
		inorm = (float *)malloc(size * sizeof(float));
		memcpy(inorm, irgb, size * sizeof(float));
		(void)balance_f32(inorm, size, nb_min, nb_max);
		/*
		* apply the I normalization to the RGB channels:
		* RGB = RGB * Inorm / I, with a projection towards (0,0,0)
		* on the RGB cube if needed
		*/
		for (i = 0; i < size; i++) {
			m = MAX3(rgb[i], rgb[i + size], rgb[i + 2 * size]);
			s = inorm[i] / irgb[i];
			/* if m * s > 1, a projection is needed by adjusting s */
			s = (1. < m * s ? 1. / m : s);
			rgb[i] *= s;
			rgb[i + size] *= s;
			rgb[i + 2 * size] *= s;
		}
		free(irgb);
		free(inorm);
		return rgb;
	}
}
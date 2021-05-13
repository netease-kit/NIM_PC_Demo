#include "gauss_blur.h"
#include <ppl.h>
#include <intrin.h>
#include <math.h>
#include <vector>

using namespace Concurrency;
using namespace std;

#pragma region macros
#define MM_CVTI32_PS(x) _mm_set_ps(((unsigned char*)x)[3],((unsigned char*)x)[2],((unsigned char*)x)[1],((unsigned char*)x)[0])
#define MM_CVTI32_PS_EX(x,y) x.m128_f32[0] = ((unsigned char*)y)[0];x.m128_f32[1] = ((unsigned char*)y)[1];x.m128_f32[2] = ((unsigned char*)y)[2];x.m128_f32[3] = ((unsigned char*)y)[3];
#define MM_LOAD_SS_SHUFFLE_PS(x,y) x.m128_f32[0] = x.m128_f32[1] = x.m128_f32[2] = x.m128_f32[3] = y;
#define MM_MUL_PS(x,y,z) x.m128_f32[0] = y.m128_f32[0]*z.m128_f32[0];x.m128_f32[1] = y.m128_f32[1]*z.m128_f32[1];x.m128_f32[2] = y.m128_f32[2]*z.m128_f32[2];x.m128_f32[3] = y.m128_f32[3]*z.m128_f32[3];
#define MM_ADD_PS(x,y,z) x.m128_f32[0] = y.m128_f32[0]+z.m128_f32[0];x.m128_f32[1] = y.m128_f32[1]+z.m128_f32[1];x.m128_f32[2] = y.m128_f32[2]+z.m128_f32[2];x.m128_f32[3] = y.m128_f32[3]+z.m128_f32[3];
#define MM_SUB_PS(x,y,z) x.m128_f32[0] = y.m128_f32[0]-z.m128_f32[0];x.m128_f32[1] = y.m128_f32[1]-z.m128_f32[1];x.m128_f32[2] = y.m128_f32[2]-z.m128_f32[2];x.m128_f32[3] = y.m128_f32[3]-z.m128_f32[3];

#define MM_LOAD_PS(x,y)  x.m128_f32[0] = y[0];x.m128_f32[1] = y[1];x.m128_f32[2] = y[2]; x.m128_f32[3] = y[3]; //float =>__m128i
#define MM_STORE_PS(x,y) x[0] = y.m128_f32[0];   x[1] = y.m128_f32[1]; x[2] = y.m128_f32[2]; x[3] = y.m128_f32[3];//__m128i =>float
#define MM_CVTTPS_EPI32(x,y) x.m128i_i32[0] = y.m128_f32[0]; x.m128i_i32[1] = y.m128_f32[1];x.m128i_i32[2] = y.m128_f32[2];x.m128i_i32[3] = y.m128_f32[3];//f32=>i32

#define MM_MUL_PS_EX(x,y,z) for (int k=0;k < 4;k++)	x.m128_f32[k] = y.m128_f32[k]*z.m128_f32[k];
#define MM_ADD_PS_EX(x,y,z) for (int k=0;k < 4;k++)	x.m128_f32[k] = y.m128_f32[k]+z.m128_f32[k];
#define MM_SUB_PS_EX(x,y,z) for (int k=0;k < 4;k++)	x.m128_f32[k] = y.m128_f32[k]-z.m128_f32[k];

#define MM_LOAD_PS_EX(x,y)  for (int k=0;k < 4;k++)	x.m128_f32[k] = y[k];  //float =>__m128i
#define MM_STORE_PS_EX(x,y) for (int k=0;k < 4;k++)	x[k] = y.m128_f32[k];  //__m128i =>float
#define MM_CVTTPS_EPI32_EX(x,y) for (int k=0;k < 4;k++) x.m128i_i32[k] = y.m128_f32[k]; //f32=>i32
#pragma endregion
#define THREADS_MAX 8
#define THREADS_MIN 2
namespace ImageEffects
{
	inline void coverti32_ps(__m128& m,unsigned int* in)
	{
		m.m128_f32[0] = ((unsigned char*)in)[0];
		m.m128_f32[1] = ((unsigned char*)in)[1];
		m.m128_f32[2] = ((unsigned char*)in)[2];
		m.m128_f32[3] = ((unsigned char*)in)[3];
	}
	//计算滤波系数;
	iir_coefs CaculateCoefs(float sigma)
	{
		iir_coefs coefs;

		float alpha, lamma,  k; 
		if (sigma < 0.5f) //不小于0.5;
			sigma = 0.5f;

		alpha = (float) exp((0.726)*(0.726)) / sigma;
		lamma = (float)exp(-alpha);
		coefs.b2 = (float)exp(-2*alpha);
		k = (1-lamma)*(1-lamma)/(1+2*alpha*lamma- (coefs.b2));
		coefs.a0 = k;
		coefs.a1 = k*(alpha-1)*lamma;
		coefs.a2 = k*(alpha+1)*lamma;
		coefs.a3 = -k* (coefs.b2);
		coefs.b1 = -2*lamma;
		coefs.cprev = (coefs.a0 + coefs.a1)/(1+ coefs.b1 + coefs.b2);
		coefs.cnext = (coefs.a2 + coefs.a3)/(1+ coefs.b1 + coefs.b2);
		return coefs;
	}

	//水平方向像素模糊(SSE指令集优化);;
	void GaussHorizontalSSE(iir_coefs& coefs,bool has_alpha, int width, int height,float *temp,  unsigned int* in, float *out)
	{
		__m128 prevIn, currIn, prevOut, prev2Out, coeft, coefa0, coefa1, coefb1, coefb2;

		//边界处理;
		coeft = _mm_load_ss((float *) &coefs.cprev);
		coeft = _mm_shuffle_ps(coeft, coeft, 0x00);

		prevIn = MM_CVTI32_PS(in);
		prev2Out = _mm_mul_ps(prevIn,  coeft);
		prevOut = prev2Out;


		//加载系数到__m128;
		coefa0 = _mm_load_ss((float *) &coefs.a0);
		coefa0 = _mm_shuffle_ps(coefa0, coefa0, 0x00);
		coefa1 = _mm_load_ss((float *) &coefs.a1);
		coefa1 = _mm_shuffle_ps(coefa1, coefa1, 0x00);
		coefb1 = _mm_load_ss((float *) &coefs.b1);
		coefb1 = _mm_shuffle_ps(coefb1, coefb1, 0x00);
		coefb2 = _mm_load_ss((float *) &coefs.b2);
		coefb2 = _mm_shuffle_ps(coefb2, coefb2, 0x00);

		//left-to-right pass
		for (int i = 0; i < width; i++) 
		{
			__m128 currIn  = MM_CVTI32_PS(in);
			__m128 currComp = _mm_mul_ps(currIn, coefa0);			
			__m128 temp1 = _mm_mul_ps(prevIn, coefa1);				
			__m128 temp2 = _mm_mul_ps(prevOut,coefb1);				
			__m128 temp3 = _mm_mul_ps(prev2Out, coefb2);			
			currComp = _mm_add_ps(currComp, temp1);					
			temp2 = _mm_add_ps(temp2, temp3);						

			prev2Out = prevOut;										
			prevOut= _mm_sub_ps(currComp, temp2);					
			prevIn = currIn;			
			_mm_storeu_ps((float *)(temp), prevOut);

			in += 1 ; temp += 4;
		}

		// right-to-left horizontal pass
		in -= 1;
		temp -=4;

		out += 4*(width-1);


		//边界处理;
		coeft = _mm_load_ss((float *) &coefs.cnext);
		coeft = _mm_shuffle_ps(coeft, coeft, 0x00);
		prevIn  = MM_CVTI32_PS(in);
		prev2Out = _mm_mul_ps(prevIn, coeft);
		prevOut = prev2Out;
		currIn = prevIn;


		////加载系数到__m128;
		coefa0 = _mm_load_ss((float *) &coefs.a2);
		coefa0 = _mm_shuffle_ps(coefa0, coefa0, 0x00);
		coefa1 = _mm_load_ss((float *) &coefs.a3);
		coefa1 = _mm_shuffle_ps(coefa1, coefa1, 0x00);

		for (int i = width-1; i >= 0; i--) 
		{
			__m128 inNext  = MM_CVTI32_PS(in);
			__m128 output = _mm_loadu_ps((float *) (temp)); 

			__m128 currComp = _mm_mul_ps(currIn, coefa0); 
			__m128 temp1 = _mm_mul_ps(prevIn, coefa1);     
			__m128 temp2 = _mm_mul_ps(prevOut, coefb1); 
			__m128 temp3 = _mm_mul_ps(prev2Out, coefb2);  
			currComp = _mm_add_ps(currComp, temp1);   
			temp2 = _mm_add_ps(temp2, temp3);		

			prev2Out = prevOut;		
			prevOut= _mm_sub_ps(currComp, temp2);   
			prevIn = currIn;							
			currIn = inNext;					

			output = _mm_add_ps(output, prevOut);

			_mm_storeu_ps((float *)(out), output);
			in -= 1; out -= 4; 
			temp -=4;
		}
	}

	//垂直方向像素模糊(SSE指令集优化);;
	void GaussVerticalSSE(iir_coefs& coefs,bool has_alpha, int width, int height,int real_width,float *temp,  float* in, unsigned int *out)
	{
		__m128 prevIn, currIn, prevOut, prev2Out, coeft, coefa0, coefa1, coefb1, coefb2;

		//边界处理;
		coeft = _mm_load_ss((float *) &coefs.cprev);
		coeft = _mm_shuffle_ps(coeft, coeft, 0x00);

		prevIn = _mm_loadu_ps((float *) (in));
		prev2Out = _mm_mul_ps(prevIn,  coeft);
		prevOut = prev2Out;

		//加载系数到__m128;
		coefa0 = _mm_load_ss((float *) &coefs.a0);
		coefa0 = _mm_shuffle_ps(coefa0, coefa0, 0x00);
		coefa1 = _mm_load_ss((float *) &coefs.a1);
		coefa1 = _mm_shuffle_ps(coefa1, coefa1, 0x00);
		coefb1 = _mm_load_ss((float *) &coefs.b1);
		coefb1 = _mm_shuffle_ps(coefb1, coefb1, 0x00);
		coefb2 = _mm_load_ss((float *) &coefs.b2);
		coefb2 = _mm_shuffle_ps(coefb2, coefb2, 0x00);

		int stride = 4 * width;
		//top-to-bottom pass
		for (int i = 0; i < height; i++) 
		{
			__m128 currIn  = _mm_loadu_ps((float*)(in));
			__m128 currComp = _mm_mul_ps(currIn, coefa0);		
			__m128 temp1 = _mm_mul_ps(prevIn, coefa1);		
			__m128 temp2 = _mm_mul_ps(prevOut,coefb1);		
			__m128 temp3 = _mm_mul_ps(prev2Out, coefb2);	
			currComp = _mm_add_ps(currComp, temp1);	
			temp2 = _mm_add_ps(temp2, temp3);

			prev2Out = prevOut;		
			prevOut= _mm_sub_ps(currComp, temp2);
			prevIn = currIn;

			_mm_storeu_ps((float *)(temp), prevOut);

			in += stride ; temp += 4;
		}

		//bottom-to-top pass
		in -= stride;
		temp -=4;
		out += real_width*(height-1);


		//边界处理;
		coeft = _mm_load_ss((float *) &coefs.cnext);
		coeft = _mm_shuffle_ps(coeft, coeft, 0x00);
		prevIn  = _mm_loadu_ps((float*)(in));
		prev2Out = _mm_mul_ps(prevIn, coeft);
		prevOut = prev2Out;
		currIn = prevIn;


		//加载系数到__m128;
		coefa0 = _mm_load_ss((float *) &coefs.a2);
		coefa0 = _mm_shuffle_ps(coefa0, coefa0, 0x00);
		coefa1 = _mm_load_ss((float *) &coefs.a3);
		coefa1 = _mm_shuffle_ps(coefa1, coefa1, 0x00);

		for (int i = height-1; i >= 0; i--) 
		{
			__m128 inNext  = _mm_loadu_ps((float*)(in));
			__m128 output = _mm_loadu_ps((float *) (temp)); 

			__m128 currComp = _mm_mul_ps(currIn, coefa0); 
			__m128 temp1 = _mm_mul_ps(prevIn, coefa1); 
			__m128 temp2 = _mm_mul_ps(prevOut, coefb1);
			__m128 temp3 = _mm_mul_ps(prev2Out, coefb2); 
			currComp = _mm_add_ps(currComp, temp1); 
			temp2 = _mm_add_ps(temp2, temp3);

			prev2Out = prevOut;	
			prevOut= _mm_sub_ps(currComp, temp2); 
			prevIn = currIn;
			currIn = inNext;

			output = _mm_add_ps(output, prevOut);
			__m128i  output0 = _mm_cvttps_epi32(output);

			((unsigned char*)out)[0] = ((unsigned char*)&output0.m128i_i32[0])[0];
			((unsigned char*)out)[1] = ((unsigned char*)&output0.m128i_i32[1])[0];
			((unsigned char*)out)[2] = ((unsigned char*)&output0.m128i_i32[2])[0];
			if (has_alpha) ((unsigned char*)out)[3]=((unsigned char*)&output0.m128i_i32[3])[0];

			in -= stride; out -= real_width; 
			temp -=4;
		}
	}

	//水平方向像素模糊(SSE指令集优化);;
	void GaussHorizontal(iir_coefs& coefs,bool has_alpha, int width, int height,float *temp,  unsigned int* in, float *out)
	{
		__m128 prevIn, currIn, prevOut, prev2Out, coeft, coefa0, coefa1, coefb1, coefb2;

		//边界处理;
		MM_LOAD_SS_SHUFFLE_PS(coeft,coefs.cprev);


		MM_CVTI32_PS_EX(prevIn,in);
		MM_MUL_PS(prev2Out,prevIn,coeft);
		prevOut = prev2Out;


		//加载系数到__m128;
		MM_LOAD_SS_SHUFFLE_PS(coefa0,coefs.a0);
		MM_LOAD_SS_SHUFFLE_PS(coefa1,coefs.a1);
		MM_LOAD_SS_SHUFFLE_PS(coefb1,coefs.b1);
		MM_LOAD_SS_SHUFFLE_PS(coefb2,coefs.b2);

		//left-to-right pass
		for (int i = 0; i < width; i++) 
		{
			__m128 currIn;
			__m128 currComp;			
			__m128 temp1;				
			__m128 temp2;				
			__m128 temp3;	
			MM_CVTI32_PS_EX(currIn,in);
			MM_MUL_PS(currComp,currIn, coefa0);
			MM_MUL_PS(temp1,prevIn, coefa1);
			MM_MUL_PS(temp2,prevOut,coefb1);
			MM_MUL_PS(temp3,prev2Out, coefb2);

			MM_ADD_PS(currComp,currComp,temp1);
			MM_ADD_PS(temp2,temp2,temp3);

			prev2Out = prevOut;										
			MM_SUB_PS(prevOut,currComp, temp2);

			prevIn = currIn;			
			MM_STORE_PS(temp,prevOut);

			in += 1 ; temp += 4;
		}

		// right-to-left horizontal pass
		in -= 1;
		temp -=4;

		out += 4*(width-1);


		//边界处理;
		MM_LOAD_SS_SHUFFLE_PS(coeft,coefs.cnext);
		MM_CVTI32_PS_EX(prevIn,in);
		//prev2Out = _mm_mul_ps(prevIn, coeft);
		MM_MUL_PS(prev2Out,prevIn, coeft);
		prevOut = prev2Out;
		currIn = prevIn;


		////加载系数到__m128;
		MM_LOAD_SS_SHUFFLE_PS(coefa0,coefs.a2);
		MM_LOAD_SS_SHUFFLE_PS(coefa1,coefs.a3);

		for (int i = width-1; i >= 0; i--) 
		{
			__m128 inNext;
			__m128 output;
			__m128 currComp;
			__m128 temp1;
			__m128 temp2;
			__m128 temp3;
			MM_CVTI32_PS_EX(inNext,in);
			MM_LOAD_PS(output,temp);
			MM_MUL_PS(currComp,currIn, coefa0);
			MM_MUL_PS(temp1,prevIn, coefa1);
			MM_MUL_PS(temp2,prevOut,coefb1);
			MM_MUL_PS(temp3,prev2Out, coefb2);

			MM_ADD_PS(currComp,currComp, temp1);
			MM_ADD_PS(temp2,temp2, temp3);

			prev2Out = prevOut;		
			MM_SUB_PS(prevOut,currComp,temp2);
			prevIn = currIn;							
			currIn = inNext;					

			MM_ADD_PS(output,output, prevOut);
			MM_STORE_PS(out,output);
			in -= 1; out -= 4; 
			temp -=4;
		}
	}

	//垂直方向像素模糊(SSE指令集优化);;
	void GaussVertical(iir_coefs& coefs,bool has_alpha, int width, int height,int real_width,float *temp,  float* in, unsigned int *out)
	{
		__m128 prevIn, currIn, prevOut, prev2Out, coeft, coefa0, coefa1, coefb1, coefb2;

		//边界处理;
		MM_LOAD_SS_SHUFFLE_PS(coeft,coefs.cprev);
		MM_LOAD_PS(prevIn,in);
		MM_MUL_PS(prev2Out,prevIn,coeft);
		prevOut = prev2Out;

		//加载系数到__m128;
		MM_LOAD_SS_SHUFFLE_PS(coefa0,coefs.a0);
		MM_LOAD_SS_SHUFFLE_PS(coefa1,coefs.a1);
		MM_LOAD_SS_SHUFFLE_PS(coefb1,coefs.b1);
		MM_LOAD_SS_SHUFFLE_PS(coefb2,coefs.b2);

		int stride = 4 * width;
		//top-to-bottom pass
		for (int i = 0; i < height; i++) 
		{
			__m128 currIn;
			__m128 currComp;		
			__m128 temp1;		
			__m128 temp2;		
			__m128 temp3;	

			MM_LOAD_PS(currIn,in);
			MM_MUL_PS(currComp,currIn, coefa0);
			MM_MUL_PS(temp1,prevIn, coefa1);
			MM_MUL_PS(temp2,prevOut,coefb1);
			MM_MUL_PS(temp3,prev2Out, coefb2);

			MM_ADD_PS(currComp,currComp, temp1);
			MM_ADD_PS(temp2,temp2, temp3);

			prev2Out = prevOut;		
			MM_SUB_PS(prevOut,currComp, temp2);
			prevIn = currIn;

			MM_STORE_PS(temp,prevOut);
			in += stride ; temp += 4;
		}

		//bottom-to-top pass
		in -= stride;
		temp -=4;
		out += real_width*(height-1);


		//边界处理;
		MM_LOAD_SS_SHUFFLE_PS(coeft,coefs.cnext);
		MM_LOAD_PS(prevIn,in);
		MM_MUL_PS(prev2Out,prevIn, coeft);
		prevOut = prev2Out;
		currIn = prevIn;


		//加载系数到__m128;
		MM_LOAD_SS_SHUFFLE_PS(coefa0,coefs.a2);
		MM_LOAD_SS_SHUFFLE_PS(coefa1,coefs.a3);

		for (int i = height-1; i >= 0; i--) 
		{
			__m128 inNext;
			__m128 output;
			__m128 currComp;
			__m128 temp1;
			__m128 temp2;
			__m128 temp3;
			MM_LOAD_PS(inNext,in);
			MM_LOAD_PS(output,temp);
			MM_MUL_PS(currComp,currIn, coefa0);
			MM_MUL_PS(temp1,prevIn, coefa1);
			MM_MUL_PS(temp2,prevOut,coefb1);
			MM_MUL_PS(temp3,prev2Out, coefb2);

			MM_ADD_PS(currComp,currComp, temp1);
			MM_ADD_PS(temp2,temp2, temp3);


			prev2Out = prevOut;	
			MM_SUB_PS(prevOut,currComp, temp2); 
			prevIn = currIn;
			currIn = inNext;

			MM_ADD_PS(output,output, prevOut);
			__m128i  output0;
			MM_CVTTPS_EPI32(output0,output);
			((unsigned char*)out)[0] = ((unsigned char*)&output0.m128i_i32[0])[0];
			((unsigned char*)out)[1] = ((unsigned char*)&output0.m128i_i32[1])[0];
			((unsigned char*)out)[2] = ((unsigned char*)&output0.m128i_i32[2])[0];
			if (has_alpha) ((unsigned char*)out)[3]=((unsigned char*)&output0.m128i_i32[3])[0];

			in -= stride; out -= real_width; 
			temp -=4;
		}
	}


	//模糊处理image必须是ARGB格式;
	bool GaussianBlur(	int parallel_num,
						bool sse_optimize,
						bool has_alpha_channel,
						int x,
						int y,
						int width,
						int height,
						int stride,
						float sigma,
						unsigned int* in,
						unsigned int* out)
	{
		if (width * height < 4*4 || width <= 1 || height <= 1) return true; //如果小于8*8的面积,或者长货款小于等于1均不做处理;
		int threads_num = max(min(parallel_num,THREADS_MAX),THREADS_MIN);
		int buffer_size_each_thread = 4 * max(width,height)+64;

		int real_width = stride / 4;
		unsigned int offset = y*real_width + x;

		int buffer_size = width*height*4;
		float* temp = (float*)malloc(buffer_size_each_thread*sizeof(float)*threads_num);
		if (!temp) return false;

		float* buffers = (float*) malloc((buffer_size *sizeof(float))+64);
		if (!buffers)  //内存分配失败;
		{
			printf_s("memory alloc error\r\n");
			free(temp);
			return false;
		}

		//计算系数;
		iir_coefs coefs = CaculateCoefs(sigma);

		//方法一:并发运算;
		//计算水平模糊并发数;
		int horizontal_funcs = height <= 128 ?1:threads_num;
		int vertical_funcs = width <= 128?1:threads_num;

		int block = height/horizontal_funcs;
		int mod = height%horizontal_funcs;

		//水平方向;
		for (int i = 0; i < horizontal_funcs;i++)
		{			
			int start = i * block;
			int end = (i+1)*block;
			if ((horizontal_funcs-1) == i) //最后一项判断余数;
				end = max(height,end);
		}


		block = width/vertical_funcs;
		mod = width%vertical_funcs;
		//垂直方向;
		for (int i = 0; i < vertical_funcs;i++)
		{
			int start = i * block;
			int end = (i+1)*block;
			if ((vertical_funcs-1) == i) //最后一项判断余数;
				end = max(width,end);
		}

		//方法二:不并发运算;
		if (sse_optimize)
		{
			//水平方向模糊;
			for (int i = 0; i < height; i++)
			{
				GaussHorizontalSSE(coefs, has_alpha_channel, width, height, temp, &in[i*real_width + offset], &buffers[i*width * 4]);
			}

			//垂直方向模糊;
			for (int i = 0; i < width; i++)
			{
				GaussVerticalSSE(coefs, has_alpha_channel, width, height, real_width, temp, &buffers[i * 4], &out[i + offset]);
			}
		}
		else
		{
			//水平方向模糊;
			for (int i = 0; i < height; i++)
			{
				GaussHorizontal(coefs, has_alpha_channel, width, height, temp, &in[i*real_width + offset], &buffers[i*width * 4]);
			}

			//垂直方向模糊;
			for (int i = 0; i < width; i++)
			{
				GaussVertical(coefs, has_alpha_channel, width, height, real_width, temp, &buffers[i * 4], &out[i + offset]);
			}
		}

		free(temp);
		free(buffers);
		return true;
	}
}
#ifndef IMAGE_EFFECTS_GAUSS_BLUR_H
#define IMAGE_EFFECTS_GAUSS_BLUR_H

namespace ImageEffects
{
	typedef struct iir_coefs   //高斯滤波系数;
	{
		float a0;
		float a1;
		float a2;
		float a3;
		float b1;
		float b2; 
		float cprev;
		float cnext;
	}COEFS;
	
	//模糊处理image必须是ARGB格式;
	//i.e.
	//BitmapData bd;
	//src.LockBits(&rt,ImageLockModeRead|ImageLockModeWrite,PixelFormat32bppARGB,&bd);
	//ImageEffects::GaussianBlur(nCPU,sse_optmize,has_alpha_channel,bd.Width,bd.Height,bd.Stride,sigma,(unsigned int*)bd.Scan0,(unsigned int*)bd.Scan0);	
	//src.UnlockBits(&bd);
	//
	bool GaussianBlur(	int parallel_num,		//并发数 [1,logic_processor_num];
						bool sse_optimize,		//sse指令优化(sse_check::simd_sse_level检测);
						bool has_alpha_channel, //是否存在透明通道;
						int x,					//坐标偏移(x,y);
						int y,
						int width,				//图片模糊区域宽;
						int height,				//图片模糊区域高;
						int stride,				//图片实际row扫描线;
						float sigma,			//模糊参数 理论上支持[0.5,+∞),大于500或<0.5基本上没啥意义了;
						unsigned int* in,		//图片数据输入;
						unsigned int* out);     //图片数据输出;
}
#endif //IMAGE_EFFECTS_GAUSS_BLUR_H
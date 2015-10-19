#ifndef IMAGE_EFFECTS_MOSAIC_H
#define IMAGE_EFFECTS_MOSAIC_H

namespace ImageEffects
{
	void MosaicTrans(	int real_width,		//图像数据宽度
						int real_height, 	//图像数据高度
						int left,			//马赛克处理区域（0，0，2，2）代表0,0点到2,2点3*3的区域
						int top,			//马赛克处理区域
						int right,			//马赛克处理区域
						int bottom,			//马赛克处理区域
						long* data,			//图像数据指针，ARGB，PC数据为从下往上的
						int mosaic_w,		//马赛克宽度
						int mosaic_h,		//马赛克高度
						bool random);		//加入随机
}
#endif //IMAGE_EFFECTS_MOSAIC_H
#ifndef _NIM_GUI_CAPTURE_FORM_CAPTURE_DEF_H_
#define _NIM_GUI_CAPTURE_FORM_CAPTURE_DEF_H_
#pragma once

#include "duilib/UIlib.h"

// 编辑模式
enum EditMode
{	
	EM_NONE,			// 未选中
	EM_RECTANGLE,		// 矩形
	EM_ELLIPSE,			// 椭圆
	EM_LINE,			// 直线
	EM_ARROW,			// 箭头
	EM_TEXT,			// 文字
	EM_BRUSH,			// 画刷 (实质是N个线段)
	EM_MOSAIC,			// 画刷类型的加强，显示为马赛克
	EM_GAUSS,			// 画刷类型的加强，显示为毛玻璃
};

// 绘制单元参数类型
enum DrawUnitParamType
{
	DUPT_NONE,
	DUPT_LINEWIDTH,		// 线宽
	DUPT_COLOR,			// 颜色
	DUPT_FONTSIZE,		// 字体大小
	DUPT_BOLD,			// 加粗
	DUPT_UNDERLINE,		// 下划线
	DUPT_ITALIC,		// 斜体
};

// 线宽
enum LineWidth		
{
	LW_1POUND		=	1,
	LW_1_5POUNDS	=	2,
	LW_3POUNDS		=	3,
	LW_4_5POUNDS	=	4,
	LW_6POUNDS		=	5,
};

const DWORD	kdwDefaultColor		=	RGB(0xe6,0x00,0x13);	// 默认颜色
const int	knDefaultFontSize	=	18;						// 默认字体大小
const int	knDefaultLineWidth	=	LW_1POUND;				// 默认线粗
const int	knFontSizeLength	=	14;
const int	vtFontSize[knFontSizeLength] = {8,9,10,11,knDefaultFontSize,14,16,18,20,22,24,26,28,36};
const int	knDefaultIndex = 4;
const int	knEditOffset = 3;

// 快速四舍五入
template <typename FloatType>
inline int __roundToInt (const FloatType value) throw()
{
	union { int asInt[2]; double asDouble; } n;
	n.asDouble = ((double) value) + 6755399441055744.0;

	return n.asInt [0];
}

// 四舍五入, 得整数
#define   ROUND(X)  __roundToInt(X)

#endif  // _NIM_GUI_CAPTURE_FORM_CAPTURE_DEF_H_
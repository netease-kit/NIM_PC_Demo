/*
 *
 *	Author       huangjun
 *	Email        hzhuangjun2013@corp.netease.com
 *	Date         2013-12-05
 *	Copyright    Hangzhou, Netease Inc.
 *	Brief        头像浏览/预览控件
 *
 */

#ifndef _NIM_GUI_CAPTURE_FORM_UI_HEAD_VIEW_CTRL_H_
#define _NIM_GUI_CAPTURE_FORM_UI_HEAD_VIEW_CTRL_H_
#pragma once


using namespace Gdiplus;

namespace nim_comp
{
// 头像图片浏览控件
class HeadViewControl : public ui::Control
{
public:
	HeadViewControl();
	~HeadViewControl();
	void Init(bool release_old_image = true);
	bool RenderImage(const std::wstring& path, bool compress = false);
	void RenderCanvas(int canvas_width, int canvas_height);
	void RenderTempCanvas(int select_width);
	Bitmap* GetSelectBitmap(RECT select_rect, bool use_original_image = false); // 选区抠图
	void GetImageSize(int &width, int &height);
	void SetZoomRatio(float zoom_ratio);
	bool SaveSelectBitmap(RECT select_rect,
		const std::wstring &save_path, const std::wstring &mime_type); // 保存选区头像
	bool CheckHasLoadImage(); // 判断是否已经加载过图片
protected:
	virtual void PaintBkImage(HDC hDC) override; // 自绘背景
private:
	/* 采用临时图抠图。	每次选区缩放时，每一个选区宽度生成一张临时图。
	   当前选区对应临时图中的实际选区一定是200*200，这样移动时不需要缩放，
	   解决图像很大时，拖动图像每次都需要压缩到200*200导致的很卡的情况 */
	Bitmap* GetSelectBitmapFromTempCanvas(RECT select_rect);
	// 采用原图中抠图(清晰，但图像大时每次压缩到200*200效率很低)
	Bitmap* GetSelectBitmapFromImage(RECT select_rect); 
	// 采用显示区的画布抠图(适合原图比显示区画布小的情况)
	Bitmap* GetSelectBitmapFormCanvas(RECT select_rect); 
	// 判断是否要生成或使用临时画布
	bool NeedRenderOrUseTempCanvas(int select_width);
	// 设置绘图的模式
	virtual void SetGraphicsMode(Graphics &gp);
private:
	std::wstring last_image_path_;			  // 最后一次真实图片路径
	std::unique_ptr<Bitmap> image_;		  // 真实图片（或压缩到2000的压缩图）
	std::unique_ptr<Bitmap> canvas_;		  // 显示区画布
	std::unique_ptr<Bitmap> temp_canvas_;   // 临时的画布（抠图用）
	std::unique_ptr<Bitmap> select_bitmap_; // 选区图像（预览用）
	float zoom_ratio_;						  // 真实图的缩放比例(真图/画布)
	float temp_zoom_ratio_;					  // 临时图的缩放比例(临时图/画布)
	int last_select_width_;					  // 最后一次选区的宽度（如果宽度变化，需要重新生成临时图）
	int select_width_change_threshold_;		  // 选区宽度变化阈值（超过阈值才重新生成临时图）
	int real_select_width_threshold_;		  // 真实图（可能有压缩）选区阈值（小于该阈值用真实图抠图）
	int fixed_generated_width_;				  // 固定生成头像的宽度200
	int fixed_generated_height_;			  // 固定生成头像的高度200
	int compressed_max_width_;			      // 压缩后的图片最大宽度2000
	int compressed_max_height_;				  // 压缩后的图片最大高度2000
};

// 头像预览控件
class HeadPreviewControl : public ui::Control
{
public:
	HeadPreviewControl();
	~HeadPreviewControl();
	void Init();
	void SetPaintBitmap(Bitmap* select_bitmap);
	bool SetMaskPicturePath(std::wstring mask_path, RECT padding_rect);
	Bitmap* GetPaintBitmap(){ return select_bitmap_; };
protected:
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 
private:
	Bitmap* select_bitmap_;					// 选区图像
	std::unique_ptr<Bitmap> mask_image_;  // 蒙版图像 
	RECT padding_rect_;					    // 头像与蒙版的间距
	ui::Image headimage_mask_;
};

// 辅助工具类
class HeadImageUtil
{
public:
	static bool CheckImageValid(std::wstring image_path);
	// 保存选区图像
	static bool SaveImage(Bitmap *image,const std::wstring &file_path, 
		const std::wstring &mime_type, long quality_num = 100); 
	static Bitmap* GetFormatBitmap(const std::wstring &file_path);
	static Bitmap* GetFormatBitmap(Bitmap* bitmap);
private:	
	// 获取图像格式的ID
	static bool GetImageCLSID(const wchar_t* format, CLSID* pCLSID);
};
}

#endif  // _NIM_GUI_CAPTURE_FORM_UI_HEAD_VIEW_CTRL_H_
#pragma once
using namespace Gdiplus;

namespace nim_comp
{
/** @class HeadViewControl
  * @brief 头像图片浏览控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author hj
  * @date 2013/12/05
  */
class HeadViewControl : public ui::Control
{
public:
	HeadViewControl();
	~HeadViewControl();

	/**
	* 初始化控件
	* @param[in] release_old_image 是否释放原来的图片资源
	* @return void	无返回值
	*/
	void Init(bool release_old_image = true);

	/**
	* 在内存中生成原图
	* @param[in] path 图片路径
	* @param[in] compress 是否需要压缩位图
	* @return bool 成功，false 失败
	*/
	bool RenderImage(const std::wstring& path, bool compress = false);
	
	/**
	* 生成画布图
	* @param[in] canvas_width 画布宽度
	* @param[in] canvas_height 画布高度
	* @return void 无返回值
	*/
	void RenderCanvas(int canvas_width, int canvas_height);

	/**
	* 生成抠图用的临时图
	* @param[in] select_width 选区宽度
	* @return void 无返回值
	*/
	void RenderTempCanvas(int select_width);

	/**
	* 获取选区位置中的图片
	* @param[in] select_width 选区位置
	* @param[in] use_original_image 是否在原图中抠图
	* @return Bitmap* 位图指针
	*/
	Bitmap* GetSelectBitmap(RECT select_rect, bool use_original_image = false);

	/**
	* 获取加载的图片的尺寸
	* @param[out] width 图片宽度
	* @param[out] height 图片高度
	* @return void 无返回值
	*/
	void GetImageSize(int &width, int &height);

	/**
	* 设置缩放比
	* @param[in] zoom_ratio 缩放比
	* @return void 无返回值
	*/
	void SetZoomRatio(float zoom_ratio);

	/**
	* 保存选区头像
	* @param[in] select_rect 选区位置
	* @param[in] save_path 保存路径
	* @param[in] mime_type 保存格式
	* @return void 无返回值
	*/
	bool SaveSelectBitmap(RECT select_rect,
		const std::wstring &save_path, const std::wstring &mime_type);

	/**
	* 判断是否已经加载过图片
	* @return bool true 加载过，false 没有加载过
	*/
	bool CheckHasLoadImage();
protected:
	/**
	* 重写父类虚函数，自绘背景
	* @param[in] hDC 目标DC
	* @return void	无返回值
	*/
	virtual void PaintBkImage(HDC hDC) override;
private:

	/**
	* 采用临时图抠图。每次选区缩放时，每一个选区宽度生成一张临时图。
	* 当前选区对应临时图中的实际选区一定是200*200，这样移动时不需要缩放，
	* 解决图像很大时，拖动图像每次都需要压缩到200*200导致的很卡的情况
	* @param[in] select_rect 选区位置
	* @return Bitmap* 位图指针
	*/
	Bitmap* GetSelectBitmapFromTempCanvas(RECT select_rect);

	/**
	* 采用原图中抠图(清晰，但图像大时每次压缩到200*200效率很低)
	* @param[in] select_rect 选区位置
	* @return Bitmap* 位图指针
	*/
	Bitmap* GetSelectBitmapFromImage(RECT select_rect); 

	/**
	* 采用显示区的画布抠图(适合原图比显示区画布小的情况)
	* @param[in] select_rect 选区位置
	* @return Bitmap* 位图指针
	*/
	Bitmap* GetSelectBitmapFormCanvas(RECT select_rect); 

	/**
	* 判断是否要生成或使用临时画布
	* @param[in] select_rect 选区位置
	* @return bool true 需要，false 不需要
	*/
	bool NeedRenderOrUseTempCanvas(int select_width);

	/**
	* 设置绘图的模式
	* @param[in] gp 绘图对象
	* @return void	无返回值
	*/
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

/** @class HeadPreviewControl
  * @brief 头像预览控件
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author hj
  * @date 2013/12/05
  */
class HeadPreviewControl : public ui::Control
{
public:
	HeadPreviewControl();
	~HeadPreviewControl();

	/**
	* 初始化控件
	* @return void	无返回值
	*/
	void Init();

	/**
	* 设置背景位图
	* @param[in] select_bitmap 位图指针
	* @return void	无返回值
	*/
	void SetPaintBitmap(Bitmap* select_bitmap);

	/**
	* 设置蒙板位图
	* @param[in] mask_path 蒙板图片路径
	* @param[in] padding_rect 内边距
	* @return void	无返回值
	*/
	bool SetMaskPicturePath(std::wstring mask_path, RECT padding_rect);

	/**
	* 获取背景位图
	* @return Bitmap* 位图指针
	*/
	Bitmap* GetPaintBitmap(){ return select_bitmap_; };
protected:
	/**
	* 覆盖父类虚函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 可用绘制区域
	* @return void	无返回值
	*/
	virtual void Paint(HDC hDC, const ui::UiRect& rcPaint) override; 
private:
	Bitmap* select_bitmap_;					// 选区图像
	std::unique_ptr<Bitmap> mask_image_;	// 蒙版图像 
	RECT padding_rect_;						// 头像与蒙版的间距
	ui::Image headimage_mask_;
};

/** @class HeadImageUtil
  * @brief 辅助工具类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author hj
  * @date 2013/12/05
  */
class HeadImageUtil
{
public:
	/**
	* 判断某个图片是否有效
	* @param[in] image_path 图片路径
	* @return bool true 有效，false 无效
	*/
	static bool CheckImageValid(std::wstring image_path);

	/**
	* 保存选区头像
	* @param[in] image 要保存的位图
	* @param[in] file_path 保存路径
	* @param[in] mime_type 保存格式
	* @param[in] quality_num 保存质量
	* @return bool true 成功，false 失败
	*/
	static bool SaveImage(Bitmap *image,const std::wstring &file_path, 
		const std::wstring &mime_type, long quality_num = 100); 

	/**
	* 把某个图片转换为200x200大小
	* @param[in] file_path 图片路径
	* @return Bitmap* 生成后的位图指针
	*/
	static Bitmap* GetFormatBitmap(const std::wstring &file_path);

	/**
	* 把某个图片转换为200x200大小
	* @param[in] bitmap 位图指针
	* @return Bitmap* 生成后的位图指针
	*/
	static Bitmap* GetFormatBitmap(Bitmap* bitmap);
private:	

	/**
	* 获取图像格式的ID
	* @param[in] format 图片格式
	* @param[out] pCLSID 图片格式ID
	* @return bool true 成功，false 失败
	*/
	static bool GetImageCLSID(const wchar_t* format, CLSID* pCLSID);
};
}
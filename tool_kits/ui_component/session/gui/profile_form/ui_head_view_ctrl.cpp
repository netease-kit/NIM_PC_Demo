#include "ui_head_view_ctrl.h"

/*
	优化策略: huangjun 2013-12-09
	[1].原图默认压缩到最大宽高不超过2000，保存在内存中作为替代原图的抠图对象。
	[2].为解决用原图抠图在拖动时很卡的问题，原因是每次移动，都要重新抠图渲染，
	画板上选区对应原图中的选区很大，要压缩到200*200比较耗时，造成画面顿卡。
	采用DUDU的解决方案：选区拉伸时，每产生一个选区宽度生成一个临时画布用作抠图，
	画板上选区对应该临时画布的选区一定是200*200，那么在渲染时不必压缩，快速。
	[3].对于2，并不是每次都生成临时画布，因为生成临时画布也比较慢，会造成选区拉伸
	的顿卡。优化方案：当选区宽度变化超过一个阈值时且画板上选区对应原图的选区超过
	一个阈值时，生成此临时画布，用临时画布进行抠图。否则当画板上选区对应真实图的
	选区比较小时，采用真实图进行抠图，压缩尺度较小，不会顿卡又能保证清晰。
	[4].当原图比显示区画布还小时，画布会放大原图，采用显示区画布进行抠图。
	[5].由于采用临时图进行抠图会有一定的模糊，当鼠标UP的时候采用真实图（可能压缩）
	来抠图重新渲染。
	[6].最终生成的200*200保存图，采用未压缩过的原图进行抠图。
	[7].为了降低加载图片时因为渲染较慢造成界面卡死的情况，将Load部分拆分，先在MISC
	线程中渲染好原图、画布图、临时画布，再回调DUI线程显示渲染的结果。
*/
namespace nim_comp
{
HeadViewControl::HeadViewControl()
{
	Init();
}

HeadViewControl::~HeadViewControl()
{
	
}

void HeadViewControl::PaintBkImage(HDC hDC)
{
	Graphics graph(hDC);
	graph.DrawImage(canvas_.get(),m_rcItem.left,m_rcItem.top);
}

void HeadViewControl::Init(bool release_old_image /*= true*/)
{
	if(release_old_image)
	{
		image_.reset(NULL);
		canvas_.reset(NULL);
		temp_canvas_.reset(NULL);
		select_bitmap_.reset(NULL);
	}

	zoom_ratio_ = 0.0f;
	temp_zoom_ratio_ = 0.0f;
	last_select_width_ = 0;

	// 固定
	fixed_generated_width_ = 200; 
	fixed_generated_height_ = 200;
	// 根据清晰度和性能调整
	compressed_max_width_ = 3000; 
	compressed_max_height_ = 3000;
	select_width_change_threshold_ = 40;
	real_select_width_threshold_ = 300; // 经验值
}

void HeadViewControl::GetImageSize( int &width, int &height )
{
	if(image_.get())
	{
		width = image_->GetWidth();
		height = image_->GetHeight();
	}
	else
	{
		width = 0;
		height = 0;
	}
}

void HeadViewControl::SetZoomRatio( float zoom_ratio )
{
	zoom_ratio_ = zoom_ratio;
}

void HeadViewControl::SetGraphicsMode( Graphics &gp )
{
	gp.SetSmoothingMode(SmoothingModeAntiAlias);
#if (GDIPVER >= 0x0110)
	gp.SetSmoothingMode(SmoothingModeAntiAlias8x8);
#endif
	gp.SetCompositingMode(CompositingModeSourceOver);
	gp.SetInterpolationMode(InterpolationModeHighQuality);
	gp.SetPixelOffsetMode(PixelOffsetModeHighQuality);
}

// 生成原图在内存中
bool HeadViewControl::RenderImage(const std::wstring& path, bool compress)
{
	// 获取图像数据
	Bitmap* bitmap = new Bitmap(path.c_str());
	Status status = bitmap->GetLastStatus();
	ASSERT(status == Ok);
	if (status != Ok) {
		return false;
	}
	image_.reset(bitmap);
	int bmp_cx = bitmap->GetWidth();
	int bmp_cy = bitmap->GetHeight();
	// 进行压缩
	if(compress)
	{
		int compressed_width = compressed_max_width_;
		int compressed_height = compressed_max_height_;
		float ratio = (bmp_cx * 1.0f) / (bmp_cy * 1.0f);
		bool is_need_compress = false;
		if(bmp_cx > bmp_cy)
		{
			// 按宽缩放
			if(bmp_cx > compressed_max_width_)
			{
				compressed_height = (int)(1.0f * compressed_width / ratio); 
				is_need_compress = true;
			}
		}
		else
		{
			// 按高缩放
			if(bmp_cy > compressed_max_height_)
			{
				compressed_width = (int)(1.0f * compressed_height * ratio); 
				is_need_compress = true;
			}
		}
		if(is_need_compress)
		{
			// 缩放
			Bitmap* compressed_bitmap = new Bitmap(compressed_width, compressed_height);
			Graphics gp(compressed_bitmap);
			SetGraphicsMode(gp);
			RectF rf((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)compressed_width, (Gdiplus::REAL)compressed_height);
			gp.DrawImage(image_.get(), rf);
			 // 替换原图
			image_.reset(compressed_bitmap);
		}
	}
	if(image_.get())
	{
		// 设置新图片路径
		last_image_path_ = path;
		return true;
	}
	return false;
}

// 生成画布图
void HeadViewControl::RenderCanvas( int canvas_width, int canvas_height )
{
	// 兼容实际画布大小的绘制
	RectF rf_canvas((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)canvas_width, (Gdiplus::REAL)canvas_height);
	canvas_.reset(new Bitmap(canvas_width, canvas_height));
	Graphics gp(canvas_.get());
	SetGraphicsMode(gp);
	gp.DrawImage(image_.get(), rf_canvas, 0, 0, (Gdiplus::REAL)image_->GetWidth(), (Gdiplus::REAL)image_->GetHeight(), UnitPixel);
}

// 生成抠图用的临时图
void HeadViewControl::RenderTempCanvas( int select_width )
{
	// 当选区宽度变化超过10px时重新生成
	if(select_width > 0 && select_width != last_select_width_ &&
		abs(select_width - last_select_width_) > select_width_change_threshold_ &&
		NeedRenderOrUseTempCanvas(select_width))
	{
		// 原始图已有，画布图已有，根据选区宽度，计算一张临时图，移动时抠图用
		float image_select_width = zoom_ratio_ * select_width;
		float temp_image_width = fixed_generated_width_ * image_->GetWidth() / image_select_width;
		float temp_image_height = fixed_generated_height_ * image_->GetHeight() / image_select_width;

		RectF temp_rf(0, 0, temp_image_width, temp_image_height);
		temp_canvas_.reset(new Bitmap((INT)temp_image_width, (INT)temp_image_height));
		Graphics gp(temp_canvas_.get());
		gp.DrawImage(image_.get(), temp_rf, 0 , 0, (Gdiplus::REAL)image_->GetWidth(), (Gdiplus::REAL)image_->GetHeight(), UnitPixel);

		temp_zoom_ratio_ = temp_image_width / (image_->GetWidth() / zoom_ratio_); // 临时图/画布
		last_select_width_ = select_width;
	}
}

// 抠图
Bitmap* HeadViewControl::GetSelectBitmap( RECT select_rect, bool use_original_image/*= false */)
{
	if(zoom_ratio_ > 1.0f)
	{
		if(use_original_image)
		{
			return GetSelectBitmapFromImage(select_rect); // 用真实图抠图
		}
		else
		{
			if(!NeedRenderOrUseTempCanvas(select_rect.right - select_rect.left))
				return GetSelectBitmapFromImage(select_rect); // 用真实图抠图
			else
				return GetSelectBitmapFromTempCanvas(select_rect); // 用临时画布抠图(解决大图拖动卡的问题)
		}
	}
	else
	{
		return GetSelectBitmapFormCanvas(select_rect);
	}
}

// 采用真实图抠图
Bitmap* HeadViewControl::GetSelectBitmapFromImage( RECT select_rect )
{
	if(!image_.get())
		return NULL;

	RectF real_select_rectf(select_rect.left * zoom_ratio_, select_rect.top * zoom_ratio_,
		(select_rect.right - select_rect.left) * zoom_ratio_, (select_rect.bottom - select_rect.top) * zoom_ratio_);
	// 要生成的图200*200
	select_bitmap_.reset(new Bitmap(fixed_generated_width_, fixed_generated_height_));
	Graphics gp(select_bitmap_.get());
	SetGraphicsMode(gp);
	RectF dest_rf((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)fixed_generated_width_, (Gdiplus::REAL)fixed_generated_height_); // 目标区域
	// 用原图换算后的区域裁剪
	gp.DrawImage(image_.get(), dest_rf, 
		real_select_rectf.X, real_select_rectf.Y, real_select_rectf.Width, real_select_rectf.Height, UnitPixel);
	return select_bitmap_.get();
}

// 采用显示区画布抠图
Bitmap* HeadViewControl::GetSelectBitmapFormCanvas( RECT select_rect )
{
	if(!canvas_.get())
		return NULL;

	int bitmap_width = select_rect.right - select_rect.left;
	int bitmap_height = select_rect.bottom - select_rect.top;
	select_bitmap_.reset(new Bitmap(bitmap_width, bitmap_height)) ;
	Graphics gp(select_bitmap_.get());
	SetGraphicsMode(gp);
	RectF dest_rf((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)bitmap_width, (Gdiplus::REAL)bitmap_height); // 目标区域
	gp.DrawImage(canvas_.get(), dest_rf, 
		(Gdiplus::REAL)select_rect.left, (Gdiplus::REAL)select_rect.top, (Gdiplus::REAL)bitmap_width, (Gdiplus::REAL)bitmap_height, UnitPixel);
	return select_bitmap_.get();
}

// 采用临时图抠图(换算后的在临时图中的选区应该也是200*200)
Bitmap* HeadViewControl::GetSelectBitmapFromTempCanvas( RECT select_rect )
{
	if(!temp_canvas_.get())
		return NULL;

	// 这里real_select_rectf的宽度和高度也是200*200
	RectF real_select_rectf(select_rect.left * temp_zoom_ratio_, select_rect.top * temp_zoom_ratio_,
		(select_rect.right - select_rect.left) * temp_zoom_ratio_, (select_rect.bottom - select_rect.top) * temp_zoom_ratio_);
	// 要生成的图200*200
	if(select_bitmap_.get() == NULL)
		select_bitmap_.reset(new Bitmap(fixed_generated_width_, fixed_generated_height_));
	Graphics gp(select_bitmap_.get());
	RectF dest_rf((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)fixed_generated_width_, (Gdiplus::REAL)fixed_generated_height_); // 目标区域
	// 这里不需要压缩
	gp.DrawImage(temp_canvas_.get(), dest_rf, 
		real_select_rectf.X, real_select_rectf.Y, real_select_rectf.Width, real_select_rectf.Height, UnitPixel);
	return select_bitmap_.get();
}

// 判断是否要生成或者使用临时画布,为了解决实际选区太大，用真实图抠图很卡的问题，采用临时画布抠图
bool HeadViewControl::NeedRenderOrUseTempCanvas( int select_width )
{
	int real_select_width = (int)(select_width * zoom_ratio_);
	if(real_select_width < real_select_width_threshold_) 
		return false;  // 小于阈值，用真实图抠图
	return true; // 实际选区太大了，移动时会卡，要用临时画布抠图
}

bool HeadViewControl::SaveSelectBitmap( RECT select_rect,
	const std::wstring &save_path, const std::wstring &mime_type )
{
	if(select_bitmap_.get())
		return HeadImageUtil::SaveImage(select_bitmap_.get(), save_path, mime_type);
	return false;
}

bool HeadViewControl::CheckHasLoadImage()
{
	return !last_image_path_.empty();
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// 头像预览控件 ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

HeadPreviewControl::HeadPreviewControl()
{
	RECT empty_rect = {0, 0, 0, 0};
	padding_rect_ = empty_rect;
	Init();
}

HeadPreviewControl::~HeadPreviewControl()
{
	Init();
}

void HeadPreviewControl::Init()
{
	select_bitmap_ = NULL; // 避免野指针，在Load时候必须Init()
}

bool HeadPreviewControl::SetMaskPicturePath( std::wstring mask_path, RECT padding_rect)
{
	padding_rect_ = padding_rect;
	// 加入缓存
	headimage_mask_.SetImageString(mask_path);
	GetImage(headimage_mask_);
	if (!headimage_mask_.imageCache)
		return false;

	// 获取图像数据
	BITMAP bmp_info;
	::GetObject(headimage_mask_.imageCache->GetHBitmap(0), sizeof(BITMAP), &bmp_info);
	int bmp_cx = bmp_info.bmWidth;
	int bmp_cy = bmp_info.bmHeight;
	void* bits = bmp_info.bmBits;
	mask_image_ = std::make_unique<Bitmap>(bmp_cx, bmp_cy, bmp_cx*4, PixelFormat32bppARGB, (BYTE*)bits);
	if(mask_image_.get())
		return true;
	return false;
}

void HeadPreviewControl::Paint( HDC hDC, const ui::UiRect& rcPaint )
{
	__super::Paint(hDC, rcPaint);

	if(select_bitmap_ != NULL)
	{
		Graphics graph(hDC);
		graph.SetSmoothingMode(SmoothingModeAntiAlias);
#if (GDIPVER >= 0x0110)
		graph.SetSmoothingMode(SmoothingModeAntiAlias8x8);
#endif
		// 目标绘制区域
		RectF dest_rectf((Gdiplus::REAL)m_rcItem.left, (Gdiplus::REAL)m_rcItem.top,
			(Gdiplus::REAL)(m_rcItem.right - m_rcItem.left), (Gdiplus::REAL)(m_rcItem.bottom - m_rcItem.top));

		// 画头像(若有蒙版则裁剪为圆形)
		RECT head_rect = m_rcItem;
		head_rect.left += padding_rect_.left;
		head_rect.right -= padding_rect_.right;
		head_rect.top += padding_rect_.top;
		head_rect.bottom -= padding_rect_.bottom;
		RectF head_rectf((Gdiplus::REAL)head_rect.left, (Gdiplus::REAL)head_rect.top,
			(Gdiplus::REAL)(head_rect.right - head_rect.left), (Gdiplus::REAL)(head_rect.bottom - head_rect.top));
		if(mask_image_.get())
		{
			GraphicsPath path;
			path.AddEllipse(head_rectf);
			graph.SetClip(&path);
		}
		graph.DrawImage(select_bitmap_, head_rectf, 0, 0,
			(Gdiplus::REAL)select_bitmap_->GetWidth(), (Gdiplus::REAL)select_bitmap_->GetHeight(), UnitPixel);
		graph.ResetClip();

		// 画蒙版
		//if(mask_image_.get())
		//	graph.DrawImage(mask_image_.get(), dest_rectf);

		// 释放句柄
		graph.ReleaseHDC(hDC);
	}
}

void HeadPreviewControl::SetPaintBitmap( Bitmap* select_bitmap )
{
	if(select_bitmap)
	{
		select_bitmap_ = select_bitmap;
		Invalidate(); // 重绘
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// 图片辅助工具类 //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

bool HeadImageUtil::CheckImageValid( std::wstring image_path )
{
	// 判断路径是否有效
	if (!nbase::FilePathIsExist(image_path, false) || nbase::FilePathIsExist(image_path, true))
		return false;

	// 判断是否为图片
	Image image(image_path.c_str());
	if (image.GetLastStatus() != Ok)
		return false;

	// 判断图片大小是否超过 200*200
	if(image.GetWidth() < 200 || image.GetHeight() < 200)
		return false;

	return true;
}

bool HeadImageUtil::SaveImage( Bitmap *image,const std::wstring &file_path,
	const std::wstring &mime_type, long quality_num /*= 100*/ )
{
	CLSID clsid;
	if (!GetImageCLSID(mime_type.c_str(),&clsid))
		return false;

	EncoderParameters encoder_paramters;
	encoder_paramters.Count = 1;
	encoder_paramters.Parameter[0].Guid = EncoderQuality;
	encoder_paramters.Parameter[0].NumberOfValues = 1;
	encoder_paramters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoder_paramters.Parameter[0].Value = (void*)&quality_num;

	Status status = image->Save(file_path.c_str(), &clsid, &encoder_paramters);
	return status == Ok;
}

bool HeadImageUtil::GetImageCLSID( const wchar_t* format, CLSID* pCLSID )
{
	UINT num = 0; 
	UINT size = 0; 
	ImageCodecInfo* pImageCodecInfo = NULL; 
	GetImageEncodersSize(&num, &size); 

	if (size == 0) 
		return false; 

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL) 
		return false; 
	GetImageEncoders(num, size, pImageCodecInfo); 

	for (UINT i = 0; i < num; ++i) 
	{
		if ( wcscmp(pImageCodecInfo[i].MimeType, format) == 0 ) 
		{ 
			*pCLSID = pImageCodecInfo[i].Clsid; 
			free(pImageCodecInfo); 
			return true; 
		} 
	}
	free(pImageCodecInfo); 

	return false; 
}

Bitmap* HeadImageUtil::GetFormatBitmap( const std::wstring &file_path )
{
	std::unique_ptr<Gdiplus::Image> image;
	image.reset(Gdiplus::Image::FromFile(file_path.c_str())); 
	Bitmap* format_bitmap = new Bitmap(200, 200);
	Graphics gp(format_bitmap);
	RectF rf(0, 0, 200, 200);
	gp.DrawImage(image.get(), rf);
	// 指针由用户自己析构
	return format_bitmap;
}

Bitmap* HeadImageUtil::GetFormatBitmap( Bitmap* bitmap )
{
	if(bitmap)
	{
		Bitmap* image = bitmap;
		Bitmap* format_bitmap = new Bitmap(200, 200);
		Graphics gp(format_bitmap);
		RectF rf(0, 0, 200, 200);
		gp.DrawImage(image, rf);
		// 指针由用户自己析构
		return format_bitmap;
	}
	return NULL;
}
}
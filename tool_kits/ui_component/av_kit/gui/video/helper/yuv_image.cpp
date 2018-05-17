
#include "yuv_image.h"
#include "libyuv.h"

namespace nim_comp
{
void YUV420ToARGB(char* src, char* dst, int width, int height)
{
	uint8_t* src_y = (uint8_t*)src;
	uint8_t* src_u = src_y + width * height;
	uint8_t* src_v = src_u + width * height / 4;

	// 		libyuv::I420ToRGB24(
	// 			src_y, width,
	// 			src_u, width / 2,
	// 			src_v, width / 2,
	// 			(uint8_t*)dst, width * 3,
	// 			width, height);
	libyuv::I420ToARGB(
		src_y, width,
		src_u, width / 2,
		src_v, width / 2,
		(uint8_t*)dst, width * 4,
		width, height);
}

void ARGBToYUV420(char* src, char* dst, int width, int height)
{
	int byte_width = width * 4;
	width -= width % 2;
	height -= height % 2;
	int wxh = width * height;
	uint8_t* des_y = (uint8_t*)dst;
	uint8_t* des_u = des_y + wxh;
	uint8_t* des_v = des_u + wxh / 4;



	// 		libyuv::RGB24ToI420((const uint8*)src, byte_width,
	// 			des_y, width,
	// 			des_u, width / 2,
	// 			des_v, width / 2,
	// 			width, height);
	libyuv::ARGBToI420((const uint8_t*)src, byte_width,
		des_y, width,
		des_u, width / 2,
		des_v, width / 2,
		width, height);
}

YuvImage::YuvImage()
{
	time_start_ = 0;
}

YuvImage::~YuvImage()
{
}
bool YuvImage::LoadImage(std::wstring path)
{
	if (path.empty())
	{
		return false;
	}
	if (image_path_ == path)
	{
		return true;
	}
	image_list_.clear();
	time_start_ = 0;

	Gdiplus::Status status;
	src_image_.reset(new Gdiplus::Bitmap(path.c_str()));
	status = src_image_->GetLastStatus();
	ASSERT(status == Gdiplus::Ok);
	if (status != Gdiplus::Ok) {
		return false;
	}
	UINT nCount = src_image_->GetFrameDimensionsCount();
	std::unique_ptr<GUID[]> pDimensionIDs(new GUID[nCount]);
	src_image_->GetFrameDimensionsList(pDimensionIDs.get(), nCount);
	int iFrameCount = src_image_->GetFrameCount(&pDimensionIDs.get()[0]);
	Gdiplus::PropertyItem* propertyItem = nullptr;
	if (iFrameCount > 1) {
		int iSize = src_image_->GetPropertyItemSize(PropertyTagFrameDelay);
		Gdiplus::PropertyItem* propertyItem = (Gdiplus::PropertyItem*) malloc(iSize);
		status = src_image_->GetPropertyItem(PropertyTagFrameDelay, iSize, propertyItem);
		ASSERT(status == Gdiplus::Ok);
		if (status != Gdiplus::Ok) {
			return false;
		}
	}
	time_all_ = 0;
	for (int i = 0; i < iFrameCount; i++) {
		ImageFrame image_frame;
		int interval = 100;
		if (propertyItem)
		{
			int interval = ((long*)(propertyItem->value))[i] * 10;
			if (interval < 30) {
				interval = 100;
			}
			else if (interval < 50)
			{
				interval = 50;
			}
		}
		time_all_ += interval;
		image_frame.time_ = time_all_;

		if (CheckImageFrame(i, &image_frame, 150, 150))
		{
			image_list_.push_back(image_frame);
		}
	}

	return true;
}
bool YuvImage::AddText(const std::wstring& text, int width, int height, DWORD color, int iFont, UINT uStyle, BYTE uFade, bool bLineLimit)
{
	if (text.empty())
	{
		return false;
	}
	text_ = text;
	image_list_.clear();
	width += width % 2;
	height += height % 2;
	src_image_.reset(new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB));
	time_all_ = 0;
	bool ret = DrawText(0, width, height, color, iFont, uStyle, uFade, bLineLimit);
	return ret;
}
bool YuvImage::DrawText(int index, int width, int height, DWORD color, int iFont, UINT uStyle, BYTE uFade, bool bLineLimit)
{
	Gdiplus::Graphics graphics(src_image_.get());
	HDC hdc = ::GetDC(NULL);
	Gdiplus::Font font(hdc, ui::GlobalManager::GetFont(iFont));
	int color_alpha = color >> 24;
	uFade *= double(color_alpha) / 255;
	if (uFade == 255) {
		uFade = 254;
	}
	Gdiplus::SolidBrush tBrush(Gdiplus::Color(uFade, GetBValue(color), GetGValue(color), GetRValue(color)));

	Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();
	if ((uStyle & DT_END_ELLIPSIS) != 0) {
		stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	}

	int formatFlags = 0;
	if ((uStyle & DT_NOCLIP) != 0) {
		formatFlags |= Gdiplus::StringFormatFlagsNoClip;
	}
	if ((uStyle & DT_SINGLELINE) != 0) {
		formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
	}
	if (bLineLimit) {
		formatFlags |= Gdiplus::StringFormatFlagsLineLimit;
	}
	stringFormat.SetFormatFlags(formatFlags);

	if ((uStyle & DT_LEFT) != 0) {
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	}
	else if ((uStyle & DT_CENTER) != 0) {
		stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if ((uStyle & DT_RIGHT) != 0) {
		stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	else {
		stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
	}

	if ((uStyle & DT_TOP) != 0) {
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	}
	else if ((uStyle & DT_VCENTER) != 0) {
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if ((uStyle & DT_BOTTOM) != 0) {
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
	}
	else {
		stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	}

	Gdiplus::RectF rectF((Gdiplus::REAL)0, (Gdiplus::REAL)0, (Gdiplus::REAL)width, (Gdiplus::REAL)height);
	graphics.DrawString(text_.c_str(), text_.length(), &font, rectF, &stringFormat, &tBrush);

	ImageFrame image_frame;
	time_all_ += 100;
	image_frame.time_ = time_all_;

	bool ret = false;
	if (CheckImageFrame(index, &image_frame, width, height))
	{
		image_list_.push_back(image_frame);
		ret = true;
	}
	::ReleaseDC(NULL, hdc);
	return ret;
}
bool YuvImage::CheckImageFrame(int index, ImageFrame* image_frame, int width, int height)
{
	if (src_image_ == nullptr)
	{
		return false;
	}
	Gdiplus::Status status;
	status = src_image_->SelectActiveFrame(&Gdiplus::FrameDimensionTime, index);
	ASSERT(status == Gdiplus::Ok);
	if (status != Gdiplus::Ok) {
		return false;
	}

	int src_width = src_image_->GetWidth();
	int src_height = src_image_->GetHeight();
	src_width -= src_width % 2;
	src_height -= src_height % 2;
	if (width <= 0) width = src_width; else width -= width % 2;
	if (height <= 0) height = src_height; else height -= height % 2;
	if (image_frame->width_ == width && image_frame->height_ == height && !image_frame->data_.empty())
	{
		return true;
	}
	Gdiplus::BitmapData data;
	Gdiplus::Bitmap* bitmap = nullptr;
	bool creat_bitmap = false;
	if (src_height == height && src_width == width)
	{
		bitmap = src_image_.get();
	} 
	else
	{
		bitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
		Gdiplus::Graphics g(bitmap);

		g.DrawImage(src_image_.get(), Gdiplus::Rect(0, 0, width, height), 0, 0, src_width, src_height, Gdiplus::UnitPixel);
	}
	if (bitmap == nullptr)
	{
		return false;
	}
	Gdiplus::Rect rect(0, 0, width, height);
	int type = bitmap->GetPixelFormat();
	ASSERT(type == PixelFormat32bppARGB);
	bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data);

	int byte_width = data.Stride;
	image_frame->width_ = width;
	image_frame->height_ = height;
	image_frame->index_ = index;
	image_frame->alpha_ = CopyImageData((const char*)data.Scan0, data.Stride, image_frame->data_, width, height);
	bitmap->UnlockBits(&data);
	if (creat_bitmap)
	{
		delete bitmap;
		bitmap = nullptr;
	}
	return true;
}
bool YuvImage::CopyImageData(const char* src, int byte_width, std::string& dest, int width, int height)
{
	bool alpha = false;
	int wxh = width * height;
	dest.clear();
	dest.append(wxh * 3, (char)0);
	uint8_t* des_y = (uint8_t*)dest.c_str();
	uint8_t* des_u = des_y + wxh * 2;
	uint8_t* des_v = des_u + wxh / 2;
	const uint8_t* src_argb = (const uint8_t*)src;
	libyuv::ARGBToI420(src_argb, byte_width,
		des_y, width * 2,
		des_u, width,
		des_v, width,
		width, height);
	uint8_t* a1 = (uint8_t*)src + 3;
	uint8_t* des_ya = des_y + width;
	uint8_t* des_ua = des_u + width / 2;
	uint8_t* des_va = des_v + width / 2;
	for (int i = 0; i < height / 2; ++i)
	{
		uint8_t* a2 = a1 + byte_width;
		uint8_t* des_ya2 = des_ya + width * 2;
		for (int j = 0; j < width / 2; ++j)
		{
			int y1 = *a1;
			a1 += 4;
			*(des_ya++) = y1;
			int y2 = *a1;
			a1 += 4;
			*(des_ya++) = y2;
			int y3 = *a2;
			a2 += 4;
			*(des_ya2++) = y3;
			int y4 = *a2;
			a2 += 4;
			*(des_ya2++) = y4;
			uint8_t a_uv = (y1 + y2 + y3 + y4) / 4;
			*(des_ua++) = a_uv;
			*(des_va++) = a_uv;
			if (a_uv != 255)
			{
				alpha = true;
			}
		}
		a1 += byte_width;
		des_ya += width * 3;
		des_ua += width / 2;
		des_va += width / 2;
	}
	return alpha;
}
ImageFrame* YuvImage::GetImageFrame(int index, int width, int height)
{
	if (image_list_.size() > index && index >= 0)
	{
		auto it = &image_list_.at(index);
		if (CheckImageFrame(it->index_, it, width, height))
		{
			return it;
		}
	}
	return nullptr;
}
ImageFrame* YuvImage::GetImageFrameEx(int64_t time, int width, int height)
{
	if (time_start_ == 0)
	{
		time_start_ = time;
	}
	int64_t time_d = time - time_start_;
	if (time_all_ > 0)
	{
		time_d = time_d % time_all_;
	}
	else
	{
		time_d = 0;
	}
	for (auto& it : image_list_)
	{
		if (it.time_ >= time_d)
		{
			if (CheckImageFrame(it.index_, &it, width, height))
			{
				return &it;
			} 
		}
	}
	return nullptr;
}

}
#include "ui_image_view.h"


UiImageView::UiImageView(void) 
	: angle_(0)
	, scale_(0.0)
	, image_()
	, offset_x_(0)
	, offset_y_(0)
	, can_moving_(false)
	,image_transform_()
	,bAdjustScale_(false)
{
	rect_image_.left = 0;
	rect_image_.top = 0;
	rect_image_.right = 0;
	rect_image_.bottom = 0;
}

void UiImageView::DoInit()
{

}

UiImageView::~UiImageView(void)
{
}

void UiImageView::HandleMessage(ui::EventArgs& event)
{
	return Control::HandleMessage(event);
}

void UiImageView::Paint(ui::IRenderContext* pRender, const ui::UiRect& rcPaint)
{
	 if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	 if (NULL == image_)
	 {
		 return;
	 }
	 int image_width = image_->GetWidth();
	 int image_height = image_->GetHeight();
	 if (image_width == 0 || image_height == 0)
	 {
		 return;
	 }
	
	 int control_width = m_rcItem.right - m_rcItem.left ;
	 int control_height = m_rcItem.bottom - m_rcItem.top ;

	 //计算初始显示比例;
	 if (scale_ <= 0)
	 {
		 scale_ = GetDefaultScale(control_width, control_height, image_width, image_height);
		 image_transform_.reset();
	 }

	 int image_transform_width = int(image_width * scale_);
	 int image_transform_height = int(image_height * scale_);

	 if (!image_transform_)
	 {
		 image_transform_.reset(new Gdiplus::Bitmap(image_transform_width, image_transform_height));
		 Gdiplus::Graphics g(image_transform_.get());

		 g.DrawImage(image_.get(), 0.0, 0.0, (Gdiplus::REAL)image_transform_width, (Gdiplus::REAL)image_transform_height);
		 image_transform_->RotateFlip((Gdiplus::RotateFlipType)(GetAngle()/90));
	 }

	 image_transform_width = image_transform_->GetWidth();
	 image_transform_height = image_transform_->GetHeight();

	 //修正x、y方向的偏移，让偏移限定在一定范围内
	 CheckOffset(offset_x_, control_width, image_transform_width);
	 CheckOffset(offset_y_, control_height, image_transform_height);

	 int default_x = (control_width - image_transform_width) / 2 + m_rcItem.left;
	 int default_y = (control_height - image_transform_height) / 2 + m_rcItem.top;

	 can_moving_ = control_width < image_transform_width || control_height < image_transform_height;

	 rect_image_.left = max(default_x + offset_x_, m_rcItem.left);
	 rect_image_.top = max(default_y + offset_y_, m_rcItem.top);
	 rect_image_.right = min(rect_image_.left + image_transform_width, m_rcItem.right);
	 rect_image_.bottom = min(rect_image_.top + image_transform_height, m_rcItem.bottom);

	 Gdiplus::Graphics graph(pRender->GetDC()); 

	 //图片的绘制范围，可能超出显示范围
	 Gdiplus::RectF rect_image((Gdiplus::REAL)rect_image_.left, (Gdiplus::REAL)rect_image_.top, 
					(Gdiplus::REAL)(rect_image_.right - rect_image_.left), (Gdiplus::REAL)(rect_image_.bottom - rect_image_.top));
	 //实际显示出来的范围
	 Gdiplus::RectF rect_show((Gdiplus::REAL)m_rcItem.left, (Gdiplus::REAL)m_rcItem.top,
					(Gdiplus::REAL)(m_rcItem.right - m_rcItem.left), (Gdiplus::REAL)(m_rcItem.bottom - m_rcItem.top));
	 rect_show.Intersect(rect_image);

	 int src_x = int(rect_show.GetLeft() - default_x + offset_x_);
	 int src_y = int(rect_show.GetTop() - default_y + offset_y_);

	 graph.DrawImage(image_transform_.get(), rect_show, (Gdiplus::REAL)src_x, (Gdiplus::REAL)src_y, rect_show.Width, rect_show.Height, Gdiplus::UnitPixel);
	 graph.ReleaseHDC(pRender->GetDC());
}

void UiImageView::RotatePic(PicDirectionOfRotation direction)
{
	switch (direction)
	{
	case kClockWise:
	{
		offset_x_ = 0;
		offset_y_ = 0;

		int new_angle = GetAngle() + 90;

		image_transform_.reset();
		SetAngle(new_angle);
	}
	break;
	case kAntiClockWise:
	{
		offset_x_ = 0;
		offset_y_ = 0;
		int new_angle = GetAngle() - 90;
		image_transform_.reset();
		SetAngle(new_angle);
	}
	break;
	default:
		break;
	}
}

void UiImageView::ScalePic(PicZoom zoom)
{
	switch (zoom)
	{
	case kReducePic:
	{
		if (GetScale() <= 0.1)
		{
			bAdjustScale_ = true;
			return;
		}
		float fScale = GetScale() - 0.10f;
		if (fScale < 0.1)
		{
			fScale = 0.1f;
		}

		image_transform_.reset();
		SetScale(fScale);
	}
	break;
	case kMagnifyPic:
	{
		if (GetScale() >= 5.0)
		{
			return;
		}
		float fScale = GetScale() + 0.10f;
		if (fScale > 1.0 && fScale < 1.1)
		{
			fScale = 1.0;
		}
		if (fScale > 5.0)
		{
			fScale = 5.0;
		}
		if (fScale >= 1.0)
		{
			bAdjustScale_ = true;
		}
		image_transform_.reset();
		SetScale(fScale);
	}
	break;
	default:
		break;
	}
}

int UiImageView::SetPicPath(std::wstring image_path)
{ 
	bAdjustScale_ = false;
	image_path_ = image_path;
	angle_ = 0;
	scale_ = 0.0;

	if (image_)
	{
		image_.reset();
	}
	image_.reset(Gdiplus::Image::FromFile(image_path_.c_str()));

	GUID guid = Gdiplus::FrameDimensionTime;
	int frame_count = image_->GetFrameCount(&guid);

	//旋转原图;
	Gdiplus::RotateFlipType type = GetRotateFlipType(*image_.get());
	if (type != Gdiplus::RotateNoneFlipNone)
		image_->RotateFlip(type);

	Invalidate();
	return frame_count;
}

UINT UiImageView::GetImageWidth()
{
	if (image_)
		return image_->GetWidth();
	return 0;
}

UINT UiImageView::GetImageHeight()
{
	if (image_)
		return image_->GetHeight();
	return 0;
}

bool UiImageView::PtInImageView(POINT point)
{
	if (can_moving_ && PtInRect(&rect_image_, point))
	{
		return true;
	}
	return false;
}

void UiImageView::SetOriginalSize()
{
	image_transform_.reset();
	SetScale(1.0f);
}

float UiImageView::GetDefaultScale(int control_width, int control_height, int image_width, int image_height)
{
	float fScale = 10.0;
	int width_sub = control_width - image_width;
	int height_sub = control_height - image_height;

	if (width_sub < 0 || height_sub < 0)//超出显示区域
	{
		if (width_sub < height_sub)	//让超出显示区域更多的量按比例收缩，这样最终缩小后，可以让图片正好充满窗体
		{
			fScale = float(control_width * 10.0 / image_width);
		}
		else
		{
			fScale = float(control_height * 10.0 / image_height);
		}
	}

	if (fScale<1)
	{
		fScale = 1;
	}
	return fScale/10;
}

void UiImageView::SetPicOffset(int offset_x, int offset_y)
{ 
	offset_x_ += offset_x;
	offset_y_ += offset_y;
	Invalidate(); 
}

void UiImageView::SetAngle(int angle)
{ 
	angle_ = (angle + 360) % 360;

	Invalidate(); 
}

void UiImageView::SetScale(float scale)
{
	if (scale <= 0.0)
		return;

	scale_ = scale;
	Invalidate();
}

HBITMAP UiImageView::GetRotatedBitmap(HDC hDC, HBITMAP bitmap_source, float radians, COLORREF back_clr)
{
	HDC dc_source = ::CreateCompatibleDC(hDC);
	HDC dc_dest = ::CreateCompatibleDC(hDC);

	float cosine = cos(radians);
	float sine = sin(radians);

	// Get logical coordinates
	BITMAP bm;
	::GetObject(bitmap_source, sizeof(bm), &bm);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int minx = min(0, min(x1, min(x2, x3)));
	int miny = min(0, min(y1, min(y2, y3)));
	int maxx = max(0, max(x1, max(x2, x3)));
	int maxy = max(0, max(y1, max(y2, y3)));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP bitmap_result = ::CreateCompatibleBitmap(hDC, w, h);

	HBITMAP dc_old_source = (HBITMAP)::SelectObject(dc_source, bitmap_source);
	HBITMAP dc_old_dest = (HBITMAP)::SelectObject(dc_dest, bitmap_result);

	// Draw the background color before we change mapping mode
	HBRUSH back_brush = CreateSolidBrush(back_clr);
	HBRUSH old_back_brush = (HBRUSH)::SelectObject(dc_dest, back_brush);
	PatBlt(dc_dest, 0, 0, w, h, PATCOPY);
	::DeleteObject(::SelectObject(dc_dest, old_back_brush));

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(dc_dest, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;

	SetWorldTransform(dc_dest, &xform);

	// Now do the actual rotating - a pixel at a time
	BitBlt(dc_dest, 0, 0, bm.bmWidth, bm.bmHeight, dc_source, 0, 0, SRCCOPY);

	// Restore DCs
	::SelectObject(dc_source, dc_old_source);
	::SelectObject(dc_dest, dc_old_dest);
	::DeleteDC(dc_source);
	::DeleteDC(dc_dest);
	return bitmap_result;
}

Gdiplus::RotateFlipType UiImageView::GetRotateFlipType(Gdiplus::Image& image)
{
	short direction = 0;
	UINT nSize = image.GetPropertyItemSize(PropertyTagOrientation);
	if (nSize > 0)
	{
		Gdiplus::PropertyItem* property_item = (Gdiplus::PropertyItem*) malloc( nSize );
		if (image.GetPropertyItem( PropertyTagOrientation, nSize, property_item ) == Gdiplus::Ok)
		{
			direction = *(short*)property_item->value;
		}
		free(property_item);
	}
	return GetRotateFlipType(direction);
}

Gdiplus::RotateFlipType UiImageView::GetRotateFlipType(short direction)
{
	Gdiplus::RotateFlipType type = Gdiplus::RotateNoneFlipNone;
	switch (direction)
	{
	case 1:
		break;
	case 2:
		type = Gdiplus::RotateNoneFlipX;
		break;
	case 3:
		type = Gdiplus::RotateNoneFlipXY;
		break;
	case 4:
		type = Gdiplus::RotateNoneFlipY;
		break;
	case 5:
		type = Gdiplus::Rotate90FlipX;
		break;
	case 6:
		type = Gdiplus::Rotate90FlipNone;
		break;
	case 7:
		type = Gdiplus::Rotate90FlipY;
		break;
	case 8:
		type = Gdiplus::Rotate270FlipNone;
		break;
	}
	return type;
}

void UiImageView::CheckOffset(int& nOffset, int nControl, int nImage)
{
	//如果控件的宽度(高度)比图片的宽度(高度)要大，那么最终显示的图片不需要偏移
	if (nControl >= nImage)
	{
		nOffset = 0;
	}
	else
	{
		//如果控件的宽度比图片的宽度小，那么允许用户拖拽图片产生图片的偏移
		//但是偏移后的图片必须填充满整个控件，不能有任意的偏移量
		//这里检查偏移量，让他锁定在一定范围内
		int normal_offset = (nControl - nImage) / 2;
		normal_offset = -normal_offset;

		if (nOffset > normal_offset)
		{
			nOffset = normal_offset;
		}
		else if (nOffset < -normal_offset)
		{
			nOffset = -normal_offset;
		}
	}
}

LPCTSTR UiImageView::kControlName = L"ImageView";
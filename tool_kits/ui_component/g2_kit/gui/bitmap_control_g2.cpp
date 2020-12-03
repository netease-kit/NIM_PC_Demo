#include "pch.h"
#include "bitmap_control_g2.h"
#include <sys/timeb.h>
#include "libyuv.h"
//#include "module/video/video_manager.h"

#define TIMER_FOR_CHECK_BITMAP	163
namespace ui
{

BitmapControlG2::BitmapControlG2(Window* parent)
{
	parent_wnd_ = parent;
	camera_width_ = 0;
	camera_height_ = 0;
	scaled_width_ = 0;
	scaled_height_ = 0;
	auto_size_ = false;
	enable_capture_ = true;
}

BitmapControlG2::~BitmapControlG2(void)
{
	//Clear();
}

void BitmapControlG2::SetAutoSize(bool auto_size)
{
	auto_size_ = auto_size;
}

void BitmapControlG2::Paint(IRenderContext* pRender, const UiRect& rcPaint)
{
	try
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
			return;
		Control::Paint(pRender, rcPaint);
		//paint bitmap

		if (scaled_width_ * scaled_height_ > 0)
		{
			int item_w = m_rcItem.right-m_rcItem.left;
			int item_h = m_rcItem.bottom-m_rcItem.top;
			int item_x = m_rcItem.left;
			int item_y = m_rcItem.top;
			int source_w = scaled_width_;
			int source_h = scaled_height_;
			if (source_w > 0 && source_h > 0 && parent_wnd_)
			{
				//居中
				item_x += (item_w - source_w) / 2;
				item_y += (item_h - source_h) / 2;
				UiRect rcClient;
				::GetClientRect(parent_wnd_->GetHWND(), &rcClient);
				int width = rcClient.right - rcClient.left;
				int height = rcClient.bottom - rcClient.top;

				//计算实际绘制区域坐标
				int draw_x = max(rcPaint.left, item_x);
				draw_x = max(m_rcItem.left, draw_x);
				int draw_y = max(rcPaint.top, item_y);
				draw_y = max(m_rcItem.top, draw_y);
				int draw_h = min(rcPaint.bottom - draw_y, min(item_y + source_h, m_rcItem.bottom) - draw_y);
				draw_h = max(draw_h, 0);
				int src_x = draw_x - item_x;
				int src_y = draw_y - item_y;
				int src_w = min(rcPaint.right - draw_x, min(item_x + source_w, m_rcItem.right) - draw_x);
				src_w = max(src_w, 0);

				int dest_byte_width = width * 4;
				int src_byte_width = source_w * 4;
				int paint_byte_width = src_w * 4;
				char* dest_data = (char*)parent_wnd_->GetRenderContext()->GetBits();
				int bottom = draw_y;// height - draw_y - 1;
				dest_data += bottom * dest_byte_width + draw_x * 4;
				{
					//nbase::NAutoLock auto_lock(&frame_data_lock_);
					char* src_data = (char*)scaled_data_.c_str();
					src_data += src_y * src_byte_width + src_x * 4;
					for (int i = 0; i < draw_h; ++i)
					{
						memcpy(dest_data, src_data, paint_byte_width);
						dest_data += dest_byte_width;//-=
						src_data += src_byte_width;
					}
				}
				
			}
		}
		//绘制子控件
		for( auto it = m_items.begin(); it != m_items.end(); it++ )
		{
			Control* pControl = *it;
			if( !pControl->IsVisible() ) continue;
			UiRect controlPos = pControl->GetPos();
			if( !::IntersectRect( &m_rcPaint, &rcPaint, &controlPos ) ) continue;
			pControl->AlphaPaint( pRender, rcPaint );
		}
	}
	catch (...)
	{
		throw "CBitmapControlG2::DoPaint";
	}
}

//缩放缓存的YUV数据，并返回ARGB格式的数据
bool BitmapControlG2::ScaleVideoFrame(char* out_data, int& width, int& height)
{
	if (camera_width_ <= 0 || camera_height_ <= 0)
		return false;

	nbase::NAutoLock auto_lock(&camera_data_lock_);
	int src_w = camera_width_;
	int src_h = camera_height_;
	//等比
	if (width <= 0 || height <= 0)
	{
		width = src_w;
		height = src_h;
	}
	else if (src_h * width > src_w * height)
	{
		width = src_w * height / src_h;
	}
	else
	{
		height = src_h * width / src_w;
	}
	width -= width % 2;
	height -= height % 2;

	std::string ret_data;
	if (width != src_w || height != src_h)
	{
		ret_data.append(width * height * 3 / 2, (char)0);
		uint8_t* src_y = (uint8_t*)camera_data_.c_str();
		uint8_t* src_u = src_y + src_w * src_h;
		uint8_t* src_v = src_u + src_w * src_h / 4;
		uint8_t* des_y = (uint8_t*)ret_data.c_str();
		uint8_t* des_u = des_y + width * height;
		uint8_t* des_v = des_u + width * height / 4;
		libyuv::FilterMode filter_mode = libyuv::kFilterBox;
		libyuv::I420Scale(
			src_y, src_w,
			src_u, src_w / 2,
			src_v, src_w / 2,
			src_w, src_h,
			des_y, width,
			des_u, width / 2,
			des_v, width / 2,
			width, height,
			filter_mode);
	}
	else
	{
		ret_data.append(camera_data_.c_str(), camera_data_.size());
	}

	uint8_t* des_y = (uint8_t*)ret_data.c_str();
	uint8_t* des_u = des_y + width * height;
	uint8_t* des_v = des_u + width * height / 4;
	libyuv::I420ToARGB(
		des_y, width,
		des_u, width / 2,
		des_v, width / 2,
		(uint8_t*)out_data, width * 4,
		width, height);

	return true;
}

bool BitmapControlG2::Refresh()
{
	int item_w = m_rcItem.right - m_rcItem.left;
	int item_h = m_rcItem.bottom - m_rcItem.top;
	if (auto_size_)
	{
		item_w = GetMaxWidth();
		item_h = GetMaxHeight();
	}
	bool ret = false;
	if (item_w > 0 && item_h > 0)
	{
		scaled_data_.resize(item_w * item_h * 4);
		ret = ScaleVideoFrame((char*)scaled_data_.c_str(), item_w, item_h);
		if (ret)
		{
			scaled_width_ = item_w;
			scaled_height_ = item_h;
			if (auto_size_)
			{
				SetFixedWidth(scaled_width_);
				SetFixedHeight(scaled_height_);
			}
			Invalidate();
		}
	}
	return ret;
}
//摄像头关闭为异步操作，Clear之后仍有可能进入CaptureVideoFrame中
void BitmapControlG2::EnableCaptureVideoFrame(bool enable)
{
	enable_capture_ = enable;
}
bool BitmapControlG2::IsEnableCaptureVideoFrame()
{
	return enable_capture_;
}
//清理失效数据
void BitmapControlG2::Clear()
{
	camera_width_ = 0;
	camera_height_ = 0;
	scaled_width_ = 0;
	scaled_height_ = 0;
}


void BitmapControlG2::CaptureVideoFrame(uint64_t uid,		/**< uid */
										void* data,         /**< 数据指针 */
										uint32_t type,      /**< NERtcVideoType */
										uint32_t width,     /**< 宽度 */
										uint32_t height,    /**< 高度 */
										uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
										uint32_t offset[4], /**< 每类数据偏移 */
										uint32_t stride[4], /**< 每类数据步进 */
										uint32_t rotation,  /**< NERtcVideoRotation */
										void* user_data     /**< 用户透传数据 */)
{
	if (!enable_capture_)
	{
		return;
	}
		
	nbase::NAutoLock auto_lock(&camera_data_lock_);
	camera_width_ = width;
	camera_height_ = height;
	camera_data_.resize(width * height * 3 / 2);

	auto src = reinterpret_cast<uint8_t*>(data);

	uint8_t* des_y = (uint8_t*)camera_data_.c_str();
	uint8_t* des_u = des_y + width * height;
	uint8_t* des_v = des_u + width * height / 4;

	libyuv::I420Copy(
		src + offset[0], stride[0],
		src + offset[1], stride[1],
		src + offset[2], stride[2],
		des_y, width,
		des_u, width / 2,
		des_v, width / 2,
		width, height
	);

	//delete data;
}
}



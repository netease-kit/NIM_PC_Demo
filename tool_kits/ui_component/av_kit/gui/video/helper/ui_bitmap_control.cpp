#include "ui_bitmap_control.h"
#include <sys/timeb.h>
#include "module/video/video_manager.h"

#define TIMER_FOR_CHECK_BITMAP	163
namespace ui
{

BitmapControl::BitmapControl(nim_comp::VideoFrameMng* video_frame_mng)
{
	video_frame_mng_ = video_frame_mng;
	timestamp_ = 0;
	width_ = 0;
	height_ = 0;
	auto_size_ = false;
}

BitmapControl::~BitmapControl(void)
{
	Clear();
}

void BitmapControl::Paint(IRenderContext* pRender, const UiRect& rcPaint)
{
	try
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
			return;
		Control::Paint(pRender, rcPaint);
		//paint bitmap
		if (width_ * height_ > 0)
		{
			int item_w = m_rcItem.right-m_rcItem.left;
			int item_h = m_rcItem.bottom-m_rcItem.top;
			int item_x = m_rcItem.left;
			int item_y = m_rcItem.top;
			int source_w = width_;
			int source_h = height_;
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
				char* src_data = (char*)data_.c_str();
				src_data += src_y * src_byte_width + src_x * 4;
				for (int i = 0; i < draw_h; ++i)
				{
					memcpy(dest_data, src_data, paint_byte_width);
					dest_data += dest_byte_width;//-=
					src_data += src_byte_width;
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
		throw "CBitmapControl::DoPaint";
	}
}
bool BitmapControl::Refresh(Window* wnd, bool captrue, bool mirror)
{
	if (!video_frame_mng_ || (!captrue && account_.empty()))
	{
		return false;
	}
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
		parent_wnd_ = wnd;
		data_.resize(item_w * item_h * 4);

		ret = video_frame_mng_->GetVideoFrame((captrue ? "" : account_), timestamp_, (char*)data_.c_str(), item_w, item_h, mirror);
		if (ret)
		{
			width_ = item_w;
			height_ = item_h;
			if (auto_size_)
			{
				SetFixedWidth(width_);
				SetFixedHeight(height_);
			}
			Invalidate();
		}
	}
	return ret;
}
//清理失效数据
void BitmapControl::Clear()
{
	timestamp_ = 0;
	width_ = 0;
	height_ = 0;
}
bool BitmapControl::IsRefreshTimeout()
{
	timeb time_now;
	ftime(&time_now); // 秒数
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒

	if (cur_timestamp > timestamp_ + 1500)
	{
		return true;
	}
	return false;
}

}



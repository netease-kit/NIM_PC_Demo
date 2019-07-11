#include "ui_video_player_bitmap_control.h"
#include <sys/timeb.h>
#ifdef SUPPORTLOCALPLAYER
#include "nls_play\libyuv\include\scale_argb.h"
#define TIMER_FOR_CHECK_BITMAP	163
namespace ui
{

VideoPlayerBitmapControl::VideoPlayerBitmapControl()
{
#ifdef SUPPORTLOCALPLAYER
	static std::once_flag o_f;
	std::call_once(o_f, []() {

		TCHAR path_envirom[4096] = { 0 };
		GetEnvironmentVariable(L"path", path_envirom, 4096);
		std::wstring local_video_player_path = QPath::GetAppPath().append(L"live_player").append(L";");
		std::wstring new_envirom(local_video_player_path);
		new_envirom.append(path_envirom);
		SetEnvironmentVariable(L"path", new_envirom.c_str());
	});
#endif
	timestamp_ = 0;
	width_ = 0;
	height_ = 0;

	auto refresh_task = ToWeakCallback(nbase::Bind(&VideoPlayerBitmapControl::Refresh, this));
	nbase::ThreadManager::PostRepeatedTask(refresh_task, nbase::TimeDelta::FromMilliseconds(40));
}

VideoPlayerBitmapControl::~VideoPlayerBitmapControl()
{
	Clear();
}

void VideoPlayerBitmapControl::Paint(IRenderContext* pRender, const UiRect& rcPaint)
{
	Window* parent_wnd_ = GetWindow();
	if (!parent_wnd_)
		return;

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
			pControl->AlphaPaint( pRender, rcPaint);
		}
	}
	catch (...)
	{
		throw "CBitmapControl::DoPaint";
	}
}

bool VideoPlayerBitmapControl::Update(const char* image_data, int image_w, int image_h)
{
	nbase::NAutoLock auto_lock(&lock_);
	if (image_width_ * image_height_ != image_w * image_h)
		image_data_.resize(image_w * image_h * 4);
	image_width_ = image_w;
	image_height_ = image_h;
	memcpy((char*)image_data_.c_str(), image_data, image_w * image_h * 4);
	updated_ = true;
	return true;
}

void VideoPlayerBitmapControl::Refresh()
{
	int item_w = m_rcItem.right - m_rcItem.left;
	int item_h = m_rcItem.bottom - m_rcItem.top;
	if (item_w == width_ && item_h == height_ && !updated_) //画布大小没变化，图象也没更新，不需要刷新
		return;

	if (image_data_.empty() || image_width_ <= 0 || image_height_ <= 0 || item_w <= 0 || item_h <= 0)
		return;

	nbase::NAutoLock auto_lock(&lock_);

	bool ret = false;
	data_.resize(item_w * item_h * 4);
	memset((char*)data_.c_str(), 0, item_w * item_h * 4);
	int shift = 0;
	int clip_w = item_w, clip_h = item_h;
	if (clip_w * image_height_ < clip_h * image_width_)
	{
		clip_h = clip_w * image_height_ / image_width_;
		int clip_y = (item_h - clip_h) / 2;
		clip_w -= clip_w % 2;
		clip_h -= clip_h % 2;
		clip_y -= clip_y % 2;
		shift = item_w * clip_y * 4;
	}
	else if (clip_w * image_height_ > clip_h * image_width_)
	{
		clip_w = clip_h * image_width_ / image_height_;
		int clip_x = (item_w - clip_w) / 2;
		clip_w -= clip_w % 2;
		clip_h -= clip_h % 2;
		clip_x -= clip_x % 2;
		shift = clip_x * 4;
	}

	ret = libyuv::ARGBScale((const uint8*)image_data_.c_str(), image_width_ * 4, image_width_, image_height_,
		(uint8*)data_.c_str() + shift, item_w * 4, clip_w, clip_h, libyuv::kFilterBox) >= 0;
	if (ret)
	{
		width_ = item_w;
		height_ = item_h;
		updated_ = false;
		Invalidate();
	}
}

//清理失效数据
void VideoPlayerBitmapControl::Clear()
{
	nbase::NAutoLock auto_lock(&lock_);
	timestamp_ = 0;
	data_.clear();
	image_data_.clear();
	image_width_ = 0;
	image_height_ = 0;
	if (width_ * height_ > 0)
	{
		width_ = 0;
		height_ = 0;
		Invalidate();
	}
}
bool VideoPlayerBitmapControl::IsRefreshTimeout()
{
	timeb time_now;
	ftime(&time_now); // 秒数
	int64_t cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒

	if (cur_timestamp > timestamp_ )
	{
		return true;
	}
	return false;
}

}
#endif




#include "screen_capture_tool.h"
#include "module/video/video_manager.h"
#include "libyuv.h"
#include <sys/timeb.h>

#define BORDER_SIZE	27
namespace nim_comp
{
const LPTSTR ScreenCaptureTool::kClassName = L"ScreenCaptureTool";

void MixClr(const uint8_t* src_clr, uint8_t* dest_clr, uint8_t a)
{
	uint16_t temp = ((uint16_t)*src_clr) * a + ((uint16_t)*dest_clr) * (255 - a);
	*dest_clr = temp / 255;
};
void I420CopyByA(const uint8* src_y, int src_stride_y,
	const uint8* src_u, int src_stride_u,
	const uint8* src_v, int src_stride_v,
	uint8* dst_y, int dst_stride_y,
	uint8* dst_u, int dst_stride_u,
	uint8* dst_v, int dst_stride_v,
	int width, int height, uint8_t alpha)
{
	for (int ih = 0; ih < height; ++ih)
	{
		bool uv = ih % 2 == 0;
		for (int iw = 0; iw < width / 2; ++iw)
		{
			MixClr(src_y++, dst_y++, alpha);
			MixClr(src_y++, dst_y++, alpha);
			if (uv)
			{
				MixClr(src_u++, dst_u++, alpha);
				MixClr(src_v++, dst_v++, alpha);
			}
		}
		src_y += (src_stride_y - width);
		dst_y += (dst_stride_y - width);
		if (uv)
		{
			src_u += (src_stride_u - width / 2);
			src_v += (src_stride_v - width / 2);
			dst_u += (dst_stride_u - width / 2);
			dst_v += (dst_stride_v - width / 2);
		}
	}
}
void I420CopyEx(const uint8* src_y, int src_stride_y, const uint8* src_ya, int src_stride_ya,
	const uint8* src_u, int src_stride_u, const uint8* src_ua, int src_stride_ua,
	const uint8* src_v, int src_stride_v, const uint8* src_va, int src_stride_va,
	uint8* dst_y, int dst_stride_y,
	uint8* dst_u, int dst_stride_u,
	uint8* dst_v, int dst_stride_v,
	int width, int height)
{
	if (src_stride_ya == 0 && src_stride_ua == 0 && src_stride_va == 0)
	{
		libyuv::I420Copy(src_y, src_stride_y,
			src_u, src_stride_u,
			src_v, src_stride_v,
			dst_y, dst_stride_y,
			dst_u, dst_stride_u,
			dst_v, dst_stride_v,
			width, height);
	}
	else
	{
		for (int ih = 0; ih < height; ++ih)
		{
			bool uv = ih % 2 == 0;
			for (int iw = 0; iw < width / 2; ++iw)
			{
				MixClr(src_y++, dst_y++, *src_ya++);
				MixClr(src_y++, dst_y++, *src_ya++);
				if (uv)
				{
					MixClr(src_u++, dst_u++, *src_ua++);
					MixClr(src_v++, dst_v++, *src_va++);
				}
			}
			src_y += (src_stride_y - width);
			src_ya += (src_stride_ya - width);
			dst_y += (dst_stride_y - width);
			if (uv)
			{
				src_u += (src_stride_u - width / 2);
				src_v += (src_stride_v - width / 2);
				src_ua += (src_stride_ua - width / 2);
				src_va += (src_stride_va - width / 2);
				dst_u += (dst_stride_u - width / 2);
				dst_v += (dst_stride_v - width / 2);
			}
		}
	}
}

ScreenCaptureTool::ScreenCaptureTool()
{
	width_ = kScreenDefWidth;
	height_ = kScreenDefHeight;
	track_state_ = NOTRACK;
	video_frame_mng_ = nullptr;
}

ScreenCaptureTool::~ScreenCaptureTool()
{
	StopCapture();
}

std::wstring ScreenCaptureTool::GetSkinFolder()
{
	return L"video";
}

std::wstring ScreenCaptureTool::GetSkinFile()
{
	return L"screen_tool.xml";
}

std::wstring ScreenCaptureTool::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ScreenCaptureTool::GetWindowId() const
{
	return kClassName;
}

UINT ScreenCaptureTool::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void ScreenCaptureTool::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&ScreenCaptureTool::OnClicked, this, std::placeholders::_1));

	track_box_ = (ui::Box*)FindControl(L"track_box");
	track_NW_ = FindControl(L"track_NW");
	track_T_ = FindControl(L"track_T");
	track_NE_ = FindControl(L"track_NE");
	track_L_ = FindControl(L"track_L");
	track_R_ = FindControl(L"track_R");
	track_SW_ = FindControl(L"track_SW");
	track_B_ = FindControl(L"track_B");
	track_SE_ = FindControl(L"track_SE");

	RECT rcWindow;

	screen_width_ = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	screen_height_ = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	screen_x_ = GetSystemMetrics(SM_XVIRTUALSCREEN);
	screen_y_ = GetSystemMetrics(SM_YVIRTUALSCREEN);

	rcWindow.left = screen_x_;
	rcWindow.top = screen_y_;
	rcWindow.right = screen_x_ + screen_width_;
	rcWindow.bottom = screen_y_ + screen_height_;
	track_rect_.left = 100;
	track_rect_.top = 100;
	track_rect_.right = screen_width_ - 100;
	track_rect_.bottom = screen_height_ - 100;

	POINT pt = { 0, 0 };
	ResizeTrackRect(pt);

	LONG windowLongEx = GetWindowLong(m_hWnd, GWL_EXSTYLE);

	SetWindowLong(m_hWnd, GWL_EXSTYLE, windowLongEx | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

	// 调试时用  HWND_NOTOPMOST，用完换回来 HWND_TOPMOST
	SetPos(ui::UiRect(screen_x_, screen_y_, screen_x_ + screen_width_, screen_y_ + screen_height_), SWP_NOACTIVATE, HWND_TOPMOST);

	//StartCapture(GetDesktopWindow(), 60);

}
LRESULT ScreenCaptureTool::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	point_offset_.x = GET_X_LPARAM(lParam);
	point_offset_.y = GET_Y_LPARAM(lParam);
	int type = CheckPtType(point_offset_);
	if (type != NOTRACK)
	{
		track_state_ = type;
		bHandled = FALSE;
		SetHitTestCursor(track_state_);
	}
	return 0;
}
LRESULT ScreenCaptureTool::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (track_state_ != NOTRACK)
	{
		bHandled = FALSE;
		track_state_ = NOTRACK;
		SetHitTestCursor(track_state_);
	}
	return 0;
}
LRESULT ScreenCaptureTool::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	if (track_state_ != NOTRACK)
	{
		POINT pt_move;
		pt_move.x = pt.x - point_offset_.x;
		pt_move.y = pt.y - point_offset_.y;
		ResizeTrackRect(pt_move);
		point_offset_.x += pt_move.x;
		point_offset_.y += pt_move.y;
		bHandled = FALSE;
		SetHitTestCursor(track_state_);
	}
	else
	{
		int type = CheckPtType(pt);
		if (type != NOTRACK)
		{
			SetHitTestCursor(type);
		}
	}
	return 0;
}

LRESULT ScreenCaptureTool::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SETCURSOR)
	{
		if (track_state_ != NOTRACK)
		{
			return 0;
		}
	}
	else if (uMsg == WM_KEYDOWN) 
	{
		if (wParam == VK_ESCAPE) 
		{
			return 0;
		}
	}
	else if (uMsg == WM_SYSKEYDOWN)
	{
		// Alt+F4
		if (wParam == VK_F4)
		{
			return 0;
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}
void ScreenCaptureTool::SetHitTestCursor(int hit_type)
{
	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	switch (hit_type)
	{
	case SIZEALL:		cursor = LoadCursor(NULL, IDC_SIZEALL); break;
	case SIZETOP:
	case SIZEBOTTOM:	cursor = LoadCursor(NULL, IDC_SIZENS); break;
	case SIZELEFT:
	case SIZERIGHT:		cursor = LoadCursor(NULL, IDC_SIZEWE); break;
	case SIZESE:
	case SIZENW:		cursor = LoadCursor(NULL, IDC_SIZENWSE); break;
	case SIZESW:
	case SIZENE:		cursor = LoadCursor(NULL, IDC_SIZENESW); break;
	}
	SetCursor(cursor);
}
int ScreenCaptureTool::CheckPtType(POINT pt)
{
	int type = NOTRACK;
	{
		ui::UiRect rect = track_box_->GetPos();
		if (PtInRect(&rect, pt))
		{
			type = SIZEALL;
			if (PtInRect(&track_NW_->GetPos(), pt))
			{
				type = SIZENW;
			}
			else if (PtInRect(&track_T_->GetPos(), pt))
			{
				type = SIZETOP;
			}
			else if (PtInRect(&track_NE_->GetPos(), pt))
			{
				type = SIZENE;
			}
			else if (PtInRect(&track_L_->GetPos(), pt))
			{
				type = SIZELEFT;
			}
			else if (PtInRect(&track_R_->GetPos(), pt))
			{
				type = SIZERIGHT;
			}
			else if (PtInRect(&track_SW_->GetPos(), pt))
			{
				type = SIZESW;
			}
			else if (PtInRect(&track_B_->GetPos(), pt))
			{
				type = SIZEBOTTOM;
			}
			else if (PtInRect(&track_SE_->GetPos(), pt))
			{
				type = SIZESE;
			}
		}
	}
	return type;
}
void ScreenCaptureTool::ResizeTrackRect(POINT& pt)
{
	if (track_state_ == SIZEALL)
	{
		::OffsetRect(&track_rect_, pt.x, pt.y);
	} 
	else
	{
		if (track_state_ == SIZENW || track_state_ == SIZESW || track_state_ == SIZELEFT)
		{
			if (track_rect_.right - track_rect_.left - pt.x < 100)
			{
				pt.x = track_rect_.right - 100 - track_rect_.left;
				track_rect_.left = track_rect_.right - 100;
			} 
			else
			{
				track_rect_.left += pt.x;
			}
		}
		else if (track_state_ == SIZENE || track_state_ == SIZESE || track_state_ == SIZERIGHT)
		{
			if (track_rect_.right - track_rect_.left + pt.x < 100)
			{
				pt.x = track_rect_.left + 100 - track_rect_.right;
				track_rect_.right = track_rect_.left + 100;
			}
			else
			{
				track_rect_.right += pt.x;
			}
		}
		if (track_state_ == SIZENW || track_state_ == SIZENE || track_state_ == SIZETOP)
		{
			if (track_rect_.bottom - track_rect_.top - pt.y < 100)
			{
				pt.y = track_rect_.bottom - 100 - track_rect_.top;
				track_rect_.top = track_rect_.bottom - 100;
			}
			else
			{
				track_rect_.top += pt.y;
			}
		}
		else if (track_state_ == SIZESW || track_state_ == SIZESE || track_state_ == SIZEBOTTOM)
		{
			if (track_rect_.bottom - track_rect_.top + pt.y < 100)
			{
				pt.y = track_rect_.top + 100 - track_rect_.bottom;
				track_rect_.bottom = track_rect_.top + 100;
			}
			else
			{
				track_rect_.bottom += pt.y;
			}
		}
	}
	ui::UiRect rect_box = track_rect_;
	{
		nbase::NAutoLock auto_lock(&lock_);
		capture_.SetCutMode(true, track_rect_);
	}
	rect_box.Inflate(BORDER_SIZE, BORDER_SIZE);
	rect_box.right = screen_width_ - rect_box.right;
	rect_box.bottom = screen_height_ - rect_box.bottom;
	track_box_->SetMargin(rect_box);
}


LRESULT ScreenCaptureTool::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;
}

bool ScreenCaptureTool::Notify(ui::EventArgs * msg)
{
	return false;
}

bool ScreenCaptureTool::OnClicked(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"1")
	{
	}
	return false;
}
void ScreenCaptureTool::StartCapture(CaptureType type, HWND id, int time_ms)
{
	nbase::NAutoLock auto_lock(&lock_);
	capture_type_ = type;
	if (type == kCaptureTypeScreen || type == kCaptureTypeScreenAndCamera)
	{
		//capture_.SetCutMode(true, { 100, 100, screen_width_, screen_height_ });
		if (id != 0)
		{
			capture_.StartCapture(id, time_ms);
		}
		width_ = kScreenDefWidth;
		height_ = kScreenDefHeight;
	}
	else
	{
		width_ = kCameraDefWidth;
		height_ = kCameraDefHeight;
	}

	capture_timer_.Cancel();
	StdClosure task = nbase::Bind(&ScreenCaptureTool::MakeFrame, this);
	nbase::ThreadManager::PostRepeatedTask(kThreadScreenCapture, capture_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(time_ms));
}
void ScreenCaptureTool::SetCaptureType(CaptureType type)
{
	nbase::NAutoLock auto_lock(&lock_);
	capture_type_ = type;
}
void ScreenCaptureTool::SetCaptureWnd(HWND id, bool cut)
{
	nbase::NAutoLock auto_lock(&lock_);
	if (id != 0)
	{
		capture_.StartCapture(id);
		capture_.SetCutMode(cut, track_rect_);
		ShowWindow(cut, false);
	}
}
void ScreenCaptureTool::SetTextLogo(std::wstring text)
{
	nbase::NAutoLock auto_lock(&lock_);
	yuv_image_list_.clear();
	YuvImage image_text;
	image_text.AddText(text.c_str(), 200, 50, 0xffff00ff, 25, 0, 100, false);
	yuv_image_list_.push_back(image_text);
	std::wstring dir = QPath::GetAppPath();
	dir.append(L"themes\\default\\login\\logo.png");
	YuvImage image0;
	image0.LoadImage(dir.c_str());
	yuv_image_list_.push_back(image0);
	//YuvImage image1;
	//image1.LoadImage(L"D:\\1.gif");
	//yuv_image_list_.push_back(image1);
	//YuvImage image2;
	//image2.LoadImage(L"D:\\2.gif");
	//yuv_image_list_.push_back(image2);
}
void ScreenCaptureTool::StopCapture()
{
	capture_timer_.Cancel();
	nbase::NAutoLock auto_lock(&lock_);
}
void ScreenCaptureTool::MakeFrame()
{
	static char clr_temp = 0x80;
	nbase::NAutoLock auto_lock(&lock_);
	int size = width_ * height_ * 3 / 2;
	std::string data_ret;
	data_ret.append(size, (char)clr_temp);
	std::string data_temp;
	data_temp.append(size, (char)clr_temp);
	bool frame_show = false;
	//取屏
	if (capture_type_ == kCaptureTypeScreen || capture_type_ == kCaptureTypeScreenAndCamera)
	{
		int width = width_;
		int height = height_;
		int64_t time = 0;
		capture_.CustomFrame();
		if (capture_.GetFrame()->GetVideoFrame("", time, (char*)data_temp.c_str(), width, height, false, false))
		{
			CopyI420Data(SIZEALL, data_ret, data_temp, width, height);
			frame_show = true;
		}
	}
	//本地摄像头
	if (capture_type_ == kCaptureTypeCamera || capture_type_ == kCaptureTypeScreenAndCamera)
	{
		int width = width_;
		int height = height_;
		if (capture_type_ == kCaptureTypeScreenAndCamera)
		{
			width = 200;
			height = 200;
		}
		int64_t time = 0;
		if (nim_comp::VideoManager::GetInstance()->video_frame_mng_.GetVideoFrame("", time, (char*)data_temp.c_str(), width, height, false, false))
		{
			//if (1)
			{
				CopyI420Data(capture_type_ == kCaptureTypeCamera ? SIZEALL : SIZENE,
					data_ret, data_temp, width, height);
			} 
			//else
			//{
			//	int left = width_ - width;
			//	left -= left % 2;
			//	int top = 0;
			//	uint8_t* src_y = (uint8_t*)data_temp.c_str();
			//	uint8_t* src_u = src_y + width * height;
			//	uint8_t* src_v = src_u + width * height / 4;
			//	uint8_t* des_y = (uint8_t*)data_ret.c_str();
			//	uint8_t* des_u = des_y + width_ * height_;
			//	uint8_t* des_v = des_u + width_ * height_ / 4;
			//	des_y += left + top * width_;
			//	des_u += left / 2 + top * width_ / 4;
			//	des_v += left / 2 + top * width_ / 4;
			//	I420CopyByA(src_y, width,
			//		src_u, width / 2,
			//		src_v, width / 2,
			//		des_y, width_,
			//		des_u, width_ / 2,
			//		des_v, width_ / 2,
			//		width, height, 200);
			//}

			frame_show = true;
		}
	}
	//对方画面
	{
		//int width = 200;
		//int height = 200;
		//int64_t time = 0;
		//if (nim_comp::VideoManager::GetInstance()->video_frame_mng_.GetVideoFrame("", time, (char*)data_temp.c_str(), width, height, false, false))
		//{
		//	CopyI420Data(SIZESE, data_ret, data_temp, width, height);
		//	frame_show = true;
		//}
	}
	//贴图
	{
		timeb time_now;
		ftime(&time_now); // 秒数
		__int64 cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒
		POINT pt = { 0, 0 };
		for (auto& it : yuv_image_list_)
		{
			ImageFrame* image_frame = it.GetImageFrameEx(cur_timestamp, 0, 0);
			if (image_frame)
			{
				CopyI420Data(SIZENW, data_ret, image_frame->data_, image_frame->width_, image_frame->height_, image_frame->alpha_ ? kYuvDataTypeImageAlpha : kYuvDataTypeImage, pt);
				pt.y += image_frame->height_;
			}
		}
	}
	timeb time_now;
	ftime(&time_now); // 秒数
	__int64 cur_timestamp = time_now.time * 1000 + time_now.millitm; // 毫秒
	if (video_frame_mng_)
	{
		video_frame_mng_->AddVideoFrame(true, cur_timestamp, data_ret.c_str(), size, width_, height_, "", nim_comp::VideoFrameMng::Ft_I420);
	}
}
void ScreenCaptureTool::CopyI420Data(int type, std::string& dest, const std::string& src, int width, int height, ImageYuvDataType image_type, POINT pt)
{
	int left = (width_ - width) / 2;
	left -= left % 2;
	int top = (height_ - height) / 2;
	top -= top % 2;
	switch (type)
	{
	case SIZENW:
	{
		left = pt.x;
		top = pt.y;
		break;
	}
	case SIZENE:
	{
		left = width_ - width - pt.x;
		left -= left % 2;
		top = pt.y;
		break;
	}
	case SIZESW:
	{
		left = pt.x;
		top = height_ - height - pt.y;
		top -= top % 2;
		break;
	}
	case SIZESE:
	{
		left = width_ - width - pt.x;
		left -= left % 2;
		top = height_ - height - pt.y;
		top -= top % 2;
		break;
	}
	default:
		break;
	}
	if (left < 0)
		left = 0;
	if (top < 0)
		top = 0;
	int stride_y = width;
	int stride_u = width / 2;
	int stride_v = width / 2;
	int stride_ya = 0;
	int stride_ua = 0;
	int stride_va = 0;
	if (image_type == kYuvDataTypeImage || image_type == kYuvDataTypeImageAlpha)
	{
		stride_y *= 2;
		stride_u *= 2;
		stride_v *= 2;
		if (image_type == kYuvDataTypeImageAlpha)
		{
			stride_ya = stride_y;
			stride_ua = stride_u;
			stride_va = stride_v;
		}
	}
	uint8_t* src_y = (uint8_t*)src.c_str();
	uint8_t* src_u = src_y + stride_y * height;
	uint8_t* src_v = src_u + stride_u * height / 2;
	uint8_t* des_y = (uint8_t*)dest.c_str();
	uint8_t* des_u = des_y + width_ * height_;
	uint8_t* des_v = des_u + width_ * height_ / 4;
	des_y += left + top * width_;
	des_u += left / 2 + top * width_ / 4;
	des_v += left / 2 + top * width_ / 4;
	I420CopyEx(src_y, stride_y, src_y + width, stride_ya,
		src_u, stride_u, src_u + width / 2, stride_ua,
		src_v, stride_v, src_v + width / 2, stride_va,
		des_y, width_,
		des_u, width_ / 2,
		des_v, width_ / 2,
		width, height);
}

}
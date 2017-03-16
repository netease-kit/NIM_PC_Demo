#include "image_view_form.h"
#include "image_view_manager.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "shared/zoom_image.h"

#define MINWNDWIDTH 700//620
#define MINWNDHEIGHT 500//420

using namespace ui;
using namespace nbase;


LRESULT ImageViewForm::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	POINT point;
	GetCursorPos(&point);
	point_off_ = point;
	ScreenToClientEx(point);

	if (IsOnOtherControl(point))
	{
		return 0;
	}

	if (ui_image_view_->PtInImageView(point))//判断需要移动图片
	{
		pic_moving_ = true;
		SetHitTestCursor(CURSOR_HAND_MOVE);
	}
	return 0;
}

LRESULT ImageViewForm::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (pic_moving_)
	{
		pic_moving_ = false;
	}
	return 0;
}

LRESULT ImageViewForm::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if (pic_moving_)//通知控件调整图片位置
	{
		POINT point;
		GetCursorPos(&point);
		if (point.x == point_off_.x && point.y == point_off_.y)
		{
			return 0;
		}

		ui_image_view_->SetPicOffset(point_off_.x - point.x, point_off_.y - point.y);

		point_off_ = point;
		SetHitTestCursor(CURSOR_HAND_MOVE);
		bHandled = TRUE;
	}
	else
	{
		POINT point;
		GetCursorPos(&point);
		point_off_ = point;
		ScreenToClientEx(point);

		bool close_btn_msg = IsOnOtherControl(point);
		if (!close_btn_msg && ui_image_view_->PtInImageView(point))//判断需要移动图片
		{
			SetHitTestCursor(CURSOR_HAND_IMAGE);
		}
	}
	return 0;
}

LRESULT ImageViewForm::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT point;
	GetCursorPos(&point);
	point_off_ = point;
	ScreenToClientEx(point);

	if (!IsZoomed(m_hWnd))
	{
		RECT rect = button_close_->GetPos();
		if (point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			return HTCLIENT;
		}
	}
	else
	{
		RECT rect = button_close_max_->GetPos();
		if (point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			return HTCLIENT;
		}
	}


	if (ui_image_view_->PtInImageView(point))//判断需要移动图片
	{
		RECT rt = tool_bar_btns_->GetPos();
		if (PtInRect(&rt, point) && !IsZoomed(m_hWnd))
		{
			return WindowImplBase::OnNcHitTest(uMsg, wParam, lParam, bHandled);
		}
		else
		{
			return HTCLIENT;
		}
	}
	else
	{
		return WindowImplBase::OnNcHitTest(uMsg, wParam, lParam, bHandled);
	}
}

LRESULT ImageViewForm::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	short zDelta = (short)HIWORD(wParam);
	if (ui_image_view_->IsVisible())
	{
		ui_image_view_->ScalePic(zDelta > 0 ? UiImageView::kMagnifyPic : UiImageView::kReducePic);
		ShowImageScale(true);
		POINT point;
		GetCursorPos(&point);
		ScreenToClientEx(point);
		if (ui_image_view_->PtInImageView(point))//判断需要移动图片
		{
			SetHitTestCursor(pic_moving_ ? CURSOR_HAND_MOVE : CURSOR_HAND_IMAGE);
		}
		else
		{
			SetHitTestCursor(CURSOR_ARROW);
		}
	}
	AdjustToolbarStatus();
	return 0;
}

LRESULT ImageViewForm::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if (back_image_ == NULL || ui_image_view_ == NULL)
	{
		return 0;
	}

	if (wParam == SIZE_RESTORED)
	{
		RestoredWindowSize();
	}
	if (wParam == SIZE_MAXIMIZED)
	{
		MaxWindowSize();
	}
	return 0;
}

bool ImageViewForm::StartViewPic(std::wstring path_pic, bool size, bool nosave)
{
	if (path_pic.empty())
		return false;

	image_path_ = path_pic;
	button_save_->SetEnabled(!nosave);
	SetMinInfo(MINWNDWIDTH, MINWNDHEIGHT);
	ShowImageScale(false);

	if (IsZoomed(m_hWnd))
	{
		ui_image_view_->SetMargin(ui::UiRect(kBackgroundCorner, kBackgroundCorner, kBackgroundCorner, kBackgroundCorner));
	}
	else
	{
		ui_image_view_->SetMargin(ui::UiRect(0, 0, 0, 0));
	}

	gif_image_->SetVisible(false);
	ui_image_view_->SetVisible(true);
	show_scale_ = true;

	if (nbase::FilePathIsExist(path_pic, false))
	{
		int wnd_width = 0;
		int wnd_height = 0;
		int bg_corner = 0;
		int framecount = ui_image_view_->SetPicPath(path_pic);
		GetDefaultWindowSize(wnd_width, wnd_height, bg_corner);

		if (size)
		{
			int width = wnd_width + bg_corner > MINWNDWIDTH ? wnd_width + bg_corner : MINWNDWIDTH;
			int height = wnd_height + bg_corner > MINWNDHEIGHT ? wnd_height + bg_corner : MINWNDHEIGHT;
			SetPos(ui::UiRect(0, 0, width, height), true, SWP_NOMOVE);
		}

		if (framecount > 1)
		{
			SetMinInfo(wnd_width + bg_corner > MINWNDWIDTH ? wnd_width + bg_corner : MINWNDWIDTH,
				wnd_height + bg_corner > MINWNDHEIGHT ? wnd_height + bg_corner : MINWNDHEIGHT);
			gif_image_->SetBkImage(path_pic.c_str());
			gif_image_->SetFixedWidth(wnd_width);
			gif_image_->SetFixedHeight(wnd_height);

			gif_image_->SetVisible(true);
			ui_image_view_->SetVisible(false);

			//CenterWindow();
			SetToolbarEnable(false);
			return true;
		}

		SetPos(ui::UiRect(0, 0, 0, 0), true, SWP_NOSIZE | SWP_NOMOVE, HWND_NOTOPMOST);
	}
	else
	{
		show_scale_ = false;
		SetPos(ui::UiRect(0, 0, 0, 0), true, SWP_NOMOVE, HWND_NOTOPMOST);
	}

	CheckNextOrPreImageBtnStatus();
	return true;
}

void ImageViewForm::ShowImageScale(bool show)
{
	if (show)
	{
		hide_scale_timer_weakflag_.Cancel();
		StdClosure cb = nbase::Bind(&ImageViewForm::ShowImageScale, this, false);
		auto weak_cb = hide_scale_timer_weakflag_.ToWeakCallback(cb);
		nbase::ThreadManager::PostDelayedTask(weak_cb, nbase::TimeDelta::FromSeconds(2));


		std::wstring size;
		int size_hum = int(ui_image_view_->GetScale() * 100);
		if (ui_image_view_->IsAdjustScale())//大于100需要调整
		{
			if (size_hum % 10 >= 5)
			{
				size_hum = size_hum / 10 * 10 + 10;
			}
			else
			{
				size_hum = size_hum / 10 * 10;
			}
		}

		//如果是打开了新的图片，而且图片没有缩放，就不显示缩放信息
		if (size_hum == 100 && show_scale_ == true)
		{
			return;
		}

		if (size_hum != 100 && ui_image_view_->IsVisible())//不可显示表示为gif
		{
			button_normal_->SetEnabled(true);
		}
		else
		{
			button_normal_->SetEnabled(false);
		}

		size = nbase::StringPrintf(_T("%d%%"), size_hum);
		scale_info_->SetText(size.c_str());

		scale_info_bg_->SetVisible(true);
	}
	else
	{
		scale_info_bg_->SetVisible(false);
	}

}

void ImageViewForm::SetToolbarEnable(bool eable)
{
	button_normal_->SetEnabled(eable);
	button_magnify_->SetEnabled(eable);
	button_reduce_->SetEnabled(eable);
	button_rotate_->SetEnabled(eable);
	button_max_->SetEnabled(eable);
	button_restore_->SetEnabled(eable);
}

void ImageViewForm::AdjustToolbarStatus()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClientEx(point);

	RECT rect_client;
	GetClientRect(m_hWnd, &rect_client);
	bool show_btn = ::PtInRect(&rect_client, point) == TRUE;

	if (show_nextorpre_btn_)
	{
		button_nextimage_->SetVisible(show_btn);
		button_preimage_->SetVisible(show_btn);
	}
	if (show_btn)
	{
		float fScale = ui_image_view_->GetScale();

		if (fScale != 1.0f && ui_image_view_->IsVisible())//不可显示表示为gif
		{
			button_normal_->SetEnabled(true);
		}
		else
		{
			button_normal_->SetEnabled(false);
		}

		if (IsZoomed(m_hWnd))
		{
			button_max_->SetVisible(false);
			button_restore_->SetVisible(true);
		}
		else
		{
			button_max_->SetVisible(true);
			button_restore_->SetVisible(false);
		}
	}

	tool_bar_->SetVisible(show_btn);
}

void ImageViewForm::SetHitTestCursor(CursorState hit_test)
{
	HCURSOR cursor = NULL;
	switch (hit_test)
	{
	case CURSOR_HAND_IMAGE:
		cursor = LoadCursor(NULL, IDC_SIZEALL);
		break;
	case CURSOR_HAND_MOVE:
		cursor = LoadCursor(NULL, IDC_HAND);
		break;
	default:
		cursor = LoadCursor(NULL, IDC_ARROW);
		break;
	}

	SetCursor(cursor);
}

bool ImageViewForm::IsOnOtherControl(POINT pt)
{
	std::vector<Control*> control_list;
	control_list.push_back(button_close_);
	control_list.push_back(button_close_max_);
	control_list.push_back(tool_bar_btns_);
	control_list.push_back(button_nextimage_);
	control_list.push_back(button_preimage_);
	for (int i=0;i<(int)control_list.size();i++)
	{
		Control* control = control_list.at(i);
		if (control && control->IsVisible())
		{
			RECT rect = control->GetPos();
			if (PtInRect(&rect,pt))
			{
				return true;
			}
		}
	}
	return false;
}

void ImageViewForm::CheckNextOrPreImageBtnStatus()
{
	bool next = ImageViewManager::GetInstance()->IsExistNextImage(true);
	bool pre = ImageViewManager::GetInstance()->IsExistNextImage(false);

	button_nextimage_->SetEnabled(next);
	std::wstring tip;
	if (!next)
	{
		ui::MutiLanSupport *multilan = ui::MutiLanSupport::GetInstance();
		if (multilan)
		{
			tip = multilan->GetStringViaID(L"STRID_IMAGEVIEWFORM_LAST");
		}
	}
	button_nextimage_->SetToolTipText(tip);

	button_preimage_->SetEnabled(pre);
	if (!pre)
	{
		ui::MutiLanSupport *multilan = ui::MutiLanSupport::GetInstance();
		if (multilan)
		{
			tip = multilan->GetStringViaID(L"STRID_IMAGEVIEWFORM_FIRST");
		}
	}
	button_preimage_->SetToolTipText(tip);

	UpdateImagePage();
	AdjustToolbarStatus();
}

void ImageViewForm::SetNextOrPreImageBtnVisible(bool show)
{
	if (!show)
	{
		button_nextimage_->SetVisible(false);
		button_preimage_->SetVisible(false);
	}
	show_nextorpre_btn_ = show;
	AdjustToolbarStatus();
}

void ImageViewForm::UpdateImagePage()
{
	long sum = 0;
	long cur = 0;
	ImageViewManager::GetInstance()->GetImagePage(cur, sum);
	if (sum > 1 && cur > 0)
	{
		std::wstring num;
		nbase::StringPrintf(num, L"%d / %d", cur, sum);
		num_info_->SetText(num);
		num_tip_bg_->SetVisible();
	}
	else
	{
		num_tip_bg_->SetVisible(false);
	}
}


void ImageViewForm::MaxWindowSize()
{
	back_image_->SetBkImage(kMaxWindowBg);
	ui_image_view_->SetMargin(ui::UiRect(0, 0, 0, 0));
	SetPos(ui::UiRect(0, 0, 0, 0), true, SWP_NOSIZE | SWP_NOMOVE, HWND_TOPMOST);

	button_max_->SetVisible(false);
	button_restore_->SetVisible(true);
	button_close_->SetVisible(false);
	button_close_max_->SetVisible(true);

}
void ImageViewForm::RestoredWindowSize()
{
	back_image_->SetBkImage(kRestoredWindowBg);
	ui_image_view_->SetMargin(ui::UiRect(kBackgroundCorner, kBackgroundCorner, kBackgroundCorner, kBackgroundCorner));
	SetPos(ui::UiRect(0, 0, 0, 0), true, SWP_NOSIZE | SWP_NOMOVE, HWND_NOTOPMOST);

	button_max_->SetVisible(true);
	button_restore_->SetVisible(false);
	button_close_->SetVisible(true);
	button_close_max_->SetVisible(false);

}

void ImageViewForm::ScreenToClientEx(POINT &pt)
{
	RECT rt;
	GetWindowRect(m_hWnd, &rt);
	pt.x -= rt.left;
	pt.y -= rt.top;
}

bool ImageViewForm::GetDefaultWindowSize(int &wnd_width, int &wnd_height, int &bg_corner)
{
	int image_width = ui_image_view_->GetImageWidth();
	int image_height = ui_image_view_->GetImageHeight();

	bg_corner = 24;
	if (IsZoomed(m_hWnd))
	{
		bg_corner = 0;
	}

	RECT rect_workarea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect_workarea, 0);
	int cxScreenWork = rect_workarea.right - rect_workarea.left;
	int cyScreenWork = rect_workarea.bottom - rect_workarea.top;

	float fScale = ui_image_view_->GetDefaultScale(cxScreenWork - bg_corner, cyScreenWork - bg_corner, image_width, image_height);
	wnd_width = int(image_width * fScale);
	wnd_height = int(image_height * fScale);

	if (wnd_width > cxScreenWork - bg_corner)
		wnd_width = cxScreenWork - bg_corner;
	if (wnd_height > cyScreenWork - bg_corner)
		wnd_height = cyScreenWork - bg_corner;
	return true;
}

void ImageViewForm::GetMimeType(const std::wstring& filepath, std::wstring& mime_type_out, std::wstring& exten_out)
{
	using namespace Gdiplus;

	if (filepath.empty())
	{
		return;
	}
	Gdiplus::Image image_src(filepath.c_str());
	if (image_src.GetLastStatus() != Ok)
	{
		return;
	}
	GUID guid;
	if (image_src.GetRawFormat(&guid) != Ok)
	{
		return;
	}
	mime_type_out = GetMimeType(guid);
	if (mime_type_out.empty())
	{
		return;
	}
	if (mime_type_out == kImageGIF)
	{
		exten_out = L".gif";
	}
	else if (mime_type_out == kImageJPEG)
	{
		exten_out = L".jpg";
	}
	else if (mime_type_out == kImagePNG)
	{
		exten_out = L".png";
	}
	else if (mime_type_out == kImageBMP)
	{
		exten_out = L".bmp";
	}
}

std::wstring ImageViewForm::GetMimeType(GUID& clsid)
{
	using namespace Gdiplus;
	std::wstring mime_type;
	if (clsid == ImageFormatGIF) //不支持GIF文件;
		mime_type = kImageGIF;
	else if (clsid == ImageFormatJPEG)
		mime_type = kImageJPEG;
	else if (clsid == ImageFormatPNG)
		mime_type = kImagePNG;
	else if (clsid == ImageFormatBMP)
		mime_type = kImageBMP;
	return mime_type;
}

LPCTSTR ImageViewForm::kClassName = L"ImageViewForm";
const int ImageViewForm::kBackgroundCorner = 12;
LPCTSTR ImageViewForm::kRestoredWindowBg = L"file='win_bg.png' corner='30,40,50,40'";
LPCTSTR ImageViewForm::kMaxWindowBg = L"file='win_bg.png' corner='30,40,50,40' source='13,13,184,184'";
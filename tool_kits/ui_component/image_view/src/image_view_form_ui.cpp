#include "image_view_form.h"
#include "image_view_manager.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "shared/zoom_image.h"


using namespace ui;
using namespace nbase;


ImageViewForm::ImageViewForm(void) :ui_image_view_(NULL), back_image_(NULL), pic_moving_(false), show_scale_(false)
{
	point_off_.x = 0;
	point_off_.y = 0;
	show_nextorpre_btn_ = false;	//TODO:(zhuhongshuyu)现在不让上一个/下一个按钮默认显示,云信增加相关功能后，开启此项
}


ImageViewForm::~ImageViewForm(void)
{
	ImageViewManager::GetInstance()->ResetImageViewWndPoint();
}

std::wstring ImageViewForm::GetSkinFolder()
{
	return L"imageview";
}

std::wstring ImageViewForm::GetSkinFile()
{
	return L"imageview.xml";
}

ui::UILIB_RESOURCETYPE ImageViewForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring ImageViewForm::GetZIPFileName() const
{
	return (L"imageview.zip");
}

void ImageViewForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&ImageViewForm::Notify, this, std::placeholders::_1));

	LONG styleValue = ::GetWindowLong(GetHWND(), GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(GetHWND(), GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	back_image_ = static_cast<Box *>(FindControl(L"back_image"));
	gif_image_ = static_cast<Control *>(FindControl(L"gif_image"));
	ui_image_view_ = static_cast<UiImageView*>(FindControl(L"image_view"));

	button_close_ = static_cast<Button *>(FindControl(L"close_btn"));
	button_close_max_ = static_cast<Button *>(FindControl(L"close_btn_max"));
	scale_info_bg_ = static_cast<Box *>(FindControl(L"scale_info_bg"));
	scale_info_ = static_cast<Label *>(FindControl(L"scale_info"));
	button_nextimage_ = static_cast<Button *>(FindControl(L"nextimage_btn"));
	button_preimage_ = static_cast<Button *>(FindControl(L"preimage_btn"));

	tool_bar_ = static_cast<HBox *>(FindControl(L"tool_bar"));
	tool_bar_btns_ = static_cast<HBox *>(FindControl(L"tool_bar_btns"));
	num_tip_bg_ = static_cast<Box *>(FindControl(L"num_tip_bg"));
	num_info_ = static_cast<Label *>(FindControl(L"num_info"));
	button_normal_ = static_cast<Button *>(FindControl(L"normal_btn"));
	button_max_ = static_cast<Button *>(FindControl(L"max_btn"));
	button_restore_ = static_cast<Button *>(FindControl(L"restore_btn"));
	button_magnify_ = static_cast<Button *>(FindControl(L"magnify_btn"));
	button_reduce_ = static_cast<Button *>(FindControl(L"reduce_btn"));
	button_rotate_ = static_cast<Button *>(FindControl(L"rotate_btn"));
	button_save_ = static_cast<Button *>(FindControl(L"save_btn"));
	ASSERT(back_image_ && gif_image_ && ui_image_view_ &&
		button_close_ && button_close_max_ && scale_info_bg_ && scale_info_ && button_nextimage_ && button_preimage_ &&
		tool_bar_ && tool_bar_btns_ && num_tip_bg_ && num_info_ &&
		button_normal_ && button_max_ && button_restore_ && button_magnify_ && button_reduce_ && button_rotate_ && button_save_);


	button_close_max_->SetVisible(false);
}

ui::Control* ImageViewForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == UiImageView::kControlName)
	{
		return new UiImageView();
	}

	return NULL;
}

LRESULT ImageViewForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			this->Close();
		}
		break;
		case VK_LEFT:
		{
			ImageViewManager::GetInstance()->ShowNextImageView(false);
		}
		break;
		case VK_RIGHT:
		{
			ImageViewManager::GetInstance()->ShowNextImageView(true);
		}
		break;
		default:
			break;
		}
	}
	else if (uMsg == WM_CLOSE)
	{
		if (!::IsWindowEnabled(m_hWnd))
		{
			::SetForegroundWindow(m_hWnd);
			return FALSE;
		}
	}
	else if (uMsg == WM_KILLFOCUS)
	{
		KillFocus();
	}
	else if (WM_PAINT == uMsg)
	{
		LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);
		if (show_scale_ && ui_image_view_->IsVisible())
		{
			ShowImageScale(true);
			show_scale_ = false;
		}
		return lRes;
	}
	else if (WM_NCMOUSEMOVE == uMsg)
	{
		AdjustToolbarStatus();
	}
	else if (WM_NCMOUSELEAVE == uMsg)
	{
		AdjustToolbarStatus();
	}
	else if (WM_NCLBUTTONDBLCLK == uMsg)
	{
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool ImageViewForm::Notify(ui::EventArgs* msg)
{
	if (msg->Type == kEventClick)
	{
		std::wstring control_name = msg->pSender->GetName();

		if (control_name == L"close_btn")
		{
			this->Close();
		}
		else if (control_name == L"close_btn_max")
		{
			this->Close();
		}
		else if (msg->pSender->GetName() == L"preimage_btn")
		{
			ImageViewManager::GetInstance()->ShowNextImageView(false);
		}
		else if (msg->pSender->GetName() == L"nextimage_btn")
		{
			ImageViewManager::GetInstance()->ShowNextImageView(true);
		}
		else if (control_name == L"normal_btn")
		{
			ui_image_view_->SetOriginalSize();
			ShowImageScale(true);
		}
		else if (control_name == L"max_btn")
		{
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		}
		else if (control_name == L"restore_btn")
		{
			::ShowWindow(m_hWnd, SW_RESTORE);
		}
		else if (control_name == L"magnify_btn")
		{
			ui_image_view_->ScalePic(UiImageView::kMagnifyPic);
			ShowImageScale(true);
		}
		else if (control_name == L"reduce_btn")
		{
			ui_image_view_->ScalePic(UiImageView::kReducePic);
			ShowImageScale(true);
		}
		else if (control_name == L"rotate_btn")
		{
			ui_image_view_->RotatePic(UiImageView::kClockWise);
		}
		else if (msg->pSender->GetName() == L"save_btn")
		{
			if (!image_path_.empty())
			{
				std::wstring extension;
				nbase::FilePathExtension(image_path_, extension);
				if (extension.empty())
				{
					std::wstring mime_type;
					GetMimeType(image_path_, mime_type, extension);
				}
				nbase::Time::TimeStruct now = nbase::Time::Now().ToTimeStruct(true);
				std::wstring file_name = nbase::StringPrintf(L"IMG%04d%02d%02d_%02d%02d%02d", now.year(), now.month(), now.day_of_month(),
					now.hour(), now.minute(), now.second());
				file_name += extension;

				std::wstring  file_type = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_PIC_FILE");
				std::wstring info = nbase::StringPrintf(L"%s(*%s)", file_type.c_str(), extension.c_str());
				CFileDialogEx* fileDlg = new CFileDialogEx();
				std::wstring filter = L"*" + extension;
				std::map<LPCTSTR, LPCTSTR> filters;
				filters[info.c_str()] = filter.c_str();
				fileDlg->SetFilter(filters);
				fileDlg->SetFileName(file_name.c_str());
				fileDlg->SetParentWnd(m_hWnd);
				CFileDialogEx::FileDialogCallback2 callback2 = nbase::Bind(&ImageViewForm::SaveImageDlgCallback, this, std::placeholders::_1, std::placeholders::_2);
				fileDlg->AyncShowSaveFileDlg(callback2);
			}
		}
	}

	return true;
}

void ImageViewForm::SaveImageDlgCallback(BOOL ret, std::wstring file_path_save)
{
	if (ret)
	{
		nbase::CopyFile(image_path_, file_path_save);
	}
}

void ImageViewForm::ActiveWindow()
{
	if (::IsWindow(m_hWnd))
	{
		if (::IsIconic(m_hWnd))
			::ShowWindow(m_hWnd, SW_RESTORE);
		else
			::SetForegroundWindow(m_hWnd);
	}
}
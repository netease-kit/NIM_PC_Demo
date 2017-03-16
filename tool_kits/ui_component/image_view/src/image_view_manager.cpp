#include "image_view_manager.h"

using namespace nbase;


ImageViewManager::ImageViewManager() 
	: image_view_window_(NULL)
	, message_id_(L"")
{

}

ImageViewManager::~ImageViewManager()
{
	if (image_view_window_)
	{
		image_view_window_ = NULL;
	}
}

bool ImageViewManager::StartViewPic(const std::wstring &path_pic, std::wstring message_id, bool size, bool nosave)
{
	if (!image_view_window_)
	{
		image_view_window_ = new ImageViewForm();
		image_view_window_->Create(NULL, _T(""), WS_OVERLAPPEDWINDOW, 0L);
	}
	message_id_ = message_id;
	image_view_window_->StartViewPic(path_pic, size, nosave);

	if (!IsWindowVisible(image_view_window_->GetHWND()))
	{
		image_view_window_->ShowWindow(true);
		image_view_window_->CenterWindow();
	}
	::BringWindowToTop(image_view_window_->GetHWND());
	return true;
}

bool ImageViewManager::JudgeViewImage(const std::wstring &form_id)
{
	bool bRet = false;
	if (image_view_window_)
	{
		if (form_id.compare(image_view_window_->GetWindowClassName()) == 0)
		{
			bRet = true;
		}
	}
	return bRet;
}

void ImageViewManager::ResetImageViewWndPoint(void)
{
	image_view_window_ = NULL;
}

bool ImageViewManager::SetImageViewList(ImageViewInfoList info_list, std::wstring message_id)
{
	if (image_view_window_)
	{
		if (message_id == message_id_)
		{
			image_view_info_list_ = info_list;
			image_view_window_->CenterWindow();
			image_view_window_->CheckNextOrPreImageBtnStatus();
			image_view_window_->SetNextOrPreImageBtnVisible(info_list.size() > 1);
			return true;
		}
	}
	return false;
}

bool ImageViewManager::ShowNextImageView(bool next)
{
	for (int i=0; i<(int)image_view_info_list_.size(); ++i)
	{
		ImageViewInfo info = image_view_info_list_.at(i);
		if (info.id_ == message_id_)
		{
			if (next)
			{
				i++;
			} 
			else
			{
				i--;
			}

			if (i >= 0 && i < (int)image_view_info_list_.size())
			{
				info = image_view_info_list_.at(i);

				if( nbase::FilePathIsExist(info.path_, false) || info.url_.empty() )
				{
					StartViewPic(info.path_, info.id_, false);
				}
				else
				{
					//std::string path, url;
					//nbase::UTF16ToUTF8(info.path_, path);
					//nbase::UTF16ToUTF8(info.url_, url);
					//ViewImage(url, path, info.id_, false);
				}

				return true;
			}

		}
	}
	return false;
}

bool ImageViewManager::IsExistNextImage(bool next)
{

	for (int i = 0; i < (int)image_view_info_list_.size(); ++i)
	{
		ImageViewInfo info = image_view_info_list_.at(i);
		if (info.id_ == message_id_)
		{
			if (next)
				i++;
			else
				i--;

			if (i >= 0 && i < (int)image_view_info_list_.size())
				return true;

		}
	}
	return false;
}

void ImageViewManager::GetImagePage(long& cur, long& sum)
{
	cur = 0;
	sum = image_view_info_list_.size();
	for (long i=0; i<sum; ++i)
	{
		ImageViewInfo info = image_view_info_list_.at(i);
		if (info.id_ == message_id_)
		{
			cur = i+1;
			return;
		}
	}
}

void ImageViewManager::OnImageReady(std::wstring &path_pic, bool size)
{
	if (!image_view_window_)
		return;

	if (image_view_window_->CurrentImagePath() == path_pic)
	{
		StartViewPic(path_pic, L"", size);
		image_view_window_->CenterWindow();
	}
}

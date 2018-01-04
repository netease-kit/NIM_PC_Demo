#include "bubble_video.h"
#include "image_view/src/image_view_manager.h"
#include "util/user.h"
#include "shared/zoom_image.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleVideo::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	msg_video_ = new ButtonBox;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_video_, L"session/video_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_video_, L"session/video_left.xml");
	bubble_box_->Add( msg_video_ );

	image_ = msg_video_->FindSubControl(L"image");

	msg_video_->AttachClick(nbase::Bind(&MsgBubbleVideo::OnClicked, this, std::placeholders::_1));
	msg_video_->AttachMenu(nbase::Bind(&MsgBubbleVideo::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleVideo::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);
	thumb_checked_ = false;
	InitResPath();
	CheckThumbImageBubble();
}
void MsgBubbleVideo::InitResPath()
{
	std::wstring wpath = nbase::UTF8ToUTF16(msg_.local_res_path_);
	std::string path = nim::Talk::GetAttachmentPathFromMsg(msg_);
	ASSERT(!path.empty());

	if (wpath.empty() || !nbase::FilePathIsExist(wpath, false))
	{
		path_ = nbase::UTF8ToUTF16(path);
		std::wstring directory, filename;
		nbase::FilePathApartDirectory(path_, directory);
		nbase::FilePathApartFileName(path_, filename);
		thumb_ = directory + L"thumb_" + filename;
	}
	else
	{
		std::wstring directory, filename;
		nbase::FilePathApartDirectory(nbase::UTF8ToUTF16(path), directory);
		nbase::FilePathApartFileName(wpath, filename);
		thumb_ = directory + L"thumb_" + filename;
		path_ = wpath;
	}
}
bool MsgBubbleVideo::CheckThumbImageBubble()
{
	if (thumb_checked_)
		return true;

	if (thumb_.empty() || !nbase::FilePathIsExist(thumb_, false))
		return false;
	
	Gdiplus::Image thumb_image(thumb_.c_str());
	Gdiplus::Status status = thumb_image.GetLastStatus();
	if (status != Gdiplus::Ok) //图片有错误
	{
		QLOG_ERR(L"Image {0} error {1}") << thumb_ << status;
		return false;
	}	
	image_->SetBkImage(thumb_);
	//image_->SetFixedHeight(150);
	//image_->SetFixedWidth(150);
	thumb_checked_ = true;
	return true;
}
bool MsgBubbleVideo::OnClicked( ui::EventArgs* arg )
{
	if( nbase::FilePathIsExist(path_, false) )
	{
		ImageViewManager::GetInstance()->StartViewPic(path_, L"", true);
	}
	return true;
}

bool MsgBubbleVideo::OnMenu( ui::EventArgs* arg )
{
	PopupMenu(false, true);
	return false;
}

bool MsgBubbleVideo::NeedDownloadResource()
{
	if( nbase::FilePathIsExist(path_, false) )
		return false;
	else
		return true;
}
void MsgBubbleVideo::OnDownloadCallback(bool success, const std::string& file_path)
{
	ASSERT(!file_path.empty());
	std::wstring wpath = nbase::UTF8ToUTF16(file_path);
	if (wpath == thumb_) //下载的是缩略图
	{
		if (success)
		{
			CheckThumbImageBubble();			
		}		
	}
	else if (wpath == path_) //下载的是源资源
	{
		if (success)
		{
		}
		else
		{			
		}
	}
}
}
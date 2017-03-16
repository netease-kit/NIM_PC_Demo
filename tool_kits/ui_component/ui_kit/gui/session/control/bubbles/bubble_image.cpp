#include "bubble_image.h"
#include "image_view/src/image_view_manager.h"
#include "util/user.h"
#include "shared/zoom_image.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleImage::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	msg_image_ = new ButtonBox;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_image_, L"session/image_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_image_, L"session/image_left.xml");
	bubble_box_->Add( msg_image_ );

	image_ = msg_image_->FindSubControl(L"image");

	msg_image_->AttachClick(nbase::Bind(&MsgBubbleImage::OnClicked, this, std::placeholders::_1));
	msg_image_->AttachMenu(nbase::Bind(&MsgBubbleImage::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleImage::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	SetCanView(false);
	InitResPath();

	if (nbase::FilePathIsExist(thumb_, false)) //thumb_图片存在
	{
		if (CheckImageBubble())
		{
			SetLoadStatus(RS_LOAD_OK);
			SetCanView(true);
		}
		else //图片有错误
			SetLoadStatus(RS_LOAD_NO);
	}
	else if (nbase::FilePathIsExist(path_, false)) //缩略图不存在，而原图存在
	{
		DoZoom();
	}
	else //thumb_图片和原图都不存在
	{
		if (msg_.rescode_ == 0) //读取消息历史
		{
			SetLoadStatus(RS_LOADING);
		}
		else //接收
		{
			if (msg_.rescode_ == nim::kNIMResSuccess
				|| msg_.rescode_ == nim::kNIMLocalResExist)
				SetLoadStatus(RS_LOADING);
			else if (msg_.rescode_ == nim::kNIMLocalResParameterError)
				SetLoadStatus(RS_LOAD_NO);
			else
			{
				SetLoadStatus(RS_LOAD_NO);
				QLOG_WAR(L"unknown receive msg code {0}") << msg_.rescode_;
			}
		}
	}
}

bool MsgBubbleImage::OnClicked( ui::EventArgs* arg )
{
	if( nbase::FilePathIsExist(path_, false) )
	{
		ImageViewManager::GetInstance()->StartViewPic(path_, L"", true);
	}
	else //原图不存在
	{
		ImageViewManager::GetInstance()->StartViewPic(path_, L"", true);
		nim::NOS::DownloadMediaCallback cb = [](nim::NIMResCode res_code, const std::string& file_path, const std::string&, const std::string&)
		{
			if (res_code == nim::kNIMResSuccess)
			{
				ImageViewManager* image_view_mgr = ImageViewManager::GetInstance();
				if (image_view_mgr)
					image_view_mgr->OnImageReady(nbase::UTF8ToUTF16(file_path), true);
			}
		};
		nim::NOS::FetchMedia(msg_, cb, nim::NOS::ProgressCallback());
	}
	return true;
}

void MsgBubbleImage::InitResPath()
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

bool MsgBubbleImage::CheckImageBubble()
{
	if (image_checked_)
		return true;

	if (thumb_.empty() || !nbase::FilePathIsExist(thumb_, false))
		return false;

	//gif图片经过压缩下载后，虽然格式仍是gif，但变成只有一帧，没有动画，尺寸也可能变小了。
	Gdiplus::Image thumb_image(thumb_.c_str());
	Gdiplus::Status status = thumb_image.GetLastStatus();
	if (status != Gdiplus::Ok) //图片有错误
	{
		QLOG_ERR(L"Image {0} error {1}") << thumb_ << status;
		return false;
	}

	int width = thumb_image.GetWidth();
	int height = thumb_image.GetHeight();
	if (width == 0 || height == 0)
	{
		QLOG_ERR(L"Get image size error: {0}, {1}.") << width << height;
		return false;
	}
	
	image_->SetFixedWidth(width, false); //压缩下载的图片直接做消息气泡的图片
	image_->SetFixedHeight(height);
	image_->SetBkImage(thumb_);
	image_checked_ = true;

	return true;
}

void MsgBubbleImage::SetCanView(bool can)
{
	msg_image_->SetEnabled(can);
}

void MsgBubbleImage::OnDownloadCallback( bool success, const std::string& file_path )
{
	if (image_checked_)
		return;

	ASSERT(!file_path.empty());
	std::wstring wpath = nbase::UTF8ToUTF16(file_path);
	if (wpath == thumb_) //下载的是缩略图
	{
		if (success)
		{
			ListBox* lbx = dynamic_cast<ListBox*>(this->GetParent());
			ASSERT(lbx);
			bool at_end = lbx->IsAtEnd();

			SetLoadStatus(RS_LOAD_OK);
			CheckImageBubble();
			SetCanView(true);
			
			if (at_end)
				lbx->EndDown(true, false);
		}
		else
		{
			SetLoadStatus(RS_LOAD_NO);
		}
	}
	else if (wpath == path_) //下载的是原图
	{
		if (success)
		{
			ListBox* lbx = dynamic_cast<ListBox*>(this->GetParent());
			ASSERT(lbx);
			bool at_end = lbx->IsAtEnd();

			SetLoadStatus(RS_LOAD_OK);

			DoZoom();

			if (at_end)
				lbx->EndDown(true, false);
		}
		else
		{
			SetLoadStatus(RS_LOAD_NO);
		}
	}
}

void MsgBubbleImage::DoZoom()
{
	if (image_checked_)
		return;

	if( !nbase::FilePathIsExist(path_, false) )
	{
		InitResPath();
		if (!nbase::FilePathIsExist(path_, false))
		{
			QLOG_ERR(L"Image not exist: {0}") << path_;
			return;
		}
	}

	const int cx = 270, cy = 180, xf = 78 - 5, yf = 57 - 5;

	std::wstring mime_type = ZoomImage::GetMimeType(path_);
	if( mime_type.empty() )
	{
		QLOG_ERR(L"Image wrong: {0}") <<path_;
		return;
	}
	SetCanView(true);

	SIZE sz = { 0, 0 };
	bool resize = CalculateImageSize(path_, sz, cx, cy);

	image_->SetFixedWidth(sz.cx, false);
	image_->SetFixedHeight(sz.cy);

	if(mime_type == kImageGIF)
	{
		image_->SetBkImage(path_);
	}
	else
	{
		if(resize)
		{
			ZoomImageF(path_, thumb_, sz.cx, sz.cy);
			image_->SetBkImage(thumb_);
		}
		else
		{
			nbase::CopyFile(path_, thumb_);
			image_->SetBkImage(thumb_);
			if(sz.cx < xf && sz.cy < yf)
			{
				image_->SetForeStateImage(kControlStateNormal, L"");
			}
		}
	}
	image_checked_ = true;
}

bool MsgBubbleImage::OnMenu( ui::EventArgs* arg )
{
	bool show_retweet = true;
	if (msg_.type_ == nim::kNIMMessageTypeCustom)
	{
		Json::Value json;
		if (StringToJson(msg_.attach_, json))
		{
			int sub_type = json["type"].asInt();
			if (sub_type == CustomMsgType_SnapChat)
				show_retweet = false;
		}
	}
	PopupMenu(false, true, show_retweet);
	return false;
}

bool MsgBubbleImage::NeedDownloadResource()
{
	//if( nbase::FilePathIsExist(thumb_, false) )
		return false;
	//else
	//	return true;
}
}
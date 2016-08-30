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

	if( nbase::FilePathIsExist(path_, false) )
	{
		DoZoom();
	}
	else
	{
		if(msg_.rescode_ == 0) //读取消息历史
		{
			SetLoadStatus(RS_LOADING);
		}
		else //接收
		{
			if (msg_.rescode_ == nim::kNIMResSuccess)
				SetLoadStatus(RS_LOADING);
			else if (msg_.rescode_ == nim::kNIMLocalResParameterError)
				SetLoadStatus(RS_LOAD_NO);
			else if (msg_.rescode_ == nim::kNIMLocalResExist)
				DoZoom();
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
	return true;
}

void MsgBubbleImage::InitResPath()
{
	std::wstring wpath = nbase::UTF8ToUTF16(msg_.local_res_path_);

	if (wpath.empty() || !nbase::FilePathIsExist(wpath, false))
	{
		nim::IMImage img;
		nim::Talk::ParseImageMessageAttach(msg_, img);
		std::wstring filename = nbase::UTF8ToUTF16(img.md5_);
		thumb_ = GetUserImagePath() + L"thumb_" + filename;
		path_ = GetUserImagePath() + filename;

	}
	else
	{
		std::wstring filename;
		nbase::FilePathApartFileName(wpath, filename);
		thumb_ = GetUserImagePath() + L"thumb_" + filename;
		path_ = wpath;
	}
}

void MsgBubbleImage::SetCanView(bool can)
{
	msg_image_->SetEnabled(can);
}

void MsgBubbleImage::OnDownloadCallback( bool success )
{
	if( success )
	{
		SetLoadStatus(RS_LOAD_OK);

		ListBox* lbx = dynamic_cast<ListBox*>( this->GetParent() );
		ASSERT(lbx);
		bool at_end = lbx->IsAtEnd();

		DoZoom();

		if(at_end)
			lbx->EndDown(true, false);
	}
	else
	{
		SetLoadStatus(RS_RELOAD);
	}
}

void MsgBubbleImage::DoZoom()
{
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
			ZoomImageF(path_, thumb_, cx, cy);
			image_->SetBkImage(thumb_);
		}
		else
		{
			image_->SetBkImage(path_);
			if(sz.cx < xf && sz.cy < yf)
			{
				image_->SetForeStateImage(kControlStateNormal, L"");
			}
		}
	}
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
	if( nbase::FilePathIsExist(path_, false) )
		return false;
	else
		return true;
}
}
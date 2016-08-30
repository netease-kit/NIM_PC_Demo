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
}
#include "bubble_location.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleLocation::InitControl(bool bubble_right)
{
	lat_ = 0.0f;
	lng_ = 0.0f;

	__super::InitControl(bubble_right);

	msg_location_ = new ButtonBox;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_location_, L"session/location_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_location_, L"session/location_left.xml");
	bubble_box_->Add(msg_location_);

	msg_location_->AttachClick(nbase::Bind(&MsgBubbleLocation::OnClicked, this, std::placeholders::_1));
	msg_location_->AttachMenu(nbase::Bind(&MsgBubbleLocation::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleLocation::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	nim::IMLocation location;
	nim::Talk::ParseLocationMessageAttach(msg, location);
	lat_ = location.latitude_;
	lng_ = location.longitude_;
	title_ = location.description_;
	msg_location_->SetUTF8Text(title_);
}

bool MsgBubbleLocation::OnClicked( ui::EventArgs* arg )
{
	if( !title_.empty() )
	{
		//默认放大倍数：15
		std::string point = nbase::StringPrintf("%lf,%lf,15", lat_, lng_);
		Post2GlobalMisc( nbase::Bind(&OpenMap, title_, "", point));
	}
	return true;
}


bool MsgBubbleLocation::OnMenu( ui::EventArgs* arg )
{
	PopupMenu(false, true);
	return false;
}
}
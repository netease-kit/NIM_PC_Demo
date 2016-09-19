#include "bubble_finger.h"


using namespace ui;

namespace nim_comp
{
void MsgBubbleFinger::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	msg_finger_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_finger_, L"session/finger_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_finger_, L"session/finger_left.xml");
	bubble_box_->Add(msg_finger_);

	finger_ = this->FindSubControl(L"finger");

	msg_finger_->AttachMenu(nbase::Bind(&MsgBubbleFinger::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleFinger::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	Json::Value json;
	if( StringToJson(msg.attach_, json) )
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Jsb && json["data"].isObject()) //finger
		{
			int v = json["data"]["value"].asInt();
			if(v == 1)
				finger_->SetBkImage(L"jsb_s.png");
			else if(v == 2)
				finger_->SetBkImage(L"jsb_j.png");
			else if(v == 3)
				finger_->SetBkImage(L"jsb_b.png");
			return;
		}
	}
	QLOG_WAR(L"user type msg undefine, attach={0}") << msg.attach_;
	
	finger_->SetBkImage(L"jsb.png");
}

bool MsgBubbleFinger::OnMenu( ui::EventArgs* arg )
{
	PopupMenu(false, true, false);
	return false;
}
}
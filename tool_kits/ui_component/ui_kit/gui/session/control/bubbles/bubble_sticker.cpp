#include "bubble_sticker.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleSticker::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);

	msg_sticker_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_sticker_, L"session/sticker_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_sticker_, L"session/sticker_left.xml");
	bubble_box_->Add(msg_sticker_);
	msg_sticker_->AttachMenu(nbase::Bind(&MsgBubbleSticker::OnMenu, this, std::placeholders::_1));
}

void MsgBubbleSticker::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	Json::Value json;
	if (StringToJson(msg.attach_, json) && json.isObject())
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Sticker && json["data"].isObject()) //finger
		{
			std::string sticker_name = json["data"]["chartlet"].asString() + ".png";
			std::string catalog = json["data"]["catalog"].asString();
			std::wstring dir = QPath::GetAppPath()+L"res\\stickers\\";
			std::wstring path = dir + nbase::UTF8ToUTF16(catalog) + L"\\" + nbase::UTF8ToUTF16(sticker_name);
			if (nbase::FilePathIsExist(path, false))
			{
				msg_sticker_->SetBkImage(path);
				return;
			}
		}
	}
	QLOG_ERR(L"parse location msg attach fail: {0}") << msg.attach_;
}
bool MsgBubbleSticker::OnMenu(ui::EventArgs* arg)
{
	PopupMenu(false, true);
	return false;
}
}
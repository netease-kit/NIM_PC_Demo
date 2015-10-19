#include "bubble_snapchat.h"
#include "image_view/src/image_view_manager.h"

namespace nim_comp
{
void MsgBubbleSnapChat::InitInfo(const MsgData &msg)
{
	MsgData new_msg = msg;
	src_image_path_ = nbase::UTF8ToUTF16(new_msg.local_file_path);
	std::wstring theme_dir = QPath::GetAppPath();
	bubble_box_->SetBkImage(L"");
	image_->SetForeStateImage(ui::kControlStateNormal, L"");
	if (my_msg_) {
		new_msg.local_file_path = nbase::UTF16ToUTF8(theme_dir + L"themes\\default\\session\\snapchat_send.png");
		image_->SetMargin(ui::UiRect(0, 0, 0, 0));
	}
	else {
		new_msg.local_file_path = nbase::UTF16ToUTF8(theme_dir + L"themes\\default\\session\\snapchat_receive.png");
		image_->SetMargin(ui::UiRect(0, 0, 0, 0));
	}
	MsgBubbleImage::InitInfo(new_msg);
	msg_image_->SetMargin(ui::UiRect(0,0,0,20));
	ui::Label* label = new ui::Label;
	bubble_box_->Add(label);
	label->SetText(L"点击查看");
	label->SetVerAlignType(ui::kVerAlignBottom);
	label->SetStateTextColor(ui::kControlStateNormal, L"lightcolor");
	if (!my_msg_)
	{
		label->SetHorAlignType(ui::kHorAlignRight);
	} 
	else
	{
		label->SetHorAlignType(ui::kHorAlignLeft);
	}
}

bool MsgBubbleSnapChat::OnClicked(ui::EventArgs* arg)
{
	//如果是阅后即焚，则需要先下载好图片
	Json::Value json;
	if (StringToJson(msg_.msg_attach, json))
	{
		std::string url = json["data"]["url"].asString();
		if (nbase::FilePathIsExist(src_image_path_, false)) {
			ImageViewManager::GetInstance()->StartViewPic(src_image_path_, L"", true, true);
			nim::MsgLog::DeleteAsync(sid_, type_, msg_.client_msg_id, nim::MsgLog::DeleteCallback());
			((ui::ListBox*)GetParent())->Remove(this);
		}
		else {
			nim::Http::DownloadResource(url, ToWeakCallback([this](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
				if (res_code == nim::kNIMResSuccess)
				{
					std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
					if (nbase::FilePathIsExist(ws_file_path, false))
					{
						ImageViewManager::GetInstance()->StartViewPic(ws_file_path, L"", true, true);
						nim::MsgLog::DeleteAsync(sid_, type_, msg_.client_msg_id, nim::MsgLog::DeleteCallback());
						((ui::ListBox*)GetParent())->Remove(this);
					}
				}
			}));
		}
	}

	return true;
}
}
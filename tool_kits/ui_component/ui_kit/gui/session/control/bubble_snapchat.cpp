#include "bubble_snapchat.h"
#include "image_view/src/image_view_manager.h"
#include "gui/session/session_form.h"

namespace nim_comp
{
void MsgBubbleSnapChat::InitInfo(const nim::IMMessage &msg)
{
	nim::IMMessage new_msg = msg;
	src_image_path_ = nbase::UTF8ToUTF16(new_msg.local_res_path_);
	std::wstring theme_dir = QPath::GetAppPath();
	bubble_box_->SetBkImage(L"");
	image_->SetForeStateImage(ui::kControlStateNormal, L"");
	if (my_msg_) {
		new_msg.local_res_path_ = nbase::UTF16ToUTF8(theme_dir + L"themes\\default\\session\\snapchat_send.png");
		image_->SetMargin(ui::UiRect(0, 0, 0, 0));
	}
	else {
		new_msg.local_res_path_ = nbase::UTF16ToUTF8(theme_dir + L"themes\\default\\session\\snapchat_receive.png");
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
	if (StringToJson(msg_.attach_, json))
	{
		std::string url = json["data"]["url"].asString();
		if (nbase::FilePathIsExist(src_image_path_, false)) {
			ImageViewManager::GetInstance()->StartViewPic(src_image_path_, L"", true, true);
			nim::MsgLog::DeleteAsync(sid_, type_, msg_.client_msg_id_, nim::MsgLog::DeleteCallback());
			SessionForm* session_form = dynamic_cast<SessionForm*>(this->GetWindow());
			if (session_form)
				session_form->OnSnapchatRead(msg_.client_msg_id_);
		}
		else {
			nim::NOS::DownloadResource(url, ToWeakCallback([this](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
				if (res_code == nim::kNIMResSuccess)
				{
					std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
					if (nbase::FilePathIsExist(ws_file_path, false))
					{
						ImageViewManager::GetInstance()->StartViewPic(ws_file_path, L"", true, true);
						nim::MsgLog::DeleteAsync(sid_, type_, msg_.client_msg_id_, nim::MsgLog::DeleteCallback());
						SessionForm* session_form = dynamic_cast<SessionForm*>(this->GetWindow());
						if (session_form)
							session_form->OnSnapchatRead(msg_.client_msg_id_);
					}
				}
			}));
		}
	}

	return true;
}
}
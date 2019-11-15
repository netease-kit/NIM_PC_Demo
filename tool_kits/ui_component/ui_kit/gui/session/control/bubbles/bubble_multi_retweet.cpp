#include "bubble_multi_retweet.h"
#include "ui_kit/module/multi_retweet/multi_message_retweet_data_def.h"
#include "gui/session/msg_record_retweet.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleMultiRetweet::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);
	ui::ButtonBox* button_box = new ui::ButtonBox;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(button_box, L"session/multi_retweet_right.xml");
	else
		GlobalManager::FillBoxWithCache(button_box, L"session/multi_retweet_left.xml");
	button_box->FindSubControl(L"container")->AttachMenu(nbase::Bind(&MsgBubbleMultiRetweet::OnMenu, this, std::placeholders::_1));
	ui::Button* msg_retweet = dynamic_cast<ui::Button*>(button_box->FindSubControl(L"show_lgos"));
	msg_retweet->AttachClick([this](ui::EventArgs* param) {
		MsgRecordRetweetForm* f = WindowsManager::SingletonShow<MsgRecordRetweetForm>(MsgRecordRetweetForm::kClassName);
		f->SetInfo(retweet_data_, msg_.sender_accid_, msg_.session_type_);
		f->ShowMessageLogs();
		return true;
	});
	bubble_box_->Add(button_box);
}

void MsgBubbleMultiRetweet::InitInfo(const nim::IMMessage &msg)
{
	__super::InitInfo(msg);

	Json::Value json;
	if (StringToJson(msg.attach_, json) && json.isObject())
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_MultiRetweet && json["data"].isObject()) //finger
		{
			retweet_data_ = std::make_shared< MultiMessageRetweetData>();
			retweet_data_->FromJsonObject(json["data"]);
			ui::Label* title = dynamic_cast<ui::Label*>( FindSubControl(L"title"));
			title->SetText(nbase::UTF8ToUTF16(retweet_data_->session_name_) + MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MSG_RECORD_LOGS_NAME"));
			for (auto& it : retweet_data_->msg_abstract_list_)
			{
				auto* abstract = new ui::Label;
				abstract->SetText(L"﹥" +  nbase::UTF8ToUTF16(it.sender_ + ":" + it.message_));
				dynamic_cast<ui::VBox*>(FindSubControl(L"abstract_container"))->Add(abstract);
			}
			return;
		}
	}
	QLOG_WAR(L"user type msg undefine, attach={0}") << msg.attach_;
}

bool MsgBubbleMultiRetweet::OnMenu( ui::EventArgs* arg )
{
	PopupMenu(false, true, true);
	return false;
}
}
#include "custom_msg.h"
#include "gui/session/control/session_util.h"
#include "module/emoji/emoji_info.h"

using namespace ui;

namespace nim_comp 
{

void CustomMsgBubble::InitControl(const nim::IMMessage &msg)
{
	GlobalManager::FillBoxWithCache(this, L"main/custom_msg.xml");

	text_ = (RichEdit*)FindSubControl(L"text");
	time_ = (Label*)FindSubControl(L"msg_time");
	name_ = (Label*)FindSubControl(L"sender_name");
	head_ = (Button*)FindSubControl(L"msg_header_button");

	ITextServices* text_services = text_->GetTextServices();
	IRichEditOleCallbackEx* richedit_cb = new IRichEditOleCallbackEx(text_services, std::function<void()>());
	text_services->Release();
	text_->SetOleCallback(richedit_cb);

	InitInfo(msg);
}

void CustomMsgBubble::InitInfo(const nim::IMMessage &msg)
{
	OnGetUserInfoCallback cb1 = ToWeakCallback([this, msg](const std::list<nim::UserNameCard> uinfos) {
		assert(nbase::MessageLoop::current()->ToUIMessageLoop());
		if (uinfos.empty()) return;

		std::string accid = uinfos.cbegin()->GetAccId();
		std::wstring sender = UserService::GetInstance()->GetUserName(accid);
		if (sender.empty())
			sender = nbase::UTF8ToUTF16(accid);
		if (msg.session_type_ == nim::kNIMSessionTypeP2P)
		{
			head_->SetBkImage(UserService::GetInstance()->GetUserPhoto(accid));
			name_->SetText(sender);
		}
		else
		{
			head_->SetBkImage(TeamService::GetInstance()->GetTeamPhoto(true));
			std::wstring team_sender = TeamService::GetInstance()->GetTeamName(msg.receiver_accid_) + L"->" + sender;
			name_->SetText(team_sender);
		}
	});
	UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, msg.sender_accid_), cb1);

	OnUserPhotoReadyCallback cb2 = ToWeakCallback([this, msg](const std::string& accid, const std::wstring& photo_path) {
		if (msg.session_type_ == nim::kNIMSessionTypeP2P && msg.sender_accid_ == accid)
			head_->SetBkImage(photo_path);
	});
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(cb2));

	std::wstring tm = GetMessageTime(msg.timetag_, false);
	time_->SetText(tm);

	std::wstring msg_body;
	if (msg.msg_setting_.server_history_saved_)
	{
		//msg_body = L"【可离线通知】";
	}
	else
	{
		msg_body = L"【在线通知】";
	}
// 	if (!msg.custom_apns_text.empty())
// 	{
// 		if (!msg_body.empty())
// 		{
// 			msg_body += L"\r\n";
// 		}
// 		msg_body += L"【推送】" + nbase::UTF8ToUTF16(msg.custom_apns_text);
// 	}
	if (!msg.content_.empty())
	{
		if (!msg_body.empty())
		{
			msg_body += L"\r\n";
		}
		msg_body += L"【附言】" + nbase::UTF8ToUTF16(msg.content_);
	}
	Json::Value json;
	if (StringToJson(msg.attach_, json))
	{
		std::string id = json["id"].asString();
		std::string content = json["content"].asString();
		if (id == "2" && !content.empty())
		{
			if (!msg_body.empty())
			{
				msg_body += L"\r\n【内容】";
			}
			msg_body = nbase::UTF8ToUTF16(content);
		}
	}

	SetMsgText(msg_body);
}

void CustomMsgBubble::SetMsgText(const std::wstring &str)
{
	emoji::InsertToEdit(text_, str);
	int width = 250;

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx);
	text_->SetFixedHeight(sz.cy);
}

}

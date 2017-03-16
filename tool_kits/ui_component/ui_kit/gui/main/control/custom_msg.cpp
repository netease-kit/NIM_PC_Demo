#include "custom_msg.h"
#include "module/session/session_util.h"
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
	sender_id_ = msg.sender_accid_;
	session_type_ = msg.session_type_;
	receiver_id_ = msg.receiver_accid_;

	std::wstring sender = UserService::GetInstance()->GetUserName(sender_id_);
	if (session_type_ == nim::kNIMSessionTypeP2P)
	{
		head_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(sender_id_));
		name_->SetText(sender);
	}
	else
	{
		head_->SetBkImage(PhotoService::GetInstance()->GetTeamPhoto(receiver_id_, true));
		std::wstring team_sender = TeamService::GetInstance()->GetTeamName(receiver_id_) + L"->" + sender;
		name_->SetText(team_sender);
	}

	std::wstring tm = GetMessageTime(msg.timetag_, false);
	time_->SetText(tm);

	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	std::wstring msg_body;
	if (msg.msg_setting_.server_history_saved_)
	{
		//msg_body = L"【可离线通知】";
	}
	else
	{
		msg_body = mls->GetStringViaID(L"STRID_MSG_CENTER_ONLINE_NOTIFY");
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
		msg_body += mls->GetStringViaID(L"STRID_MSG_CENTER_POSTSCRIPT") + nbase::UTF8ToUTF16(msg.content_);
	}
	bool fetch_data = false;
	Json::Value json;
	if (StringToJson(msg.attach_, json) && json.isObject())
	{
		if (json.isMember("id") && json.isMember("content"))
		{
			std::string id = json["id"].asString();
			std::string content = json["content"].asString();
			if (id == "2" && !content.empty())
			{
				if (!msg_body.empty())
				{
					msg_body += L"\r\n" + mls->GetStringViaID(L"STRID_MSG_CENTER_CONTENT");
				}
				msg_body = nbase::UTF8ToUTF16(content);
				fetch_data = true;
			}
		}
	}
	if (!fetch_data)
		msg_body = nbase::UTF8ToUTF16(msg.attach_);

	SetMsgText(msg_body);
}

void CustomMsgBubble::SetMsgText(const std::wstring &str)
{
	InsertTextToEdit(text_, str);
	int width = 250;

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx);
	text_->SetFixedHeight(sz.cy);
}

void CustomMsgBubble::OnUserInfoChange(const nim::UserNameCard & info)
{
	if (info.GetAccId() == sender_id_)
	{
		std::wstring sender = UserService::GetInstance()->GetUserName(info.GetAccId());
		if (session_type_ == nim::kNIMSessionTypeP2P)
		{
			name_->SetText(sender);
			head_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(info.GetAccId()));
		}
		else
		{
			std::wstring team_sender = name_->GetText();
			size_t pos = team_sender.find(L"->");
			if (pos != std::wstring::npos)
				name_->SetText(team_sender.substr(0, pos + 2) + sender);
		}
	}
}

void CustomMsgBubble::OnUserPhotoReady(const std::string & accid, const std::wstring & photo_path)
{
	if (accid == sender_id_ && session_type_ == nim::kNIMSessionTypeP2P)
		head_->SetBkImage(photo_path);
}

void CustomMsgBubble::OnTeamNameChange(const nim::TeamInfo & team_info)
{
	if (team_info.GetTeamID() == receiver_id_ && session_type_ == nim::kNIMSessionTypeTeam)
	{
		std::wstring team_sender = name_->GetText();
		size_t pos = team_sender.find(L"->");
		if (pos != std::wstring::npos)
			name_->SetText(nbase::UTF8ToUTF16(team_info.GetName()) + team_sender.substr(pos));
	}
}

}

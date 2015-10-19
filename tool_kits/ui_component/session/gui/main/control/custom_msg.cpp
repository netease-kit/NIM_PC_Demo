#include "custom_msg.h"
#include "gui/session/control/session_util.h"
#include "module/emoji/emoji_info.h"

using namespace ui;

namespace nim_comp 
{

void CustomMsgBubble::InitControl(const MsgData &msg)
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

void CustomMsgBubble::InitInfo(const MsgData &msg)
{
	OnGetUserInfoCallback cb1 = ToWeakCallback([this, msg](bool ret, const std::list<UserInfo> uinfos) {
		assert(nbase::MessageLoop::current()->ToUIMessageLoop());
		if (!ret || uinfos.empty()) return;

		std::string nick = uinfos.cbegin()->name;
		if (msg.msg_type == nim::kNIMSysMsgTypeCustomP2PMsg)
		{
			head_->SetBkImage(UserService::GetInstance()->GetUserPhoto(uinfos.cbegin()->account));
			name_->SetUTF8Text(nick.empty() ? msg.from_account : nick);
		}
		else
		{
			head_->SetBkImage(TeamService::GetInstance()->GetTeamPhoto(true));
			std::wstring tname = TeamService::GetInstance()->GetTeamName(msg.to_account);
			std::string name_test = nbase::UTF16ToUTF8(tname) + "->";
			if (uinfos.cbegin()->name.empty())
				name_test += msg.from_account;
			else
				name_test += nick;
			name_->SetUTF8Text(name_test);
		}
	});
	UserService::GetInstance()->GetUserInfoWithEffort(std::list<std::string>(1, msg.from_account), cb1);

	OnUserPhotoReadyCallback cb2 = ToWeakCallback([this, msg](const std::string& accid, const std::wstring& photo_path) {
		if (msg.msg_type == nim::kNIMSysMsgTypeCustomP2PMsg && msg.from_account == accid)
			head_->SetBkImage(photo_path);
	});
	unregister_cb.Add(UserService::GetInstance()->RegUserPhotoReady(cb2));

	std::wstring tm = GetMessageTime(msg.msg_time, false);
	time_->SetText(tm);

	std::wstring msg_body;
	if (msg.custom_save_flag == 1)
	{
		//msg_body = L"【可离线通知】";
	}
	else
	{
		msg_body = L"【在线通知】";
	}
	if (!msg.custom_apns_text.empty())
	{
		if (!msg_body.empty())
		{
			msg_body += L"\r\n";
		}
		msg_body += L"【推送】" + nbase::UTF8ToUTF16(msg.custom_apns_text);
	}
	if (!msg.msg_body.empty())
	{
		if (!msg_body.empty())
		{
			msg_body += L"\r\n";
		}
		msg_body += L"【附言】" + nbase::UTF8ToUTF16(msg.msg_body);
	}
	Json::Value json;
	if (StringToJson(msg.msg_attach, json))
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

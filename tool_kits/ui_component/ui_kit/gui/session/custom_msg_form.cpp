#include "custom_msg_form.h"
#include "module/emoji/richedit_util.h"
#include "module/login/login_manager.h"
#include "export/nim_ui_user_config.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR CustomMsgForm::kClassName = L"CustomMsgForm";

CustomMsgForm::CustomMsgForm()
{
}

CustomMsgForm::~CustomMsgForm()
{

}

std::wstring CustomMsgForm::GetSkinFolder()
{
	return L"session";
}

std::wstring CustomMsgForm::GetSkinFile()
{
	return L"custom_msg_form.xml";
}

ui::UILIB_RESOURCETYPE CustomMsgForm::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring CustomMsgForm::GetZIPFileName() const
{
	return L"custom_msg_form.zip";
}

std::wstring CustomMsgForm::GetWindowClassName() const
{
	return CustomMsgForm::kClassName;
}

std::wstring CustomMsgForm::GetWindowId() const
{
	return CustomMsgForm::kClassName;
}

UINT CustomMsgForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void CustomMsgForm::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LRESULT CustomMsgForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CustomMsgForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetIcon() > 0)
	{
		SetIcon(nim_ui::UserConfig::GetInstance()->GetIcon());
	}

	SetTaskbarTitle(L"自定义消息通知");

	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&CustomMsgForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CustomMsgForm::OnClicked, this, std::placeholders::_1));

	richedit_apns_ = (RichEdit*)FindControl(L"re_apns");
	richedit_msg_ = (RichEdit*)FindControl(L"re_msg");
	richedit_attach_ = (RichEdit*)FindControl(L"re_attach");
	rec_name_ = (Label*)FindControl(L"receiver");
	msg_mode_ = (CheckBox*)FindControl(L"msg_mode");
}

bool CustomMsgForm::Notify(ui::EventArgs* param)
{
	return true;
}
std::string GetRichText(ui::RichEdit* re)
{
	std::wstring wstr;
	ITextServices* service = re->GetTextServices();
	Re_GetText(service, wstr);
	service->Release();

	return nbase::UTF16ToUTF8(wstr);
}

bool CustomMsgForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_send")
	{
		std::string attach_text = GetRichText(richedit_attach_);
		if (attach_text.empty())
		{
			return true;
		}
		richedit_attach_->SetText(L"");
		Json::Value json;
		json["id"] = "2";
		json["content"] = attach_text;
		nim::SysMessage msg;
		msg.receiver_accid_ = session_id_;
		msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
		msg.support_offline_ = msg_mode_->IsSelected() ? nim::BS_FALSE : nim::BS_TRUE;
		msg.attach_ = json.toStyledString();
		if (richedit_msg_)
		{
			msg.content_ = GetRichText(richedit_msg_);
			richedit_msg_->SetText(L"");
		}
		if (richedit_apns_)
		{
			msg.apns_text_ = GetRichText(richedit_apns_);
			richedit_apns_->SetText(L"");
		}
		msg.type_ = session_type_ == nim::kNIMSessionTypeTeam ? nim::kNIMSysMsgTypeCustomTeamMsg : nim::kNIMSysMsgTypeCustomP2PMsg;
		msg.client_msg_id_ = QString::GetGUID();
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

		nim::SystemMsg::SendCustomNotificationMsg(msg.ToJsonString());
	}
	return true;
}

void CustomMsgForm::SetSession(const std::string& id, nim::NIMSessionType type, const std::wstring& name)
{
	msg_mode_->SetEnabled(type == nim::kNIMSessionTypeP2P);
	if (type == nim::kNIMSessionTypeTeam)
	{
		msg_mode_->Selected(true, false);
	}
	session_id_ = id;
	session_type_ = type;
	std::wstring show_text = nbase::StringPrintf(L"发送给 %s", name.c_str());
	rec_name_->SetText(show_text.c_str());
}
}
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

void CustomMsgForm::InitWindow()
{
	if (nim_ui::UserConfig::GetInstance()->GetDefaultIcon() > 0)
		SetIcon(nim_ui::UserConfig::GetInstance()->GetDefaultIcon());

	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&CustomMsgForm::OnClicked, this, std::placeholders::_1));

	richedit_apns_ = (RichEdit*)FindControl(L"re_apns");
	richedit_msg_ = (RichEdit*)FindControl(L"re_msg");
	richedit_attach_ = (RichEdit*)FindControl(L"re_attach");
	rec_name_ = (Label*)FindControl(L"receiver");
	msg_mode_ = (CheckBox*)FindControl(L"msg_mode");
}

bool CustomMsgForm::OnClicked(ui::EventArgs* param)
{
	std::wstring name = param->pSender->GetName();
	if (name == L"btn_send")
	{
		std::string attach_text = nbase::UTF16ToUTF8(GetRichText(richedit_attach_));
		if (attach_text.empty())
		{
			return true;
		}
		richedit_attach_->SetText(L"");
		Json::Value json;
		Json::FastWriter writer;
		json["id"] = "2";
		json["content"] = attach_text;

		nim::SysMessageSetting setting;
		setting.need_offline_ = msg_mode_->IsSelected() ? nim::BS_FALSE : nim::BS_TRUE;

		auto str = nim::SystemMsg::CreateCustomNotificationMsg(session_id_, session_type_ == nim::kNIMSessionTypeTeam ? nim::kNIMSysMsgTypeCustomTeamMsg : nim::kNIMSysMsgTypeCustomP2PMsg, QString::GetGUID(), writer.write(json), setting);
		nim::SystemMsg::SendCustomNotificationMsg(str);
	}
	return true;
}

void CustomMsgForm::SetSession(const std::string& id, nim::NIMSessionType type, const std::wstring& name)
{
// 	msg_mode_->SetEnabled(type == nim::kNIMSessionTypeP2P);
// 	if (type == nim::kNIMSessionTypeTeam)
// 	{
// 		msg_mode_->Selected(true, false);
// 	}
	session_id_ = id;
	session_type_ = type;
	std::wstring show_text = nbase::StringPrintf(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_CUSTOM_MSG_SEND_TO").c_str(), name.c_str());
	rec_name_->SetText(show_text.c_str());
}
}
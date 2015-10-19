#include "custom_msg_form.h"
#include "module/emoji/richedit_util.h"
#include "module/login/login_manager.h"

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
	SetIcon(IDI_ICON);
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
		MsgData msg;
		msg.to_type = session_type_;
		msg.to_account = session_id_;
		msg.from_account = LoginManager::GetInstance()->GetAccount();
		msg.resend_flag = 0;
		msg.custom_save_flag = msg_mode_->IsSelected()?0:1;
		msg.msg_status = nim::kNIMMsgLogStatusSending;
		msg.msg_attach = json.toStyledString();
		if (richedit_msg_)
		{
			msg.msg_body = GetRichText(richedit_msg_);
			richedit_msg_->SetText(L"");
		}
		if (richedit_apns_)
		{
			msg.custom_apns_text = GetRichText(richedit_apns_);
			richedit_apns_->SetText(L"");
		}
		msg.msg_type = session_type_ == nim::kNIMSessionTypeTeam ? nim::kNIMSysMsgTypeCustomTeamMsg : nim::kNIMSysMsgTypeCustomP2PMsg;
		Json::Value value;
		CustomSysMsgToJson(msg, value);

		nim::Talk::SendCustomSysmsg(value.toStyledString());
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
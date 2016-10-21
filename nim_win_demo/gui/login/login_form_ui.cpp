#include "resource.h"
#include "login_form.h"
#include "util/user.h"
#include "shared/tool.h"
#include "gui/main/main_form.h"
#include "gui/proxy/proxy_form.h"

using namespace ui;

const LPCTSTR LoginForm::kClassName	= L"LoginForm";

LoginForm::LoginForm()
{
}

LoginForm::~LoginForm()
{

}

std::wstring LoginForm::GetSkinFolder()
{
	return L"login";
}

std::wstring LoginForm::GetSkinFile()
{
	return L"login.xml";
}

std::wstring LoginForm::GetWindowClassName() const 
{
	return kClassName;
}

std::wstring LoginForm::GetWindowId() const
{
	return kClassName;
}

UINT LoginForm::GetClassStyle() const 
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void LoginForm::InitWindow()
{
	SetIcon(IDI_ICON);
	SetTaskbarTitle(L"登录");
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&LoginForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&LoginForm::OnClicked, this, std::placeholders::_1));

	usericon_ = FindControl(L"usericon");
	passwordicon_ = FindControl(L"passwordicon");

	user_name_edit_ = (RichEdit*) FindControl( L"username" );
	nick_name_edit_ = (RichEdit*)FindControl(L"nickname");
	password_edit_ = (RichEdit*) FindControl( L"password" );
	user_name_edit_->SetSelAllOnFocus(true);
	password_edit_->SetSelAllOnFocus(true);

	login_ing_tip_ = FindControl( L"login_ing_tip" );
	login_error_tip_ = (Label*) FindControl( L"login_error_tip" );
	register_ok_toast_ = (Label*)FindControl(L"register_ok_toast");

	btn_login_ = (Button*) FindControl( L"btn_login" );
	btn_register_ = (Button*)FindControl(L"btn_register");
	btn_cancel_ = (Button*) FindControl( L"btn_cancel" );

	//RichEdit的SetText操作放在最后，会触发TextChange事件
	std::wstring account = QCommand::Get(kCmdAccount);
	user_name_edit_->SetText(account);

	std::wstring why = QCommand::Get(kCmdExitWhy);
	if( !why.empty() )
	{
		int reason = _wtoi( why.c_str() );
		if (reason == nim::kNIMLogoutKickout)
			ShowLoginTip(L"你已被踢出");
		else if (reason == nim::kNIMLogoutRelogin)
			ShowLoginTip(L"网络连接已断开");
		else if (reason == nim::kNIMResExist)
			ShowLoginTip(L"你在其他设备上登录过，请重新登录");
		else
			ShowLoginTip(nbase::StringPrintf(L"登录失败，错误码：%d", reason));

		QCommand::Erase(kCmdExitWhy);
	}

	user_name_edit_->SetLimitText(32);
	nick_name_edit_->SetLimitText(64);
	password_edit_->SetLimitText(128);
	((ui::Button*)FindControl(L"register_account"))->AttachClick([this](ui::EventArgs* msg) {
		SetTaskbarTitle(L"注册");
		FindControl(L"enter_panel")->SetBkImage(L"user_password_nickname.png");
		msg->pSender->GetWindow()->FindControl(L"nick_name_panel")->SetVisible();
		msg->pSender->GetWindow()->FindControl(L"enter_login")->SetVisible();
		btn_register_->SetVisible();
		btn_login_->SetVisible(false);
		user_name_edit_->SetText(L"");
		user_name_edit_->SetPromptText(L"帐号限32位字母或数字");
		usericon_->SetEnabled(true);
		password_edit_->SetText(L"");
		password_edit_->SetPromptText(L"密码限6~128位字母或数字");
		passwordicon_->SetEnabled(true);
		nick_name_edit_->SetText(L"");
		nick_name_edit_->SetPromptText(L"昵称为汉字、字母或数字的组合");
		msg->pSender->GetWindow()->FindControl(L"register_account")->SetVisible(false);
		return true; });

	((ui::Button*)FindControl(L"enter_login"))->AttachClick([this](ui::EventArgs* msg) {
		SetTaskbarTitle(L"登录");
		FindControl(L"enter_panel")->SetBkImage(L"user_password.png");
		msg->pSender->GetWindow()->FindControl(L"nick_name_panel")->SetVisible(false);
		msg->pSender->GetWindow()->FindControl(L"enter_login")->SetVisible(false);
		btn_register_->SetVisible(false);
		btn_login_->SetVisible();
		user_name_edit_->SetText(L"");
		user_name_edit_->SetPromptText(L"帐号");
		password_edit_->SetText(L"");
		password_edit_->SetPromptText(L"密码");
		msg->pSender->GetWindow()->FindControl(L"register_account")->SetVisible(true);
		return true; });

	this->RegLoginManagerCallback();
}

LRESULT LoginForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT LoginForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	nim_ui::LoginManager::GetInstance()->DoLogout(false);
	return 1;

	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

bool LoginForm::Notify( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == kEventTextChange)
	{
		if(name == L"username")
		{
			OnCancelLogin();
			password_edit_->SetText(L"");
		}
		else if(name == L"password")
		{
			//去除中文字符
			bool has_chinise = false;
			std::wstring text = password_edit_->GetText(), text_fixed;
			for (size_t i = 0; i < text.length(); i++)
			{
				if (IsAsciiChar(text[i]))
					text_fixed.push_back(text[i]);
				else
					has_chinise = true;
			}
			password_edit_->SetText(text_fixed);

			if (has_chinise)
			{
				ShowLoginTip(L"密码不能含有中文字符");
				passwordicon_->SetEnabled(false);
			}
			else 
			{
				login_error_tip_->SetVisible(false);
				passwordicon_->SetEnabled(true);
			}
		}
		else if (name == L"nickname")
		{
			login_error_tip_->SetVisible(false);
			FindControl(L"nick_name_icon")->SetEnabled(true);
		}
	}
	else if(msg->Type == kEventTab)
	{
		if(user_name_edit_->IsFocused())
		{
			if (nick_name_edit_->IsVisible())
			{
				nick_name_edit_->SetSelAll();
				nick_name_edit_->SetFocus();
			}
			else
			{
				password_edit_->SetSelAll();
				password_edit_->SetFocus();
			}
		}
		else if(password_edit_->IsFocused())
		{
			user_name_edit_->SetSelAll();
			user_name_edit_->SetFocus();
		}
		else if (nick_name_edit_->IsFocused())
		{
			password_edit_->SetSelAll();
			password_edit_->SetFocus();
		}
	}
	else if (msg->Type == kEventReturn)
	{
		if (name == L"username" || name == L"nickname" || name == L"password")
		{
			if (!nick_name_edit_->IsVisible())
			{
				btn_login_->SetFocus();
				DoBeforeLogin();
			}
			else
				DoRegisterAccount();
		}
	}
	return true;
}

bool LoginForm::OnClicked( ui::EventArgs* msg )
{
	std::wstring name = msg->pSender->GetName();
	if(name == L"btn_login")
	{
		DoBeforeLogin();
	}
	else if (name == L"btn_register")
	{
		DoRegisterAccount();
	}
	else if(name == L"btn_cancel")
	{
		btn_cancel_->SetEnabled(false);
		nim_ui::LoginManager::GetInstance()->CancelLogin();
	}
	else if (name == L"proxy_setting")
	{
		nim_comp::WindowsManager::SingletonShow<ProxyForm>(ProxyForm::kClassName);
	}
	return true;
}
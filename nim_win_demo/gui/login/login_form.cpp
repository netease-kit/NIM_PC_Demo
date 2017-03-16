#include "login_form.h"
#include "gui/main/main_form.h"
#include "module/login/login_db.h"

using namespace ui;

void LoginForm::DoBeforeLogin()
{
	std::string username = user_name_edit_->GetUTF8Text();
	StringHelper::Trim( username );
	std::wstring user_name_temp = nbase::UTF8ToUTF16(username);
	user_name_temp = StringHelper::MakeLowerString(user_name_temp);
	username = nbase::UTF16ToUTF8(user_name_temp);
	if( username.empty() )
	{
		usericon_->SetEnabled(false);
		ShowLoginTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_LOGIN_FORM_TIP_ACCOUNT_EMPTY"));
		return;
	}

	std::string password = password_edit_->GetUTF8Text();
	StringHelper::Trim( password );
	if( password.empty() )
	{
		passwordicon_->SetEnabled(false);
		ShowLoginTip(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_LOGIN_FORM_TIP_PASSWORD_EMPTY"));
		return;
	}

	usericon_->SetEnabled(true);
	passwordicon_->SetEnabled(true);

	StartLogin(username, password);
}

void LoginForm::DoRegisterAccount()
{
	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	std::string username = user_name_edit_->GetUTF8Text();
	StringHelper::Trim(username);
	std::string password = password_edit_->GetUTF8Text();
	StringHelper::Trim(password);
	std::string nickname = nick_name_edit_->GetUTF8Text();
	StringHelper::Trim(nickname);
	if (password.length() < 6 || password.length() > 128)
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_PASSWORD_RESTRICT"));
	}
	else if (nickname.empty())
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_NICKNAME_RESTRICT"));
	}
	else 
	{
		btn_register_->SetEnabled(false);
		btn_login_->SetVisible(false);

		password = QString::GetMd5(password);
		nim_ui::UserManager::GetInstance()->InvokeRegisterAccount(username, password, nickname, ToWeakCallback([this](int res, const std::string& err_msg) {
			if (res == 200) 
			{
				register_ok_toast_->SetVisible(true);
				nbase::ThreadManager::PostDelayedTask(ToWeakCallback([this]() {
					register_ok_toast_->SetVisible(false);
				}), nbase::TimeDelta::FromSeconds(2));

				nbase::ThreadManager::PostDelayedTask(ToWeakCallback([this]() {
					SetTaskbarTitle(MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_LOGIN_FORM_LOGIN"));
					FindControl(L"enter_panel")->SetBkImage(L"user_password.png");
					FindControl(L"nick_name_panel")->SetVisible(false);
					FindControl(L"enter_login")->SetVisible(false);
					FindControl(L"register_account")->SetVisible(true);
					btn_register_->SetEnabled(true);
					btn_register_->SetVisible(false);
					btn_login_->SetVisible(true);
				}), nbase::TimeDelta::FromMilliseconds(2500));
			}
			else
			{
				MutiLanSupport* mls = MutiLanSupport::GetInstance();
				if (res == 601) {
					ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_ACCOUNT_RESTRICT"));
				}
				else if (res == 602) {
					ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_ACCOUNT_EXIST"));
				}
				else if (res == 603) {
					ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_NICKNAME_TOO_LONG"));
				}
				else {
					ShowLoginTip(nbase::UTF8ToUTF16(err_msg));
				}
				btn_register_->SetEnabled(true);
			}
		}));
	}

}

void LoginForm::StartLogin( std::string username, std::string password )
{
	user_name_edit_->SetEnabled(false);
	password_edit_->SetEnabled(false);

	login_error_tip_->SetVisible(false);
	login_ing_tip_->SetVisible(true);

	btn_login_->SetVisible(false);
	btn_cancel_->SetVisible(true);

	nim_ui::LoginManager::GetInstance()->DoLogin(username, password);
}

void LoginForm::RegLoginManagerCallback()
{
	nim_ui::OnLoginError cb_result = [this](int error){
		this->OnLoginError(error);
	};

	nim_ui::OnCancelLogin cb_cancel = [this]{
		this->OnCancelLogin();
	};

	nim_ui::OnHideWindow cb_hide = [this]{
		this->ShowWindow(false, false);
	};

	nim_ui::OnDestroyWindow cb_destroy = [this]{
		LoginDB::GetInstance()->SaveLoginData();
		::DestroyWindow(this->GetHWND());
	};

	nim_ui::OnShowMainWindow cb_show_main = [this]{
		nim_ui::WindowsManager::SingletonShow<MainForm>(MainForm::kClassName);
	};

	nim_ui::LoginManager::GetInstance()->RegLoginManagerCallback(ToWeakCallback(cb_result),
		ToWeakCallback(cb_cancel),
		ToWeakCallback(cb_hide),
		ToWeakCallback(cb_destroy),
		ToWeakCallback(cb_show_main));
}

void LoginForm::OnLoginError( int error )
{
	OnCancelLogin();

	MutiLanSupport* mls = MutiLanSupport::GetInstance();
	if (error == nim::kNIMResUidPassError)
	{
		usericon_->SetEnabled(false);
		passwordicon_->SetEnabled(false);
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_PASSWORD_ERROR"));
	}
	else if (error == nim::kNIMResConnectionError)
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_NETWORK_ERROR"));
	}
	else if (error == nim::kNIMResExist)
	{
		ShowLoginTip(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_LOCATION_CHANGED"));
	}
	else
	{
		std::wstring tip = nbase::StringPrintf(mls->GetStringViaID(L"STRID_LOGIN_FORM_TIP_ERROR_CODE").c_str(), error);
		ShowLoginTip(tip);
	}
}

void LoginForm::OnCancelLogin()
{
	usericon_->SetEnabled(true);
	passwordicon_->SetEnabled(true);

	user_name_edit_->SetEnabled(true);
	password_edit_->SetEnabled(true);

	login_ing_tip_->SetVisible(false);
	login_error_tip_->SetVisible(false);

	btn_login_->SetVisible(true);
	btn_cancel_->SetVisible(false);
	btn_cancel_->SetEnabled(true);
}

void LoginForm::ShowLoginTip(std::wstring tip_text)
{
	login_ing_tip_->SetVisible(false);

	login_error_tip_->SetText(tip_text);
	login_error_tip_->SetVisible(true);
}

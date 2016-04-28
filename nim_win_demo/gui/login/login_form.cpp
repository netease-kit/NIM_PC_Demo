#include "login_form.h"
#include "gui/main/main_form.h"


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
		ShowLoginTip(L"帐号为空");
		return;
	}

	std::string password = password_edit_->GetUTF8Text();
	StringHelper::Trim( password );
	if( password.empty() )
	{
		passwordicon_->SetEnabled(false);
		ShowLoginTip(L"密码为空");
		return;
	}

	usericon_->SetEnabled(true);
	passwordicon_->SetEnabled(true);

	StartLogin(username, password);
}

void LoginForm::DoRegisterAccount()
{
	std::string username = user_name_edit_->GetUTF8Text();
	StringHelper::Trim(username);
	std::string password = password_edit_->GetUTF8Text();
	StringHelper::Trim(password);
	std::string nickname = nick_name_edit_->GetUTF8Text();
	StringHelper::Trim(nickname);
	if (password.length() < 6 || password.length() > 128)
	{
		ShowLoginTip(L"密码为6~128位字母或者数字组合");
	}
	else if (nickname.empty())
	{
		ShowLoginTip(L"昵称为汉字、字母或数字的组合");
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
					SetTaskbarTitle(L"登录");
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
				if (res == 601) {
					ShowLoginTip(L"帐号为6~32位字母或者数字组合");
				}
				else if (res == 602) {
					ShowLoginTip(L"此帐号已存在");
				}
				else if (res == 603) {
					ShowLoginTip(L"昵称为汉字、字母或数字的组合");
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
	nim_ui::OnLoginResult cb_result = [this](int error){
		this->OnLoginResult(error);
	};

	nim_ui::OnCancelLogin cb_cancel = [this]{
		this->Reset();
	};

	nim_ui::OnHideWindow cb_hide = [this]{
		this->ShowWindow(false, false);
	};

	nim_ui::OnDestroyWindow cb_destroy = [this]{
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

void LoginForm::OnLoginResult( int error )
{
	Reset();

	if (error == nim::kNIMResUidPassError)
	{
		usericon_->SetEnabled(false);
		passwordicon_->SetEnabled(false);
		ShowLoginTip(L"用户名或密码错误，请重新输入");
	}
	else if (error == nim::kNIMResConnectionError)
	{
		ShowLoginTip(L"网络出现问题，请确认网络连接");
	}
	else if (error == nim::kNIMResExist)
	{
		ShowLoginTip(L"你在其他设备上登录过，请重新登录");
	}
	else
	{
		std::wstring tip = nbase::StringPrintf(L"登录失败，错误码：%d", error);
		ShowLoginTip(tip);
	}
}

void LoginForm::Reset()
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

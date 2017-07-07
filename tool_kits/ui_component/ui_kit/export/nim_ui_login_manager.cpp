#include "nim_ui_login_manager.h"
#include "callback/login/login_callback.h"
#include "module/login/login_manager.h"

namespace nim_ui
{

void LoginManager::RegLoginManagerCallback(const OnLoginError& cb_result, const OnCancelLogin& cb_cancel, const OnHideWindow& cb_hide, const OnDestroyWindow& cb_destroy, const OnShowMainWindow& cb_show_main)
{
	cb_login_error_ = cb_result;
	cb_cancel_login_ = cb_cancel;
	cb_hide_window_ = cb_hide;
	cb_destroy_window_ = cb_destroy;
	cb_show_main_window_ = cb_show_main;
}

void LoginManager::InvokeLoginError(int error)
{
	if (cb_login_error_)
		cb_login_error_(error);
}

void LoginManager::InvokeCancelLogin()
{
	if (cb_cancel_login_)
		cb_cancel_login_();
}

void LoginManager::InvokeHideWindow()
{
	if (cb_hide_window_)
		cb_hide_window_();
}

void LoginManager::InvokeDestroyWindow()
{
	if (cb_destroy_window_)
		cb_destroy_window_();
}

void LoginManager::InvokeShowMainForm()
{
	if (cb_show_main_window_)
		cb_show_main_window_();
}

bool LoginManager::IsLoginFormValid()
{
	bool is_valid = cb_cancel_login_ && cb_destroy_window_ && cb_hide_window_ && cb_login_error_ && cb_show_main_window_;
	return is_valid;
}

void LoginManager::DoLogout(bool over, nim::NIMLogoutType type /*= nim::kNIMLogoutAppExit*/)
{
	nim_comp::LoginCallback::DoLogout(over, type);
}

void LoginManager::DoLogin(std::string user, std::string pass)
{
	nim_comp::LoginCallback::DoLogin(user, pass);
}

void LoginManager::CancelLogin()
{
	nim_comp::LoginCallback::CacelLogin();
}

std::string LoginManager::GetAccount()
{
	return nim_comp::LoginManager::GetInstance()->GetAccount();
}

bool LoginManager::IsEqual(const std::string& account)
{
	return nim_comp::LoginManager::GetInstance()->IsEqual(account);
}

bool LoginManager::IsLinkActive()
{
	return nim_comp::LoginManager::GetInstance()->IsLinkActive();
}

bool LoginManager::CheckSingletonRun(const std::wstring& username)
{
	return nim_comp::LoginManager::GetInstance()->CheckSingletonRun(username);
}

}


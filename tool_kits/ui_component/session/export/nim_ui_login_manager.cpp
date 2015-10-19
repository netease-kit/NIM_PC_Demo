#include "nim_ui_login_manager.h"
#include "callback/login/login_callback.h"
#include "module/login/login_manager.h"

namespace nim_ui
{

LoginManager::LoginManager()
{

}

LoginManager::~LoginManager()
{

}

// void LoginManager::RegLoginResult(const OnLoginResult& cb)
// {
// 	if (cb)
// 		cb_login_result_ = cb;
// }
// 
// void LoginManager::RegCancelLogin(const OnCancelLogin& cb)
// {
// 	if (cb)
// 		cb_cancel_login_ = cb;
// }
// 
// void LoginManager::RegHideWindow(const OnHideWindow& cb)
// {
// 	if (cb)
// 		cb_hide_window_ = cb;
// }
// 
// void LoginManager::RegDestroyWindow(const OnDestroyWindow& cb)
// {
// 	if (cb)
// 		cb_destroy_window_ = cb;
// }
// 
// void LoginManager::InvokeLoginResult(int error)
// {
// 	if (cb_login_result_)
// 		cb_login_result_(error);
// }
// 
// void LoginManager::InvokeCancelLogin()
// {
// 	if (cb_cancel_login_)
// 		cb_cancel_login_();
// }
// 
// void LoginManager::InvokeHideWindow()
// {
// 	if (cb_hide_window_)
// 		cb_hide_window_();
// }
// 
// void LoginManager::InvokeDestroyWindow()
// {
// 	if (cb_destroy_window_)
// 		cb_destroy_window_();
// }
// 
// bool LoginManager::IsLoginFormValid()
// {
// 	return cb_cancel_login_ && cb_destroy_window_ && cb_hide_window_ && cb_login_result_;
// }

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

}


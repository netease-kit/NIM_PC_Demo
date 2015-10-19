#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{
// 原本用于ui分离时让demo设置回调函数
// 	typedef std::function<void(int error)> OnLoginResult;
// 	typedef std::function<void()> OnCancelLogin;
// 	typedef std::function<void()> OnHideWindow;
// 	typedef std::function<void()> OnDestroyWindow;

/** @class LoginManager
  * @brief 提供登录控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API LoginManager
{
public:
	SINGLETON_DEFINE(LoginManager);
public:
	LoginManager();
	~LoginManager();
public:
// 	void RegLoginResult(const OnLoginResult& cb);
// 	void RegCancelLogin(const OnCancelLogin& cb);
// 	void RegHideWindow(const OnHideWindow& cb);
// 	void RegDestroyWindow(const OnDestroyWindow& cb);
//
// 	void InvokeLoginResult(int error);
// 	void InvokeCancelLogin();
// 	void InvokeHideWindow();
// 	void InvokeDestroyWindow();
//
//	bool IsLoginFormValid();

	void DoLogout(bool over, nim::NIMLogoutType type = nim::kNIMLogoutAppExit);
	void DoLogin(std::string user, std::string pass);
	void CancelLogin();

	std::string GetAccount();
	bool IsEqual(const std::string& account);
	bool IsLinkActive();

private:
// 	OnLoginResult cb_login_result_;
// 	OnCancelLogin cb_cancel_login_;
// 	OnHideWindow cb_hide_window_;
// 	OnDestroyWindow cb_destroy_window_;
};

}
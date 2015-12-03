#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{
	typedef std::function<void(int error)> OnLoginResult;
	typedef std::function<void()> OnCancelLogin;
	typedef std::function<void()> OnHideWindow;
	typedef std::function<void()> OnDestroyWindow;
	typedef std::function<void()> OnShowMainWindow;

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
	void DoLogout(bool over, nim::NIMLogoutType type = nim::kNIMLogoutAppExit);
	void DoLogin(std::string user, std::string pass);
	void CancelLogin();

	std::string GetAccount();
	bool IsEqual(const std::string& account);
	bool IsLinkActive();

	/**
	* 注册登录窗体的回调函数，用来让UI组件控制登录窗体行为
	* @param[in] cb_result		通知登录错误并返回错误原因的回调函数
	* @param[in] cb_cancel		通知取消登录的回调函数
	* @param[in] cb_hide		通知隐藏登录窗体的回调函数
	* @param[in] cb_destroy		通知销毁登录窗体的回调函数
	* @param[in] cb_show_main	通知显示主窗体的回调函数
	* @return void 无返回值
	*/
	void RegLoginManagerCallback(const OnLoginResult& cb_result,
		const OnCancelLogin& cb_cancel,
		const OnHideWindow& cb_hide,
		const OnDestroyWindow& cb_destroy,
		const OnShowMainWindow& cb_show_main
		);
public:

	void InvokeShowMainForm();
	void InvokeLoginResult(int error);
	void InvokeCancelLogin();
	void InvokeHideWindow();
	void InvokeDestroyWindow();

	bool IsLoginFormValid();
private:
	OnLoginResult cb_login_result_;
	OnCancelLogin cb_cancel_login_;
	OnHideWindow cb_hide_window_;
	OnDestroyWindow cb_destroy_window_;
	OnShowMainWindow cb_show_main_window_;
};

}
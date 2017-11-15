#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{
	typedef std::function<void(int error)> OnLoginError;
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
	/**
	* 进行登录操作
	* @param[in] user 用户帐号
	* @param[in] pass 用户密码
	* @return void	无返回值
	*/
	void DoLogin(std::string user, std::string pass);

	/**
	* 退出登录
	* @param[in] over 外部调用此函数时，over设置为false。true表示直接退出程序，内部使用
	* @param[in] type 退出原因（主动退出或被踢）
	* @return void	无返回值
	*/
	void DoLogout(bool over, nim::NIMLogoutType type = nim::kNIMLogoutAppExit);

	/**
	* 取消登录
	* @return void	无返回值
	*/
	void CancelLogin();

	/**
	* 获取已经的登录帐号
	* @return string 已登录帐号
	*/
	std::string GetAccount();

	/**
	* 判断是否为已登录账号
	* @return bool true 是，false 否
	*/
	bool IsEqual(const std::string& account);

	/**
	* 当前是否连接到服务器
	* @return bool true 是，false 否
	*/
	bool IsLinkActive();

	/**
	* 检查是否重复登录帐号
	* @param[in] username 用户id
	* @return bool	true：没有重复，false：重复
	*/
	bool CheckSingletonRun(const std::wstring& username);

	/**
	* 注册登录窗体的回调函数，用来让UI组件控制登录窗体行为，登陆之前应该调用此函数注册相关回调
	* @param[in] cb_result		通知登录错误并返回错误原因的回调函数
	* @param[in] cb_cancel		通知取消登录的回调函数
	* @param[in] cb_hide		通知隐藏登录窗体的回调函数
	* @param[in] cb_destroy		通知销毁登录窗体的回调函数
	* @param[in] cb_show_main	通知显示主窗体的回调函数
	* @return void 无返回值
	*/
	void RegLoginManagerCallback(const OnLoginError& cb_result,
		const OnCancelLogin& cb_cancel,
		const OnHideWindow& cb_hide,
		const OnDestroyWindow& cb_destroy,
		const OnShowMainWindow& cb_show_main
		);

	void RegLoginOKCallback(const OnShowMainWindow& cb_show_main) { cb_show_main_window_ = cb_show_main; }
public:
	/**
	* 通知显示主窗体
	* @return void 无返回值
	*/
	void InvokeShowMainForm();

	/**
	* 通知登录错误并返回错误原因
	* @param[in] error	错误码
	* @return void 无返回值
	*/
	void InvokeLoginError(int error);

	/**
	* 通知取消登录
	* @return void 无返回值
	*/
	void InvokeCancelLogin();

	/**
	* 通知隐藏登录窗体
	* @return void 无返回值
	*/
	void InvokeHideWindow();

	/**
	* 通知销毁登录窗体
	* @return void 无返回值
	*/
	void InvokeDestroyWindow();

	/**
	* 判断登录窗口是否有效
	* @return bool true 有效，false 无效
	*/
	bool IsLoginFormValid();
private:
	LoginManager(){};
	~LoginManager(){};

	OnLoginError cb_login_error_;
	OnCancelLogin cb_cancel_login_;
	OnHideWindow cb_hide_window_;
	OnDestroyWindow cb_destroy_window_;
	OnShowMainWindow cb_show_main_window_;
};

}
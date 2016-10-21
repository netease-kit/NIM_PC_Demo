#pragma once

/** @class LoginForm
  * @brief 登录窗体
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @date 2016/10/12
  */
class LoginForm : public nim_comp::WindowEx
{
public:
	LoginForm();
	~LoginForm();
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	
	/**
	* 拦截并处理WM_CLOSE消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return LRESULT 处理结果
	*/
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 注册UIKIT回调函数，让UIKIT控制登录窗体的一些行为
	* @return void	无返回值
	*/
	void RegLoginManagerCallback();

	/**
	* 响应登录结果的回调，
	* @return void	无返回值
	*/
	void OnLoginError(int error);

	/**
	* 响应取消登录的回调，重置界面控件效果
	* @return void	无返回值
	*/
	void OnCancelLogin();

private:
	/**
	* 检查用户输入内容，如果输入合法则开始登录
	* @return void	无返回值
	*/
	void DoBeforeLogin();

	/**
	* 检查用户输入内容，如果输入合法则注册帐号
	* @return void	无返回值
	*/
	void DoRegisterAccount();

	/**
	* 执行登录操作
	* @param[in] username 用户id
	* @param[in] password 用户密码
	* @return void	无返回值
	*/
	void StartLogin(std::string username, std::string password);

	/**
	* 在界面上显示一些提示文字
	* @return void	无返回值
	*/
	void ShowLoginTip(std::wstring tip_text);
public:
	static const LPCTSTR kClassName;
private:
	ui::Control*	usericon_;
	ui::Control*	passwordicon_;
	ui::RichEdit*	user_name_edit_;
	ui::RichEdit*	nick_name_edit_;
	ui::RichEdit*	password_edit_;
	ui::Control*	login_ing_tip_;
	ui::Label*		login_error_tip_;
	ui::Label*		register_ok_toast_;
	ui::Button*		btn_login_;
	ui::Button*		btn_register_;
	ui::Button*		btn_cancel_;
};

#pragma once

class LoginForm : public nim_comp::WindowEx
{
public:
	LoginForm();
	~LoginForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT OnClose(UINT u, WPARAM w, LPARAM l, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked( ui::EventArgs* msg );

	void RegLoginManagerCallback(); //注册回调函数，让ui_kit控制登录窗体的一些行为
	void OnLoginResult(int error);
	void Reset(); //取消登录
private:
	void DoBeforeLogin();
	void DoRegisterAccount();
	void StartLogin(std::string username, std::string password);
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

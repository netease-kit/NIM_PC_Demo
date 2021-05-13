/*
MsgBoxRet ret =	MsgBox(L"TITLE_ID", L"TEXT_ID")
	->SetIcon(L"warning.png")					//小图标
	->AddButton(L"BUTTON_TEXT_ID")				//添加按钮，按钮个数1<= N <=3
	->AddButton(L"BUTTON_TEXT_ID")				//
	->AddButton(L"BUTTON_TEXT_ID")				//
	->AsynShow(m_hWnd, cb);						//显示消息框（伪模态），真正模态请使用ShowModal
*/
#ifndef MSG_BOX_H_
#define MSG_BOX_H_
#include "stdafx.h"

class MsgForm;

//返回值
enum MsgBoxRet { kMsgBtnClose = 0, kMsgBtn1 = 1, kMsgBtn2 = 2, kMsgBtn3 = 4,  kMsgCheck = 64 };

//图标类型
enum MsgBoxIconType { kMsgIconQuestion = 0, kMsgIconTip, kMsgIconWarn, kMsgIconError, kMsgIconOK };

MsgForm* MsgBox();
MsgForm* MsgBox(std::wstring titleID, std::wstring inforID);

class MsgForm : public ui::WindowImplBase
{
	struct MsgBoxButton
	{
		std::wstring text_;  //文本
		bool		 gray_;  //状态图样式
	};
public:
	typedef std::function<void(MsgBoxRet)> MsgboxCallback; 
	typedef std::function<void(int)> MsgboxCallback2; 

	MsgForm();
	virtual ~MsgForm();

public:
	MsgForm* SetTitle(std::wstring titleID);
	MsgForm* SetIcon(MsgBoxIconType type);
	MsgForm* SetInfor(std::wstring inforID, bool is_id = true);
	MsgForm* SetInfor2(std::wstring infor2){ infor2_ = infor2; return this; }
	MsgForm* SetPathInfo(std::wstring path_info){ path_info_ = path_info; return this; }
	MsgForm* SetCheckBox(std::wstring checkID, bool checked = false);
	MsgForm* AddButton(std::wstring strID, bool gray = false);
	int AsynShow(HWND parent_hwnd, const MsgboxCallback& msgbox_callback);
	int AsynShow(HWND parent_hwnd, const MsgboxCallback2& msgbox_callback); //带CheckBox的使用这个回调

public:
	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

public:
	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual UINT GetClassStyle() const override;
	virtual void OnFinalMessage( HWND hWnd );
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);

private:
	std::wstring GetWindowId() const;
	MsgBoxRet ShowModal(HWND hwnd);   //一般情况都使用AsynShow，特殊情况需要使用ShowModal的话，需要加为此类的友元类，如：friend MainThread;
	void AdjustContent();	// 调整内容区的位置
	void AdjustButtons();	// 调整按钮们的位置
	void OnClicked(MsgBoxRet ret);
	void ActiveWindow();

public:
	static const LPCTSTR kClassName;
	static const LPCTSTR kCheckBoxName;

private:
	std::wstring	title_;		//窗口标题

	MsgBoxIconType	icon_;		//图标
	std::wstring	infor_;		//提示信息
	std::wstring	path_info_;		//提示信息
	std::wstring	infor2_;		//提示信息
	bool			is_use_infor_id_;

	bool			has_check_;
	std::wstring	check_;		//复选框
	bool			is_checked_;

	MsgBoxButton	button_[3];		// 3个按钮
	int				buttons_;       // 按钮个数

private:
	//TabKeyManager	 tabkey_manager_;
	MsgboxCallback	 msgbox_callback_;
	MsgboxCallback2  msgbox_callback2_;
};

#endif // MSG_BOX_H_

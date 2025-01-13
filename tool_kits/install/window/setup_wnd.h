
#ifndef SETUP_WND_H_
#define SETUP_WND_H_
#include "stdafx.h"
#include "msg_box.h"

#define PROGRESS_DELFILE		10.0
#define PROGRESS_UNZIP			80.0
#define PROGRESS_CREATE_LINK	5.0
#define PROGRESS_WRITE_REG		5.0

class SetupForm : public ui::WindowImplBase
{
public:
	SetupForm(bool update);
	virtual ~SetupForm();

	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

public:
	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override;
	virtual UINT GetClassStyle() const override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void InitWindow() override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual bool Notify(ui::EventArgs* msg);

private:
	void OnFinalMessageEx(HWND hWnd);
	//选择安装路径
	void SelectPath();
	void SelectPathCallBack(BOOL ret, std::wstring path);
	//准备安装
	void SetupCheck();
	//开始安装
	void Setup();
	//开始删除旧文件
	void DelFile();
	//开始解压
	void BeginUnzip();
	//解压结束
	void EndUnzip(std::wstring tip, uint32_t res);
	//安装可在发行组件包
	void InstallRedist();
	//创建快捷方式等
	void CreateLink();
	//写注册表
	void WriteRegList();
	void EndSetupCallback(std::wstring tip, uint32_t res);
	uint32_t ShowUnzipMsg(std::wstring tip, std::wstring path = L"", std::wstring tip2 = L"");
	void ShowUnzipMsgUI(std::wstring tip, std::wstring path, std::wstring tip2);
	void ShowMsgCallBack(MsgBoxRet res);
	void SetProgressCurStepPos(uint32_t pos);
	void ShowProgress(uint32_t pos);
	//获取剩余空间
	double GetUserFreeSpace(std::wstring path);
	//检查路径
	void CheckSetupPath();
	//开始动画
	void StartPathBoxAnimationPlayer();
	//动画中
	void PathBoxAnimationPlayer();


private:
	ui::Box*		box_setup_1_ = NULL;
	ui::Box*		box_setup_2_ = NULL;
	ui::Box*		box_path_ = NULL;
	ui::Box*		box_fast_setup_ = NULL;
	ui::CheckBox*	check_path_ = NULL;
	ui::RichEdit*   richedit_path_ = NULL;
	ui::Progress*	progress_ = NULL;
	ui::Label*		progress_pos_ = NULL;
	ui::Label*		free_space_label_ = NULL;
	ui::Label*		path_err_label_ = NULL;
	ui::Button*		btn_setup_ = NULL;
	ui::CheckBox*	check_read_ = NULL;

	std::wstring	last_setup_path_;
	bool			update_yixin_;
	bool			start_setup_ = false;

	//动画weakfalg
	nbase::WeakCallbackFlag pathbox_timer_weakfalg_;

	static int32_t show_msg_res_;
	static bool destroy_wnd_;
	static uint32_t pre_progress_pos_;
	static const LPCTSTR kClassName;
};

#endif // SETUP_WND_H_

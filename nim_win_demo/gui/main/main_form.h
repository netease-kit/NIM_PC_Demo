#pragma once

#include "gui/tray_icon/tray_icon.h"
#include "shared/ui/ui_menu.h"
#include "base/framework/task.h"


/** @class MainForm
  * @brief demo程序主界面窗口类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class MainForm : 
	public nim_comp::WindowEx,
	public TrayIconDelegate
{
public:
	MainForm();
	~MainForm();

	virtual void LeftClick() override;
	virtual void LeftDoubleClick() override;
	virtual void RightClick() override;

	/**
	* 虚函数，指定本界面的xml布局文件和图片素材所在的目录的相对路径
	* @return std::wstring 返回该目录相对于[安装目录]/bin/themes/default的路径
	*/
	virtual std::wstring GetSkinFolder() override;

	/**
	* 虚函数，指定本界面的xml布局文件名
	* @return std::wstring 返回该文件名
	*/
	virtual std::wstring GetSkinFile() override;

	/**
	* 虚函数，指定本界面使用的资源文件类型（xml或者zip），目前仅支持xml类型
	* @return ui::UILIB_RESOURCETYPE 返回一个枚举值，表示资源文件类型
	*/
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void OnFinalMessage(HWND hWnd);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnEsc(BOOL &bHandled);

	/**
	* 虚函数，处理系统定义的一些UI事件（如窗口创建，窗口关闭，窗口大小变化，鼠标被点击，键盘按键被按下）。
	* @param[in] uMsg	系统定义的事件类型
	* @param[in] wParam	uMsg类型限定下，更详细的事件类型
	* @param[in] lParam	wParam类型限定下，更详细的事件类型
	* @return LRESULT	返回系统定义的整型值，表示处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 虚函数，创建WindowEx对象时会自动执行，完成控件的初始化和控件事件处理函数的绑定。
	* @return void 无返回值
	*/
	virtual void InitWindow() override;

	/**
	* Notify, OnCloseBtnClicked, OnClicked: 自定义的界面事件响应函数，一般在InitWindow方法中注册。
	* @param[in] msg	事件内容
	* @return bool	返回处理结果
	*/
	bool Notify(ui::EventArgs* msg);
	bool OnCloseBtnClicked(ui::EventArgs* msg);
	bool OnClicked(ui::EventArgs* msg);

private:
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);
	void OnUnreadCountChange(int unread_count);
	void InitHeader();

	bool SearchEditChange(ui::EventArgs* param);
	bool OnClearInputBtnClicked(ui::EventArgs* param);
	bool MainMenuButtonClick(ui::EventArgs* param);

	void PopupMainMenu(POINT point);
	void PopupMainTrayMenu(POINT point);
	bool LookLogMenuItemClick(ui::EventArgs* param);
	bool FileTransMenuItemClick(ui::EventArgs* param);
	bool AddressMenuItemClick(ui::EventArgs* param);
	bool ExportMsglogMenuItemClick(ui::EventArgs* param);
	bool ImportMsglogMenuItemClick(ui::EventArgs* param);
	bool ClearChatRecordMenuItemClick(bool del_session, ui::EventArgs* param);
	bool ClearChatRecordBySessionTypeMenuItemClick(bool del_session, nim::NIMSessionType type, ui::EventArgs* param);
	bool VChatSettingMenuItemClick(ui::EventArgs* param);
	bool RtsReplayMenuItemClick(ui::EventArgs* param);
	bool SessionListMenuItemClick(ui::EventArgs* param);
	bool AboutMenuItemClick(ui::EventArgs* param);
	bool LogoffMenuItemClick(ui::EventArgs* param);
	bool QuitMenuItemClick(ui::EventArgs* param);

public:
	static const LPCTSTR kClassName;

private:
	ui::Button*		btn_header_;
	ui::Label*		label_name_;

	ui::Box*		box_unread_;
	ui::Label*		label_unread_;

	ui::RichEdit*	search_edit_;
	ui::Button*		btn_clear_input_;
	ui::ListBox*	search_result_list_;
	bool			is_trayicon_left_double_clicked_;

	AutoUnregister	unregister_cb;
};

using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(MainForm);

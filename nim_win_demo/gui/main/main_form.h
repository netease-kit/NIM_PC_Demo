#pragma once

#include "gui/tray_icon/tray_icon.h"
#include "shared/ui/ui_menu.h"
#include "shared/ui/msgbox.h"
#include "base/framework/task.h"
#include "gui/main/control/custom_button_box.h"


/** @class MainForm
  * @brief demo程序主界面窗口类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class MainForm : 
	public nim_comp::WindowEx,
	public ITrayIconDelegate
{
public:
	MainForm();
	~MainForm();

	/**
	* 虚函数，指定本界面的xml布局文件和图片素材所在的目录的相对路径
	* @return std::wstring 返回该目录相对于[安装目录]/bin/themes/default的路径
	*/
	virtual std::wstring GetSkinFolder() override;

	//覆盖虚函数
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	/**
	* 处理窗口销毁消息
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd);

	/**
	* 拦截并处理WM_CLOSE消息
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return void	无返回值
	*/
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/**
	* 拦截ESC键的消息处理
	* @param[in out] bHandled 是否处理了消息，如果处理了不继续传递消息
	* @return void	无返回值
	*/
	virtual void OnEsc(BOOL &bHandled);

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

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
	* 处理主窗口弹出菜单按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool MainMenuButtonClick(ui::EventArgs* param);

	/**
	* 弹出主菜单
	* @param[in] point 弹出坐标
	* @return void	无返回值
	*/
	void PopupMainMenu(POINT point);

	/**
	* 处理查看日志目录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool LookLogMenuItemClick(ui::EventArgs* param);

	/**
	* 处理我的手机菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool FileTransMenuItemClick(ui::EventArgs* param);

	/**
	* 处理刷新通讯录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool AddressMenuItemClick(ui::EventArgs* param);

	/**
	* 处理导出聊天记录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ExportMsglogMenuItemClick(ui::EventArgs* param);

	/**
	* 处理导入聊天记录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ImportMsglogMenuItemClick(ui::EventArgs* param);

	/**
	* 处理清空所有聊天记录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ClearChatRecordMenuItemClick(bool del_session, ui::EventArgs* param);

	/**
	* 处理清空记录菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ClearChatRecordBySessionTypeMenuItemClick(bool del_session, nim::NIMSessionType type, ui::EventArgs* param);

	/**
	* 处理音视频设置菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool VChatSettingMenuItemClick(ui::EventArgs* param);

	/**
	* 处理白板回放菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool RtsReplayMenuItemClick(ui::EventArgs* param);

	/**
	* 处理适配DPI菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool AdaptDpiMenuItemClick(ui::EventArgs* param);

	/**
	* 显示语言列表
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool ShowLanguageList(ui::EventArgs* param);

	/**
	* 关闭语言列表
	* @param[in] msg 消息的相关信息
	* @param[in] check_mouse true: 检查到鼠标在语言列表上就不关闭；false：不检查鼠标位置，直接关闭
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool CloseLanguageList(ui::EventArgs* param, bool check_mouse);

	/**
	* 选择语言
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelectLanguage(ui::EventArgs* param);

	/**
	* 选择语言的回调
	* @param[in] ret 选择的按钮
	* @return void 无返回值
	*/
	void OnSelectLanguageCallback(MsgBoxRet ret, const std::string& language);

	/**
	* 处理显示会话列表菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool SessionListMenuItemClick(ui::EventArgs* param);

	/**
	* 处理关于菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool AboutMenuItemClick(ui::EventArgs* param);

	/**
	* 处理注销菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool LogoffMenuItemClick(ui::EventArgs* param);

	/**
	* 处理退出菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool QuitMenuItemClick(ui::EventArgs* param);

	/**
	* 在线状态菜单按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnlineStateMenuButtonClick(ui::EventArgs* param);

	/**
	* 在线状态菜单项的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnlineStateMenuItemClick(ui::EventArgs* param);

	/**
	* 如果断网重连了则重新发送在线状态
	* @param[in] json 断网重连信息
	* @return void	无返回值
	*/
	void CheckOnlineState(const Json::Value& json);

	/**
	* 设置在线状态
	* @return void	无返回值
	*/
	void SetOnlineState();

	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;

public:
	/**
	* 实现ITrayIconDelegate接口函数
	* @return void	无返回值
	*/
	virtual void LeftClick() override;
	virtual void LeftDoubleClick() override;
	virtual void RightClick() override;

private:
	/**
	* 处理搜索栏内容改变的消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool SearchEditChange(ui::EventArgs* param);

	/**
	* 处理清理搜索栏按钮的单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClearInputBtnClicked(ui::EventArgs* param);

	/**
	* 弹出托盘菜单
	* @param[in] point 弹出坐标
	* @return void	无返回值
	*/
	void PopupTrayMenu(POINT point);

private:
	/**
	* 响应个人资料改变的回调函数
	* @param[in] uinfos 新的个人资料列表
	* @return void 无返回值
	*/
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);

	/**
	* 响应头像下载完成的回调函数
	* @param[in] type 头像类型
	* @param[in] account 头像下载完成的用户id
	* @param[in] photo_path 头像本地路径
	* @return void 无返回值
	*/
	void OnUserPhotoReady(PhotoType type, const std::string& account, const std::wstring& photo_path);

	/**
	* 响应头最近会话列表未读消息总数改变的回调函数
	* @param[in] unread_count 未读总数
	* @return void 无返回值
	*/
	void OnUnreadCountChange(int unread_count);

	/**
	* 初始化用户头像
	* @return void	无返回值
	*/
	void InitHeader();
	bool OnReturnEventsClick(ui::EventArgs* param);

public:
	static const LPCTSTR kClassName;

private:
	ui::Button*		btn_header_;
	ui::Label*		label_name_;
	ui::Button*		btn_online_state_;
	ui::HBox* option_panel_ = nullptr;
	bool			is_busy_;

	nim_comp::CustomButtonBox*	box_unread_;
	ui::Label*		label_unread_;

	ui::RichEdit*	search_edit_;
	ui::Button*		btn_clear_input_;
	ui::ListBox*	search_result_list_;
	bool			is_trayicon_left_double_clicked_;

	AutoUnregister	unregister_cb;
};

using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(MainForm);

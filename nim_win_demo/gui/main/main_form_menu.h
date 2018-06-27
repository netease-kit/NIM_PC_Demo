#pragma once
#include "guiex/main/main_form_menu.h"
class MainFormMenu : public nim_comp::IMainFormMenuHandler, public nbase::SupportWeakCallback
{
protected:
	virtual void OnPopupMainMenu(POINT point) override;
private:
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

};
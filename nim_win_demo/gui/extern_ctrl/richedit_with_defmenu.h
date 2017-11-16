
#pragma once
#include "shared\ui\ui_menu.h"
/** @class RichEditWithDefMenu
* @brief 带缺省右键菜单的edit
* @copyright (c) 2017, NetEase Inc. All rights reserved
* @date 2017/10/17
*/
class MenuBtnHandel;
class RichEditWithDefMenu : public ui::RichEdit
{
public:
	RichEditWithDefMenu();
	virtual ~RichEditWithDefMenu();
	virtual void DoInit();
private:
	//创建并初始化菜单窗口
	ui::CMenuWnd* CreateMenuWnd(const POINT& mouse_pos);
	bool IsMenuButtonShow(const std::wstring& name);
	virtual void SetAttribute(const std::wstring& pstrName, const std::wstring& pstrValue);	
	bool OnMenu(ui::EventArgs* event);
	void ShowDefaultMenu(ui::EventArgs* event);
	//注册菜单按钮处理对象
	void RegisterMenuButtonHandel();
	void AddShowMenuButton(const std::wstring& name);
	bool OnMenuWndClose(ui::EventArgs* event);
private:	
	std::map<std::wstring,int> show_menu_list_ ;	
	std::map<std::wstring,std::shared_ptr<MenuBtnHandel>> menu_button_handel_;
	static const std::wstring kMenuBTNName_Cut;
	static const std::wstring kMenuBTNName_Copy;
	static const std::wstring kMenuBTNName_Paste;
	static const std::wstring kMenuUIConfigPath;
};
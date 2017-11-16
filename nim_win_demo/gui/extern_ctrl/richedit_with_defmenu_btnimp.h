#pragma once
namespace ui{
	class CMenuElementUI;
}
class RichEditWithDefMenu;
/** @class MenuBtnHandel
* @brief 菜单按钮处理对象基类
* @copyright (c) 2017, NetEase Inc. All rights reserved
* @date 2017/10/17
*/
class MenuBtnHandel : public nbase::SupportWeakCallback
{
public:
	MenuBtnHandel(RichEditWithDefMenu* edit, const std::wstring& name);
	virtual ~MenuBtnHandel();
public:
	virtual void Reset(ui::CMenuElementUI* menu_element, unsigned sel_start, unsigned sel_end);
	virtual std::wstring GetMenuName() const;
protected:
	virtual void DoReset(ui::CMenuElementUI* menu_element) = 0;
protected:
	RichEditWithDefMenu* edit_;
	std::wstring menu_name_;
	unsigned sel_start_;
	unsigned sel_end_;
};
/** @class MenuBtnHandel
* @brief 菜单按钮(copy) 实现
* @copyright (c) 2017, NetEase Inc. All rights reserved
* @date 2017/10/17
*/
class MenuBtnHandel_Copy : public MenuBtnHandel
{
public:
	MenuBtnHandel_Copy(RichEditWithDefMenu* edit);
protected:
	virtual void DoReset(ui::CMenuElementUI* menu_element);
};
/** @class MenuBtnHandel
* @brief 菜单按钮(cut) 实现
* @copyright (c) 2017, NetEase Inc. All rights reserved
* @date 2017/10/17
*/
class MenuBtnHandel_Cut : public MenuBtnHandel
{
public:
	MenuBtnHandel_Cut(RichEditWithDefMenu* edit);
protected:
	virtual void DoReset(ui::CMenuElementUI* menu_element);
};
/** @class MenuBtnHandel
* @brief 菜单按钮(paste) 实现
* @copyright (c) 2017, NetEase Inc. All rights reserved
* @date 2017/10/17
*/
class MenuBtnHandel_Paste : public MenuBtnHandel
{
public:
	MenuBtnHandel_Paste(RichEditWithDefMenu* edit);
protected:
	virtual void DoReset(ui::CMenuElementUI* menu_element);
};
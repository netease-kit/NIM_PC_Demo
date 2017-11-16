#pragma once
#include "base/memory/singleton.h"
/** @class ExternCtrlManager
* @brief 扩展控件管理器
* @copyright (c) 2015, NetEase Inc. All rights reserved
* @date 2017/10/16
*/
class ExternCtrlManager 
{
public:
	ExternCtrlManager();
	virtual ~ExternCtrlManager();
	SINGLETON_DEFINE(ExternCtrlManager);
public:
	static ui::Control* CreateExternCtrl (const std::wstring& );
};
#pragma once
#include "base/synchronization/lock.h"
#include "db/db_sqlite3.h"
#include "shared/db_helper.h"
#include "cef_control/cef_control.h"

namespace nim_comp
{

using namespace ui;

/** @class CefGlobalMethods
  * @brief 提供 CefControl 控件与 JS 交互的一些全局方法
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class CefGlobalFunctions : public nbase::SupportWeakCallback
{
public:
	CefGlobalFunctions(CefControl* cef_control);
	~CefGlobalFunctions();

	void Init();

private:
	void OnShowMsgBox(const std::string& params, nim_cef::ReportResultFunction callback);

private:
	CefControl* cef_control_;

};
}
#pragma once

#include "nim_ui_dll.h"
#include "module/service/notify_center.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class NotifyCenter
  * @brief 事件通知中心
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/4/20
  */
class NIM_UI_DLL_API NotifyCenter
{
public:
	SINGLETON_DEFINE(NotifyCenter);

public:
	/**
	* 注册事件通知的回调函数
	* @param[in] type 事件类型
	* @param[in] notify 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegNotify(NotifyType type, OnNotify notify);

private:

	NotifyCenter() {}
	~NotifyCenter() {}
};

}
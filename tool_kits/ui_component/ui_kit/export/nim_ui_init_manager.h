#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{

/** @class InitManager
  * @brief 提供nim sdk初始化接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API InitManager
{
public:
	SINGLETON_DEFINE(InitManager);
public:
	InitManager();
	~InitManager();
public:
	void InitUiKit();
	void CleanupUiKit();
};

}
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
	InitManager(){};
	~InitManager(){};
public:
	/**
	* 初始化UI组件（应该在程序初始化时调用）
	* @return void	无返回值
	*/
	void InitUiKit();

	/**
	* 清理UI组件
	* @return void	无返回值
	*/
	void CleanupUiKit();
};

}
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
	enum InitMode
	{
		kIM,
		kAnonymousChatroom,
	};

public:
	/**
	* 初始化UI组件（应该在程序初始化时调用）
	* @param[in] enable_subscribe_event	是否启用事件订阅模块
	* @return void	无返回值
	*/
	void InitUiKit(bool enable_subscribe_event, InitMode mode);

	/**
	* 清理UI组件
	* @return void	无返回值
	*/
	void CleanupUiKit();

private:
	InitManager(){};
	~InitManager(){};

private:
	bool	init_ = false;
};

}
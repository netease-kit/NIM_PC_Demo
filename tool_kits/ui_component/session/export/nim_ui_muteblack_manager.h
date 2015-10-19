#pragma once

#include "nim_ui_dll.h"
#include "module/service/mute_black_service.h"

namespace nim_ui
{

/** @class MuteBlackManager
  * @brief 提供静音/黑名单列表控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API MuteBlackManager
{
public:
	SINGLETON_DEFINE(MuteBlackManager);
public:
	MuteBlackManager();
	~MuteBlackManager();
public:
	const std::set<std::string> &GetMuteList();					//获取静音列表
	const std::set<std::string> &GetBlackList();				//获取黑名单列表
	bool IsInMuteList(const std::string& id);					//检查用户是否在静音列表中
	bool IsInBlackList(const std::string& id);					//检查用户是否在黑名单中
	void InvokeSetMute(const std::string& id, bool mute);		//改变某个用户的静音状态
	void InvokeSetBlack(const std::string& id, bool black);		//改变某个用户的拉黑状态
	UnregisterCallback RegSyncSetMuteCallback(const SetStateCallback& cb);	//注册静音事件的响应函数
	UnregisterCallback RegSyncSetBlackCallback(const SetStateCallback& cb);	//注册黑名单事件的响应函数

};

}
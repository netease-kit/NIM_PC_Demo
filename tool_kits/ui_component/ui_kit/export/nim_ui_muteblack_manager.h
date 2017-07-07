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
	/**
	* 获取静音名单
	* @return std::set<std::string> 被静音的用户id列表
	*/
	const std::set<std::string> &GetMuteList();

	/**
	* 获取黑名单
	* @return std::set<std::string> 被拉入黑名单的用户id列表
	*/
	const std::set<std::string> &GetBlackList();

	/**
	* 判断某个用户是否在静音列表中
	* @param[in] id 用户id
	* @return bool true 是，false 否
	*/
	bool IsInMuteList(const std::string& id);

	/**
	* 判断某个用户是否在黑名单中
	* @param[in] id 用户id
	* @return bool true 是，false 否
	*/
	bool IsInBlackList(const std::string& id);

	/**
	* 改变判断某个用户的静音状态
	* @param[in] id 帐号id
	* @param[in] bool 是否设为静音
	* @return void	无返回值
	*/
	void InvokeSetMute(const std::string& id, bool mute);

	/**
	* 改变判断某个用户的黑名单状态
	* @param[in] id 帐号id
	* @param[in] bool 是否拉入黑名单
	* @return void	无返回值
	*/
	void InvokeSetBlack(const std::string& id, bool black);

	/**
	* 注册静音事件的响应函数
	* @param[in] cb 回调函数
	* @return UnregisterCallback	反注册对象
	*/
	UnregisterCallback RegSyncSetMuteCallback(const SetStateCallback& cb);

	/**
	* 注册黑名单事件的响应函数
	* @param[in] cb 回调函数
	* @return UnregisterCallback	反注册对象
	*/
	UnregisterCallback RegSyncSetBlackCallback(const SetStateCallback& cb);

private:
	MuteBlackManager(){};
	~MuteBlackManager(){};
};

}
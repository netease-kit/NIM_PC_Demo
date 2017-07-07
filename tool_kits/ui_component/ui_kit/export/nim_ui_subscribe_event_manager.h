#pragma once

#include "nim_ui_dll.h"
#include "module/subscribe_event/subscribe_event_manager.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class SubscribeEventManager
  * @brief 提供订阅事件有关的接口
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/4/6
  */
class NIM_UI_DLL_API SubscribeEventManager
{
public:
	SINGLETON_DEFINE(SubscribeEventManager);

public:
	/**
	* 启用/禁用事件订阅模块
	* @param[in] enable	是否启用事件订阅模块
	* @return void	无返回值
	*/
	void SetEnabled(bool enable);

	/**
	* 是否启用了事件订阅模块
	* @return bool	true:启用, false : 禁用
	*/
	bool IsEnabled() const;

	/**
	* 获取事件信息
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[out] data 事件
	* @return void	无返回值
	*/
	bool GetEventData(int event_type, const std::string &accid, nim_comp::EventDataEx &data);

	/**
	* 订阅超时后自动重新订阅事件
	* @return void	无返回值
	*/
	void StartAutoSubscribe();

	/**
	* 订阅指定用户的事件
	* @param[in] event_type 事件类型
	* @param[in] subscribe_account 订阅用户uid
	* @return void	无返回值
	*/
	void SubscribeEvent(int event_type, const std::string &subscribe_account);

	/**
	* 订阅指定用户的事件
	* @param[in] event_type 事件类型
	* @param[in] subscribe_list 订阅列表
	* @return void	无返回值
	*/
	void SubscribeEvent(int event_type, const std::list<std::string> &subscribe_list);

	/**
	* 取消订阅指定用户的事件
	* @param[in] event_type 事件类型
	* @param[in] unsubscribe_account 取消订阅用户uid
	* @return void	无返回值
	*/
	void UnSubscribeEvent(int event_type, const std::string &unsubscribe_account);

	/**
	* 取消订阅指定用户的事件
	* @param[in] event_type 事件类型
	* @param[in] unsubscribe_list 取消订阅列表
	* @return void	无返回值
	*/
	void UnSubscribeEvent(int event_type, const std::list<std::string> &unsubscribe_list);

	/**
	* 生成忙碌状态事件
	* @param[in] busy 是否忙碌
	* @return EventData	事件数据
	*/
	static nim::EventData CreateBusyEvent(bool busy);

private:
	SubscribeEventManager() {}
	~SubscribeEventManager() {}
};

}
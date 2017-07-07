#pragma once
#include "module/subscribe_event/online_state_event_helper.h"

namespace nim_comp
{

struct EventDataEx
{
	nim::EventData				data_;
	nim::EventOnlineClientType	online_client_;
	EventMultiConfig			multi_config_;
};
typedef std::function<void(int event_type, const std::string &accid, const EventDataEx &data)> OnReceiveEventCallback;

/** @class SubscribeEventManager
  * @brief 订阅事件管理类，用于统一管理订阅事件功能
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/03/31
  */
class SubscribeEventManager : public nbase::SupportWeakCallback
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
	* 接收订阅的事件的回调
	* @param[in] event_list	订阅的事件
	* @return void	无返回值
	*/
	void OnPushEventCallback(const std::list<nim::EventData>& event_list);

	/**
	* 注册接收事件的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegReceiveEventCallback(const OnReceiveEventCallback& callback);

	/**
	* 获取事件信息
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[out] data 事件
	* @return bool	true:成功,false:失败
	*/
	bool GetEventData(int event_type, const std::string &accid, EventDataEx &data);

public:
	/**
	* 订阅超时后自动重新订阅事件
	* @return void	无返回值
	*/
	void StartAutoSubscribe();

	/**
	* 订阅所有好友的事件
	* @return void	无返回值
	*/
	void SubscribeAllFriendEvent();

	/**
	* 订阅最近会话列表中所有陌生人的事件
	* @param[in] session_list 会话列表数据
	* @return void	无返回值
	*/
	void SubscribeAllSessionEvent(const std::list<nim::SessionData>& session_list);

public:
	/**
	* 订阅好友的事件
	* @param[in] event_type 事件类型
	* @param[in] friend_account 订阅用户uid
	* @return void	无返回值
	*/
	void SubscribeFriendEvent(const std::string &friend_account);

	/**
	* 订阅好友的事件
	* @param[in] event_type 事件类型
	* @param[in] friend_list 订阅列表
	* @return void	无返回值
	*/
	void SubscribeFriendEvent(const std::list<std::string> &friend_list);

	/**
	* 取消订阅好友的事件
	* @param[in] event_type 事件类型
	* @param[in] friend_account 取消订阅用户uid
	* @return void	无返回值
	*/
	void UnSubscribeFriendEvent(const std::string &friend_account);

	/**
	* 取消订阅好友的事件
	* @param[in] event_type 事件类型
	* @param[in] friend_list 取消订阅列表
	* @return void	无返回值
	*/
	void UnSubscribeFriendEvent(const std::list<std::string> &friend_list);

public:
	/**
	* 订阅最近会话列表中陌生人的事件
	* @param[in] session 会话列表数据
	* @return void	无返回值
	*/
	void SubscribeSessionEvent(const nim::SessionData& session);

	/**
	* 取消订阅最近会话列表中陌生人的事件
	* @param[in] sessions 会话列表数据
	* @return void	无返回值
	*/
	void UnSubscribeSessionEvent(const nim::SessionData& session);

	/**
	* 取消订阅最近会话列表中陌生人的事件
	* @param[in] session_list 会话列表数据
	* @return void	无返回值
	*/
	void UnSubscribeSessionEvent(const std::list<nim::SessionData>& session_list);

public:
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

private:
	/**
	* 取消订阅所有在线事件，会把其他端的在线事件也都取消了，慎用！
	* @return void	无返回值
	*/
	void UnSubscribeAllOnlineEvent();

	/**
	* 触发接收事件的的回调
	* @param[in] event_type 事件类型
	* @param[in] accid 用户id
	* @param[in] data 事件
	* @return void	无返回值
	*/
	void InvokeReceiveEventCallback(int event_type, const std::string &accid, const EventDataEx &data);

	/**
	* 用户掉线重连后的回调函数
	* @param[in] json 消息体Json，包含是否重连成功的信息
	* @return void 无返回值
	*/
	void OnRelink(const Json::Value &json);

private:
	SubscribeEventManager();
	~SubscribeEventManager() {};

private:
	bool enabled_;
	bool auto_subscribe_;
	std::map<int, std::unique_ptr<OnReceiveEventCallback>> receive_event_cb_list_;	//接收事件回调列表

	typedef std::map<std::string, EventDataEx> EventTypeDataMap;					//<accid,EventDataEx>
	std::map<int, EventTypeDataMap> event_list_;									//<event_type,EventTypeDataMap>
};
}
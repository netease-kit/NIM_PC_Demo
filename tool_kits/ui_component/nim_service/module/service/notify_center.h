#pragma once
#include "shared/auto_unregister.h"
#include <utility>

enum NotifyType
{
	//relink
	NT_LINK,			//json["link"] = bool;
};

typedef std::function<void(const Json::Value&)> OnNotify;

namespace nim_comp
{
/** @class NotifyCenter
  * @brief 事件通知中心
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class NotifyCenter : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(NotifyCenter);
	NotifyCenter() { }
	~NotifyCenter() { }

public:
	/**
	* 注册事件通知的回调函数
	* @param[in] type 事件类型
	* @param[in] notify 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegNotify(NotifyType type, OnNotify notify);

	/**
	* 触发事件通知的的回调
	* @param[in] type 事件类型
	* @param[in] json 通知信息
	* @return void	无返回值
	*/
	void InvokeNotify(NotifyType type, const Json::Value &json);

private:
	struct NotifyItem
	{
		NotifyItem(OnNotify* _notify = nullptr)
		{
			notify.reset(_notify);
		}

		NotifyItem(NotifyItem &&item) throw() 
			: notify(std::move(item.notify))
		{
		}

		NotifyItem& operator=(NotifyItem &&item) throw()
		{
			if (this != &item)
				notify = std::move(item.notify);
			return *this;
		}

		std::unique_ptr<OnNotify> notify;
	};

	std::map<NotifyType, std::map<int, NotifyItem>> map_map_notify_;
};
}
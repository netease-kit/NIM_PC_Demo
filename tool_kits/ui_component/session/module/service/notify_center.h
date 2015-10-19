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
class NotifyCenter : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(NotifyCenter);
	NotifyCenter() { }
	~NotifyCenter() { }

public:
	UnregisterCallback RegNotify(NotifyType type, OnNotify notify);
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
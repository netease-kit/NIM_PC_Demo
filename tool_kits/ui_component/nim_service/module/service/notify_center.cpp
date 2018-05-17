#include "notify_center.h"

namespace nim_comp
{
UnregisterCallback NotifyCenter::RegNotify( NotifyType type, OnNotify notify )
{
	assert( notify );

	OnNotify* new_notify_cb = new OnNotify(notify);
	int cb_id = (int)new_notify_cb;
	map_map_notify_[type][cb_id] = std::move(NotifyItem(new_notify_cb));

	auto unregister = ToWeakCallback([this, type, cb_id]() {
		auto iter = map_map_notify_.find(type);
		if (iter != map_map_notify_.end())
		{
			iter->second.erase(cb_id);
		}
	});
	return unregister;
}

void NotifyCenter::InvokeNotify( NotifyType type, const Json::Value &json )
{
	auto it = map_map_notify_.find(type);
	if(it != map_map_notify_.end())
	{
		auto &evt = it->second;
		for(auto i = evt.begin(); i != evt.end(); i++)
		{
			(*i->second.notify)(json);
		}
	}
}

}
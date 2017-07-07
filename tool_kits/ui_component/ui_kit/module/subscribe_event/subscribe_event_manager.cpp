#include "subscribe_event_manager.h"
#include "module/service/user_service.h"
#include "export/nim_ui_session_list_manager.h"

namespace nim_comp
{
	namespace
	{
		const int kSubscribeEventDays = 1;	// 订阅有效天数
	}

SubscribeEventManager::SubscribeEventManager()
{
	enabled_ = false;
	auto_subscribe_ = false;
	NotifyCenter::GetInstance()->RegNotify(NT_LINK, nbase::Bind(&SubscribeEventManager::OnRelink, this, std::placeholders::_1));
}

void SubscribeEventManager::SetEnabled(bool enable)
{
	enabled_ = enable;
}

bool SubscribeEventManager::IsEnabled() const
{
	return enabled_;
}

void SubscribeEventManager::OnPushEventCallback(const std::list<nim::EventData>& event_list)
{
	if (!enabled_)
		return;

	// 更新event_list_里面的数据
	auto update_event_list = [this](const EventDataEx &data){
		auto iter = event_list_.find(data.data_.event_type_);
		if (iter != event_list_.end())
		{
			EventTypeDataMap &event_map = iter->second;

			// 服务器无法保证事件下发完全有序，所以这里过滤掉老的事件，只有最新的事件才需要被处理
			if (data.data_.readonly_event_time_ < event_map[data.data_.readonly_publisher_accid_].data_.readonly_event_time_)
			{
				QLOG_APP(L"OnPushEventCallback receive old event, type:{0}, value:{1}, event_timetag:{2}, cache_timetag:{3}, publisher:{4}")
					<< data.data_.event_type_ 
					<< data.data_.event_value_
					<< data.data_.readonly_event_time_
					<< event_map[data.data_.readonly_publisher_accid_].data_.readonly_event_time_
					<< data.data_.readonly_publisher_accid_;
				return;
			}
			event_map[data.data_.readonly_publisher_accid_] = data;
		}
		else
		{
			EventTypeDataMap event_map;
			event_map[data.data_.readonly_publisher_accid_] = data;
			event_list_[data.data_.event_type_] = event_map;
		}
		
		InvokeReceiveEventCallback(data.data_.event_type_, data.data_.readonly_publisher_accid_, data);
	};

	for (auto const &iter: event_list)
	{
		EventDataEx data;
		data.data_ = iter;
		data.online_client_.ParseConfig(iter.readonly_nim_config_);
		data.multi_config_ = OnlineStateEventHelper::JsonStringToMultiConfig(iter.readonly_multi_config_);

		update_event_list(data);
	}
}

UnregisterCallback SubscribeEventManager::RegReceiveEventCallback(const OnReceiveEventCallback& callback)
{
	OnReceiveEventCallback* new_callback = new OnReceiveEventCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	receive_event_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		receive_event_cb_list_.erase(cb_id);
	});
	return cb;
}

bool SubscribeEventManager::GetEventData(int event_type, const std::string &accid, EventDataEx &data)
{
	if (!enabled_)
		return false; 

	bool succeed = false;
	auto iter = event_list_.find(event_type);
	if (iter != event_list_.end())
	{
		auto it = iter->second.find(accid);
		if (it != iter->second.end())
		{
			data = it->second;
			succeed = true;
		}
	}

	return succeed;
}

void SubscribeEventManager::StartAutoSubscribe()
{	
	if (!enabled_)
		return;

	if (auto_subscribe_)
		return;

	nbase::ThreadManager::PostRepeatedTask(this->ToWeakCallback([this](){
		this->auto_subscribe_ = true;
		this->SubscribeAllFriendEvent();
		nim_ui::SessionListManager::GetInstance()->InvokeLoadSessionList();
	})
	, nbase::TimeDelta::FromDays(kSubscribeEventDays));
}

void SubscribeEventManager::SubscribeAllFriendEvent()
{
	if (!enabled_)
		return;

	std::list<std::string> subscribe_list;
	const std::map<std::string, nim::FriendProfile>& all_friend = UserService::GetInstance()->GetAllFriendInfos();
	for (auto const &it : all_friend)
		subscribe_list.push_back(it.first);

	if (subscribe_list.empty())
		return;

	this->SubscribeEvent(nim::kNIMEventTypeOnlineState, subscribe_list);
}

void SubscribeEventManager::SubscribeAllSessionEvent(const std::list<nim::SessionData>& session_list)
{
	if (!enabled_)
		return;

	std::list<std::string> subscribe_list;
	for (auto const &it : session_list)
	{
		// 如果是陌生人，则加入到订阅列表
		if (it.type_ == nim::kNIMSessionTypeP2P && UserService::GetInstance()->GetUserType(it.id_) != nim::kNIMFriendFlagNormal)
		{
			subscribe_list.push_back(it.id_);
		}
	}

	if (subscribe_list.empty())
		return;

	this->SubscribeEvent(nim::kNIMEventTypeOnlineState, subscribe_list);
}

void SubscribeEventManager::SubscribeFriendEvent(const std::string &friend_account)
{
	if (friend_account.empty())
		return;

	std::list<std::string> subscribe_list;
	subscribe_list.push_back(friend_account);
	SubscribeFriendEvent(subscribe_list);	
}

void SubscribeEventManager::SubscribeFriendEvent(const std::list<std::string> &friend_list)
{	
	std::list<std::string> subscribe_list;
	for (auto const &it : friend_list)
	{
		// 如果不在最近会话列表中，则加入到订阅列表
		SessionItem *item = nim_ui::SessionListManager::GetInstance()->GetSessionItem(it);
		if (NULL == item || item->GetIsTeam())
		{
			subscribe_list.push_back(it);
		}	
	}

	if (subscribe_list.empty())
		return;
	
	this->SubscribeEvent(nim::kNIMEventTypeOnlineState, subscribe_list);
}

void SubscribeEventManager::UnSubscribeFriendEvent(const std::string &friend_account)
{
	if (friend_account.empty())
		return;

	std::list<std::string> unsubscribe_list;
	unsubscribe_list.push_back(friend_account);
	UnSubscribeFriendEvent(unsubscribe_list);
}

void SubscribeEventManager::UnSubscribeFriendEvent(const std::list<std::string> &friend_list)
{
	std::list<std::string> unsubscribe_list;
	for (auto const &it : friend_list)
	{
		// 如果不在最近会话列表中，则加入到取消订阅列表
		SessionItem *item = nim_ui::SessionListManager::GetInstance()->GetSessionItem(it);
		if (NULL == item || item->GetIsTeam())
		{
			unsubscribe_list.push_back(it);
		}
	}

	if (unsubscribe_list.empty())
		return;

	this->UnSubscribeEvent(nim::kNIMEventTypeOnlineState, unsubscribe_list);
}

void SubscribeEventManager::SubscribeSessionEvent(const nim::SessionData& session)
{
	// 如果是陌生人，则加入到订阅列表
	if (session.type_ == nim::kNIMSessionTypeP2P && UserService::GetInstance()->GetUserType(session.id_) != nim::kNIMFriendFlagNormal)
	{
		this->SubscribeEvent(nim::kNIMEventTypeOnlineState, session.id_);
	}	
}

void SubscribeEventManager::UnSubscribeSessionEvent(const nim::SessionData& session)
{
	std::list<nim::SessionData> unsubscribe_list;
	unsubscribe_list.push_back(session);
	UnSubscribeSessionEvent(unsubscribe_list);
}

void SubscribeEventManager::UnSubscribeSessionEvent(const std::list<nim::SessionData>& session_list)
{
	std::list<std::string> unsubscribe_list;
	for (auto const &it : session_list)
	{
		// 如果是陌生人，则加入到取消订阅列表
		if (it.type_ == nim::kNIMSessionTypeP2P && UserService::GetInstance()->GetUserType(it.id_) != nim::kNIMFriendFlagNormal)
		{
			unsubscribe_list.push_back(it.id_);
		}
	}

	if (unsubscribe_list.empty())
		return;
	
	this->UnSubscribeEvent(nim::kNIMEventTypeOnlineState, unsubscribe_list);
}

void SubscribeEventManager::SubscribeEvent(int event_type, const std::string &subscribe_account)
{
	if (subscribe_account.empty())
		return;

	std::list<std::string> subscribe_list;
	subscribe_list.push_back(subscribe_account);
	SubscribeEvent(event_type, subscribe_list);
}

void SubscribeEventManager::SubscribeEvent(int event_type, const std::list<std::string> &subscribe_list)
{
	if (!enabled_)
		return;

	if (subscribe_list.empty())
		return;

	auto closure = [this](int event_type, const std::list<std::string> &subscribe_ids)
	{
		nim::SubscribeEvent::Subscribe(event_type,
			nbase::TimeDelta::FromDays(kSubscribeEventDays).ToSeconds(),
			nim::kNIMEventSubscribeSyncTypeSync,
			subscribe_ids,
			this->ToWeakCallback([](nim::NIMResCode res_code, int event_type, const std::list<std::string>& faild_list){
			if (res_code != nim::kNIMResSuccess)
			{
				QLOG_ERR(L"SubscribeEventManager Subscribe event error, code:{0}, event_type:{1}") << res_code << event_type;
			}
		}));
	};

	//SDK限制一次服务器订阅数量不超过100
	if (subscribe_list.size() > 100)
	{
		std::list<std::string> ids;
		for (auto iter = subscribe_list.begin(); iter != subscribe_list.end(); ++iter)
		{
			ids.push_back(*iter);
			if (ids.size() == 100)
			{
				closure(event_type, ids);
				ids.clear();
			}
		}
		if (!ids.empty())
		{
			closure(event_type, ids);
			ids.clear();
		}
	}
	else
	{
		closure(event_type, subscribe_list);
	}
}

void SubscribeEventManager::UnSubscribeEvent(int event_type, const std::string &unsubscribe_account)
{
	if (unsubscribe_account.empty())
		return;

	std::list<std::string> unsubscribe_list;
	unsubscribe_list.push_back(unsubscribe_account);
	UnSubscribeEvent(event_type, unsubscribe_list);
}

void SubscribeEventManager::UnSubscribeEvent(int event_type, const std::list<std::string> &unsubscribe_list)
{
	if (!enabled_)
		return;

	if (unsubscribe_list.empty())
		return;

	auto closure = [this](int event_type, const std::list<std::string> &unsubscribe_ids)
	{
		nim::SubscribeEvent::UnSubscribe(event_type,
			unsubscribe_ids,
			this->ToWeakCallback([this, unsubscribe_ids](nim::NIMResCode res_code, int event_type, const std::list<std::string>& faild_list){
			if (res_code != nim::kNIMResSuccess)
			{
				QLOG_ERR(L"SubscribeEventManager Unsubscribe event error, code:{0}, event_type:{1}") << res_code << event_type;
			}
			else
			{
				// 把成功取消订阅的id从缓存中清除
				std::set<std::string> erase_set(unsubscribe_ids.begin(), unsubscribe_ids.end());
				for (auto const &it : faild_list)
				{
					erase_set.erase(it);
				}
	
				auto iter = event_list_.find(event_type);
				if (iter != event_list_.end())
				{
					for (auto const & it : erase_set)
					{
						iter->second.erase(it);
					}
				}
			}
		}));
	};


	//SDK限制一次服务器订阅数量不超过100
	if (unsubscribe_list.size() > 100)
	{
		std::list<std::string> ids;
		for (auto iter = unsubscribe_list.begin(); iter != unsubscribe_list.end(); ++iter)
		{
			ids.push_back(*iter);
			if (ids.size() == 100)
			{
				closure(event_type, ids);
				ids.clear();
			}
		}
		if (!ids.empty())
		{
			closure(event_type, ids);
			ids.clear();
		}
	}
	else
	{
		closure(event_type, unsubscribe_list);
	}
}

void SubscribeEventManager::UnSubscribeAllOnlineEvent()
{
	if (!enabled_)
		return;

	nim::SubscribeEvent::BatchUnSubscribe(nim::kNIMEventTypeOnlineState,
		this->ToWeakCallback([this](nim::NIMResCode res_code, int event_type){
		if (res_code != nim::kNIMResSuccess)
		{
			QLOG_ERR(L"SubscribeEventManager BatchUnSubscribe event error, code:{0}, event_type:{1}") << res_code << event_type;
		}
		else
		{
			event_list_.clear();
		}
	}));
}

void SubscribeEventManager::InvokeReceiveEventCallback(int event_type, const std::string &accid, const EventDataEx &data)
{
	for (auto& it : receive_event_cb_list_)
	{
		(*(it.second))(event_type, accid, data);
	}
}

void SubscribeEventManager::OnRelink(const Json::Value &json)
{
	bool link = json["link"].asBool();
	if (!link)
	{
		event_list_.clear();
	}
}
}
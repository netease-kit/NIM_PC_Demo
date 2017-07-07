#include "subscribe_event_callback.h"
#include "module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{
	void SubscribeEventCallback::OnPushEventCallback(nim::NIMResCode res_code, const nim::EventData& event_data)
	{	
		if (res_code == nim::kNIMResSuccess)
		{
			std::list<nim::EventData> event_list;
			event_list.push_back(event_data);
			SubscribeEventManager::GetInstance()->OnPushEventCallback(event_list);
		}
		else
		{
			QLOG_APP(L"OnPushEventCallback error, code:{0}") << res_code;
		}
	}

	void SubscribeEventCallback::OnBatchPushEventCallback(nim::NIMResCode res_code, const std::list<nim::EventData>& event_list)
	{
		if (res_code == nim::kNIMResSuccess)
		{
			SubscribeEventManager::GetInstance()->OnPushEventCallback(event_list);
		}
		else
		{
			QLOG_APP(L"OnBatchPushEventCallback error, code:{0}, event_count:{1}") << res_code << event_list.size();
		}
	}
}
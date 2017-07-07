#include "nim_ui_subscribe_event_manager.h"

namespace nim_ui
{

void SubscribeEventManager::SetEnabled(bool enable)
{
	nim_comp::SubscribeEventManager::GetInstance()->SetEnabled(enable);
}

bool SubscribeEventManager::IsEnabled() const
{
	return nim_comp::SubscribeEventManager::GetInstance()->IsEnabled();
}

bool SubscribeEventManager::GetEventData(int event_type, const std::string &accid, nim_comp::EventDataEx &data)
{
	return nim_comp::SubscribeEventManager::GetInstance()->GetEventData(event_type, accid, data);
}

void SubscribeEventManager::StartAutoSubscribe()
{
	nim_comp::SubscribeEventManager::GetInstance()->StartAutoSubscribe();
}

void SubscribeEventManager::SubscribeEvent(int event_type, const std::string &subscribe_account)
{
	nim_comp::SubscribeEventManager::GetInstance()->SubscribeEvent(event_type, subscribe_account);
}

void SubscribeEventManager::SubscribeEvent(int event_type, const std::list<std::string> &subscribe_list)
{
	nim_comp::SubscribeEventManager::GetInstance()->SubscribeEvent(event_type, subscribe_list);
}

void SubscribeEventManager::UnSubscribeEvent(int event_type, const std::list<std::string> &unsubscribe_list)
{
	nim_comp::SubscribeEventManager::GetInstance()->UnSubscribeEvent(event_type, unsubscribe_list);
}

void SubscribeEventManager::UnSubscribeEvent(int event_type, const std::string &unsubscribe_account)
{
	nim_comp::SubscribeEventManager::GetInstance()->UnSubscribeEvent(event_type, unsubscribe_account);
}

nim::EventData SubscribeEventManager::CreateBusyEvent(bool busy)
{
	return nim_comp::OnlineStateEventHelper::CreateBusyEvent(busy);
}

}
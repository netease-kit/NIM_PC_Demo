#include "online_state_event_util.h"
#include "nim_service/module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{

	static std::wstring GenerateOnlineString(EventConfig::OnlineState online_state)
	{
		static const std::wstring busy_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_BUSY");
		static const std::wstring online_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_ONLINE");
		static const std::wstring leave_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_LEAVE");

		switch (online_state)
		{
		case EventConfig::kOnlineStateOnline:
			return online_string;
			break;
		case EventConfig::kOnlineStateBusy:
			return busy_string;
			break;
		case EventConfig::kOnlineStateLeave:
			return leave_string;
			break;
		default:
			break;
		}

		return L"";
	}


	static std::wstring GenerateNetString(EventConfig::NetState net_state)
	{
		switch (net_state)
		{
		case EventConfig::kNetStateUnknow:
			return L"Unknow";
			break;
		case EventConfig::kNetStateWifi:
			return L"Wifi";
			break;
		case EventConfig::kNetStateWwan:
			return L"Wwan";
			break;
		case EventConfig::kNetState2G:
			return L"2G";
			break;
		case EventConfig::kNetState3G:
			return L"3G";
			break;
		case EventConfig::kNetState4G:
			return L"4G";
			break;
		default:
			break;
		}

		return L"";
	}

	std::wstring OnlineStateEventUtil::GetOnlineState(const std::string& accid, const EventDataEx& data, bool is_simple)
	{
		if (!SubscribeEventManager::GetInstance()->IsEnabled())
			return L"";

		static const std::wstring online_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_ONLINE");
		static const std::wstring offline_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_OFFLINE");
		static const std::wstring pc_string = L"PC";

		std::wstring online_state = offline_string;
		const std::set<nim::NIMClientType>& online_client = data.online_client_.online_client_type_;

		if (online_client.empty())
		{
			if (is_simple)
				return L"[" + online_state + L"]";
			else
				return online_state;
		}

		// 不判断本地 PC 在线状态
		if (accid != LoginManager::GetInstance()->GetAccount() && online_client.find(nim::kNIMClientTypePCWindows) != online_client.end())
		{
			online_state = pc_string + online_string;

			auto iter = data.multi_config_.find(nim::kNIMClientTypePCWindows);
			if (iter != data.multi_config_.end())
			{
				if (EventConfig::kOnlineStateOnline != iter->second.online_state_)
				{
					online_state = GenerateOnlineString(iter->second.online_state_);
				}
			}
		}
		else if (online_client.find(nim::kNIMClientTypeMacOS) != online_client.end())
		{
			online_state = L"Mac" + online_string;

			auto iter = data.multi_config_.find(nim::kNIMClientTypeMacOS);
			if (iter != data.multi_config_.end())
			{
				if (EventConfig::kOnlineStateOnline != iter->second.online_state_)
				{
					online_state = GenerateOnlineString(iter->second.online_state_);
				}
			}
		}
		else if (online_client.find(nim::kNIMClientTypeiOS) != online_client.end())
		{
			auto iter = data.multi_config_.find(nim::kNIMClientTypeiOS);
			if (iter != data.multi_config_.end() && EventConfig::kNetStateUnknow != iter->second.net_state_)
			{
				if (is_simple)
				{
					online_state = GenerateNetString(iter->second.net_state_) + online_string;
				}
				else
				{
					online_state = L"iOS-" + GenerateNetString(iter->second.net_state_) + online_string;
				}
			}
			else
			{
				online_state = L"iOS" + online_string;
			}
		}
		else if (online_client.find(nim::kNIMClientTypeAndroid) != online_client.end())
		{
			auto iter = data.multi_config_.find(nim::kNIMClientTypeAndroid);
			if (iter != data.multi_config_.end() && EventConfig::kNetStateUnknow != iter->second.net_state_)
			{
				if (is_simple)
				{
					online_state = GenerateNetString(iter->second.net_state_) + online_string;
				}
				else
				{
					online_state = L"Android-" + GenerateNetString(iter->second.net_state_) + online_string;
				}
			}
			else
			{
				online_state = L"Android" + online_string;
			}
		}
		else if (online_client.find(nim::kNIMClientTypeWeb) != online_client.end())
		{
			online_state = L"Web" + online_string;
		}
		else
		{
			online_state = offline_string;
		}

		if (is_simple)
			return L"[" + online_state + L"]";
		else
			return online_state;
	}

}
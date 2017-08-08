#include "online_state_event_helper.h"
#include "subscribe_event_manager.h"

namespace nim_comp
{
	namespace
	{
		const char *kMultiConfigNetState	= "net_state";		//网络状态，客户端之间自定义的config字段Json格式
		const char *kMultiConfigOnlineState = "online_state";	//在线状态，客户端之间自定义的config字段Json格式
	}

nim::EventData OnlineStateEventHelper::CreateBusyEvent(bool busy)
{
	// 发布在线状态事件，更新PC端的自定义扩展字段为忙碌
	// 自定义扩展字段的意义由Demo层自定义
	nim::EventData event_data;
	event_data.event_type_ = nim::kNIMEventTypeOnlineState;
	event_data.event_value_ = nim::kNIMEventOnlineStateValueUpdateConfig;
	event_data.client_msg_id_ = QString::GetGUID();
	event_data.ttl_ = nbase::TimeDelta::FromDays(7).ToSeconds();
	event_data.broadcast_type_ = nim::kNIMEventBroadcastTypeAll;
	event_data.sync_self_ = nim::kNIMEventSyncTypeSelf;

	EventConfig config;
	if (busy)
		config.online_state_ = EventConfig::kOnlineStateBusy;
	else
		config.online_state_ = EventConfig::kOnlineStateOnline;
	config.net_state_ = EventConfig::kNetStateUnknow;
	event_data.config_ = EventConfigToJsonString(config);

	return event_data;
}

std::string OnlineStateEventHelper::EventConfigToJsonString(const EventConfig &config)
{
	Json::Value values;
	values[kMultiConfigNetState] = config.net_state_;
 	values[kMultiConfigOnlineState] = config.online_state_;

	return nim::GetJsonStringWithNoStyled(values);
}

EventConfig OnlineStateEventHelper::JsonStringToEventConfig(const std::string& config_json)
{
	EventConfig event_config;
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(config_json, values) && values.isObject())
	{
		if (values.isMember(kMultiConfigOnlineState))
			event_config.online_state_ = (EventConfig::OnlineState)values[kMultiConfigOnlineState].asUInt();
		if (values.isMember(kMultiConfigNetState))
			event_config.net_state_ = (EventConfig::NetState)values[kMultiConfigNetState].asUInt();
	}

	return event_config;
}

EventMultiConfig OnlineStateEventHelper::JsonStringToMultiConfig(const std::string& multi_config_json)
{
	EventMultiConfig multi_config;
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(multi_config_json, values) && values.isObject())
	{
		auto members = values.getMemberNames();
		for (auto it = members.begin(); it != members.end(); ++it)
		{
			int client_type = 0;
			if (nbase::StringToInt(*it, &client_type))
			{
				std::string config_json = values[*it].asString();
				if (!config_json.empty())
				{
					multi_config[(nim::NIMClientType)client_type] = JsonStringToEventConfig(config_json);
				}
			}
		}
	}

	return multi_config;
}

std::wstring GenerateOnlineString(EventConfig::OnlineState online_state)
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

std::wstring GenerateNetString(EventConfig::NetState net_state)
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

std::wstring OnlineStateEventHelper::GetOnlineState(const nim::EventOnlineClientType& online_client_type, const EventMultiConfig& multi_config, bool is_simple)
{
	if (!SubscribeEventManager::GetInstance()->IsEnabled())
		return L"";

	static const std::wstring online_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_ONLINE");
	static const std::wstring offline_string = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STDID_SUBSCRIBE_EVENT_OFFLINE");	
	static const std::wstring pc_string = L"PC";

	std::wstring online_state = offline_string;
	const std::set<nim::NIMClientType> &online_client = online_client_type.online_client_type_;

	if (online_client.empty())
	{
		if (is_simple)
			return L"[" + online_state + L"]";
		else
			return online_state;
	}

	if (online_client.find(nim::kNIMClientTypePCWindows) != online_client.end())
	{
		online_state = pc_string + online_string;

		auto iter = multi_config.find(nim::kNIMClientTypePCWindows);
		if (iter != multi_config.end())
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

		auto iter = multi_config.find(nim::kNIMClientTypeMacOS);
		if (iter != multi_config.end())
		{
			if (EventConfig::kOnlineStateOnline != iter->second.online_state_)
			{
				online_state = GenerateOnlineString(iter->second.online_state_);
			}
		}
	}
	else if (online_client.find(nim::kNIMClientTypeiOS) != online_client.end())
	{
		auto iter = multi_config.find(nim::kNIMClientTypeiOS);
		if (iter != multi_config.end() && EventConfig::kNetStateUnknow != iter->second.net_state_)
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
		auto iter = multi_config.find(nim::kNIMClientTypeAndroid);
		if (iter != multi_config.end() && EventConfig::kNetStateUnknow != iter->second.net_state_)
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
		online_state = online_string;
	}

	if (is_simple)
		return L"[" + online_state + L"]";
	else
		return online_state;
}

}
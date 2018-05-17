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

}
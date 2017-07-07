/** @file nim_subscribe_event_helper.cpp
  * @brief 事件订阅辅助方法和数据结构定义
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2017/03/23
  */

#include "nim_subscribe_event_helper.h"


namespace nim
{

#if NIMAPI_UNDER_WIN_DESKTOP_ONLY
bool ParseEventData( const std::string& event_json, EventData& event_data )
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(event_json, values) && values.isObject())
	{
		ParseEventData(values, event_data);
		return true;
	}
	return false;
}

bool ParseEventData( const Json::Value& values, EventData& event_data )
{
	if (values.isMember(kNIMEventEventType))
		event_data.event_type_ = values[kNIMEventEventType].asUInt();

	if (values.isMember(kNIMEventEventValue))
		event_data.event_value_ = values[kNIMEventEventValue].asUInt();

	if (values.isMember(kNIMEventMsgIdClient))
		event_data.client_msg_id_ = values[kNIMEventMsgIdClient].asString();

	if (values.isMember(kNIMEventConfig))
		event_data.config_ = values[kNIMEventConfig].asString();

	if (values.isMember(kNIMEventTTL))
		event_data.ttl_ = values[kNIMEventTTL].asUInt64();

	if (values.isMember(kNIMEventBroadcastType))
		event_data.broadcast_type_ = (NIMEventBroadcastType)values[kNIMEventBroadcastType].asUInt();

	if (values.isMember(kNIMEventSyncSelf))
		event_data.sync_self_ = (NIMEventSyncType)values[kNIMEventSyncSelf].asUInt();

	if (values.isMember(kNIMEventTTLType))
		event_data.readonly_ttl_type = values[kNIMEventTTLType].asUInt();

	if (values.isMember(kNIMEventDurable))
		event_data.readonly_durable_ = values[kNIMEventDurable].asUInt();

	if (values.isMember(kNIMEventEventTime))
		event_data.readonly_event_time_ = values[kNIMEventEventTime].asUInt64();

	if (values.isMember(kNIMEventMsgIdServer))
		event_data.readonly_server_msg_id_ = values[kNIMEventMsgIdServer].asString();

	if (values.isMember(kNIMEventClientType))
		event_data.readonly_client_type_ = values[kNIMEventClientType].asUInt();

	if (values.isMember(kNIMEventNimConfig))
		event_data.readonly_nim_config_ = values[kNIMEventNimConfig].asString();

	if (values.isMember(kNIMEventMultiConfig))
		event_data.readonly_multi_config_ = values[kNIMEventMultiConfig].asString();

	if (values.isMember(kNIMEventPublisherAccid))
		event_data.readonly_publisher_accid_ = values[kNIMEventPublisherAccid].asString();

	if (values.isMember(kNIMEventConsid))
		event_data.readonly_consid_ = values[kNIMEventConsid].asString();

	return true;
}

bool ParseEventDataList( const std::string& event_list_json, std::list<EventData>& event_data_list )
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(event_list_json, values) && values.isArray())
	{
		int len = (int)values.size();
		for (int i = 0; i < len; i++)
		{
			EventData data;
			ParseEventData(values[i], data);
			event_data_list.push_back(data);
		}
		return true;
	}
	return false;
}

bool ParseEventSubscribeData( const Json::Value& values, EventSubscribeData& event_subscribe )
{
	if (values.isMember(kNIMEventSubscribeEventType))
		event_subscribe.event_type_ = values[kNIMEventSubscribeEventType].asUInt();

	if (values.isMember(kNIMEventSubscribeTTL))
		event_subscribe.ttl_ = values[kNIMEventSubscribeTTL].asUInt64();

	if (values.isMember(kNIMEventSubscribeSyncEvent))
		event_subscribe.sync_event_ = (NIMEventSubscribeSyncEventType)values[kNIMEventSubscribeSyncEvent].asUInt();

	if (values.isMember(kNIMEventSubscribePublisherAccid))
		event_subscribe.publisher_accid_ = values[kNIMEventSubscribePublisherAccid].asString();

	if (values.isMember(kNIMEventSubscribeSubscribeAccid))
		event_subscribe.subscribe_accid_ = values[kNIMEventSubscribeSubscribeAccid].asString();

	if (values.isMember(kNIMEventSubscribeSubscribeTime))
		event_subscribe.subscribe_time_ = values[kNIMEventSubscribeSubscribeTime].asUInt64();

	return true;
}

bool ParseEventSubscribeDataList( const std::string& subscribe_list_json, std::list<EventSubscribeData>& event_subscribe_list )
{
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(subscribe_list_json, values) && values.isArray())
	{
		int len = (int)values.size();
		for (int i = 0; i < len; i++)
		{
			EventSubscribeData data;
			ParseEventSubscribeData(values[i], data);
			event_subscribe_list.push_back(data);
		}
		return true;
	}
	return false;
}
#endif

}

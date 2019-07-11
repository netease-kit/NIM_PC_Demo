/** @file nim_cpp_subscribe_event.cpp
  * @brief NIM 事件订阅相关接口
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @date 2017/03/23
  */

#include "src/cpp_sdk/nim/api//nim_cpp_subscribe_event.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT

typedef void(*nim_subscribe_event_reg_push_event_cb)(const char *json_extension, nim_push_event_cb_func cb, const void *user_data);
typedef void(*nim_subscribe_event_reg_batch_push_event_cb)(const char *json_extension, nim_batch_push_event_cb_func cb, const void *user_data);
typedef void(*nim_publish_event)(const char *event_json, const char *json_extension, nim_publish_event_cb_func cb, const void *user_data);
typedef void(*nim_subscribe_event)(int event_type, int64_t ttl, int sync_event, const char *accid_list_json, const char *json_extension, nim_subscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_unsubscribe_event)(int event_type, const char *accid_list_json, const char *json_extension, nim_unsubscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_batch_unsubscribe_event)(int event_type, const char *json_extension, nim_batch_unsubscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_query_subscribe_event)(int event_type, const char *accid_list_json, const char *json_extension, nim_query_subscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_batch_query_subscribe_event)(int event_type, const char *json_extension, nim_batch_query_subscribe_event_cb_func cb, const void *user_data);
#else
#include "nim_subscribe_event.h"
#endif

// 不销毁该回调函数
static void CallbackPushEvent(int res_code, const char *event_info_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::PushEventCallback>(user_data, [=](const SubscribeEvent::PushEventCallback& cb){

		EventData event_data;
		ParseEventData(PCharToString(event_info_json), event_data);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)res_code, event_data);
	});
}

static void CallbackBatchPushEvent(int res_code, const char *event_list_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::BatchPushEventCallback>(user_data, [=](const SubscribeEvent::BatchPushEventCallback& cb){

		std::list<EventData> event_data_list;
		ParseEventDataList(PCharToString(event_list_json), event_data_list);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)res_code, event_data_list);
	});

}

// 执行完回调函数后销毁之
static void CallbackPublishEvent(int res_code, int event_type, const char *event_info_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::PublishEventCallback>(user_data, [=](const SubscribeEvent::PublishEventCallback& cb){

		EventData event_data;
		ParseEventData(PCharToString(event_info_json), event_data);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type, event_data);
	},true);

}

static void CallbackSubscribe(int res_code, int event_type, const char *faild_list_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::SubscribeEventCallback>(user_data, [=](const SubscribeEvent::SubscribeEventCallback& cb){

		std::list<std::string> faild_list;
		JsonArrayStringToList(PCharToString(faild_list_json), faild_list);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type, faild_list);
	}, true);

}

static void CallbackUnSubscribe(int res_code, int event_type, const char *faild_list_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::UnSubscribeEventCallback>(user_data, [=](const SubscribeEvent::UnSubscribeEventCallback& cb){

		std::list<std::string> faild_list;
		JsonArrayStringToList(PCharToString(faild_list_json), faild_list);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type, faild_list);
	}, true);
}

static void CallbackBatchUnSubscribe(int res_code, int event_type, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::BatchUnSubscribeEventCallback>(user_data, [=](const SubscribeEvent::BatchUnSubscribeEventCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type);
	}, true);

}

static void CallbackQuerySubscribe(int res_code, int event_type, const char *subscribe_list_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::QuerySubscribeEventCallback>(user_data, [=](const SubscribeEvent::QuerySubscribeEventCallback& cb){

		std::list<EventSubscribeData> subscribe_list;
		ParseEventSubscribeDataList(PCharToString(subscribe_list_json), subscribe_list);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type, subscribe_list);
	}, true);

}

static void CallbackBatchQuerySubscribe(int res_code, int event_type, const char *subscribe_list_json, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<SubscribeEvent::BatchQuerySubscribeEventCallback>(user_data, [=](const SubscribeEvent::BatchQuerySubscribeEventCallback& cb){

		std::list<EventSubscribeData> subscribe_list;
		ParseEventSubscribeDataList(PCharToString(subscribe_list_json), subscribe_list);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, event_type, subscribe_list);
	}, true);
}

static SubscribeEvent::PushEventCallback g_cb_push_event = nullptr;
void SubscribeEvent::RegPushEventCb( const PushEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	g_cb_push_event = cb;
	NIM_SDK_GET_FUNC(nim_subscribe_event_reg_push_event_cb)(json_extension.c_str(), &CallbackPushEvent, &g_cb_push_event);
}

static SubscribeEvent::BatchPushEventCallback g_cb_batch_push_event = nullptr;
void SubscribeEvent::RegBatchPushEventCb( const BatchPushEventCallback &cb, const std::string& json_extension /*= ""*/ )
{	
	g_cb_batch_push_event = cb;
	NIM_SDK_GET_FUNC(nim_subscribe_event_reg_batch_push_event_cb)(json_extension.c_str(), &CallbackBatchPushEvent, &g_cb_batch_push_event);
}

bool SubscribeEvent::Publish( const EventData &event_data, const PublishEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	bool param_error = false;
	if (event_data.event_type_ == nim::kNIMEventTypeOnlineState && event_data.event_value_ >= kNIMEventOnlineStateValueCustom)
	{

	}
	else if (event_data.event_value_ >= nim::kNIMEventTypeCustom)
	{

	}
	else
	{
		param_error = true;
	}

	if (param_error)
		return false;

	PublishEventCallback* opt_cb = nullptr;
	if (cb)
	{
		opt_cb = new PublishEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_publish_event)(event_data.ToJsonString().c_str(), json_extension.c_str(), &CallbackPublishEvent, opt_cb);

	return true;
}

bool SubscribeEvent::Subscribe( int event_type, int64_t ttl, NIMEventSubscribeSyncEventType sync_type, const std::list<std::string> &accid_list, const SubscribeEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	if (event_type == 0 || ttl == 0 || accid_list.size() > 100)
		return false;

	SubscribeEventCallback* opt_cb = nullptr;
	if (cb)
	{
		opt_cb = new SubscribeEventCallback(cb);
	}

	std::string out;
	StrListToJsonString(accid_list, out);
	NIM_SDK_GET_FUNC(nim_subscribe_event)(event_type, ttl, sync_type, out.c_str(), json_extension.c_str(), &CallbackSubscribe, opt_cb);

	return true;
}

bool SubscribeEvent::UnSubscribe( int event_type, const std::list<std::string> &accid_list, const UnSubscribeEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	if (event_type == 0  || accid_list.size() > 100)
		return false;

	UnSubscribeEventCallback* opt_cb = nullptr;
	if (cb)
	{
		opt_cb = new UnSubscribeEventCallback(cb);
	}

	std::string out;
	StrListToJsonString(accid_list, out);
	NIM_SDK_GET_FUNC(nim_unsubscribe_event)(event_type, out.c_str(), json_extension.c_str(), &CallbackUnSubscribe, opt_cb);

	return true;
}

bool SubscribeEvent::BatchUnSubscribe( int event_type, const BatchUnSubscribeEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	if (event_type == 0)
		return false;

	BatchUnSubscribeEventCallback* opt_cb = nullptr;
	if (cb)
	{
		opt_cb = new BatchUnSubscribeEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_batch_unsubscribe_event)(event_type, json_extension.c_str(), &CallbackBatchUnSubscribe, opt_cb);

	return true;
}

bool SubscribeEvent::QuerySubscribe( int event_type, const std::list<std::string> &accid_list, const QuerySubscribeEventCallback &cb, const std::string& json_extension /*= ""*/ )
{
	if (event_type == 0 || accid_list.size() > 100)
		return false;

	QuerySubscribeEventCallback* opt_cb = nullptr;
	if (cb)
	{
		opt_cb = new QuerySubscribeEventCallback(cb);
	}

	std::string out;
	StrListToJsonString(accid_list, out);
	NIM_SDK_GET_FUNC(nim_query_subscribe_event)(event_type, out.c_str(), json_extension.c_str(), &CallbackQuerySubscribe, opt_cb);

	return true;
}

}
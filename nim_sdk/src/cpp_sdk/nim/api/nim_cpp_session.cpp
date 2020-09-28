/** @file nim_cpp_session.cpp
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "src/cpp_sdk/nim/api//nim_cpp_session.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_session_reg_change_cb)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_reg_badge_count_cb)(const char *json_extension, nim_session_badge_count_cb_func cb, const void *user_data);
typedef void(*nim_session_query_last_few_session_async)(int limit, const char *json_extension, nim_session_query_recent_session_cb_func cb, const void *user_data);
typedef void(*nim_session_query_all_recent_session_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, const void* user_data);
typedef void(*nim_session_query_all_recent_session_with_last_msg_excluded_type_async)(const char *json_extension, nim_session_query_recent_session_cb_func cb, enum NIMMessageType last_msg_excluded_type,const void* user_data);
typedef void(*nim_session_delete_recent_session_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_delete_session_roaming_async)(NIMSessionType to_type, const char* id, const char* json_extension, nim_session_delete_session_roaming_cb_func cb, const void* user_data);
typedef void(*nim_session_delete_all_recent_session_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_unread_count_zero_async)(NIMSessionType to_type, const char *id, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_top)(enum NIMSessionType to_type, const char *id, bool top, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_set_extend_data)(enum NIMSessionType to_type, const char *id, const char *data, const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_reset_all_unread_count_async)(const char *json_extension, nim_session_change_cb_func cb, const void *user_data);
typedef void(*nim_session_query_sessiondata_by_id_async)(enum NIMSessionType to_type, const char *id, const char *json_extension, nim_session_query_sessiondata_by_id_cb_func cb, const void *user_data);
typedef void(*nim_session_reg_set_to_stick_top_notify_cb)(nim_session_set_to_stick_top_notify_cb_func cb, const void* user_data);
typedef void(*nim_session_reg_cancell_stick_top_notify_cb)(nim_session_cancel_stick_top_notify_cb_func cb, const void* user_data);
typedef void(*nim_session_reg_update_stick_top_notify_cb)(nim_session_update_stick_top_notify_cb_func cb, const void* user_data);
typedef void(*nim_session_update_stick_top)(const char* session_id, enum NIMSessionType to_type, const char* ext, nim_session_update_stick_top_cb_func cb, const void* user_data);
typedef void(*nim_session_cancel_stick_top)(const char* session_id, enum NIMSessionType to_type, nim_session_cancel_stick_top_cb_func cb, const void* user_data);
typedef void(*nim_session_set_to_stick_top)(const char* session_id, enum NIMSessionType to_type, const char* ext, nim_session_set_to_stick_top_cb_func cb, const void* user_data);
typedef void(*nim_session_query_stick_top_session_list)(nim_session_query_stick_top_session_list_cb_func cb, const void* user_data);
typedef void(*nim_session_query_session_hasmore_roammsg)(const SessionMainTagInfo* session_main_tag_info, nim_session_query_session_hasmore_roammsg_cb_func cb, const void* user_data);
typedef void(*nim_session_query_all_session_hasmore_roammsg)(nim_session_query_all_session_hasmore_roammsg_cb_func cb, const void* user_data);
typedef void(*nim_session_update_session_hasmore_roammsg)(const char *json_msg, nim_session_update_session_hasmore_roammsg_cb_func cb, const void* user_data);
typedef void(*nim_session_delete_session_hasmore_roammsg)(const SessionMainTagInfo* session_main_tag_info, nim_session_delete_session_hasmore_roammsg_cb_func cb, const void* user_data);
#else
#include "nim_session.h"
#endif

static void CallbackQuerySession(int total_unread_count, const char *result, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::QuerySessionListCallabck>(user_data, [=](const Session::QuerySessionListCallabck& cb){

		SessionDataList sl;
		ParseSessionList(PCharToString(result), sl);
		CallbackProxy::Invoke(cb, total_unread_count, sl);
	},true);

}

static void CallbackNotifySession(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{
	CallbackProxy::DoSafeCallback<Session::ChangeCallback>(user_data, [=](const Session::ChangeCallback& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session, total_unread_counts);
	}, true);

}

static void CallbackSessionChange(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::DeleteRecentSessionCallabck>(user_data, [=](const Session::DeleteRecentSessionCallabck& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session, total_unread_counts);
	});
}

static int32_t CallbackBadgeCount(const char *json_extension, const void *user_data)
{
    return CallbackProxy::DoSafeCallback<Session::BadgeCountCallback>(user_data, [=](const Session::BadgeCountCallback& cb){

        return CallbackProxy::Invoke(cb, PCharToString(json_extension));
    });
}
static void CallbackQuerySessionData(int rescode, const char *result, const void *user_data)
{

	CallbackProxy::DoSafeCallback<Session::QuerySessionDataCallback>(user_data, [=](const Session::QuerySessionDataCallback& cb){

		SessionData session;
		ParseSession(PCharToString(result), session);
		CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session);
	},true);
}

static Session::ChangeCallback g_cb_session_changed_ = nullptr;
void Session::RegChangeCb(const ChangeCallback& cb, const std::string& json_extension)
{
	g_cb_session_changed_ = cb;
	NIM_SDK_GET_FUNC(nim_session_reg_change_cb)(json_extension.c_str(), &CallbackSessionChange, &g_cb_session_changed_);
}

static Session::BadgeCountCallback g_cb_badge_count_ = nullptr;
void Session::RegBadgeCountCb(const BadgeCountCallback& cb, const std::string& json_extension)
{
    g_cb_badge_count_  = cb;
    NIM_SDK_GET_FUNC(nim_session_reg_badge_count_cb)(json_extension.c_str(), &CallbackBadgeCount, &g_cb_badge_count_);
}
void Session::QueryLastFewSessionAsync(int limit, const QuerySessionListCallabck& cb, const std::string& json_extension /*= ""*/)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_last_few_session_async)(limit, json_extension.c_str(), &CallbackQuerySession, cb_pointer);
}

void Session::QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_all_recent_session_async)(json_extension.c_str(), &CallbackQuerySession, cb_pointer);
}
void Session::QueryAllRecentSessionAsync(NIMMessageType last_msg_excluded_type, const QuerySessionListCallabck& cb, const std::string& json_extension /*= ""*/)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_all_recent_session_with_last_msg_excluded_type_async)(json_extension.c_str(), &CallbackQuerySession, last_msg_excluded_type,cb_pointer);
}
bool Session::DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension)
{
	if (id.empty())
		return false;

	DeleteRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteRecentSessionCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_delete_recent_session_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackNotifySession, cb_pointer);

	return true;
}
bool Session::DeleteSessionRoamingMessage(nim::NIMSessionType to_type, const std::string& id, const DeleteSessionRoammsgCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (id.empty())
		return false;

	DeleteSessionRoammsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteSessionRoammsgCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_delete_session_roaming_async)(to_type, id.c_str(), json_extension.c_str(), 
		[](int rescode, const char* to_type, const char* id, const void* user_data) {
			CallbackProxy::DoSafeCallback<DeleteSessionRoammsgCallback>(user_data, [=](const DeleteSessionRoammsgCallback& cb) {
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode,(nim::NIMSessionType)std::atoi(to_type),id);
				}, true);
		}, cb_pointer);
	return true;
}
void Session::DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb,  const std::string& json_extension)
{
	DeleteAllRecentSessionCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllRecentSessionCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_delete_all_recent_session_async)(json_extension.c_str(), &CallbackNotifySession, cb_pointer);
}

bool Session::SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension)
{
	if (id.empty())
		return false;

	SetUnreadCountZeroCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetUnreadCountZeroCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_unread_count_zero_async)(to_type, id.c_str(), json_extension.c_str(), &CallbackNotifySession, cb_pointer);

	return true;
}

bool Session::SetSessionTop(enum NIMSessionType to_type, const std::string& id, bool top, const ChangeCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (id.empty())
		return false;

	ChangeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ChangeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_top)(to_type, id.c_str(), top, json_extension.c_str(), &CallbackSessionChange, cb_pointer);
	return true;
}

bool Session::SetSessionExtendData(enum NIMSessionType to_type, const std::string& id, const std::string& data, const ChangeCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (id.empty() && data.length() > 4096)
		return false;

	ChangeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ChangeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_set_extend_data)(to_type, id.c_str(), data.c_str(), json_extension.c_str(), &CallbackSessionChange, cb_pointer);
	return true;
}

bool Session::SetAllUnreadCountZeroAsync(const SetUnreadCountZeroCallback& cb, const std::string& json_extension/* = ""*/)
{
	SetUnreadCountZeroCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetUnreadCountZeroCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_reset_all_unread_count_async)(json_extension.c_str(), &CallbackSessionChange, cb_pointer);

	return true;
}
void Session::QuerySessionDataById(NIMSessionType to_type, const std::string& id, const QuerySessionDataCallback& cb, const std::string& json_extension/* = ""*/)
{
	Session::QuerySessionDataCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new Session::QuerySessionDataCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_query_sessiondata_by_id_async)(to_type,id.c_str(),json_extension.c_str(), &CallbackQuerySessionData, cb_pointer);

}
Session::SetToStickTopSessionNotifyCallback g_set_to_stick_topsession_notify_callback_ = nullptr;
void Session::RegSetToStickTopSessionNotifyCB(const SetToStickTopSessionNotifyCallback& cb)
{
	g_set_to_stick_topsession_notify_callback_ = cb;
	NIM_SDK_GET_FUNC(nim_session_reg_set_to_stick_top_notify_cb)([](const char* stick_session, const void* user_data) {
		CallbackProxy::DoSafeCallback<SetToStickTopSessionNotifyCallback>(user_data, [=](const SetToStickTopSessionNotifyCallback& cb) {
			nim::StickTopSession session;
			ParseStickTopSession(std::string(stick_session), session);
			CallbackProxy::Invoke(cb, session);
			}, false);
		}
		, &g_set_to_stick_topsession_notify_callback_);
}
Session::CancelStickTopSessionNotifyCallback g_cancel_stick_top_session_notify_callback_ = nullptr;
void Session::RegCancelStickTopSessionNotifyCB(const CancelStickTopSessionNotifyCallback& cb)
{
	g_cancel_stick_top_session_notify_callback_ = cb;
	NIM_SDK_GET_FUNC(nim_session_reg_cancell_stick_top_notify_cb)(
		[](const char* session_id, enum NIMSessionType to_type, const void* user_data) {
			CallbackProxy::DoSafeCallback<CancelStickTopSessionNotifyCallback>(user_data, [=](const CancelStickTopSessionNotifyCallback& cb) {
				CallbackProxy::Invoke(cb, session_id,to_type);
			}, false);
		}, &g_cancel_stick_top_session_notify_callback_);
}
Session::UpdateStickTopSessionNotifyCallback g_updatesticktopsessionnotifycallback_ = nullptr;
void Session::RegUpdateStickTopSessionNotifyCB(const UpdateStickTopSessionNotifyCallback& cb)
{
	g_updatesticktopsessionnotifycallback_ = cb;
	NIM_SDK_GET_FUNC(nim_session_reg_update_stick_top_notify_cb)(
		[](const char* stick_session, const void* user_data) {
			CallbackProxy::DoSafeCallback<UpdateStickTopSessionNotifyCallback>(user_data, [=](const UpdateStickTopSessionNotifyCallback& cb) {
				nim::StickTopSession session;
				ParseStickTopSession(std::string(stick_session), session);
				CallbackProxy::Invoke(cb, session);
				}, false);
		}, &g_updatesticktopsessionnotifycallback_);
}
void Session::QueryStickTopSessionList(const QueryStickTopSessionListCallback& cb)
{
	QueryStickTopSessionListCallback* cb_pointer = ((cb != nullptr) ? (new QueryStickTopSessionListCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_query_stick_top_session_list)(
		[](int rescode, const char* result, const void* user_data) {		
			CallbackProxy::DoSafeCallback<QueryStickTopSessionListCallback>(user_data, [=](const QueryStickTopSessionListCallback& cb) {
				nim::StickTopSessionList session_list;
				if(rescode == nim::NIMResCode::kNIMResSuccess)
					ParseStickTopSessionList(result, session_list);
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session_list);
				}, true);
		}, cb_pointer);
}
void Session::SetToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext, const SetToStickTopSessionCallback& cb)
{
	SetToStickTopSessionCallback* cb_pointer = ((cb != nullptr) ? (new SetToStickTopSessionCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_set_to_stick_top)(session_id.c_str(),to_type,ext.c_str(),
		[](int rescode, const char* stick_session, const void* user_data) {
			CallbackProxy::DoSafeCallback<SetToStickTopSessionCallback>(user_data, [=](const SetToStickTopSessionCallback& cb) {
				nim::StickTopSession session;
				if (rescode == nim::NIMResCode::kNIMResSuccess)
					ParseStickTopSession(std::string(stick_session), session);
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode,session);
				}, true);
		}, cb_pointer);
}
void Session::UpdateToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext, const UpdateStickTopSessionCallback& cb)
{
	UpdateStickTopSessionCallback* cb_pointer = ((cb != nullptr) ? (new UpdateStickTopSessionCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_update_stick_top)(session_id.c_str(), to_type,ext.c_str(),
		[](int rescode, const char* stick_session, const void* user_data) {
			CallbackProxy::DoSafeCallback<UpdateStickTopSessionCallback>(user_data, [=](const UpdateStickTopSessionCallback& cb) {
				nim::StickTopSession session;
				if (rescode == nim::NIMResCode::kNIMResSuccess)
					ParseStickTopSession(std::string(stick_session), session);
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session);
				}, true);
		}, cb_pointer);
}
void Session::CancelToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const CancelStickTopSessionCallback& cb)
{
	CancelStickTopSessionCallback* cb_pointer = ((cb != nullptr) ? (new CancelStickTopSessionCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_cancel_stick_top)(session_id.c_str(), to_type,
		[](int rescode, const char* session_id, enum NIMSessionType to_type, const void* user_data) {
			CallbackProxy::DoSafeCallback<CancelStickTopSessionCallback>(user_data, [=](const CancelStickTopSessionCallback& cb) {
			CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, session_id, to_type);
			}, true);
		}, cb_pointer);
}
void FillSessionMainTagInfo(const std::string& session_id, nim::NIMSessionType to_type, SessionMainTagInfo& info)
{
	memset(&info, 0, sizeof(SessionMainTagInfo));
	info.to_type = to_type;
	memcpy(info.session_id, session_id.data(), session_id.size() * sizeof(std::string::value_type));
}
void Session::QueryHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const QueryHasmoreRoammsgCallback& cb)
{
	SessionMainTagInfo main_info;
	FillSessionMainTagInfo(session_id,to_type,main_info);
	QueryHasmoreRoammsgCallback* cb_pointer = ((cb != nullptr) ? (new QueryHasmoreRoammsgCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_query_session_hasmore_roammsg)(&main_info,
		[](int rescode, const SessionRoamMsgHasMoreTagInfo* tag_info, const void* user_data) {
			CallbackProxy::DoSafeCallback<QueryHasmoreRoammsgCallback>(user_data, [=](const QueryHasmoreRoammsgCallback& cb) {
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, *tag_info);
				}, true);
		}, cb_pointer);
}
void Session::QueryAllHasmoreRoammsg(const QueryAllHasmoreRoammsgCallback& cb)
{
	QueryAllHasmoreRoammsgCallback* cb_pointer = ((cb != nullptr) ? (new QueryAllHasmoreRoammsgCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_query_all_session_hasmore_roammsg)(
		[](int rescode, const SessionRoamMsgHasMoreTagInfo* tag_info_list, int size, const void* user_data) {
			std::list<SessionRoamMsgHasMoreTagInfo> info_list;
			auto* temp = tag_info_list;
			for (auto index = 0; index < size; index++)
			{
				info_list.emplace_back(*temp++);
			}
			CallbackProxy::DoSafeCallback<QueryAllHasmoreRoammsgCallback>(user_data, [=](const QueryAllHasmoreRoammsgCallback& cb) {
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, info_list);
				}, true);
		}, cb_pointer);
}
void Session::UpdateHasmoreRoammsg(const IMMessage& msg, const UpdateHasmoreRoammsgCallback& cb)
{
	UpdateHasmoreRoammsgCallback* cb_pointer = ((cb != nullptr) ? (new UpdateHasmoreRoammsgCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_update_session_hasmore_roammsg)(msg.ToJsonString(false).c_str(),
		[](int rescode, const void* user_data) {			
			CallbackProxy::DoSafeCallback<UpdateHasmoreRoammsgCallback>(user_data, [=](const UpdateHasmoreRoammsgCallback& cb) {
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode);
				}, true);
		}, cb_pointer);
}
void Session::DeleteHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const DeleteHasmoreRoammsgCallback& cb)
{
	SessionMainTagInfo main_info;
	FillSessionMainTagInfo(session_id, to_type, main_info);
	DeleteHasmoreRoammsgCallback* cb_pointer = ((cb != nullptr) ? (new DeleteHasmoreRoammsgCallback(cb)) : (nullptr));
	NIM_SDK_GET_FUNC(nim_session_delete_session_hasmore_roammsg)(&main_info,
		[](int rescode, const void* user_data) {
			CallbackProxy::DoSafeCallback<DeleteHasmoreRoammsgCallback>(user_data, [=](const DeleteHasmoreRoammsgCallback& cb) {
				CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode);
				}, true);
		}, cb_pointer);
}

void Session::UnregSessionCb()
{
	g_cb_session_changed_ = nullptr;
    g_cb_badge_count_ = nullptr;
	g_set_to_stick_topsession_notify_callback_ = nullptr;
	g_cancel_stick_top_session_notify_callback_ = nullptr;
	g_updatesticktopsessionnotifycallback_ = nullptr;
}

}

#include "src/cpp_sdk/nim/api/nim_cpp_session_online_service.h"
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "public_define/defines/nim_define/nim_session_online_service_def.h"

namespace nim
{

#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_session_online_service_query_session_list)(uint64_t, uint64_t, bool, int, nim_session_online_service_query_session_list_cb_func, const void*);
typedef void(*nim_session_online_service_query_session)(int, const char*, nim_session_online_service_query_session_cb_func, const void*);
typedef void(*nim_session_online_service_update_session)(int, const char*, const char*, nim_session_online_service_update_session_cb_func, const void*);
typedef void(*nim_session_online_service_delete_session)(const char*, nim_session_online_service_delete_session_cb_func, const void*);
typedef void(*nim_session_online_service_reg_session_changed)(nim_session_online_service_session_changed_cb_func, const void*);
#else
#include "nim_session_online_service.h"
#endif
SessionOnLineService::SessionChangedCallback g_session_changed_callback = nullptr;
void SessionOnLineService::RegSessionChanged(const SessionChangedCallback& cb)
{
	g_session_changed_callback = cb;
	NIM_SDK_GET_FUNC(nim_session_online_service_reg_session_changed)(
		[](const char *result, const void *user_data) {
		CallbackProxy::DoSafeCallback<SessionChangedCallback>(user_data, [=](const SessionChangedCallback& cb) {
			SessionOnLineServiceHelper::SessionInfo info;
			SessionOnLineServiceHelper::ParseSessionInfo(PCharToString(result), info);
			CallbackProxy::Invoke(cb, info);
		}, false);
	}, &g_session_changed_callback);
}
void SessionOnLineService::QuerySessionList(uint64_t min_time, uint64_t max_time, bool need_last_msg, int limit, const QuerySessionListCallabck& cb)
{
	QuerySessionListCallabck* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionListCallabck(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_online_service_query_session_list)(min_time, max_time, need_last_msg, limit,
		[](int rescode, const char *result, const void *user_data) {
		CallbackProxy::DoSafeCallback<QuerySessionListCallabck>(user_data, [=](const QuerySessionListCallabck& cb) {
			SessionOnLineServiceHelper::QuerySessionListResult sl;
			sl.res_code = (nim::NIMResCode)rescode;
			SessionOnLineServiceHelper::ParseQuerySessionListResult(PCharToString(result), sl);
			CallbackProxy::Invoke(cb, sl);
		}, true);
	}, cb_pointer);
}
void SessionOnLineService::QuerySession(nim::NIMSessionType to_type, const std::string& session_id, const QuerySessionInfoCallback& cb)
{
	QuerySessionInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySessionInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_online_service_query_session)(to_type, session_id.c_str(),
		[](int rescode, const char *result, const void *user_data) {
		CallbackProxy::DoSafeCallback<QuerySessionInfoCallback>(user_data, [=](const QuerySessionInfoCallback& cb) {
			SessionOnLineServiceHelper::SessionInfo info;
			SessionOnLineServiceHelper::ParseSessionInfo(PCharToString(result), info);
			CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode, info);
		}, true);
	}, cb_pointer);
}
void SessionOnLineService::UpdateSession(nim::NIMSessionType to_type, const std::string& session_id, const std::string& ext, const UpdateSessionInfoCallback& cb)
{
	UpdateSessionInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new UpdateSessionInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_online_service_update_session)(to_type, session_id.c_str(), ext.c_str(),
		[](int rescode, const void *user_data) {
		CallbackProxy::DoSafeCallback<UpdateSessionInfoCallback>(user_data, [=](const UpdateSessionInfoCallback& cb) {
			CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode);
		}, true);
	}, cb_pointer);
}
void SessionOnLineService::DeleteSession(const SessionOnLineServiceHelper::DeleteSessionParam& param, const DeleteSessionInfoCallback& cb)
{
	DeleteSessionInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteSessionInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_session_online_service_delete_session)(SessionOnLineServiceHelper::DeleteSessionParamToJsonString(param).c_str(),
		[](int rescode, const void *user_data) {
		CallbackProxy::DoSafeCallback<DeleteSessionInfoCallback>(user_data, [=](const DeleteSessionInfoCallback& cb) {
			CallbackProxy::Invoke(cb, (nim::NIMResCode)rescode);
		}, true);
	}, cb_pointer);
}
void SessionOnLineService::UnregSessionOnLineServiceCb()
{
	g_session_changed_callback = nullptr;
}

}

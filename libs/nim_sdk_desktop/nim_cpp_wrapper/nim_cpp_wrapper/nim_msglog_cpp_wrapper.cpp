#include "nim_msglog_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

typedef void(*nim_msglog_query_msg_async)(const char* account_id, nim::NIMSessionType to_type, int limit_count, __int64 last_time, const char *json_extension, nim_msglog_query_cb_func cb, const void* user_data);
typedef void(*nim_msglog_batch_status_read_async)(const char* account_id, nim::NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_msglog_set_status_async)(const char* msg_id, nim::NIMMsgLogStatus msglog_status, const char *json_extension, nim_msglog_res_cb_func cb, const void* user_data);
typedef void(*nim_msglog_set_sub_status_async)(const char* msg_id, nim::NIMMsgLogSubStatus msglog_sub_status, const char *json_extension, nim_msglog_res_cb_func cb, const void* user_data);
typedef void(*nim_msglog_batch_status_delete_async)(const char* account_id, nim::NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_msglog_write_db_only_async)(const char *account_id, NIMSessionType to_type, const char *msg_id, const char *json_msg, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_by_session_type_async)(bool delete_sessions, NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_async)(const char *account_id, NIMSessionType to_type, const char *msg_id, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_all_async)(bool delete_sessions, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_online_async)(const char *id, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse,
	bool need_save_to_local, const char *json_extension, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_export_db_async)(const char *dst_path, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_import_db_async)(const char *src_path, const char *json_extension, nim_msglog_res_cb_func res_cb, const void *res_user_data, nim_msglog_import_prg_cb_func prg_cb, const void *prg_user_data);


struct ImportDbCallbackUserData
{
	ImportDbCallbackUserData() :
		response_cb(),
		progress_cb_pointer(nullptr)
	{

	}
	MsgLog::ImportDbCallback response_cb;
	MsgLog::ImportDbPrgCallback* progress_cb_pointer;
};

static void CallbackWrapper1(int res_code, const char *id, nim::NIMSessionType to_type, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		MsgLog::QueryMsgCallback* cb_pointer = (MsgLog::QueryMsgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, PCharToString(id), to_type, PCharToString(result)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper2(int res_code, const char *uid, nim::NIMSessionType to_type, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		MsgLog::BatchStatusReadCallback* cb_pointer = (MsgLog::BatchStatusReadCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, PCharToString(uid), to_type));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper3(int res_code, const char *msg_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		MsgLog::SetStatusCallback* cb_pointer = (MsgLog::SetStatusCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, PCharToString(msg_id)));
		}
		delete cb_pointer;
	}
}
static void ImportDbCallbackWrapper(int res_code, const char *msg_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		MsgLog::ImportDbCallback cb = ((ImportDbCallbackUserData*)user_data)->response_cb;
		if (cb)
		{
			PostTaskToUIThread(std::bind(cb, (nim::NIMResCode)res_code, PCharToString(msg_id)));
		}
		delete ((ImportDbCallbackUserData*)user_data)->progress_cb_pointer;
		delete user_data;
	}
}
static void ImportDbPrgCallbackWrapper(__int64 imported_count, __int64 total_count, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		MsgLog::ImportDbPrgCallback* cb_pointer = (MsgLog::ImportDbPrgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), imported_count, total_count));
		}
	}
}

void MsgLog::QueryMsgAsync(const std::string& account_id, nim::NIMSessionType to_type, int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension)
{
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_query_msg_async)(account_id.c_str(), to_type, limit_count, last_time, json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void MsgLog::QueryMsgOnlineAsync(const std::string &id, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse,
	bool need_save_to_local, const QueryMsgCallback& cb, const std::string& json_extension /*= ""*/)
{
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_query_msg_online_async)(id.c_str(), to_type, limit_count, from_time, end_time, end_msg_id,
		reverse, need_save_to_local, json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void MsgLog::BatchStatusReadAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusReadCallback& cb, const std::string& json_extension)
{
	BatchStatusReadCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new BatchStatusReadCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_batch_status_read_async)(account_id.c_str(), to_type, json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void MsgLog::BatchStatusDeleteAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusDeleteCallback& cb, const std::string& json_extension)
{
	BatchStatusReadCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new BatchStatusReadCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_batch_status_delete_async)(account_id.c_str(), to_type, json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void MsgLog::SetStatusAsync(const std::string& msg_id, nim::NIMMsgLogStatus msglog_status, const SetStatusCallback& cb, const std::string& json_extension)
{
	SetStatusCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetStatusCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_set_status_async)(msg_id.c_str(), msglog_status, json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}

void MsgLog::SetSubStatusAsync(const std::string& msg_id, nim::NIMMsgLogSubStatus msglog_sub_status, const SetSubStatusCallback& cb, const std::string& json_extension)
{
	SetSubStatusCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetSubStatusCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_set_sub_status_async)(msg_id.c_str(), msglog_sub_status, json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}

void MsgLog::WriteMsglogOnlyAsync(const std::string& account_id, NIMSessionType to_type, const std::string& msg_id, const std::string& json_msg, const WriteMsglogCallback& cb, const std::string& json_extension)
{
	WriteMsglogCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new WriteMsglogCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_write_db_only_async)(account_id.c_str(), to_type, msg_id.c_str(), json_msg.c_str(), json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}
void MsgLog::DeleteBySessionTypeAsync(bool delete_sessions, NIMSessionType to_type, const DeleteBySessionTypeCallback& cb, const std::string& json_extension)
{
	DeleteBySessionTypeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteBySessionTypeCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_delete_by_session_type_async)(delete_sessions, to_type, json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void MsgLog::DeleteAsync(const std::string& session_id, NIMSessionType to_type, const std::string& msg_id, const DeleteCallback& cb, const std::string& json_extension)
{
	DeleteCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_delete_async)(session_id.c_str(), to_type, msg_id.c_str(), json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}

void MsgLog::DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension)
{
	DeleteAllCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_delete_all_async)(delete_sessions, json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}
void MsgLog::ExportDbAsync(const std::string& dst_path, const ExportDbCallback& cb, const std::string& json_extension)
{
	DeleteAllCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_msglog_export_db_async)(dst_path.c_str(), json_extension.c_str(), &CallbackWrapper3, cb_pointer);
}
void MsgLog::ImportDbAsync(const std::string& src_path, const ImportDbCallback& res_cb, const ImportDbPrgCallback& prg_cb, const std::string& json_extension)
{
	ImportDbPrgCallback* progress_cb_pointer = nullptr;
	if (prg_cb)
	{
		progress_cb_pointer = new ImportDbPrgCallback(prg_cb);
	}
	ImportDbCallbackUserData* response_cb_userdata = nullptr;
	if (res_cb)
	{
		response_cb_userdata = new ImportDbCallbackUserData();
		response_cb_userdata->response_cb = res_cb;
		if (prg_cb)
		{
			response_cb_userdata->progress_cb_pointer = progress_cb_pointer;
		}
	}
	return NIM_SDK_Get_Function(nim_msglog_import_db_async)(src_path.c_str(), json_extension.c_str(), &ImportDbCallbackWrapper, response_cb_userdata, &ImportDbPrgCallbackWrapper, progress_cb_pointer);
}

}
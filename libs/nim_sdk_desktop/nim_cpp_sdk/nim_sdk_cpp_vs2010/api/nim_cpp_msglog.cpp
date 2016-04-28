/** @file nim_cpp_msglog.cpp
  * @brief NIM SDK提供的消息历史接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#include "nim_cpp_msglog.h"
#include "nim_sdk_helper.h"
#include "nim_common_helper.h"
#include "nim_cpp_talk.h"

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
typedef void(*nim_msglog_delete_all_async)(bool delete_sessions, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_online_async)(const char *id, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse, bool need_save_to_local, const char *json_extension, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_id_async)(const char *client_msg_id, const char *json_extension, nim_msglog_query_single_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_options_async)(NIMMsgLogQueryRange query_range, const char *ids, int limit_count, __int64 from_time, __int64 end_time, const char *end_client_msg_id, bool reverse, NIMMessageType msg_type, const char *search_content, const char *json_extension, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_update_localext_async)(const char *msg_id, const char *local_ext, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
						
typedef void(*nim_msglog_export_db_async)(const char *dst_path, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_import_db_async)(const char *src_path, const char *json_extension, nim_msglog_modify_res_cb_func res_cb, const void *res_user_data, nim_msglog_import_prg_cb_func prg_cb, const void *prg_user_data);

typedef void(*nim_msglog_send_receipt_async)(const char *json_msg, const char *json_extension, nim_msglog_status_changed_cb_func cb, const void *user_data);
typedef bool(*nim_msglog_query_be_readed)(const char *json_msg, const char *json_extension);
typedef void(*nim_msglog_reg_status_changed_cb)(const char *json_extension, nim_msglog_status_changed_cb_func cb, const void *user_data);


struct ImportDbCallbackUserData
{
	ImportDbCallbackUserData() :
		response_cb(),
		progress_cb_pointer(nullptr)
	{

	}
	MsgLog::DBFunctionCallback response_cb;		/**< 导入消息历史回调 */
	MsgLog::ImportDbPrgCallback* progress_cb_pointer;	/**< 导入消息历史过程回调 */
};	

static void CallbackQueryMsg(int res_code
	, const char *id
	, nim::NIMSessionType to_type
	, const char *result
	, const char *json_extension
	, const void *callback)
{
	if (callback)
	{
		MsgLog::QueryMsgCallback* cb_pointer = (MsgLog::QueryMsgCallback*)callback;
		if (*cb_pointer)
		{
			QueryMsglogResult res;
			ParseMsglogs(PCharToString(result), res);
			(*cb_pointer)((nim::NIMResCode)res_code, PCharToString(id), to_type, res);
		}
		delete cb_pointer;
	}
}

static void CallbackModifyMultipleMsglog(int res_code
	, const char *uid
	, nim::NIMSessionType to_type
	, const char *json_extension
	, const void *callback)
{
	if (callback)
	{
		MsgLog::ModifyMultipleMsglogCallback* cb_pointer = (MsgLog::ModifyMultipleMsglogCallback*)callback;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)res_code, PCharToString(uid), to_type);
		}
		delete cb_pointer;
	}
}

static void CallbackModifySingleMsglog(int res_code
	, const char *msg_id
	, const char *json_extension
	, const void *user_data)
{
	if (user_data)
	{
		MsgLog::ModifySingleMsglogCallback* cb_pointer = (MsgLog::ModifySingleMsglogCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)res_code, PCharToString(msg_id));
		}
		delete cb_pointer;
	}
}

static void CallbackMsglogRes(int res_code
	, const char *json_extension
	, const void *user_data)
{
	if (user_data)
	{
		MsgLog::DBFunctionCallback* cb_pointer = (MsgLog::DBFunctionCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)res_code);
		}
		delete cb_pointer;
	}
}

static void CallbackImportDBProgress(__int64 imported_count
	, __int64 total_count
	, const char *json_extension
	, const void *callback)
{
	if (callback)
	{
		MsgLog::ImportDbPrgCallback* cb_pointer = (MsgLog::ImportDbPrgCallback*)callback;
		if (*cb_pointer)
		{
			(*cb_pointer)(imported_count, total_count);
		}
	}
}

static void CallbackQueryMsgByID(int res_code
	, const char *msg_id
	, const char *msg
	, const char *json_extension
	, const void *callback)
{
	if (callback)
	{
		MsgLog::QuerySingleMsgCallback* cb_pointer = (MsgLog::QuerySingleMsgCallback*)callback;
		if (*cb_pointer)
		{
			IMMessage msg_out;
			ParseMessage(PCharToString(msg), msg_out);
			(*cb_pointer)((nim::NIMResCode)res_code, PCharToString(msg_id), msg_out);
		}
		delete cb_pointer;
	}
}

bool MsgLog::QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension/* = ""*/)
{
	if (client_msg_id.empty())
		return false;

	QuerySingleMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QuerySingleMsgCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_by_id_async)(client_msg_id.c_str(), json_extension.c_str(), &CallbackQueryMsgByID, cb_pointer);
	return true;
}

bool MsgLog::QueryMsgAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, int limit_count
	, __int64 last_time
	, const QueryMsgCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (account_id.empty() || limit_count <= 0)
		return false;

	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_async)(account_id.c_str()
		, to_type
		, limit_count
		, last_time
		, json_extension.c_str()
		, &CallbackQueryMsg
		, cb_pointer);

	return true;
}

bool MsgLog::QueryMsgOnlineAsync(const std::string &id
	, nim::NIMSessionType to_type
	, int limit_count
	, __int64 from_time
	, __int64 end_time
	, __int64 end_msg_id
	, bool reverse
	, bool need_save_to_local
	, const QueryMsgCallback& cb
	, const std::string& json_extension /*= ""*/)
{
	if (id.empty() || limit_count <= 0)
		return false;

	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_online_async)(id.c_str()
		, to_type
		, limit_count
		, from_time
		, end_time
		, end_msg_id
		, reverse
		, need_save_to_local
		, json_extension.c_str()
		, &CallbackQueryMsg
		, cb_pointer);

	return true;
}

bool MsgLog::QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range
	, const std::list<std::string> &ids
	, int limit_count
	, __int64 from_time
	, __int64 end_time
	, const std::string &end_client_msg_id
	, bool reverse
	, NIMMessageType msg_type
	, const std::string &search_content
	, const QueryMsgCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (ids.empty() || limit_count <= 0 || search_content.empty())
		return false;

	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}

	std::string ids_json;
	StrListToJsonString(ids, ids_json);
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_by_options_async)(query_range
		, ids_json.c_str()
		, limit_count
		, from_time
		, end_time
		, end_client_msg_id.c_str()
		, reverse
		, msg_type
		, search_content.c_str()
		, json_extension.c_str()
		, &CallbackQueryMsg
		, cb_pointer);

	return true;
}

bool MsgLog::BatchStatusReadAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, const BatchStatusReadCallback& cb
	, const std::string& json_extension)
{
	if (account_id.empty())
		return false;

	BatchStatusReadCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new BatchStatusReadCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_batch_status_read_async)(account_id.c_str()
		, to_type
		, json_extension.c_str()
		, &CallbackModifyMultipleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::BatchStatusDeleteAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, const BatchStatusDeleteCallback& cb
	, const std::string& json_extension)
{
	if (account_id.empty())
		return false;

	BatchStatusReadCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new BatchStatusReadCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_batch_status_delete_async)(account_id.c_str()
		, to_type
		, json_extension.c_str()
		, &CallbackModifyMultipleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::SetStatusAsync(const std::string& msg_id
	, nim::NIMMsgLogStatus msglog_status
	, const SetStatusCallback& cb
	, const std::string& json_extension)
{
	if (msg_id.empty())
		return false;

	SetStatusCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetStatusCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_set_status_async)(msg_id.c_str()
		, msglog_status
		, json_extension.c_str()
		, &CallbackModifySingleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::SetSubStatusAsync(const std::string& msg_id
	, nim::NIMMsgLogSubStatus msglog_sub_status
	, const SetSubStatusCallback& cb
	, const std::string& json_extension)
{
	if (msg_id.empty())
		return false;

	SetSubStatusCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new SetSubStatusCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_set_sub_status_async)(msg_id.c_str()
		, msglog_sub_status
		, json_extension.c_str()
		, &CallbackModifySingleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::WriteMsglogOnlyAsync(const std::string& account_id
	, NIMSessionType to_type
	, const std::string& msg_id
	, const IMMessage& msg
	, const WriteMsglogCallback& cb
	, const std::string& json_extension)
{
	if (account_id.empty() || msg_id.empty() || msg.client_msg_id_.empty())
		return false;

	WriteMsglogCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new WriteMsglogCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_write_db_only_async)(account_id.c_str()
		, to_type
		, msg_id.c_str()
		, msg.ToJsonString(false).c_str()
		, json_extension.c_str()
		, &CallbackModifySingleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::DeleteBySessionTypeAsync(bool delete_sessions
	, NIMSessionType to_type
	, const DeleteBySessionTypeCallback& cb
	, const std::string& json_extension)
{
	DeleteBySessionTypeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteBySessionTypeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_by_session_type_async)(delete_sessions
		, to_type
		, json_extension.c_str()
		, &CallbackModifyMultipleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::DeleteAsync(const std::string& session_id
	, NIMSessionType to_type
	, const std::string& msg_id
	, const DeleteCallback& cb
	, const std::string& json_extension)
{
	if (session_id.empty() || msg_id.empty())
		return false;

	DeleteCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_async)(session_id.c_str()
		, to_type
		, msg_id.c_str()
		, json_extension.c_str()
		, &CallbackModifySingleMsglog
		, cb_pointer);

	return true;
}

bool MsgLog::DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension)
{
	DeleteAllCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_all_async)(delete_sessions, json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

	return true;
}

bool MsgLog::ExportDbAsync(const std::string& dst_path, const DBFunctionCallback& cb, const std::string& json_extension)
{
	if (dst_path.empty())
		return false;

	DBFunctionCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DBFunctionCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_export_db_async)(dst_path.c_str(), json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

	return true;
}

bool MsgLog::ImportDbAsync(const std::string& src_path
	, const DBFunctionCallback& res_cb
	, const ImportDbPrgCallback& prg_cb
	, const std::string& json_extension)
{
	if (src_path.empty())
		return false;

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
	NIM_SDK_GET_FUNC(nim_msglog_import_db_async)(src_path.c_str()
		, json_extension.c_str()
		, &CallbackMsglogRes
		, response_cb_userdata
		, &CallbackImportDBProgress
		, progress_cb_pointer);

	return true;
}

static void CallbackMsgStatusChanged(int rescode, const char *result, const char *json_extent, const void *callback)
{
	if (callback)
	{
		MsgLog::MessageStatusChangedCallback* cb_pointer = (MsgLog::MessageStatusChangedCallback*)callback;
		if (*cb_pointer)
		{
			MessageStatusChangedResult result(rescode, PCharToString(result));
			(*cb_pointer)(result);
		}
	}
}

void MsgLog::SendReceiptAsync(const std::string& json_msg, const MsgLog::MessageStatusChangedCallback& cb)
{
	MsgLog::MessageStatusChangedCallback *callback = nullptr;
	if (cb)
		callback = new MsgLog::MessageStatusChangedCallback(cb);
	NIM_SDK_GET_FUNC(nim_msglog_send_receipt_async)(json_msg.c_str(), nullptr, &CallbackMsgStatusChanged, callback);
}

bool MsgLog::QueryMessageBeReaded(const IMMessage& msg)
{
	return NIM_SDK_GET_FUNC(nim_msglog_query_be_readed)(msg.ToJsonString(false).c_str(), nullptr);
}

static MsgLog::MessageStatusChangedCallback* g_cb_msg_status_changed_cb_ = nullptr;
void MsgLog::RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension/* = ""*/)
{
	if (g_cb_msg_status_changed_cb_)
	{
		delete g_cb_msg_status_changed_cb_;
		g_cb_msg_status_changed_cb_ = nullptr;
	}
	g_cb_msg_status_changed_cb_ = new MessageStatusChangedCallback(cb);

	NIM_SDK_GET_FUNC(nim_msglog_reg_status_changed_cb)(json_extension.c_str(), &CallbackMsgStatusChanged, g_cb_msg_status_changed_cb_);
}

bool MsgLog::UpdateLocalExtAsync(const std::string& msg_id
	, const std::string& local_ext
	, const UpdateLocalExtCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (msg_id.empty())
		return false;

	UpdateLocalExtCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new UpdateLocalExtCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_msglog_update_localext_async)(msg_id.c_str()
		, local_ext.c_str()
		, json_extension.c_str()
		, &CallbackModifySingleMsglog
		, cb_pointer);

	return true;

}

}
/** @file nim_cpp_msglog.cpp
  * @brief NIM SDK提供的消息历史接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#include "src/cpp_sdk/nim/api//nim_cpp_msglog.h"
#include <algorithm>
#include "src/cpp_sdk/nim/helper/nim_sdk_loader_helper.h"
#include "src/cpp_sdk/nim/api//nim_cpp_talk.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void (*nim_msglog_register_delete_msgs_self_callback)(const nim_msglog_delete_message_self_notify_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_async)(const char* account_id, nim::NIMSessionType to_type, int limit_count, int64_t last_time, const char *json_extension, nim_msglog_query_cb_func cb, const void* user_data);
typedef void(*nim_msglog_batch_status_read_async)(const char* account_id, nim::NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_msglog_set_status_async)(const char* msg_id, nim::NIMMsgLogStatus msglog_status, const char *json_extension, nim_msglog_res_cb_func cb, const void* user_data);
typedef void(*nim_msglog_set_sub_status_async)(const char* msg_id, nim::NIMMsgLogSubStatus msglog_sub_status, const char *json_extension, nim_msglog_res_cb_func cb, const void* user_data);
typedef void(*nim_msglog_batch_status_delete_async)(const char* account_id, nim::NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_msglog_batch_status_delete_async_ex)(const char* account_id, nim::NIMSessionType to_type, bool revert_by_online_query,const char *json_extension, nim_msglog_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_msglog_insert_msglog_async)(const char *talk_id, const char *json_msg, bool need_update_session, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_by_session_type_async)(bool delete_sessions, NIMSessionType to_type, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_by_session_type_async_ex)(bool delete_sessions, NIMSessionType to_type, bool revert_by_query_online, const char *json_extension, nim_msglog_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_async)(const char *account_id, NIMSessionType to_type, const char *msg_id, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_all_async)(bool delete_sessions, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_all_async_ex)(bool delete_sessions, bool revert_by_query_online, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_by_time_async)(const char *account_id, enum NIMSessionType to_type, uint64_t from_time, uint64_t to_time, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_delete_by_time_async_ex)(const char *account_id, enum NIMSessionType to_type, bool revert_by_query_online, uint64_t from_time, uint64_t to_time, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_online_async)(const char *id, nim::NIMSessionType to_type, int limit_count, int64_t from_time, int64_t end_time, int64_t end_msg_id, bool reverse, bool need_save_to_local, const char *json_extension, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_online_async2)(const char *id,NIMQueryMsgOnlineAsyncParam query_param, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_keyword_online_async)(const char *id, const char* keyword, NIMQueryMsgByKeywordOnlineAsyncParam query_param, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_id_async)(const char *client_msg_id, const char *json_extension, nim_msglog_query_single_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_options_async)(NIMMsgLogQueryRange query_range, const char *ids, int limit_count, int64_t from_time, int64_t end_time, const char *end_client_msg_id, bool reverse, NIMMessageType msg_type, const char *search_content, const char *json_extension, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_query_msg_by_options_async_ex)( const char *json_param, nim_msglog_query_cb_func cb, const void *user_data);
typedef void(*nim_msglog_update_localext_async)(const char *msg_id, const char *local_ext, const char *json_extension, nim_msglog_res_cb_func cb, const void *user_data);
						
typedef void(*nim_msglog_export_db_async)(const char *dst_path, const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);
typedef void(*nim_msglog_import_db_async)(const char *src_path, const char *json_extension, nim_msglog_modify_res_cb_func res_cb, const void *res_user_data, nim_msglog_import_prg_cb_func prg_cb, const void *prg_user_data);

typedef void(*nim_msglog_send_receipt_async)(const char *json_msg, const char *json_extension, nim_msglog_status_changed_cb_func cb, const void *user_data);
typedef bool(*nim_msglog_query_be_readed)(const char *json_msg, const char *json_extension);
typedef bool(*nim_msglog_query_receipt_sent)(const char *json_msg, const char *json_extension);
typedef void(*nim_msglog_reg_status_changed_cb)(const char *json_extension, nim_msglog_status_changed_cb_func cb, const void *user_data);
typedef void(*nim_msglog_read_all_async)(const char *json_extension, nim_msglog_modify_res_cb_func cb, const void *user_data);

typedef bool(*nim_export_backup_to_remote)(const NIMLogsBackupExportInfo* export_info);
typedef bool(*nim_import_backup_from_remote)(const NIMLogsBackupImportInfo* import_info);
typedef bool(*nim_cancel_export_backup_to_remote)();
typedef bool(*nim_cancel_import_backup_from_remote)();
typedef void(*nim_msglog_delete_history_online_async)(const char *, bool , const char *, nim_msglog_delete_history_online_res_cb_func , const void *);
typedef void(*nim_msglog_query_the_message_of_the_specified_type_async)(enum NIMSessionType to_type, 
	const char *id, 
	int limit_count, 
	int64_t from_time, 
	int64_t end_time,
	const char *end_client_msg_id,
	bool reverse,
	const char *msg_types,
	const char *json_extension, 
	nim_msglog_query_cb_func cb, 
	const void *user_data);
typedef void (*nim_msglog_delete_message_self_async)(const char *, const char *, const char *, nim_msglog_delete_message_self_res_cb_func cb, const void *user_data);
//thread talk
typedef void (*nim_msglog_query_message_is_thread_root_async)(const char* msg_client_id, nim_msglog_query_message_is_thread_root_async_cb_func cb, const void* user_data);
typedef void (*nim_msglog_query_message_online)(const NIMQueryMsgAsyncParam& param, nim_msglog_query_single_cb_func cb, const void* user_data);
typedef void (*nim_msglog_query_thread_history_msg)(const char* json_msg, const NIMQueryThreadHistoryMsgAsyncParam& param, const nim_msglog_query_thread_history_msg_cb_func& cb, const void* user_data);
#else
#include "nim_msglog.h"
#endif

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
MsgLog::AllMessageTypeList::AllMessageTypeList()
{
	list_.emplace_back(nim::kNIMMessageTypeText);
	list_.emplace_back(nim::kNIMMessageTypeImage);
	list_.emplace_back(nim::kNIMMessageTypeAudio);	
	list_.emplace_back(nim::kNIMMessageTypeVideo);
	list_.emplace_back(nim::kNIMMessageTypeLocation);
	list_.emplace_back(nim::kNIMMessageTypeNotification);
	list_.emplace_back(nim::kNIMMessageTypeFile);
	list_.emplace_back(nim::kNIMMessageTypeTips);
	list_.emplace_back(nim::kNIMMessageTypeRobot);
	list_.emplace_back(nim::kNIMMessageTypeCustom);
}
std::vector<nim::NIMMessageType> MsgLog::AllMessageTypeList::ExclusionType(const std::vector<nim::NIMMessageType>& exclusion_type_list) const
{
	std::vector<nim::NIMMessageType> ret;
	auto it = list_.begin();
	while(it != list_.end())
	{
		if(std::find_if(exclusion_type_list.begin(),exclusion_type_list.end(),[&](nim::NIMMessageType type ){
			return type == *it;
		}) == exclusion_type_list.end())
			ret.emplace_back(*it);			
		it++;
	}
	return ret;
}
const MsgLog::AllMessageTypeList MsgLog::QueryMsgOnlineAsyncParam::AllMsgTypeList;
MsgLog::QueryMsgOnlineAsyncParam::QueryMsgOnlineAsyncParam() :
		limit_count_(20),
		reverse_(true),
		need_save_to_local_(true),
		auto_download_attachment_(true),
		is_exclusion_type_(false)
{

}

bool MsgLog::QueryMsgOnlineAsyncParam::FormatParam()
{
	if (id_.empty() || limit_count_ <= 0)
		return false;
	if(msg_type_list_.size() >0)
		need_save_to_local_ = false;
	nim_cpp_wrapper_util::Json::Value extension;
	nim_cpp_wrapper_util::Json::FastWriter fw;
	extension[kNIMMsglogJsonExtKeyNeedAutoDownloadAttachment] = auto_download_attachment_;
	std::vector<nim::NIMMessageType> msg_type_list;
	if(!is_exclusion_type_)//获取指定消息类型
		msg_type_list.assign(msg_type_list_.begin(),msg_type_list_.end());		
	else//排除指定的消息类型
		msg_type_list = std::move(AllMsgTypeList.ExclusionType(msg_type_list_));
	auto it = msg_type_list.begin();
	while(it != msg_type_list.end())
	{
		extension[kNIMMsglogJsonExtKeyQueryMsgTypeList].append(*it);
		it++;
	}
	json_extension_ = fw.write(extension);
	return true;
}


MsgLog::QueryMsgByKeywordOnlineParam::QueryMsgByKeywordOnlineParam()
	: limit_count_(20)
	, reverse_(true)
{

}
std::string MsgLog::QueryMsgByOptionsAsyncParam::ToJsonString() const
{
	nim_cpp_wrapper_util::Json::Value param_json;
	param_json[kNIMQueryMsgByOptionsAsyncKeyQueryRange] = query_range_;
	for (auto it : ids_)
		param_json[kNIMQueryMsgByOptionsAsyncKeyIDS].append(it);
	param_json[kNIMQueryMsgByOptionsAsyncKeyLimit] = limit_count_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyFromTime] = from_time_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyEndTime] = end_time_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyEndClientMSGID] = end_client_msg_id_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyReverse] = reverse_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyMsgType] = msg_type_;
	param_json[kNIMQueryMsgByOptionsAsyncKeyMsgSubType] = msg_sub_type_;
	param_json[kNIMQueryMsgByOptionsAsyncKeySearchContent] = search_content_;
	return nim_cpp_wrapper_util::Json::FastWriter().write(param_json);
}
static void CallbackQueryMsg(int res_code
	, const char *id
	, nim::NIMSessionType to_type
	, const char *result
	, const char *json_extension
	, const void *callback)
{

	CallbackProxy::DoSafeCallback<MsgLog::QueryMsgCallback>(callback, [=](const MsgLog::QueryMsgCallback& cb){

		QueryMsglogResult res;
		ParseMsglogs(PCharToString(result), res);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(id), to_type,res);
	}, true);

}

static void CallbackModifyMultipleMsglog(int res_code
	, const char *uid
	, nim::NIMSessionType to_type
	, const char *json_extension
	, const void *callback)
{
	CallbackProxy::DoSafeCallback<MsgLog::ModifyMultipleMsglogCallback>(callback, [=](const MsgLog::ModifyMultipleMsglogCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(uid),to_type);
	}, true);
}

static void CallbackModifySingleMsglog(int res_code
	, const char *msg_id
	, const char *json_extension
	, const void *user_data)
{
	CallbackProxy::DoSafeCallback<MsgLog::ModifySingleMsglogCallback>(user_data, [=](const MsgLog::ModifySingleMsglogCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(msg_id));
	}, true);
}

static void CallbackMsglogRes(int res_code
	, const char *json_extension
	, const void *user_data)
{

	CallbackProxy::DoSafeCallback<MsgLog::DBFunctionCallback>(user_data, [=](const MsgLog::DBFunctionCallback& cb){

		CallbackProxy::Invoke(cb, (NIMResCode)res_code);
	},true);
}

static void CallbackImportDBProgress(int64_t imported_count
	, int64_t total_count
	, const char *json_extension
	, const void *callback)
{
	CallbackProxy::DoSafeCallback<MsgLog::ImportDbPrgCallback>(callback, [=](const MsgLog::ImportDbPrgCallback& cb){

		CallbackProxy::Invoke(cb, imported_count, total_count);
	});
}

static void CallbackQueryMsgByID(int res_code
	, const char *msg_id
	, const char *msg
	, const char *json_extension
	, const void *callback)
{

	CallbackProxy::DoSafeCallback<MsgLog::QuerySingleMsgCallback>(callback, [=](const MsgLog::QuerySingleMsgCallback& cb){

		IMMessage msg_out;
		ParseMessage(PCharToString(msg), msg_out);
		CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(msg_id), msg_out);
	},true);

}
MsgLog::DeleteMsglogSelfNotifyCallback g_delete_msglog_self_notify_cb = nullptr;
void MsgLog::RegDeleteMsglogSelfNotify(const DeleteMsglogSelfNotifyCallback &cb)
{
	g_delete_msglog_self_notify_cb = cb;
	NIM_SDK_GET_FUNC(nim_msglog_register_delete_msgs_self_callback)([](const char *infos, const void *user_data) {
		CallbackProxy::DoSafeCallback<MsgLog::DeleteMsglogSelfNotifyCallback>(user_data, [=](const MsgLog::DeleteMsglogSelfNotifyCallback& cb) {
			nim_cpp_wrapper_util::Json::Value json_param;
			if (nim_cpp_wrapper_util::Json::Reader().parse(infos, json_param) &&
				json_param.isArray())
			{
				DeleteMsglogSelfNotifyParam param;
				for (auto it : json_param)
				{
					DeleteMsglogSelfNotifyItemInfo info;
					info.session_id_ = it[kNIMDELMSGSelfNotifyKeySessionID].asString();
					info.client_id_ = it[kNIMDELMSGSelfNotifyKeyMsgClientID].asString();
					info.ext_ = it[kNIMDELMSGSelfNotifyKeyEXT].asString();
					param.item_list.emplace_back(info);
				}
				if(!param.item_list.empty())
					CallbackProxy::Invoke(cb, param);
			}
		}, false);
	}, &g_delete_msglog_self_notify_cb);
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
	, int64_t anchor_msg_time
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
		, anchor_msg_time
		, json_extension.c_str()
		, &CallbackQueryMsg
		, cb_pointer);

	return true;
}

bool MsgLog::QueryMsgOnlineAsync(const std::string &id
	, nim::NIMSessionType to_type
	, int limit_count
	, int64_t from_time
	, int64_t end_time
	, int64_t end_msg_id
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
bool MsgLog::QueryMsgOnlineAsync(const MsgLog::QueryMsgOnlineAsyncParam& param, const QueryMsgCallback& cb)
{
	if (param.id_.empty() || param.limit_count_ <= 0)
		return false;
	if (!(const_cast<MsgLog::QueryMsgOnlineAsyncParam&>(param)).FormatParam())
		return false;
	/*return QueryMsgOnlineAsync(
		param.id_,
		param.to_type_,
		param.limit_count_,
		param.from_time_,
		param.end_time_,
		param.end_msg_id_,
		param.reverse_,
		param.need_save_to_local_,
		cb,
		param.json_extension_);*/
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	NIMQueryMsgOnlineAsyncParam query_param;
	query_param.to_type_ = param.to_type_;
	query_param.limit_count_ = param.limit_count_;
	query_param.from_time_ = param.from_time_;
	query_param.end_time_ = param.end_time_;
	query_param.end_msg_id_ = param.end_msg_id_;
	query_param.reverse_ = param.reverse_;
	query_param.is_exclusion_type_ = param.is_exclusion_type_;
	query_param.need_save_to_local_ = param.need_save_to_local_;
	query_param.auto_download_attachment_ = param.auto_download_attachment_;
	query_param.msg_type_list_ = const_cast<NIMMessageType*>(param.msg_type_list_.data());
	query_param.type_list_length_ = param.msg_type_list_.size();
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_online_async2)(param.id_.c_str(),
		query_param, &CallbackQueryMsg, cb_pointer);
	return true;
}

bool MsgLog::QueryMsgByKeywordOnlineAsync(const MsgLog::QueryMsgByKeywordOnlineParam& param, const QueryMsgCallback& cb)
{
	if (param.id_.empty() || param.limit_count_ <= 0 || param.keyword_.empty())
		return false;
	if (!(const_cast<MsgLog::QueryMsgByKeywordOnlineParam&>(param)).FormatParam())
		return false;
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	NIMQueryMsgByKeywordOnlineAsyncParam query_param;
	query_param.to_type_ = param.to_type_;
	query_param.limit = param.limit_count_;
	query_param.from_time_ = param.from_time_;
	query_param.end_time_ = param.end_time_;
	query_param.reverse_ = param.reverse_;
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_by_keyword_online_async)(param.id_.c_str(),
		param.keyword_.c_str(), query_param, &CallbackQueryMsg, cb_pointer);

	return true;
}

bool MsgLog::QueryMsgOfSpecifiedTypeInASessionAsync(nim::NIMSessionType to_type
	, const std::string &id
	, int limit_count
	, int64_t from_time
	, int64_t end_time
	, const std::string &end_client_msg_id
	, bool reverse
	, std::list<NIMMessageType> msg_type
	, const QueryMsgCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}

	nim_cpp_wrapper_util::Json::Value values;
	nim_cpp_wrapper_util::Json::FastWriter fw;
	for (auto iter = msg_type.begin(); iter != msg_type.end(); ++iter)
	{
		values.append(*iter);
	}

	NIM_SDK_GET_FUNC(nim_msglog_query_the_message_of_the_specified_type_async)(to_type
		, id.c_str()
		, limit_count
		, from_time
		, end_time
		, end_client_msg_id.c_str()
		, reverse
		, fw.write(values).c_str()
		, json_extension.c_str()
		, &CallbackQueryMsg
		, cb_pointer);

	return true;
}

bool MsgLog::QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range
	, const std::list<std::string> &ids
	, int limit_count
	, int64_t from_time
	, int64_t end_time
	, const std::string &end_client_msg_id
	, bool reverse
	, NIMMessageType msg_type
	, const std::string &search_content
	, const QueryMsgCallback& cb
	, const std::string& json_extension/* = ""*/)
{
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
bool MsgLog::QueryMsgByOptionsAsyncEx(const QueryMsgByOptionsAsyncParam& param, const QueryMsgCallback& cb)
{
	QueryMsgCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMsgCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_query_msg_by_options_async_ex)(param.ToJsonString().c_str()
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

bool MsgLog::BatchStatusDeleteAsyncEx(const std::string& account_id
	, nim::NIMSessionType to_type
	, bool revert_by_query_online
	, const BatchStatusDeleteCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (account_id.empty())
		return false;

	BatchStatusReadCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new BatchStatusReadCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_batch_status_delete_async_ex)(account_id.c_str()
		, to_type
		, revert_by_query_online
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

bool MsgLog::WriteMsglogToLocalAsync(const std::string& talk_id
	, const IMMessage& msg
	, bool need_update_session
	, const WriteMsglogCallback& cb
	, const std::string& json_extension)
{
	if (talk_id.empty() || msg.client_msg_id_.empty())
		return false;

	WriteMsglogCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new WriteMsglogCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_insert_msglog_async)(talk_id.c_str()
		, msg.ToJsonString(false).c_str()
		, need_update_session
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
bool MsgLog::DeleteBySessionTypeAsyncEx(bool delete_sessions
	, NIMSessionType to_type
	, bool revert_by_query_online
	, const DeleteBySessionTypeCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	DeleteBySessionTypeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteBySessionTypeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_by_session_type_async_ex)(delete_sessions
		, to_type
		, revert_by_query_online
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

bool MsgLog::DeleteAllAsyncEx(bool delete_sessions, bool revert_by_query_online, const DeleteAllCallback& cb, const std::string& json_extension)
{
	DeleteAllCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteAllCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_all_async_ex)(delete_sessions, revert_by_query_online, json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

	return true;
}

bool MsgLog::DeleteMsgByTimeAsync(const std::string& session_id, NIMSessionType to_type, uint64_t timestamp1, uint64_t timestamp2, const DeleteMsgByTimeCallback& cb, const std::string& json_extension /*= ""*/)
{
	DeleteMsgByTimeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteMsgByTimeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_by_time_async)(session_id.c_str(), to_type, timestamp1, timestamp2, json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

	return true;
}

bool MsgLog::DeleteMsgByTimeAsyncEx(const std::string& session_id, NIMSessionType to_type, bool revert_by_query_online, uint64_t timestamp1, uint64_t timestamp2, const DeleteMsgByTimeCallback& cb, const std::string& json_extension /*= ""*/)
{
	DeleteMsgByTimeCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DeleteMsgByTimeCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_by_time_async_ex)(session_id.c_str(), to_type,revert_by_query_online, timestamp1, timestamp2, json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

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
			std::string res = PCharToString(result);
			MessageStatusChangedResult result(rescode, res);
			CallbackProxy::Invoke(*cb_pointer, result);
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

bool MsgLog::QuerySentMessageBeReaded(const IMMessage& msg)
{
	return NIM_SDK_GET_FUNC(nim_msglog_query_be_readed)(msg.ToJsonString(false).c_str(), nullptr);
}

bool MsgLog::QueryReceivedMsgReceiptSent(const IMMessage& msg)
{
	return NIM_SDK_GET_FUNC(nim_msglog_query_receipt_sent)(msg.ToJsonString(false).c_str(), nullptr);
}

static MsgLog::MessageStatusChangedCallback g_cb_msg_status_changed_cb_ = nullptr;
void MsgLog::RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension/* = ""*/)
{	
	g_cb_msg_status_changed_cb_ = cb;
	NIM_SDK_GET_FUNC(nim_msglog_reg_status_changed_cb)(json_extension.c_str(), &CallbackMsgStatusChanged, &g_cb_msg_status_changed_cb_);
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
void MsgLog::UnregMsgologCb()
{
	g_cb_msg_status_changed_cb_ = nullptr;
}
bool MsgLog::ReadAllAsync(const DBFunctionCallback& cb, const std::string& json_extension/* = ""*/)
{
	DBFunctionCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new DBFunctionCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_read_all_async)(json_extension.c_str(), &CallbackMsglogRes, cb_pointer);

	return true;
}
bool MsgLog::ExportBackupToRemote(const LogsBackupExportInfo& export_info)
{
	return NIM_SDK_GET_FUNC(nim_export_backup_to_remote)(export_info.ToCStruct());
}
bool MsgLog::ImportBackupFromRemote(const LogsBackupImportInfo& import_info)
{
	return NIM_SDK_GET_FUNC(nim_import_backup_from_remote)(import_info.ToCStruct());
}
void MsgLog::CancelImportBackupFromRemote()
{
	NIM_SDK_GET_FUNC(nim_cancel_import_backup_from_remote)();
}
void MsgLog::CancelExportBackupToRemote()
{
	NIM_SDK_GET_FUNC(nim_cancel_export_backup_to_remote)();
}
void MsgLog::DeleteHistoryOnlineAsync(const std::string& accid, bool delete_roaming, const std::string& json_extension, const DeleteHistoryOnLineAsyncCallback& cb)
{
	DeleteHistoryOnLineAsyncCallback* cb_pointer = nullptr;
	if (cb != nullptr)
	{
		cb_pointer = new DeleteHistoryOnLineAsyncCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_msglog_delete_history_online_async)(accid.c_str(), delete_roaming, json_extension.c_str(), \
		[](int res_code, const char* accid, const void *user_data) {
		std::string id(accid);
		CallbackProxy::DoSafeCallback<MsgLog::DeleteHistoryOnLineAsyncCallback>(user_data, [=](const MsgLog::DeleteHistoryOnLineAsyncCallback& cb) {
			CallbackProxy::Invoke(cb, (NIMResCode)res_code,id);
		}, true);
	}, cb_pointer);
}

void MsgLog::DeleteMessageSelfAsync(const IMMessage &msg, const std::string ext, const DeleteMsglogSelfCallback& cb)
{
	DeleteMsglogSelfCallback* cb_pointer = nullptr;
	if (cb != nullptr)
	{
		cb_pointer = new DeleteMsglogSelfCallback(cb);
	}
	std::string json_msg;
	NIM_SDK_GET_FUNC(nim_msglog_delete_message_self_async)(msg.ToJsonString(false).c_str(), ext.c_str(), "", \
		[](int res_code, const void *user_data) {
			CallbackProxy::DoSafeCallback<MsgLog::DeleteMsglogSelfCallback>(user_data, [=](const MsgLog::DeleteMsglogSelfCallback& cb) {
				CallbackProxy::Invoke(cb, (NIMResCode)res_code);
				}, true);
		}, cb_pointer);
}
void MsgLog::QueryMessageIsThreadRoot(const std::string msg_client_id, const QueryMessageIsThreadRootAsyncCallback& cb)
{
	QueryMessageIsThreadRootAsyncCallback* cb_pointer = nullptr;
	if (cb != nullptr)
	{
		cb_pointer = new QueryMessageIsThreadRootAsyncCallback(cb);
	}
	std::string json_msg;
	NIM_SDK_GET_FUNC(nim_msglog_query_message_is_thread_root_async)(msg_client_id.c_str(), 
		[](int res_code, const char* client_id, bool is_root, int reply_count, const void* user_data) {
			CallbackProxy::DoSafeCallback<MsgLog::QueryMessageIsThreadRootAsyncCallback>(user_data, [=](const MsgLog::QueryMessageIsThreadRootAsyncCallback& cb) {
				CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString(client_id), is_root, reply_count);
				}, true);
		}, cb_pointer);
}
void MsgLog::QueryMessageOnline(const QueryMsgAsyncParam& param, const QueryMessageOnlineCallback& cb)
{
	QueryMessageOnlineCallback* cb_pointer = nullptr;
	if (cb != nullptr)
	{
		cb_pointer = new QueryMessageOnlineCallback(cb);
	}
	NIMQueryMsgAsyncParam c_param;
	memset(&c_param, 0, sizeof(NIMQueryMsgAsyncParam));
	c_param.to_type_ = param.to_type_;
	memcpy(c_param.from_account, param.from_account.data(), param.from_account.length());
	memcpy(c_param.to_account, param.to_account.data(), param.to_account.length());
	c_param.server_id = param.server_id;
	memcpy(c_param.client_id, param.client_id.data(), param.client_id.length());
	c_param.time = param.time;
	std::string json_msg;
	NIM_SDK_GET_FUNC(nim_msglog_query_message_online)(c_param,
		[](int res_code, const char* msg_id, const char* result, const char* json_extension, const void* user_data) {
			CallbackProxy::DoSafeCallback<MsgLog::QueryMessageOnlineCallback>(user_data, [=](const MsgLog::QueryMessageOnlineCallback& cb) {
				IMMessage msg_out;
				ParseMessage(PCharToString(result), msg_out);
				CallbackProxy::Invoke(cb, (NIMResCode)res_code, PCharToString( msg_id),msg_out);
				}, true);
		}, cb_pointer);
}
void MsgLog::QueryThreadHistoryMsg(const IMMessage& msg, const QueryThreadHistoryMsgAsyncParam& param, const QueryThreadHistoryMsgCallback& cb)
{
	QueryThreadHistoryMsgCallback* cb_pointer = nullptr;
	if (cb != nullptr)
	{
		cb_pointer = new QueryThreadHistoryMsgCallback(cb);
	}
	NIMQueryThreadHistoryMsgAsyncParam c_param;
	memset(&c_param, 0, sizeof(NIMQueryThreadHistoryMsgAsyncParam));	
	c_param.from_time = param.from_time;
	c_param.to_time = param.to_time;
	c_param.exclude_msg_id = param.exclude_msg_id;
	c_param.limit = param.limit;
	c_param.reverse = param.reverse;
	std::string json_msg;
	NIM_SDK_GET_FUNC(nim_msglog_query_thread_history_msg)(msg.ToJsonString(false).c_str(),c_param,
		[](int res_code, const char* root_msg, int total, uint64_t timestamp, const char* msgs, const void* user_data) {
			CallbackProxy::DoSafeCallback<MsgLog::QueryThreadHistoryMsgCallback>(user_data, [=](const MsgLog::QueryThreadHistoryMsgCallback& cb) {
				IMMessage obj_msg_out;
				ParseMessage(PCharToString(root_msg), obj_msg_out);
				QueryMsglogResult res;
				ParseMsglogs(PCharToString(msgs), res);
				CallbackProxy::Invoke(cb, (NIMResCode)res_code, obj_msg_out,total, timestamp, res.msglogs_);
				}, true);
		}, cb_pointer);
}

void MsgLog::UnregMsglogCb()
{
	UnregMsgologCb();
	g_delete_msglog_self_notify_cb = nullptr;
}
}

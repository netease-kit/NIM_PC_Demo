#ifndef _NIM_SDK_CPP_MSGLOG_H_
#define _NIM_SDK_CPP_MSGLOG_H_

#include <string>
#include <functional>
#include "nim_msglog_helper.h"
#include "nim_talk_helper.h"

namespace nim
{

#include "nim_msglog_def.h"
#include "nim_res_code_def.h"

/** @class MsgLog
  * @brief NIM SDK提供的消息历史接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */
class MsgLog
{

public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& id, nim::NIMSessionType to_type, const QueryMsglogResult& result)> QueryMsgCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id, const IMMessage& msg)> QuerySingleMsgCallback; /**< 查询单条消息历史回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> ModifyMultipleMsglogCallback;
	typedef ModifyMultipleMsglogCallback BatchStatusReadCallback;
	typedef ModifyMultipleMsglogCallback BatchStatusDeleteCallback;
	typedef ModifyMultipleMsglogCallback DeleteBySessionTypeCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> ModifySingleMsglogCallback;
	typedef ModifySingleMsglogCallback SetStatusCallback;
	typedef ModifySingleMsglogCallback SetSubStatusCallback;
	typedef ModifySingleMsglogCallback WriteMsglogCallback;
	typedef ModifySingleMsglogCallback DeleteCallback;
	typedef ModifySingleMsglogCallback DeleteAllCallback;

	typedef std::function<void(nim::NIMResCode res_code)> DBFunctionCallback;
	typedef std::function<void(__int64 imported_count, __int64 total_count)> ImportDbPrgCallback;

	/** @fn static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "")
	* 根据消息ID查询本地（单条）消息
	* @param[in] client_msg_id		客户端消息ID
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb				查询本地消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "");

	/** @fn static bool QueryMsgAsync(const std::string& account_id, nim::NIMSessionType to_type, int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 查询本地消息
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] limit_count	一次查询数量，建议20
	* @param[in] last_time	上次查询最后一条消息的时间戳
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询本地消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgAsync(const std::string& account_id
		, nim::NIMSessionType to_type
		, int limit_count
		, __int64 last_time
		, const QueryMsgCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool QueryMsgOnlineAsync(const std::string &id, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse, bool need_save_to_local, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 在线查询消息（不包括系统消息）
	* @param[in] id				查询id，对方的account id或者群组tid。
	* @param[in] to_type			会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] limit_count		本次查询的消息条数上限(最多100条)
	* @param[in] from_time		起始时间点，单位：毫秒
	* @param[in] end_time			结束时间点，单位：毫秒
	* @param[in] end_msg_id		结束查询的最后一条消息的server_msg_id(不包含在查询结果中)
	* @param[in] reverse			true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false）
	* @param[in] need_save_to_local true: 将在线查询结果保存到本地，false: 不保存
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb				在线查询消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgOnlineAsync(const std::string &id
		, nim::NIMSessionType to_type
		, int limit_count
		, __int64 from_time
		, __int64 end_time
		, __int64 end_msg_id
		, bool reverse
		, bool need_save_to_local
		, const QueryMsgCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range, const std::list<std::string> &ids, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse, NIMMessageType msg_type, const std::string &search_content, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 在线查询消息（不包括系统消息）
	* @param[in] query_range	消息历史的检索范围（目前暂不支持某些范围的组合检索，详见NIMMsgLogQueryRange说明）
	* @param[in] ids			会话id（对方的account id或者群组tid）的集合，目前暂不支持多个的组合检索，详见NIMMsgLogQueryRange说明
	* @param[in] limit_count	本次查询的消息条数上限(最多100条)
	* @param[in] from_time		起始时间点，单位：毫秒
	* @param[in] end_time		结束时间点，单位：毫秒
	* @param[in] end_msg_id		结束查询的最后一条消息的server_msg_id(不包含在查询结果中)
	* @param[in] reverse		true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false）
	* @param[in] msg_type		检索的消息类型（目前只支持kNIMMessageTypeText、kNIMMessageTypeImage和kNIMMessageTypeFile这三种类型消息）
	* @param[in] search_content	检索文本（目前只支持kNIMMessageTypeText和kNIMMessageTypeFile这两种类型消息的文本关键字检索，即支持文字消息和文件名的检索。如果合并检索，需使用未知类型消息kNIMMessageTypeUnknown）
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb				在线查询消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range
		, const std::list<std::string> &ids
		, int limit_count
		, __int64 from_time
		, __int64 end_time
		, __int64 end_msg_id
		, bool reverse
		, NIMMessageType msg_type
		, const std::string &search_content
		, const QueryMsgCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool BatchStatusReadAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusReadCallback& cb, const std::string& json_extension = "")
	* 在线查询消息（不包括系统消息）
	* @param[in] id				查询id，对方的account id或者群组tid。
	* @param[in] to_type			会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] limit_count		本次查询的消息条数上限(最多100条)
	* @param[in] from_time		起始时间点，单位：毫秒
	* @param[in] end_time			结束时间点，单位：毫秒
	* @param[in] end_msg_id		结束查询的最后一条消息的server_msg_id(不包含在查询结果中)
	* @param[in] reverse			true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false）
	* @param[in] need_save_to_local true: 将在线查询结果保存到本地，false: 不保存
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb				在线查询消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool BatchStatusReadAsync(const std::string& account_id
		, nim::NIMSessionType to_type
		, const BatchStatusReadCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool BatchStatusDeleteAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusDeleteCallback& cb, const std::string& json_extension = "")
	* 批量设置未读状态为已读消息状态
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool BatchStatusDeleteAsync(const std::string& account_id
		, nim::NIMSessionType to_type
		, const BatchStatusDeleteCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool SetStatusAsync(const std::string& msg_id, nim::NIMMsgLogStatus msglog_status, const SetStatusCallback& cb, const std::string& json_extension = "")
	* 设置消息状态
	* @param[in] msg_id		消息id
	* @param[in] msglog_status 消息状态枚举值
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool SetStatusAsync(const std::string& msg_id
		, nim::NIMMsgLogStatus msglog_status
		, const SetStatusCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool SetSubStatusAsync(const std::string& msg_id, nim::NIMMsgLogSubStatus msglog_sub_status, const SetSubStatusCallback& cb, const std::string& json_extension = "")
	* 设置消息子状态
	* @param[in] msg_id		消息id
	* @param[in] msglog_sub_status 消息子状态枚举值
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool SetSubStatusAsync(const std::string& msg_id
		, nim::NIMMsgLogSubStatus msglog_sub_status
		, const SetSubStatusCallback& cb
		, const std::string& json_extension = "");

	/** @fn	static bool WriteMsglogOnlyAsync(const std::string& account_id, NIMSessionType to_type, const std::string& msg_id, const std::string& json_msg, const WriteMsglogCallback& cb, const std::string& json_extension = "")
	* 只往本地消息历史数据库里写入一条消息（如果已存在这条消息，则更新。通常是APP的本地自定义消息，并不会发给服务器）
	* @param[in] account_id	会话id，对方的account id或者群组tid
	* @param[in] to_type	    会话类型
	* @param[in] msg_id		消息id
	* @param[in] msg			消息内容
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool WriteMsglogOnlyAsync(const std::string& account_id
		, NIMSessionType to_type
		, const std::string& msg_id
		, const IMMessage& msg
		, const WriteMsglogCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool DeleteBySessionTypeAsync(bool delete_sessions, NIMSessionType to_type, const DeleteBySessionTypeCallback& cb, const std::string& json_extension = "")
	* 删除指定会话类型的所有消息
	* @param[in] to_type	    会话类型
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_ex_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DeleteBySessionTypeAsync(bool delete_sessions
		, NIMSessionType to_type
		, const DeleteBySessionTypeCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool DeleteAsync(const std::string& session_id, NIMSessionType to_type, const std::string& msg_id, const DeleteCallback& cb, const std::string& json_extension = "")
	* 删除指定一条消息
	* @param[in] session_id	会话id，对方的account id或者群组tid
	* @param[in] to_type	    会话类型
	* @param[in] msg_id		消息id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DeleteAsync(const std::string& session_id
		, NIMSessionType to_type
		, const std::string& msg_id
		, const DeleteCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension = "")
	* 删除全部消息历史
	* @param[in] delete_sessions 是否删除所有会话列表项（即全部最近联系人）。
	*							   ture则删除，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandRemoveAll事件（不会触发每个会话项的kNIMSessionCommandRemove事件）；
	*							   false则不删除，并将所有会话项的最后一条消息的状态kNIMSessionMsgStatus设置为已删除状态，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandAllMsgDeleted事件（不会触发每个会话项的kNIMSessionCommandUpdate事件，避免频繁通知上层）。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ExportDbAsync(const std::string& dst_path, const ExportDbCallback& cb, const std::string& json_extension = "")
	* 导出整个消息历史DB文件（不包括系统消息历史）
	* @param[in] dst_path		导出时保存的目标全路径（UTF-8编码）。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool ExportDbAsync(const std::string& dst_path, const DBFunctionCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ImportDbAsync(const std::string& src_path, const ImportDbCallback& cb, const ImportDbPrgCallback& prg_cb, const std::string& json_extension = "")
	* 导入消息历史DB文件（不包括系统消息历史）。先验证是否自己的消息历史文件和DB加密密钥(见nim_client_def.h里的kNIMDataBaseEncryptKey），如果验证不通过，则不导入。
	* @param[in] src_path			导入源文件的全路径（UTF-8编码）。
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] res_cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] res_user_data	APP的自定义用户数据，SDK只负责传回给回调函数res_cb，不做任何处理！
	* @param[in] prg_cb			导入进度的回调函数， nim_msglog_import_prg_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] prg_user_data	APP的自定义用户数据，SDK只负责传回给回调函数prg_cb，不做任何处理！
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool ImportDbAsync(const std::string& src_path
		, const DBFunctionCallback& cb
		, const ImportDbPrgCallback& prg_cb
		, const std::string& json_extension = "");


};

} 

#endif //_NIM_SDK_CPP_MSGLOG_H_
/** @file nim_cpp_msglog.h
  * @brief NIM SDK提供的消息历史接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_MSGLOG_H_
#define _NIM_SDK_CPP_MSGLOG_H_

#include <string>
#include <functional>
#include "nim_msglog_helper.h"
#include "nim_talk_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_msglog_def.h"
#include "nim_res_code_def.h"

/** @class MsgLog
  * @brief NIM SDK提供的消息历史接口
  */
class MsgLog
{

public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& id, nim::NIMSessionType to_type, const QueryMsglogResult& result)> QueryMsgCallback; /**< 查询消息历史回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id, const IMMessage& msg)> QuerySingleMsgCallback; /**< 查询单条消息历史回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> ModifyMultipleMsglogCallback;	/**< 修改（多条）消息历史回调模板 */
	typedef ModifyMultipleMsglogCallback BatchStatusReadCallback;	/**< 批量设置消息历史已读状态回调模板 */
	typedef ModifyMultipleMsglogCallback BatchStatusDeleteCallback;	/**< 批量删除消息历史回调模板 */
	typedef ModifyMultipleMsglogCallback DeleteBySessionTypeCallback;/**< 批量删除消息历史回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> ModifySingleMsglogCallback; /**< 修改（单条）消息历史回调模板 */
	typedef ModifySingleMsglogCallback SetStatusCallback;	/**< 修改消息历史状态回调模板 */
	typedef ModifySingleMsglogCallback SetSubStatusCallback;/**< 修改消息历史子状态回调模板 */
	typedef ModifySingleMsglogCallback UpdateLocalExtCallback;/**< 修改消息历史本地扩展字段内容回调模板 */
	typedef ModifySingleMsglogCallback WriteMsglogCallback;	/**< 保存消息历史回调模板 */
	typedef ModifySingleMsglogCallback DeleteCallback;		/**< 删除消息历史回调模板 */

	typedef std::function<void(nim::NIMResCode res_code)> DBFunctionCallback;		/**< 消息历史变更回调模板 */
	typedef DBFunctionCallback DeleteAllCallback;	/**< 删除全部消息历史回调模板 */

	typedef std::function<void(__int64 imported_count, __int64 total_count)> ImportDbPrgCallback;	/**< 导入消息历史回调模板 */

	typedef std::function<void(const MessageStatusChangedResult&)>	MessageStatusChangedCallback;	/**< 发送消息已读回执通知回调模板 */

	/** @fn static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "")
	 * 根据消息ID查询本地（单条）消息
	 * @param[in] client_msg_id		客户端消息ID
	 * @param[in] json_extension	json扩展参数（备用，目前不需要）
	 * @param[in] cb				查询本地消息的回调函数
	 * @return bool 检查参数如果不符合要求则返回失败
	 */
	static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "");

	/** @fn static bool QueryMsgAsync(const std::string& account_id, nim::NIMSessionType to_type, int limit_count, __int64 anchor_msg_time, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 查询本地消息
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] limit_count	一次查询数量，建议20
	* @param[in] anchor_msg_time 作为此次查询的定位点的消息历史的消息时间戳（上次查询最后一条消息的时间戳，按指定的时间的顺序起查，默认为逆序，2.4.0之前命名为last_name）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询本地消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgAsync(const std::string& account_id
		, nim::NIMSessionType to_type
		, int limit_count
		, __int64 anchor_msg_time
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

	/** @fn static bool QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range, const std::list<std::string> &ids, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, const std::string &end_client_msg_id, bool reverse, NIMMessageType msg_type, const std::string &search_content, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 根据指定条件查询本地消息,使用此接口可以完成全局搜索等功能,具体请参阅开发手册 http://dev.netease.im/docs?doc=pc&#历史记录
	* @param[in] query_range	消息历史的检索范围（目前暂不支持某些范围的组合检索，详见NIMMsgLogQueryRange说明）
	* @param[in] ids			会话id（对方的account id或者群组tid）的集合，目前暂不支持多个的组合检索，详见NIMMsgLogQueryRange说明
	* @param[in] limit_count	本次查询的消息条数上限(最多100条)
	* @param[in] from_time		起始时间点，单位：毫秒
	* @param[in] end_time		结束时间点，单位：毫秒
	* @param[in] end_client_msg_id		结束查询的最后一条消息的end_client_msg_id(不包含在查询结果中)
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
		, const std::string &end_client_msg_id
		, bool reverse
		, NIMMessageType msg_type
		, const std::string &search_content
		, const QueryMsgCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool BatchStatusReadAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusReadCallback& cb, const std::string& json_extension = "")
	* 批量设置已读状态
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
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

	/** @fn	static bool WriteMsglogToLocalAsync(const std::string& talk_id, bool need_update_session, const std::string& json_msg, const WriteMsglogCallback& cb, const std::string& json_extension = "")
	* 只往本地消息历史数据库里写入一条消息（如果已存在这条消息，则更新。通常是APP的本地自定义消息，并不会发给服务器）
	* @param[in] account_id	会话id，对方的account id或者群组tid
	* @param[in] msg			消息内容
	* @param[in] need_update_session		是否更新会话列表（一般最新一条消息有这个需求）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool WriteMsglogToLocalAsync(const std::string& talk_id
		, const IMMessage& msg
		, bool need_update_session
		, const WriteMsglogCallback& cb
		, const std::string& json_extension = "");

	/** @fn static bool DeleteBySessionTypeAsync(bool delete_sessions, NIMSessionType to_type, const DeleteBySessionTypeCallback& cb, const std::string& json_extension = "")
	* 删除指定会话类型的所有消息
	* @param[in] delete_sessions	    是否删除会话
	* @param[in] to_type	    会话类型
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
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
	* @param[in] cb			操作结果的回调函数
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
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool ExportDbAsync(const std::string& dst_path, const DBFunctionCallback& cb, const std::string& json_extension = "");

	/** @fn static bool ImportDbAsync(const std::string& src_path, const ImportDbCallback& cb, const ImportDbPrgCallback& prg_cb, const std::string& json_extension = "")
	* 导入消息历史DB文件（不包括系统消息历史）。先验证是否自己的消息历史文件和DB加密密钥(见nim_client_def.h里的kNIMDataBaseEncryptKey），如果验证不通过，则不导入。
	* @param[in] src_path			导入源文件的全路径（UTF-8编码）。
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @param[in] prg_cb			导入进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool ImportDbAsync(const std::string& src_path
		, const DBFunctionCallback& cb
		, const ImportDbPrgCallback& prg_cb
		, const std::string& json_extension = "");

	/** @fn void SendReceiptAsync(const std::string& json_msg, const MessageStatusChangedCallback& cb)
	* 发送消息已读回执
	* @param[in] json_msg			已读消息json string。
	* @param[in] cb				操作结果的回调函数
	* @return void 无返回值
	*/
	static void SendReceiptAsync(const std::string& json_msg, const MessageStatusChangedCallback& cb);

	/** @fn bool QueryMessageBeReaded(const IMMessage& msg)
	* 查询自己发送的消息是否被对方已读
	* @param[in] msg			消息。
	* @return bool 是否被已读
	*/
	static bool QueryMessageBeReaded(const IMMessage& msg);

	/** @fn void RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension = "")
	* (全局回调)注册全局的消息状态变更通知（目前只支持已读状态的通知）
	* @param[in] cb				回调函数
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension = "");

	/** @fn static bool UpdateLocalExtAsync(const std::string& msg_id, const std::string& local_ext, const UpdateLocalExtCallback& cb, const std::string& json_extension = "")
	* 更新本地消息扩展字段内容
	* @param[in] msg_id		消息id
	* @param[in] local_ext  本地扩展字段内容
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool UpdateLocalExtAsync(const std::string& msg_id
		, const std::string& local_ext
		, const UpdateLocalExtCallback& cb
		, const std::string& json_extension = "");
};

} 

#endif //_NIM_SDK_CPP_MSGLOG_H_
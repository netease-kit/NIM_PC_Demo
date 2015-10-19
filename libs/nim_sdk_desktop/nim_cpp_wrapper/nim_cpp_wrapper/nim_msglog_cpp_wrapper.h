#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_msglog_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_res_code_def.h"
#include "nim_global_define.h"

/** @class MsgLog
  * @brief NIM SDK提供的消息历史接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class MsgLog
{

public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& id, nim::NIMSessionType to_type, const std::string& result)> QueryMsgCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> BatchStatusReadCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> BatchStatusDeleteCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> SetStatusCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> SetSubStatusCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> WriteMsglogCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> DeleteBySessionTypeCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> DeleteCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> DeleteAllCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> ExportDbCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& msg_id)> ImportDbCallback;
	typedef std::function<void(int64_t imported_count, int64_t total_count)> ImportDbPrgCallback;

	/** 
	* 查询本地消息
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] limit_count	一次查询数量，建议20
	* @param[in] last_time	上次查询最后一条消息的时间戳
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询本地消息的回调函数
	* @return void 无返回值
	*/
	static void QueryMsgAsync(const std::string& account_id, nim::NIMSessionType to_type, int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension = "");

	/** 
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
	* @return void 无返回值
	*/
	static void QueryMsgOnlineAsync(const std::string &id, nim::NIMSessionType to_type, int limit_count, __int64 from_time, __int64 end_time, __int64 end_msg_id, bool reverse,
		bool need_save_to_local, const QueryMsgCallback& cb, const std::string& json_extension = "");

	/** 
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
	* @return void 无返回值
	*/
	static void BatchStatusReadAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusReadCallback& cb, const std::string& json_extension = "");

	/** 
	* 批量设置未读状态为已读消息状态
	* @param[in] account_id	查询id，account_id/uid或者tid
	* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void BatchStatusDeleteAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusDeleteCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置消息状态
	* @param[in] msg_id		消息id
	* @param[in] msglog_status 消息状态枚举值
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void SetStatusAsync(const std::string& msg_id, nim::NIMMsgLogStatus msglog_status, const SetStatusCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置消息子状态
	* @param[in] msg_id		消息id
	* @param[in] msglog_sub_status 消息子状态枚举值
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void SetSubStatusAsync(const std::string& msg_id, nim::NIMMsgLogSubStatus msglog_sub_status, const SetSubStatusCallback& cb, const std::string& json_extension = "");

	/**
	* 只往本地消息历史数据库里写入一条消息（如果已存在这条消息，则更新。通常是APP的本地自定义消息，并不会发给服务器）
	* @param[in] account_id	会话id，对方的account id或者群组tid
	* @param[in] to_type	    会话类型
	* @param[in] msg_id		消息id
	* @param[in] json_msg		消息体Json string (Keys SEE MORE `nim_talk_def.h` 『消息结构 Json Keys』)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @return void 无返回值
	*/
	static void WriteMsglogOnlyAsync(const std::string& account_id, NIMSessionType to_type, const std::string& msg_id, const std::string& json_msg, const WriteMsglogCallback& cb, const std::string& json_extension = "");

	/** @
	* 删除指定会话类型的所有消息
	* @param[in] to_type	    会话类型
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_ex_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void DeleteBySessionTypeAsync(bool delete_sessions, NIMSessionType to_type, const DeleteBySessionTypeCallback& cb, const std::string& json_extension = "");

	/** 
	* 删除指定一条消息
	* @param[in] session_id	会话id，对方的account id或者群组tid
	* @param[in] to_type	    会话类型
	* @param[in] msg_id		消息id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void DeleteAsync(const std::string& session_id, NIMSessionType to_type, const std::string& msg_id, const DeleteCallback& cb, const std::string& json_extension = "");

	/** 
	* 删除全部消息历史
	* @param[in] delete_sessions 是否删除所有会话列表项（即全部最近联系人）。
	*							   ture则删除，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandRemoveAll事件（不会触发每个会话项的kNIMSessionCommandRemove事件）；
	*							   false则不删除，并将所有会话项的最后一条消息的状态kNIMSessionMsgStatus设置为已删除状态，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandAllMsgDeleted事件（不会触发每个会话项的kNIMSessionCommandUpdate事件，避免频繁通知上层）。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension = "");

	/**
	* 导出整个消息历史DB文件（不包括系统消息历史）
	* @param[in] dst_path		导出时保存的目标全路径（UTF-8编码）。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void ExportDbAsync(const std::string& dst_path, const ExportDbCallback& cb, const std::string& json_extension = "");

	/**
	* 导入消息历史DB文件（不包括系统消息历史）。先验证是否自己的消息历史文件和DB加密密钥(见nim_client_def.h里的kNIMDataBaseEncryptKey），如果验证不通过，则不导入。
	* @param[in] src_path			导入源文件的全路径（UTF-8编码）。
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] res_cb			操作结果的回调函数， nim_msglog_res_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] res_user_data	APP的自定义用户数据，SDK只负责传回给回调函数res_cb，不做任何处理！
	* @param[in] prg_cb			导入进度的回调函数， nim_msglog_import_prg_cb_func回调函数定义见nim_msglog_def.h
	* @param[in] prg_user_data	APP的自定义用户数据，SDK只负责传回给回调函数prg_cb，不做任何处理！
	* @return void 无返回值
	*/
	static void ImportDbAsync(const std::string& src_path, const ImportDbCallback& cb, const ImportDbPrgCallback& prg_cb, const std::string& json_extension = "");


};

} 
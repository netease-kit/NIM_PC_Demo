/** @file nim_cpp_msglog.h
* @brief 消息历史
* @copyright (c) 2015-2017, NetEase Inc. All rights reserved
* @date 2015/2/1
*/

#ifndef _NIM_SDK_CPP_MSGLOG_H_
#define _NIM_SDK_CPP_MSGLOG_H_

#include <string>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_msglog_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class MsgLog
* @brief NIM SDK提供的消息历史接口
*/
class NIM_SDK_CPPWRAPPER_DLL_API MsgLog
{

public:
typedef std::function<void(NIMResCode res_code, const std::string& id, nim::NIMSessionType to_type, const QueryMsglogResult& result)> QueryMsgCallback; /**< 查询消息历史回调模板 */
typedef std::function<void(NIMResCode res_code, const std::string& msg_id, const IMMessage& msg)> QuerySingleMsgCallback; /**< 查询单条消息历史回调模板 */
typedef std::function<void(NIMResCode res_code, const std::string& uid, nim::NIMSessionType to_type)> ModifyMultipleMsglogCallback;	/**< 修改（多条）消息历史回调模板 */
typedef ModifyMultipleMsglogCallback BatchStatusReadCallback;	/**< 批量设置消息历史已读状态回调模板 */
typedef ModifyMultipleMsglogCallback BatchStatusDeleteCallback;	/**< 批量删除消息历史回调模板 */
typedef ModifyMultipleMsglogCallback DeleteBySessionTypeCallback;/**< 批量删除消息历史回调模板 */
typedef std::function<void(NIMResCode res_code, const std::string& msg_id)> ModifySingleMsglogCallback; /**< 修改（单条）消息历史回调模板 */
typedef ModifySingleMsglogCallback SetStatusCallback;	/**< 修改消息历史状态回调模板 */
typedef ModifySingleMsglogCallback SetSubStatusCallback;/**< 修改消息历史子状态回调模板 */
typedef ModifySingleMsglogCallback UpdateLocalExtCallback;/**< 修改消息历史本地扩展字段内容回调模板 */
typedef ModifySingleMsglogCallback WriteMsglogCallback;	/**< 保存消息历史回调模板 */
typedef ModifySingleMsglogCallback DeleteCallback;		/**< 删除消息历史回调模板 */
typedef std::function < void(const DeleteMsglogSelfNotifyParam&)>DeleteMsglogSelfNotifyCallback; /**<单向删除消息记录通知回调*/

typedef std::function<void(NIMResCode res_code)> DBFunctionCallback;		/**< 消息历史变更回调模板 */	
typedef std::function<void(NIMResCode res_code)> DeleteMsglogSelfCallback; /**<单向删除消息记录回调*/

typedef DBFunctionCallback DeleteAllCallback;		/**< 删除全部消息历史回调模板 */

typedef DBFunctionCallback DeleteMsgByTimeCallback;	/**< 根据时间段删除部分消息历史回调模板 */

typedef std::function<void(int64_t imported_count, int64_t total_count)> ImportDbPrgCallback;	/**< 导入消息历史回调模板 */

typedef std::function<void(const MessageStatusChangedResult&)>	MessageStatusChangedCallback;	/**< 发送消息已读回执通知回调模板 */	

typedef std::function<void(const NIMResCode res_code,const std::string& accid)>	DeleteHistoryOnLineAsyncCallback;	/**< 删除与某账号的所有云端历史记录与漫游消息的回调模板 */

typedef std::function<void(const NIMResCode res_code, const std::string & client_id,bool is_root,int reply_count)> QueryMessageIsThreadRootAsyncCallback;
typedef std::function<void(const NIMResCode res_code, const std::string & client_id, const IMMessage & msg)> QueryMessageOnlineCallback;
typedef std::function<void(const NIMResCode res_code, const IMMessage & root_msg,int total,uint64_t last_msg_time,const std::list<IMMessage>& msg_list)> QueryThreadHistoryMsgCallback;

class NIM_SDK_CPPWRAPPER_DLL_API AllMessageTypeList
{
public:
	AllMessageTypeList();
	std::vector<nim::NIMMessageType> ExclusionType(const std::vector<nim::NIMMessageType>& exclusion_type_list) const;
private://judge
	std::vector<nim::NIMMessageType> list_;
};
/** @class MsgLog
* 在线查询消息参数（不包括系统消息）
*/	
class NIM_SDK_CPPWRAPPER_DLL_API QueryMsgOnlineAsyncParam
{
	friend class MsgLog;
public:
	QueryMsgOnlineAsyncParam();
	std::string id_;/**< 查询id，对方的account id或者群组tid */
	nim::NIMSessionType to_type_;/**< enum 会话类型，双人0，群组1 (nim_msglog_def.h) */
	int limit_count_;/**< int 本次查询的消息条数上限(最多100条) */
	int64_t from_time_;/**< int64_t 起始时间点，单位：毫秒 */
	int64_t end_time_;/**<  int64_t 结束时间点，单位：毫秒 */
	int64_t end_msg_id_;/**< int64_t 结束查询的最后一条消息的server_msg_id(不包含在查询结果中) */
	bool reverse_;/**< bool true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false） */
	bool need_save_to_local_;/**< bool 将在线查询结果保存到本地，false: 不保存 */
	bool auto_download_attachment_;/**< bool 查询结果回来后，是否需要sdk自动下载消息附件。true：需要，false：不需要 */		
	std::vector<nim::NIMMessageType> msg_type_list_;/**< vector 要获取或排除掉的消息类型 由 is_exclusion_type_ 参数决定*/
	bool is_exclusion_type_;/**< bool true : 获取除msg_type_list_中指定的所有类型消息 ,false :只获取 msg_type_list_ 中指定的类型的消息*/
private:
	bool FormatParam();
private:
	const static AllMessageTypeList AllMsgTypeList;
	std::string json_extension_;
};

class NIM_SDK_CPPWRAPPER_DLL_API QueryMsgByKeywordOnlineParam
{
	friend class MsgLog;
public:
	QueryMsgByKeywordOnlineParam();
	std::string id_;/**< 查询id，对方的account id或者群组tid */
	std::string keyword_; /**< 要查询的关键字 */
	nim::NIMSessionType to_type_;/**< enum 会话类型，双人0，群组1 (nim_msglog_def.h) */
	int limit_count_;/**< int 本次查询的消息条数上限(最多100条) */
	int64_t from_time_;/**< int64_t 起始时间点，单位：毫秒 */
	int64_t end_time_;/**<  int64_t 结束时间点，单位：毫秒 */
	bool reverse_;/**< bool true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false） */
private:
	bool FormatParam() { return true; };
private:
	std::string json_extension_;
};
class NIM_SDK_CPPWRAPPER_DLL_API QueryMsgAsyncParam
{
public:
	NIMSessionType to_type_;/**< enum 会话类型，双人0，群组1,超大群5 (nim_msglog_def.h) */
	std::string from_account;/**< string 消息的发送方 */
	std::string to_account;/**< string 消息的接收方 */
	int64_t server_id;/**< int64_t 消息的服务端id */
	std::string client_id;/**< string 消息的客户端id */
	int64_t time;/**<  int64_t 消息时间戳 */
};

class QueryThreadHistoryMsgAsyncParam
{
public:	
	int64_t from_time;/**< int64_t 起始时间 缺省0*/
	int64_t to_time;/**< int64_t 结束时间 缺省0*/
	int64_t exclude_msg_id; /**< int64_t 截至消息的服务端id，不包含在查询结果中 缺省0*/
	int32_t linit;/**<  int32_t 查询条数限制 缺省100*/
	int32_t reverse;/**<  int64_t 排序 缺省0 false*/
};

/** @fn void UnregMsglogCb()
* 反注册Msglog提供的所有回调
* @return void 无返回值
*/
static void UnregMsglogCb();
/** @fn  void RegDeleteMsglogSelfNotify(const DeleteMsglogSelfNotifyCallback &cb)
* 注册单向删除消息记录通知回调
* @param[in] cb				单向删除消息记录通知回调
* @return void 无返回值
*/
static void RegDeleteMsglogSelfNotify(const DeleteMsglogSelfNotifyCallback &cb);

/** @fn static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "")
	* 根据消息ID查询本地（单条）消息
	* @param[in] client_msg_id		客户端消息ID
	* @param[in] json_extension	json扩展参数（备用，目前不需要）
	* @param[in] cb				查询本地消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
static bool QueryMsgByIDAysnc(const std::string &client_msg_id, const QuerySingleMsgCallback &cb, const std::string &json_extension = "");

/** @fn static bool QueryMsgAsync(const std::string& account_id, nim::NIMSessionType to_type, int limit_count, int64_t anchor_msg_time, const QueryMsgCallback& cb, const std::string& json_extension = "")
* 查询本地消息
* @param[in] account_id	查询id，account_id/uid或者tid
* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
* @param[in] limit_count	一次查询数量，建议20
* @param[in] anchor_msg_time 作为此次查询的定位点的消息历史的消息时间戳（上次查询最后一条消息的时间戳，按指定的时间的顺序起查，默认为逆序，2.4.0之前命名为last_name）
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			查询本地消息的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*/
static bool QueryMsgAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, int limit_count
	, int64_t anchor_msg_time
	, const QueryMsgCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool QueryMsgOnlineAsync(const QueryMsgOnlineAsyncParam& param, const QueryMsgCallback& cb)
* 在线查询消息（不包括系统消息）
* @param[in] param			查询参数	
* @param[in] cb				在线查询消息的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				403:禁止访问(不在该群,只针对群组会话)
*				414:参数错误
*/
static bool QueryMsgOnlineAsync(const QueryMsgOnlineAsyncParam& param, const QueryMsgCallback& cb);
/** @fn static bool QueryMsgOnlineAsync(const std::string &id, nim::NIMSessionType to_type, int limit_count, int64_t from_time, int64_t end_time, int64_t end_msg_id, bool reverse, bool need_save_to_local, const QueryMsgCallback& cb, const std::string& json_extension = "")
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
* @note 错误码	200:成功
*				403:禁止访问(不在该群,只针对群组会话)
*				414:参数错误
*/
static bool QueryMsgOnlineAsync(const std::string &id
	, nim::NIMSessionType to_type
	, int limit_count
	, int64_t from_time
	, int64_t end_time
	, int64_t end_msg_id
	, bool reverse
	, bool need_save_to_local
	, const QueryMsgCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool QueryMsgByKeywordOnlineAsync(const MsgLog::QueryMsgByKeywordOnlineParam& param, const QueryMsgCallback& cb);
* 在线查询消息（不包括系统消息）
* @param[in] param			查询参数
* @param[in] cb				在线查询消息的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				403:禁止访问(不在该群,只针对群组会话)
*				414:参数错误
*/
static bool QueryMsgByKeywordOnlineAsync(const MsgLog::QueryMsgByKeywordOnlineParam& param,
	const QueryMsgCallback& cb);

/** @fn static bool QueryMsgOfSpecifiedTypeInASessionAsync(nim::NIMSessionType to_type, const std::string &id, int limit_count, int64_t from_time, int64_t end_time, const std::string &end_client_msg_id, bool reverse, std::list<NIMMessageType> msg_type, const QueryMsgCallback& cb, const std::string& json_extension = "")
* 根据指定条件在一个会话中查询指定单个或多个类型的本地消息
* @param[in] to_type			会话类型，双人0，群组1 (nim_msglog_def.h)
* @param[in] id				查询id，对方的account id或者群组tid。
* @param[in] limit_count	本次查询的消息条数上限(最多100条)
* @param[in] from_time		起始时间点，单位：毫秒
* @param[in] end_time		结束时间点，单位：毫秒
* @param[in] end_client_msg_id		结束查询的最后一条消息的end_client_msg_id(不包含在查询结果中)
* @param[in] reverse		true：反向查询(按时间正序起查，正序排列)，false：按时间逆序起查，逆序排列（建议默认为false）
* @param[in] msg_type		检索的消息类型
* @param[in] json_extension	json扩展参数（备用，目前不需要）
* @param[in] cb				在线查询消息的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*/
static bool QueryMsgOfSpecifiedTypeInASessionAsync(nim::NIMSessionType to_type
	, const std::string &id
	, int limit_count
	, int64_t from_time
	, int64_t end_time
	, const std::string &end_client_msg_id
	, bool reverse
	, std::list<NIMMessageType> msg_type
	, const QueryMsgCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range, const std::list<std::string> &ids, nim::NIMSessionType to_type, int limit_count, int64_t from_time, int64_t end_time, const std::string &end_client_msg_id, bool reverse, NIMMessageType msg_type, const std::string &search_content, const QueryMsgCallback& cb, const std::string& json_extension = "")
* 根据指定条件查询本地消息,使用此接口可以完成全局搜索等功能,具体请参阅开发手册 http://dev.netease.im/docs/product/IM%E5%8D%B3%E6%97%B6%E9%80%9A%E8%AE%AF/SDK%E5%BC%80%E5%8F%91%E9%9B%86%E6%88%90/Windows%E5%BC%80%E5%8F%91%E9%9B%86%E6%88%90/%E5%8E%86%E5%8F%B2%E8%AE%B0%E5%BD%95
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
* @note 错误码	200:成功
*/
static bool QueryMsgByOptionsAsync(NIMMsgLogQueryRange query_range
	, const std::list<std::string> &ids
	, int limit_count
	, int64_t from_time
	, int64_t end_time
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
* @note 错误码	200:成功
*				0:失败
*/
static bool BatchStatusReadAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, const BatchStatusReadCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool BatchStatusDeleteAsync(const std::string& account_id, nim::NIMSessionType to_type, const BatchStatusDeleteCallback& cb, const std::string& json_extension = "")
* 删除某个会话的全部聊天记录
* @param[in] account_id	要删除会话的id，account_id/uid或者tid
* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
* @param[in] cb			操作结果的回调函数
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool BatchStatusDeleteAsync(const std::string& account_id
	, nim::NIMSessionType to_type
	, const BatchStatusDeleteCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool BatchStatusDeleteAsyncEx(const std::string& account_id, nim::NIMSessionType to_type, bool revert_by_query_online,const BatchStatusDeleteCallback& cb, const std::string& json_extension = "")
* 删除某个会话的全部聊天记录
* @param[in] account_id	要删除会话的id，account_id/uid或者tid
* @param[in] to_type	    会话类型，双人0，群组1 (见nim_msglog_def.h)
* @param[in] revert_by_online_query	是否可以通过服务端查询消息记录(含入库选项)进行恢复,true:是,false:否
* @param[in] cb			操作结果的回调函数
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool BatchStatusDeleteAsyncEx(const std::string& account_id
	, nim::NIMSessionType to_type
	, bool revert_by_query_online
	, const BatchStatusDeleteCallback& cb
	, const std::string& json_extension = "");


/** @fn static bool SetStatusAsync(const std::string& msg_id, nim::NIMMsgLogStatus msglog_status, const SetStatusCallback& cb, const std::string& json_extension = "")
* 设置消息状态
* @param[in] msg_id		消息id
* @param[in] msglog_status 消息状态枚举值
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
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
* @note 错误码	200:成功
*				0:失败
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
* @note 错误码	200:成功
*				0:失败
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
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteBySessionTypeAsync(bool delete_sessions
	, NIMSessionType to_type
	, const DeleteBySessionTypeCallback& cb
	, const std::string& json_extension = "");

/** @fn static bool DeleteBySessionTypeAsyncEx(bool delete_sessions, NIMSessionType to_type, const DeleteBySessionTypeCallback& cb, const std::string& json_extension = "")
* 删除指定会话类型的所有消息
* @param[in] delete_sessions	    是否删除会话
* @param[in] to_type	    会话类型
* @param[in] revert_by_online_query	是否可以通过服务端查询消息记录(含入库选项)进行恢复,true:是,false:否
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteBySessionTypeAsyncEx(bool delete_sessions
	, NIMSessionType to_type
	, bool revert_by_query_online
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
* @note 错误码	200:成功
*				0:失败
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
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteAllAsync(bool delete_sessions, const DeleteAllCallback& cb, const std::string& json_extension = "");

/** @fn static bool DeleteAllAsyncEx(bool delete_sessions, bool revert_by_query_online, const DeleteAllCallback& cb, const std::string& json_extension = "")
* 删除全部消息历史
* @param[in] delete_sessions 是否删除所有会话列表项（即全部最近联系人）。
*							   ture则删除，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandRemoveAll事件（不会触发每个会话项的kNIMSessionCommandRemove事件）；
*							   false则不删除，并将所有会话项的最后一条消息的状态kNIMSessionMsgStatus设置为已删除状态，并通过nim_session_reg_change_cb注册的回调通知上层kNIMSessionCommandAllMsgDeleted事件（不会触发每个会话项的kNIMSessionCommandUpdate事件，避免频繁通知上层）。
* @param[in] revert_by_online_query	是否可以通过服务端查询消息记录(含入库选项)进行恢复,true:是,false:否
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteAllAsyncEx(bool delete_sessions, bool revert_by_query_online, const DeleteAllCallback& cb, const std::string& json_extension = "");

/** @fn static bool DeleteMsgByTimeAsync(const std::string& session_id, NIMSessionType to_type, uint64_t from_time, uint64_t to_time, const DeleteMsgByTimeCallback& cb, const std::string& json_extension = "");
* 根据时间段删除部分会话的历史消息
* @param[in] session_id	要删除消息的会话ID
* @param[in] to_type	要删除消息的会话类型
* @param[in] 单位ms timestamp1	与 timestamp2 组成一个时间段，SDK 内部会判断大小调整入参顺序
* @param[in] 单位ms timestamp2	与 timestamp1 组成一个时间段，SDK 内部会判断大小调整入参顺序
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteMsgByTimeAsync(const std::string& session_id, NIMSessionType to_type, uint64_t timestamp1, uint64_t timestamp2, const DeleteMsgByTimeCallback& cb, const std::string& json_extension = "");

/** @fn static bool DeleteMsgByTimeAsyncEx(const std::string& session_id, NIMSessionType to_type, bool revert_by_query_online, uint64_t from_time, uint64_t to_time, const DeleteMsgByTimeCallback& cb, const std::string& json_extension = "");
* 根据时间段删除部分会话的历史消息
* @param[in] session_id	要删除消息的会话ID
* @param[in] to_type	要删除消息的会话类型
* @param[in] revert_by_online_query	是否可以通过服务端查询消息记录(含入库选项)进行恢复,true:是,false:否
* @param[in] 单位ms timestamp1	与 timestamp2 组成一个时间段，SDK 内部会判断大小调整入参顺序
* @param[in] 单位ms timestamp2	与 timestamp1 组成一个时间段，SDK 内部会判断大小调整入参顺序
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool DeleteMsgByTimeAsyncEx(const std::string& session_id, NIMSessionType to_type, bool revert_by_query_online, uint64_t timestamp1, uint64_t timestamp2, const DeleteMsgByTimeCallback& cb, const std::string& json_extension = "");

/** @fn static bool ExportDbAsync(const std::string& dst_path, const ExportDbCallback& cb, const std::string& json_extension = "")
* 导出整个消息历史DB文件（不包括系统消息历史）
* @param[in] dst_path		导出时保存的目标全路径（UTF-8编码）。
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool ExportDbAsync(const std::string& dst_path, const DBFunctionCallback& cb, const std::string& json_extension = "");

/** @fn static bool ImportDbAsync(const std::string& src_path, const ImportDbCallback& cb, const ImportDbPrgCallback& prg_cb, const std::string& json_extension = "")
* 导入消息历史DB文件（不包括系统消息历史）。先验证是否自己的消息历史文件和DB加密密钥(见nim_client_def.h里的kNIMDataBaseEncryptKey），如果验证不通过，则不导入。
* @param[in] src_path			导入源文件的全路径（UTF-8编码）。
* @param[in] json_extension	json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @param[in] prg_cb			导入进度的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				10600:导入消息历史时验证身份和加密密钥不通过
*				10601:导入消息历史时写记录失败
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
* @note 错误码	200:成功
*				403:服务器关闭此功能，或者应用没权限
*				404:请求的目标（用户或对象）不存在
*				10414:本地错误码，参数错误
*/
static void SendReceiptAsync(const std::string& json_msg, const MessageStatusChangedCallback& cb);

/** @fn bool QuerySentMessageBeReaded(const IMMessage& msg)
* 查询自己发送的消息是否被对方已读
* @param[in] msg			消息。
* @return bool 是否被已读
*/
static bool QuerySentMessageBeReaded(const IMMessage& msg);

/** @fn bool QueryReceivedMsgReceiptSent(const IMMessage& msg)
* 查询收到的消息是否已经发送过已读回执
* @param[in] msg			消息。
* @return bool 是否已发送过
*/
static bool QueryReceivedMsgReceiptSent(const IMMessage& msg);

/** @fn void RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension = "")
* (全局回调)注册全局的消息状态变更通知（目前只支持已读状态的通知）
* @param[in] cb				回调函数
* @param[in] json_extension	json扩展参数（备用，目前不需要）
* @return void 无返回值
* @note 错误码	200:成功
*/
static void RegMessageStatusChangedCb(const MessageStatusChangedCallback& cb, const std::string &json_extension = "");

/** @fn static bool UpdateLocalExtAsync(const std::string& msg_id, const std::string& local_ext, const UpdateLocalExtCallback& cb, const std::string& json_extension = "")
* 更新本地消息扩展字段内容
* @param[in] msg_id		消息id
* @param[in] local_ext  本地扩展字段内容
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool UpdateLocalExtAsync(const std::string& msg_id
	, const std::string& local_ext
	, const UpdateLocalExtCallback& cb
	, const std::string& json_extension = "");
/** @fn void UnregMsgologCb()
* 反注册Msglog提供的所有回调
* @return void 无返回值
*/
static void UnregMsgologCb();
	
/** @fn static bool ReadAllAsync(const DBFunctionCallback& cb, const std::string& json_extension = "")
* 全部未读消息历史标记为已读
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			操作结果的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
static bool ReadAllAsync(const DBFunctionCallback& cb, const std::string& json_extension = "");

/** @fn static bool ExportBackupToRemote(const LogsBackupExportInfo& export_info)
* 导出本地消息记录到云端
* @param[in] export_info 导出需要的参数参考LogsBackupExportInfo定义
* @return bool false : 当前有导入/导出操作正在进行中
*/
static bool ExportBackupToRemote(const LogsBackupExportInfo& export_info);

/** @fn static bool ImportBackupFromRemote(	const LogsBackupImportInfo& import_info)
* 导入已备份在云端的消息记录
* @param[in] export_info 导入需要的参数参考LogsBackupImportInfo定义
* @return bool false : 当前有导入/导出操作正在进行中
*/
static bool ImportBackupFromRemote(	const LogsBackupImportInfo& import_info);
	
/** @fn static void CancelImportBackupFromRemote()
* 取消导入已备份在云端的消息记录	
* @return void
*/
static void CancelImportBackupFromRemote();
/** @fn static void CancelExportBackupToRemote()
* 取消导出本地消息记录到云端	
* @return void
*/
static void CancelExportBackupToRemote();

/** @fn static void DeleteHistoryOnlineAsync(const std::string& accid,bool delete_roaming,const DeleteHistoryOnLineAsyncCallback& cb)
* 删除与某账号的所有云端历史记录与漫游消息
* @param[in] accid 对方的accid
* @param[in] delete_roaming 是否同时删除所有的漫游消息 true : 是 false : 否
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb	 操作结果的回调函数
* @return void 
* @note 错误码	200:成功
*/
static void DeleteHistoryOnlineAsync(const std::string& accid,bool delete_roaming,const std::string& json_extension,const DeleteHistoryOnLineAsyncCallback& cb);

/** @fn void DeleteMessageSelfAsync(const MsgLog& msg, const std::string ext, const DeleteMsglogSelfCallback& cb)
  * 单向删除某条消息记录(同时删除本地与云端)
  * @param[in] msg 要删除的消息
  * @param[in] ext 用户自定义扩展字段
  * @param[in] cb	操作结果的回调函数
  * @return void 无返回值
  * @note 错误码	200:成功
  */
static void DeleteMessageSelfAsync(const IMMessage &msg, const std::string ext, const DeleteMsglogSelfCallback& cb);

/** @fn QueryMessageIsThreadRoot(const std::string msg_client_id, const QueryMessageIsThreadRootAsyncCallback& cb)
  * 查询某条消息是否为thread聊天的根消息
  * @param[in] msg_client_id 要查询的消息的客户端ID
  * @param[in] cb			操作结果的回调函数
  * @return void 无返回值
  * @note 错误码	200:成功
  */
static void QueryMessageIsThreadRoot(const std::string msg_client_id, const QueryMessageIsThreadRootAsyncCallback& cb);

/** @fn void QueryMessageOnline(const QueryMsgAsyncParam& param, const QueryMessageOnlineCallback& cb)
  * 查询某条消息的具体内容一般用在thread talk 场景中
  * @param[in] param 要查询的消息的相关参数，可以在msglog.threadinfo中得到
  * @param[in] cb			查询结果的回调函数
  * @return void 无返回值
  * @note 错误码	200:成功
  */
static void QueryMessageOnline(const QueryMsgAsyncParam& param, const QueryMessageOnlineCallback& cb);

/** @fn QueryThreadHistoryMsg(const QueryThreadHistoryMsgAsyncParam& param, const QueryThreadHistoryMsgCallback& cb)
  * 分页查询thread talk消息历史
  * @param[in] param 要查询的消息的相关参数，可以在msglog.threadinfo中得到
  * @param[in] cb			查询结果的回调函数
  * @return void 无返回值
  * @note 错误码	200:成功
  */
static void QueryThreadHistoryMsg(const IMMessage& msg,const QueryThreadHistoryMsgAsyncParam& param, const QueryThreadHistoryMsgCallback& cb);
};
} 

#endif //_NIM_SDK_CPP_MSGLOG_H_

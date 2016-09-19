/** @file nim_cpp_sysmsg.h
  * @brief 系统消息接口；主要包括查询系统消息、删除系统消息等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_SYSMSG_H_
#define _NIM_SDK_CPP_SYSMSG_H_

#include <functional>
#include "nim_sysmsg_helper.h"
#include "nim_msg_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_sysmsg_def.h"
#include "nim_res_code_def.h"
#include "nim_session_def.h"

/** @class SystemMsg
  * @brief 系统消息接口；主要包括查询系统消息、删除系统消息等功能
  */
class SystemMsg
{

public:
	typedef std::function<void(const SysMessage&)>		ReceiveSysmsgCallback;		/**< 收到自定义通知回执回调模板 */
	typedef std::function<void(const SendMessageArc&)>	SendCustomSysmsgCallback;	/**< 发送自定义通知回调模板 */
	typedef std::function<void(int count, int unread_count, const std::list<SysMessage>& result)>		QueryMsgCallback;	/**< 查询系统消息自定义通知回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, int unread_count)>	NotifySysmsgResCallback;	/**< 修改系统消息自定义通知回调模板 */
	typedef NotifySysmsgResCallback	QuerySysmsgUnreadCallback;	/**< 查询系统消息自定义通知未读数回调模板 */
	typedef NotifySysmsgResCallback	ReadAllCallback;			/**< 设置系统消息自定义通知已读状态回调模板 */
	typedef NotifySysmsgResCallback	DeleteAllCallback;			/**< 删除全部系统消息自定义通知回调模板 */
	typedef NotifySysmsgResCallback	BatchSetCallback;			/**< 批量调整系统消息自定义通知回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, __int64 msg_id, int unread_count)>	NotifySingleSysmsgCallback;	/**< 修改（单条）系统消息自定义通知回调模板 */
	typedef NotifySingleSysmsgCallback SetStatusCallback;		/**< 设置系统消息自定义通知状态回调模板 */
	typedef NotifySingleSysmsgCallback DeleteCallback;			/**< 删除系统消息自定义通知回调模板 */

	/** @fn static void RegSysmsgCb(const ReceiveSysmsgCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册接收系统通知回调接口
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void RegSysmsgCb(const ReceiveSysmsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册发送透传消息回调函数 （必须全局注册，统一接受回调后分发消息到具体的会话。注意：客户端发包之后，服务器不一定会返回！！！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb		发送透传消息的回调函数
	* @return void 无返回值
	*/
	static void RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void SendCustomNotificationMsg(const std::string& json_msg)
	* 发送自定义通知消息
	* @param[in] json_msg		消息体Json, 可以通过CreateCustomNotificationMsg方法自动创建
	* @return void 无返回值
	*/
	static void SendCustomNotificationMsg(const std::string& json_msg);

	/** @fn std::string CreateCustomNotificationMsg(const std::string& receiver_id, const NIMSessionType session_type, const std::string& content, bool support_offline, const std::string& apns_text = "")
	* 生成自定义通知消息
	* @param[in] receiver_id	接收者id
	* @param[in] type   通知类型
	* @param[in] client_msg_id  本地消息id
	* @param[in] content 通知内容
	* @param[in] support_offline 是否需要支持离线
	* @param[in] apns_text 推送内容，如果为空则不推送
	* @return std::string 生成自定义通知消息Json字符串
	*/
	static std::string CreateCustomNotificationMsg(const std::string& receiver_id
		, const NIMSysMsgType type
		, const std::string& client_msg_id
		, const std::string& content
		, bool support_offline
		, const std::string& apns_text = "");

	/** @fn static bool QueryMsgAsync(int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension = "")
	* 查询本地系统消息
	* @param[in] limit_count	一次查询数量，建议20
	* @param[in] last_time	上次查询最后一条消息的时间戳
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询本地系统消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool QueryMsgAsync(int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryUnreadCount(const QuerySysmsgUnreadCallback& cb, const std::string& json_extension = "")
	* 查询未读消息数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询未读消息数的回调函数
	* @return void 无返回值
	*/
	static void QueryUnreadCount(const QuerySysmsgUnreadCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetStatusAsync(__int64 msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension = "")
	* 设置消息状态
	* @param[in] msg_id		消息id
	* @param[in] status		消息状态
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			设置消息状态的回调函数
	* @return void 无返回值
	*/
	static bool SetStatusAsync(__int64 msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension = "");

	/** @fn static void ReadAllAsync(const ReadAllCallback& cb, const std::string& json_extension = "")
	* 设置全部消息为已读
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			设置全部消息为已读的回调函数
	* @return void 无返回值
	*/
	static void ReadAllAsync(const ReadAllCallback& cb, const std::string& json_extension = "");

	/** @fn static bool DeleteAsync(__int64 msg_id, const DeleteCallback& cb, const std::string& json_extension = "")
	* 删除消息
	* @param[in] msg_id		消息id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			删除消息的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DeleteAsync(__int64 msg_id, const DeleteCallback& cb, const std::string& json_extension = "");

	/** @fn static void DeleteAllAsync(const DeleteAllCallback& cb, const std::string& json_extension = "")
	* 全部删除
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			删除消息的回调函数
	* @return void 无返回值
	*/
	static void DeleteAllAsync(const DeleteAllCallback& cb, const std::string& json_extension = "");

	/** @fn static void SetStatusByTypeAsync(NIMSysMsgType type, NIMSysMsgStatus status, const BatchSetCallback& cb, const std::string& json_extension = "")
	* 按类型设置系统通知状态
	* @param[in] type 类型
	* @param[in] status 状态
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			回调函数
	* @return void 无返回值
	*/
	static void SetStatusByTypeAsync(NIMSysMsgType type, NIMSysMsgStatus status, const BatchSetCallback& cb, const std::string& json_extension = "");

	/** @fn static void DeleteStatusByTypeAsync(NIMSysMsgType type, const BatchSetCallback& cb, const std::string& json_extension = "")
	* 按类型删除系统通知
	* @param[in] type 类型
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			回调函数
	* @return void 无返回值
	*/
	static void DeleteStatusByTypeAsync(NIMSysMsgType type, const BatchSetCallback& cb, const std::string& json_extension = "");

	/** @fn void UnregSysmsgCb()
	* 反注册SysMsg提供的所有回调
	* @return void 无返回值
	*/
	static void UnregSysmsgCb();

};

} 

#endif //_NIM_SDK_CPP_SYSMSG_H_
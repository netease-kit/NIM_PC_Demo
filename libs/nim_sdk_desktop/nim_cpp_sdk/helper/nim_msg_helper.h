/** @file nim_msg_helper.h
  * @brief Message 通用辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_CPP_MESSAGE_HELPER_H_
#define _NIM_SDK_CPP_MESSAGE_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_sdk_defines.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @brief 发送消息回执 */
struct SendMessageArc
{
	std::string talk_id_;	/**< 会话ID */
	std::string msg_id_;	/**< 消息ID */
	NIMResCode rescode_;	/**< 错误码 */
	int64_t	msg_timetag_;	/**< 消息时间戳 */
};

/** @brief 消息撤回通知 */
struct RecallMsgNotify
{
	std::string from_id_;	/**< 消息发送方ID */
	std::string to_id_;		/**< 消息接收方ID */
	std::string msg_id_;	/**< 客户端消息ID */
	std::string notify_;	/**< 自定义通知文案 */
	std::string from_nick_; /**< 消息发送方昵称 */
	NIMSessionType session_type_;	/**< 会话类型 */
	int64_t		notify_timetag_;	/**< 通知时间戳 */
	NIMMessageFeature notify_feature_;	/**< 通知的种类 */
	bool msglog_exist_;		/**< 客户端消息本地是否存在 */
	int64_t		msglog_timetag_;	/**< 撤回的消息的消息时间戳 */

	RecallMsgNotify() : notify_timetag_(0), msglog_exist_(true), msglog_timetag_(0) {}
};

/** @fn bool ParseRecallMsgNotify(const std::string& notify_json, std::list<RecallMsgNotify>& notifys)
  * @brief 解析消息撤回通知
  * @param[in] notify_json 消息撤回通知(Json Value数据字符串)
  * @param[out] notifys 消息撤回通知
  * @return bool 解析成功 或失败
  */
bool ParseRecallMsgNotify(const std::string& notify_json, std::list<RecallMsgNotify>& notifys);

/** @fn bool ParseSendMessageAck(const std::string& arc_json, SendMessageArc& arc)
  * @brief 解析发送消息回执
  * @param[in] arc_json 发送消息回执(Json Value数据字符串)
  * @param[out] arc 发送消息回执
  * @return bool 解析成功 或失败
  */
bool ParseSendMessageAck(const std::string& arc_json, SendMessageArc& arc);

} //namespace nim

#endif //_NIM_SDK_CPP_MESSAGE_HELPER_H_
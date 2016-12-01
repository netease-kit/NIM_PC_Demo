/** @file nim_sysmsg_helper.h
  * @brief sysmsg 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/10/20
  */

#ifndef _NIM_SDK_CPP_SYSMSG_HELPER_H_
#define _NIM_SDK_CPP_SYSMSG_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"
#include "nim_json_util.h"
#include "nim_sdk_util.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_sysmsg_def.h"
#include "nim_msglog_def.h"
#include "nim_res_code_def.h"

/** @brief 系统消息和自定义通知属性设置 */
struct SysMessageSetting
{
	BoolStatus need_push_; 				/**< 是否需要推送 */
	BoolStatus push_need_badge_;		/**< 是否要做消息计数 */
	BoolStatus push_need_prefix_;		/**< 需要推送昵称 */
	BoolStatus need_offline_;			/**< 是否支持离线消息 */
	Json::Value push_payload_;			/**< 第三方自定义的推送属性，长度2048 */
	std::string push_content_;			/**< 自定义推送文案，长度限制200字节 */
	BoolStatus anti_spam_enable_;		/**< (功能暂时不开放)是否需要过易盾反垃圾 */
	std::string anti_spam_content_;		/**< (功能暂时不开放)(可选)开发者自定义的反垃圾字段 */

	/** 构造函数 */
	SysMessageSetting() : need_push_(BS_NOT_INIT)
		, push_need_badge_(BS_NOT_INIT)
		, push_need_prefix_(BS_NOT_INIT)
		, need_offline_(BS_NOT_INIT)
		, anti_spam_enable_(BS_NOT_INIT){}

	/** @fn void ToJsonValue(Json::Value& message) const
	  * @brief 组装Json Value字符串
	  * @param[out] message 消息Json
	  * @return void
      */
	void ToJsonValue(Json::Value& message) const
	{
		if (push_need_badge_ != BS_NOT_INIT)
			message[kNIMSysMsgKeyPushNeedBadge] = push_need_badge_;
		if (need_push_ != BS_NOT_INIT)
			message[kNIMSysMsgKeyPushEnable] = need_push_;
		if (push_need_prefix_ != BS_NOT_INIT)
			message[kNIMSysMsgKeyPushNeedPrefix] = push_need_prefix_;
		if (need_offline_ != BS_NOT_INIT)
			message[kNIMSysMsgKeyCustomSaveFlag] = need_offline_;
		if (!push_payload_.empty())
			message[kNIMSysMsgKeyPushPayload] = GetJsonStringWithNoStyled(push_payload_);
		if (!push_content_.empty())
			message[kNIMSysMsgKeyCustomApnsText] = push_content_;
		if (anti_spam_enable_ != BS_NOT_INIT)
			message[kNIMSysMsgKeyAntiSpamEnable] = anti_spam_enable_ == BS_TRUE ? 1 : 0;
		if (!anti_spam_content_.empty())
			message[kNIMSysMsgKeyAntiSpamContent] = anti_spam_content_;
	}

	/** @fn void ParseMessageSetting(const Json::Value& message)
	  * @brief 从Json Value解析出消息属性设置
	  * @param[in] message 消息Json
	  * @return void
      */
	void ParseMessageSetting(const Json::Value& message)
	{
		if (message.isMember(kNIMSysMsgKeyPushNeedBadge))
			push_need_badge_ = (BoolStatus)message[kNIMSysMsgKeyPushNeedBadge].asInt() == 1 ? BS_TRUE : BS_FALSE;
		if (message.isMember(kNIMSysMsgKeyPushEnable))
			need_push_ = (BoolStatus)message[kNIMSysMsgKeyPushEnable].asInt() == 1 ? BS_TRUE : BS_FALSE;
		if (message.isMember(kNIMSysMsgKeyPushNeedPrefix))
			push_need_prefix_ = (BoolStatus)message[kNIMSysMsgKeyPushNeedPrefix].asInt() == 1 ? BS_TRUE : BS_FALSE;
		if (message.isMember(kNIMSysMsgKeyCustomSaveFlag))
			need_offline_ = (BoolStatus)message[kNIMSysMsgKeyCustomSaveFlag].asInt() == 1 ? BS_TRUE : BS_FALSE;
		Json::Reader reader;
		if (!reader.parse(message[kNIMSysMsgKeyPushPayload].asString(), push_payload_) || !push_payload_.isObject())
			//assert(0);
		push_content_ = message[kNIMSysMsgKeyCustomApnsText].asString();
		if (message.isMember(kNIMSysMsgKeyAntiSpamEnable))
			anti_spam_enable_ = message[kNIMSysMsgKeyAntiSpamEnable].asInt() == 1 ? BS_TRUE : BS_FALSE;
		anti_spam_content_ = message[kNIMSysMsgKeyAntiSpamContent].asString();
	}
};

/** @brief 系统消息和自定义通知数据 */
struct SysMessage
{
	int64_t		timetag_;			/**< 通知时间戳（毫秒） */
	NIMSysMsgType	type_;			/**< 通知类型 */
	std::string	receiver_accid_;	/**< 接收者ID */
	std::string sender_accid_;		/**< 发送者ID */
	std::string content_;			/**< 通知内容 */
	std::string	attach_;			/**< 通知附件 */
	int64_t		id_;				/**< 通知ID */
	NIMSysMsgStatus	status_;		/**< 通知状态 */
	SysMessageSetting msg_setting_;	/**< 消息属性设置 */

	NIMResCode	rescode_;			/**< 通知错误码 */
	NIMMessageFeature	feature_;	/**< 通知属性 */
	int			total_unread_count_;/**< 总计的通知未读数 */
	std::string client_msg_id_;		/**< 通知ID（客户端） */

	/** 构造函数 */
	SysMessage() : timetag_(0)
		, id_(0)
		, total_unread_count_(0)
		, type_(kNIMSysMsgTypeUnknown)
		, status_(kNIMSysMsgStatusNone)
		, feature_(kNIMMessageFeatureDefault){}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string	ToJsonString() const
	{
		Json::Value values;
		values[kNIMSysMsgKeyToAccount] = receiver_accid_;
		values[kNIMSysMsgKeyFromAccount] = sender_accid_;
		values[kNIMSysMsgKeyType] = type_;
		values[kNIMSysMsgKeyAttach] = attach_;
		values[kNIMSysMsgKeyMsg] = content_;
		values[kNIMSysMsgKeyLocalClientMsgId] = client_msg_id_;
		values[kNIMSysMsgKeyTime] = timetag_;
		values[kNIMSysMsgKeyMsgId] = id_;
		values[kNIMSysMsgKeyLocalStatus] = status_;
		msg_setting_.ToJsonValue(values);
		return GetJsonStringWithNoStyled(values);
	}
};

/** @fn bool ParseSysMessage(const std::string& sysmsg_json, SysMessage& msg)
  * @brief 解析系统消息或自定义通知
  * @param[in] sysmsg_json 系统消息或自定义通知(Json Value数据字符串)
  * @param[out] msg 系统消息或自定义通知
  * @return bool 解析成功或失败 
  */
bool ParseSysMessage(const std::string& sysmsg_json, SysMessage& msg);

/** @fn bool ParseSysMessages(const std::string& sysmsgs_json, std::list<SysMessage>& msgs, int* unread)
  * @brief 解析系统消息或自定义通知
  * @param[in] sysmsgs_json 系统消息或自定义通知(Json Value数据字符串)
  * @param[out] msgs 系统消息或自定义通知
  * @param[out] unread 当前总计的未读数
  * @return bool 解析成功或失败 
  */
bool ParseSysMessages(const std::string& sysmsgs_json, std::list<SysMessage>& msgs, int* unread);

/** @fn void ParseSysMessageContent(const Json::Value& content_json, SysMessage& msg)
  * @brief 解析系统消息或自定义通知的内容
  * @param[in] content_json 系统消息或自定义通知的内容(Json Value数据)
  * @param[out] msg 系统消息或自定义通知
  * @return void
  */
void ParseSysMessageContent(const Json::Value& content_json, SysMessage& msg);
} //namespace nim

#endif //_NIM_SDK_CPP_SYSMSG_HELPER_H_
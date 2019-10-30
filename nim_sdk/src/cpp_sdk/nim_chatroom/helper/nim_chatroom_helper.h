/** @file nim_chatroom_helper.h
  * @brief 聊天室SDK辅助方法
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/12/29
  */

#ifndef _NIM_CHATROOM_SDK_CPP_HELPER_H_
#define _NIM_CHATROOM_SDK_CPP_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include <algorithm>
#if !defined(_WIN32) && !defined(WIN32)
#include <pthread.h>
#endif
#include "public_define/nim_chatroom_define_include.h"
#include "include/depend_lib/include/nim_json_util.h"
#include "src/cpp_sdk/nim_chatroom/nim_chatroom_sdk_cpp_wrapper_dll.h"

/**
* @namespace nim_chatroom
* @brief 聊天室
*/
namespace nim_chatroom
{
	struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomPlatformConfig
	{
		
		bool ToJsonObject(nim_cpp_wrapper_util::Json::Value& values) const;
		std::string	ToJsonString() const
		{
			nim_cpp_wrapper_util::Json::Value values;
			if(ToJsonObject(values))
				return nim::GetJsonStringWithNoStyled(values);
			return std::string("");			
		}
		void AddNTServerAddress(const std::string& add)
		{
			if(std::find_if(ntserver_address_list_.begin(),ntserver_address_list_.end(),[&](const std::string& item){
				return item.compare(add) == 0;
			}) == ntserver_address_list_.end())
				ntserver_address_list_.emplace_back(add);
		}
		void AddNTServerAddress(std::string&& add)
		{
			if(std::find_if(ntserver_address_list_.begin(),ntserver_address_list_.end(),[&](const std::string& item){
				return item.compare(add) == 0;
			}) == ntserver_address_list_.end())
				ntserver_address_list_.emplace_back(add);
		}
		void EnableUploadStatisticsData(bool enable)
		{
			upload_statistics_data_ = enable;
		}
	private:
		std::vector<std::string> ntserver_address_list_;/*部分 IM 错误信息统计上报地址*/
		bool upload_statistics_data_;/*错误信息统计是否上报,私有化如不上报此项应配置为false*/
	};
/** @brief 聊天室登录信息*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomEnterInfo
{
	/** @fn void SetNick(const std::string &nick)
	  * @brief 设置进入聊天室后展示的昵称,选填
	  * @param[in] nick 聊天室昵称
	  * @return void
      */
	void SetNick(const std::string &nick)
	{
		values_[kNIMChatRoomEnterKeyNick] = nick;
	}

	/** @fn void SetAvatar(const std::string &avatar)
	  * @brief 设置进入聊天室后展示的头像,选填
	  * @param[in] avatar 聊天室头像下载地址
	  * @return void
      */
	void SetAvatar(const std::string &avatar)
	{
		values_[kNIMChatRoomEnterKeyAvatar] = avatar;
	}

	/** @fn void SetExt(const std::string &ext)
	  * @brief 设置聊天室可用的扩展字段,选填
	  * @param[in] ext 聊天室可用的扩展字段
	  * @return void
      */
	void SetExt(const nim_cpp_wrapper_util::Json::Value &ext)
	{
		values_[kNIMChatRoomEnterKeyExt] = nim::GetJsonStringWithNoStyled(ext);
	}

	/** @fn void SetNotifyExt(const std::string &notify_ext)
	  * @brief 设置进入聊天室通知开发者扩展字段,选填
	  * @param[in] notify_ext 进入聊天室通知开发者扩展字段
	  * @return void
      */
	void SetNotifyExt(const nim_cpp_wrapper_util::Json::Value &notify_ext)
	{
		values_[kNIMChatRoomEnterKeyNotifyExt] = nim::GetJsonStringWithNoStyled(notify_ext);
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return void
      */
	std::string	ToJsonString() const
	{
		return values_.empty() ? "" : nim::GetJsonStringWithNoStyled(values_);
	}

private:
	nim_cpp_wrapper_util::Json::Value		values_;
};

struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomAnoymityEnterInfo
{
	std::list<std::string> address_;	/**< 聊天室地址，地址通过应用服务器接口获取 */
	std::string app_data_file_;			/**< 应用数据目录，匿名登录时必填,使用默认路径时只需传入单个目录名（不以反斜杠结尾)，使用自定义路径时需传入完整路径（以反斜杠结尾，并确保有正确的读写权限！） */
	int sdk_log_level_;					/**< 匿名登录时选填,定义见NIMSDKLogLevel（选填，SDK默认的内置级别为kNIMSDKLogLevelPro） */
	std::string app_key_;				/**< 应用appkey，匿名登录时必填 */
	bool random_id_;					/**< 是否开启随机ID模式，默认为关闭(false)，建议默认值 */

	ChatRoomAnoymityEnterInfo()
	{
		sdk_log_level_ = 5;
		random_id_ = false;
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return void
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::FastWriter fw;
		std::string addr_str;
		nim::StrListToJsonString(address_, addr_str);
		values[kNIMChatRoomEnterKeyAddress] = addr_str;
		values[kNIMChatRoomEnterKeyAppDataPath] = app_data_file_;
		values[kNIMChatRoomEnterKeyLogLevel] = sdk_log_level_;
		values[kNIMChatRoomEnterKeyAppKey] = app_key_;
		values[kNIMChatRoomEnterKeyRandomID] = random_id_ ? 1 : 0;
		return fw.write(values);
	}
};

/** @brief 聊天室信息*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomInfo
{
	int64_t			id_;				/**< 聊天室ID */
	std::string		name_;				/**< 聊天室名称 */
	std::string		announcement_;		/**< 聊天室公告 */
	std::string		broadcast_url_;		/**< 视频直播拉流地址 */
	std::string		creator_id_;		/**< 聊天室创建者账号 */
	int				valid_flag_;		/**< 聊天室有效标记, 1:有效,0:无效 */
	std::string		ext_;				/**< 第三方扩展字段, 必须为可以解析为json的非格式化的字符串, 长度4k */
	int				online_count_;		/**< 在线人数 */
	int				mute_all_;			/**< 聊天室禁言标志 1:禁言,0:非禁言*/
	int				queuelevel;		 /**<int, 队列管理权限：0:所有人都有权限变更队列，1:只有主播管理员才能操作变更*/
	/** 构造函数 */
	ChatRoomInfo() : id_(0), valid_flag_(0), mute_all_(0) ,queuelevel(1){}

	/** @fn void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	  * @brief 从JsonValue中解析得到聊天室信息
	  * @param[in] values 聊天室信息JsonValue 
	  * @return void 
      */
	void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	{
		id_ = values[kNIMChatRoomInfoKeyID].asUInt64();
		name_ = values[kNIMChatRoomInfoKeyName].asString();
		announcement_ = values[kNIMChatRoomInfoKeyAnnouncement].asString();
		broadcast_url_ = values[kNIMChatRoomInfoKeyBroadcastUrl].asString();
		creator_id_ = values[kNIMChatRoomInfoKeyCreatorID].asString();
		valid_flag_ = values[kNIMChatRoomInfoKeyValidFlag].asUInt();
		ext_ = values[kNIMChatRoomInfoKeyExt].asString();
		online_count_ = values[kNIMChatRoomInfoKeyOnlineCount].asUInt();
		mute_all_ = values[kNIMChatRoomInfoKeyMuteAll].asUInt();
		queuelevel = values[kNIMChatRoomInfoKeyQueuelevel].asInt();
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return void
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomInfoKeyID] = id_;
		values[kNIMChatRoomInfoKeyName] = name_;
		values[kNIMChatRoomInfoKeyAnnouncement] = announcement_;
		values[kNIMChatRoomInfoKeyBroadcastUrl] = broadcast_url_;
		values[kNIMChatRoomInfoKeyCreatorID] = creator_id_;
		values[kNIMChatRoomInfoKeyValidFlag] = valid_flag_;
		values[kNIMChatRoomInfoKeyExt] = ext_;
		values[kNIMChatRoomInfoKeyOnlineCount] = online_count_;
		values[kNIMChatRoomInfoKeyMuteAll] = mute_all_;
		 values[kNIMChatRoomInfoKeyQueuelevel] = queuelevel;
		return nim::GetJsonStringWithNoStyled(values);
	}
};

/** @brief 聊天室通知*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomNotification
{
	NIMChatRoomNotificationId		id_;			/**< 通知类型 */
	std::string						ext_;			/**< 上层开发自定义的事件通知扩展字段, 必须为可以解析为json的非格式化的字符串 */
	std::string						operator_id_;	/**< 操作者的账号accid */
	std::string						operator_nick_;	/**< 操作者的账号nick */
	std::list<std::string>			target_nick_;	/**< 被操作者的账号nick列表 */
	std::list<std::string>			target_ids_;	/**< 被操作者的accid列表 */
	int64_t							temp_mute_duration_; /**<当通知为临时禁言相关时有该值，禁言时代表本次禁言的时长(秒)，解禁时代表本次禁言剩余时长(秒); 当通知为聊天室进入事件，代表临时禁言时长(秒); 其他通知事件不带该数据*/
	bool							muted_;			/**< 当通知为聊天室进入事件才有，代表是否禁言状态 */
	bool							temp_muted_;	/**< 当通知为聊天室进入事件才有，代表是否临时禁言状态 */
	std::string						queue_change_;	/**< 当通知为聊天室队列变更事件才有，代表变更的内容 */

	/** 构造函数 */
	ChatRoomNotification() : temp_mute_duration_(0), muted_(false), temp_muted_(false) { }

	/** @fn void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	  * @brief 从JsonValue中解析得到聊天室通知
	  * @param[in] values 聊天室通知JsonValue 
	  * @return void 
      */
	void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	{
		id_ = (NIMChatRoomNotificationId)values[kChatRoomNotificationKeyID].asUInt();
		ext_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyExt].asString();
		operator_id_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyOpt].asString();
		operator_nick_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyOptNick].asString();
		nim::JsonStrArrayToList(values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyTargetNick], target_nick_);
		nim::JsonStrArrayToList(values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyTarget], target_ids_);
		if (id_ == kNIMChatRoomNotificationIdMemberTempMute || id_ == kNIMChatRoomNotificationIdMemberTempUnMute)
			temp_mute_duration_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyTempMuteDuration].asInt64();
		if (id_ == kNIMChatRoomNotificationIdMemberIn)
		{
			if (values[kChatRoomNotificationKeyData].isMember(kChatRoomNotificationDataKeyMuteFlag))
				muted_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyMuteFlag].asUInt() == 1;
			if (values[kChatRoomNotificationKeyData].isMember(kChatRoomNotificationDataKeyTempMutedFlag))
				temp_muted_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyTempMutedFlag].asUInt() == 1;
			temp_mute_duration_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyMemberInTempMutedDuration].asInt64();
		}
		if (id_ == kNIMChatRoomNotificationIdQueueChanged )
		{
			queue_change_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyQueueChange].asString();
		}
		if ( id_ == kNIMChatRoomNotificationIdQueueBatchChanged)
		{
			queue_change_ = values[kChatRoomNotificationKeyData][kChatRoomNotificationDataKeyQueueChange].asString();
		}
	}
};

/** @brief 聊天室队列通知内容*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomQueueChangedNotification
{
	std::string		type_;		/**< 队列变更类型 OFFER, POLL, DROP*/
	std::string		key_;		/**< 队列变更元素的Key*/
	std::string		value_;		/**< 队列变更元素的Value*/

	/** @fn void ParseFromNotification(const ChatRoomNotification& notification)
	  * @brief 从聊天室通知中解析得到队列变更具体内容
	  * @param[in] notification 聊天室通知
	  * @return void 
      */
	void ParseFromNotification(const ChatRoomNotification& notification)
	{
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		if (reader.parse(notification.queue_change_, values) && values.isObject())
		{
			type_ = values[kNIMChatRoomNotificationQueueChangedKeyType].asString();
			key_ = values[kNIMChatRoomNotificationQueueChangedKeyKey].asString();
			value_ = values[kNIMChatRoomNotificationQueueChangedKeyValue].asString();
		}
	}
};
/** @brief  通知麦序队列中有批量变更，发生在元素提交者离开聊天室或者从聊天室异常掉线时*/

struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomQueueBatchChangedNotification
{
	std::string		type_;		/**< 队列变更类型 OFFER, POLL, DROP*/
	std::string		key_;		/**< 队列变更元素的Key*/
	std::string		value_;		/**< 队列变更元素的Value*/
	std::map<std::string, std::string> changed_values_;	/**< 队列变更元素的Value*/

	/** @fn void ParseFromNotification(const ChatRoomNotification& notification)
	* @brief 从聊天室通知中解析得到队列变更具体内容
	* @param[in] notification 聊天室通知
	* @return void
	*/
	void ParseFromNotification(const ChatRoomNotification& notification)
	{
		nim_cpp_wrapper_util::Json::Value values;
		nim_cpp_wrapper_util::Json::Reader reader;
		if (reader.parse(notification.queue_change_, values) && values.isObject())
		{
			type_ = values[kNIMChatRoomNotificationQueueBatchChangedKeyType].asString();
			if (values.isMember(kNIMChatRoomNotificationQueueBatchChangedKeyKey))
				key_ = values[kNIMChatRoomNotificationQueueBatchChangedKeyKey].asString();
			if (values.isMember(kNIMChatRoomNotificationQueueBatchChangedKeyValue))
				value_ = values[kNIMChatRoomNotificationQueueBatchChangedKeyValue].asString();
			if (values.isMember(kNIMChatRoomNotificationQueueBatchChangedKeyObject))
			{
				auto members_name = values[kNIMChatRoomNotificationQueueBatchChangedKeyObject].getMemberNames();
				auto it = members_name.begin();
				while(it != members_name.end())
				{
					changed_values_.insert(std::make_pair(*it, values[kNIMChatRoomNotificationQueueBatchChangedKeyObject][*it].asString()));
					it++;
				}
			}			
		}
	}
};

/** @brief 聊天室消息属性设置 */
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomMessageSetting
{
	bool			resend_flag_;				/**< 消息重发标记位 */
	std::string		ext_;						/**< 第三方扩展字段, 必须为可以解析为json的非格式化的字符串，长度限制4096 */
	bool			anti_spam_enable_;			/**< 是否需要过易盾反垃圾 */
	std::string		anti_spam_content_;			/**< (可选)开发者自定义的反垃圾字段,长度限制2048 */
	bool			history_save_;				/**< (可选)是否存云端消息历史，默认存 */
	std::string anti_spam_bizid_;		/**< (可选)用户配置的对某些单条消息另外的反垃圾的业务ID*/
	int anti_spam_using_yidun_;	/**< int,  (可选) 单条消息是否使用易盾反垃圾 0:(在开通易盾的情况下)不过易盾反垃圾而是通用反垃圾 其他都是按照原来的规则*/
	int				high_priority_;				/**< 高优先级消息标记,1:是; 非高优先级消息不带该字段,服务器填写,发送方不需要填写*/

	/** 构造函数 */
	ChatRoomMessageSetting() : resend_flag_(false)
		, anti_spam_enable_(false)
		, history_save_(true)
		,anti_spam_bizid_("")
		,anti_spam_using_yidun_(1)
		, high_priority_(0){}

	/** @fn void ToJsonValue(nim_cpp_wrapper_util::Json::Value& message) const
	  * @brief 组装Json Value字符串
	  * @param[out] message 消息Json
	  * @return void
      */
	void ToJsonValue(nim_cpp_wrapper_util::Json::Value& message) const
	{
		message[kNIMChatRoomMsgKeyResendFlag] = resend_flag_ ? 1 : 0;
		message[kNIMChatRoomMsgKeyExt] = ext_;
		message[kNIMChatRoomMsgKeyAntiSpamEnable] = anti_spam_enable_ ? 1 : 0;
		message[kNIMChatRoomMsgKeyAntiSpamContent] = anti_spam_content_;
		if(!anti_spam_bizid_.empty())
			message[kNIMChatRoomMsgKeyAntiSpamBizId] = anti_spam_bizid_;
		message[kNIMChatRoomMsgKeyHistorySave] = history_save_ ? 1 : 0;
		message[kNIMChatRoomMsgKeyAntiSpamUsingYiDun] = anti_spam_using_yidun_;
	}

	/** @fn void ParseMessageSetting(const nim_cpp_wrapper_util::Json::Value& message)
	  * @brief 从Json Value解析出消息属性设置
	  * @param[in] message 消息Json
	  * @return void
      */
	void ParseMessageSetting(const nim_cpp_wrapper_util::Json::Value& message)
	{
		resend_flag_ = message[kNIMChatRoomMsgKeyResendFlag].asUInt() == 1;
		ext_ = message[kNIMChatRoomMsgKeyExt].asString();
		if (message.isMember(kNIMChatRoomMsgKeyAntiSpamEnable))
			anti_spam_enable_ = message[kNIMChatRoomMsgKeyAntiSpamEnable].asInt() == 1;
		anti_spam_content_ = message[kNIMChatRoomMsgKeyAntiSpamContent].asString();
		high_priority_ = message[kNIMChatRoomMsgKeyHighPriorityFlag].asInt();
	}
};

/** @brief 聊天室消息*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomMessage
{
public:
	int64_t			room_id_;					/**< 消息所属的聊天室id,服务器填写,发送方不需要填写 */
	std::string		from_id_;					/**< 消息发送者的账号,服务器填写,发送方不需要填写 */
	int64_t			timetag_;					/**< 消息发送的时间戳(毫秒),服务器填写,发送方不需要填写 */
	NIMChatRoomClientType from_client_type_;	/**< 消息发送方客户端类型,服务器填写,发送方不需要填写 */
	std::string		from_nick_;					/**< 发送方昵称,服务器填写,发送方不需要填写 */
	std::string		from_avatar_;				/**< 发送方头像,服务器填写,发送方不需要填写 */
	std::string		from_ext_;					/**< 发送方身份扩展字段,服务器填写,发送方不需要填写 */

public:
	NIMChatRoomMsgType	msg_type_;				/**< 消息类型 */
	std::string		msg_attach_;				/**< 消息内容,长度限制2048,json结构, 文本消息和其他消息保持一致 */
	std::string		client_msg_id_;				/**< 客户端消息id */
	std::string		msg_body_;					/**< 文本消息内容（聊天室机器人文本消息） */
	ChatRoomMessageSetting msg_setting_;		/**< 消息属性设置 */

public:
	std::string	   local_res_path_;				/**< 媒体文件本地绝对路径（客户端） */
	std::string	   local_res_id_;				/**< 媒体文件ID（客户端） */

	/** 构造函数 */
	ChatRoomMessage() :room_id_(-1)
		, from_client_type_(kNIMChatRoomClientTypeDefault)
		, timetag_(0){}

	/** @fn void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	  * @brief 从JsonValue中解析得到聊天室消息
	  * @param[in] values 聊天室消息JsonValue 
	  * @return void 
      */
	void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &values)
	{
		room_id_ = values[kNIMChatRoomMsgKeyRoomID].asUInt64();
		from_id_ = values[kNIMChatRoomMsgKeyFromAccount].asString();
		timetag_ = values[kNIMChatRoomMsgKeyTime].asUInt64();
		from_client_type_ = (NIMChatRoomClientType)values[kNIMChatRoomMsgKeyFromClientType].asUInt();
		from_nick_ = values[kNIMChatRoomMsgKeyFromNick].asString();
		from_avatar_ = values[kNIMChatRoomMsgKeyFromAvatar].asString();
		from_ext_ = values[kNIMChatRoomMsgKeyFromExt].asString();
		msg_type_ = (NIMChatRoomMsgType)values[kNIMChatRoomMsgKeyType].asUInt();
		msg_attach_ = values[kNIMChatRoomMsgKeyAttach].asString();
		client_msg_id_ = values[kNIMChatRoomMsgKeyClientMsgid].asString();
		local_res_path_ = values[kNIMChatRoomMsgKeyLocalFilePath].asString();
		local_res_id_ = values[kNIMChatRoomMsgKeyLocalResId].asString();
		msg_body_ = values[kNIMChatRoomMsgKeyBody].asString();
		msg_setting_.ParseMessageSetting(values);
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string		ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomMsgKeyType] = msg_type_;
		values[kNIMChatRoomMsgKeyAttach] = msg_attach_;
		values[kNIMChatRoomMsgKeyClientMsgid] = client_msg_id_;
		values[kNIMChatRoomMsgKeyLocalFilePath] = local_res_path_;
		values[kNIMChatRoomMsgKeyLocalResId] = local_res_id_;
		values[kNIMChatRoomMsgKeyBody] = msg_body_;

		msg_setting_.ToJsonValue(values);
		return nim::GetJsonStringWithNoStyled(values);
	}
};

/** @brief 获取聊天室成员参数*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomGetMembersParameters
{
	NIMChatRoomGetMemberType type_;	/**< 成员类型 */
	int64_t timestamp_offset_;		/**< 成员时间戳偏移量*/
	int limit_;						/**< 数量*/

	/** 构造函数 */
	ChatRoomGetMembersParameters() : timestamp_offset_(0), limit_(0) {}
	
	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomGetMembersKeyType] = type_;
		values[kNIMChatRoomGetMembersKeyOffset] = timestamp_offset_;
		values[kNIMChatRoomGetMembersKeyLimit] = limit_;
		return nim::GetJsonStringWithNoStyled(values);
	}
};

/** @brief 获取聊天室消息历史参数*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomGetMsgHistoryParameters
{
	int64_t start_timetag_;			/**<开始时间,单位毫秒 */
	int limit_;						/**<本次返回的消息数量*/
	bool reverse_;					/**<是否反向查询*/
	std::vector<NIMChatRoomMsgType> msg_types_;	/**< 类型 */
	/** 构造函数 */
	ChatRoomGetMsgHistoryParameters() : start_timetag_(0), limit_(0), reverse_(false) 
	{
		if(kMsg_Types_List.empty())
		{
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeText);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeImage);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeAudio);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeVideo);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeLocation);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeNotification);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeFile);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeRobot);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeTips);
			kMsg_Types_List.emplace_back(kNIMChatRoomMsgTypeCustom);
		}
		msg_types_.assign(kMsg_Types_List.begin(),kMsg_Types_List.end());
	}	
	//考虑到兼容以前版本，只提供删除类型接口，如果所有类型都被删除，会添加Unknown类型以区别之前的版本
	void RemoveMessageType(NIMChatRoomMsgType type)
	{
		auto it = std::find_if(msg_types_.begin(),msg_types_.end(),
			[&](const NIMChatRoomMsgType& item){
				return item == type;
		});
		if( it != msg_types_.end())
			msg_types_.erase(it);
		if(msg_types_.empty())
			msg_types_.emplace_back(kNIMChatRoomMsgTypeUnknown);
	}	
	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomGetMsgHistoryKeyStartTime] = start_timetag_;
		values[kNIMChatRoomGetMsgHistoryKeyLimit] = limit_;
		values[kNIMChatRoomGetMsgHistoryKeyReverse] = reverse_;
		auto it = msg_types_.begin();
		while(it != msg_types_.end())
		{
			values[kNIMChatRoomGetMsgHistoryKeyMsgtypes].append(nim_cpp_wrapper_util::Json::Value(*it));
			it++;
		}		
		return nim::GetJsonStringWithNoStyled(values);
	}
private:	
	static std::vector<NIMChatRoomMsgType> kMsg_Types_List; /*= {kNIMChatRoomMsgTypeText,	
																  kNIMChatRoomMsgTypeImage,
																  kNIMChatRoomMsgTypeAudio,	
																  kNIMChatRoomMsgTypeVideo,
																  kNIMChatRoomMsgTypeLocation,
																  kNIMChatRoomMsgTypeNotification,
																  kNIMChatRoomMsgTypeFile,
																  kNIMChatRoomMsgTypeRobot,
																  kNIMChatRoomMsgTypeTips,
																  kNIMChatRoomMsgTypeCustom };*/
};

/** @brief 设置聊天室成员身份标识参数*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomSetMemberAttributeParameters
{
	std::string account_id_;				/**<成员ID */
	NIMChatRoomMemberAttribute attribute_;	/**<身份标识 */
	bool opt_;								/**<true:是,false:否*/
	std::string notify_ext_;				/**<通知的扩展字段, 必须为可以解析为Json的非格式化的字符串*/

	/** 构造函数 */
	ChatRoomSetMemberAttributeParameters() : level_(0) {}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return string Json Value字符串 
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomSetMemberAttributeKeyAccoutID] = account_id_;
		values[kNIMChatRoomSetMemberAttributeKeyAttribute] = attribute_;
		values[kNIMChatRoomSetMemberAttributeKeyOpt] = opt_;
		values[kNIMChatRoomSetMemberAttributeKeyNotifyExt] = notify_ext_;
		return nim::GetJsonStringWithNoStyled(values);
	}

private:
	int level_;								/**<暂时不支持*/

};

/** @brief 聊天室成员信息*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomMemberInfo
{
	int64_t			room_id_;			/**<聊天室id */
	std::string		account_id_;		/**<成员账号 */
	int				type_;				/**<成员类型, -2:未设置;-1:受限用户; 0:普通;1:创建者;2:管理员;3:临时用户,非固定成员;4:匿名非注册用户,非云信注册用户*/
	int				level_;				/**<成员级别: >=0表示用户开发者可以自定义的级别*/
	std::string		nick_;				/**<聊天室内的昵称字段,预留字段, 可从Uinfo中取*/
	std::string		avatar_;			/**<聊天室内的头像,预留字段, 可从Uinfo中取icon*/
	std::string		ext_;				/**<开发者扩展字段, 长度限制2k, 必须为可以解析为json的非格式化的字符串*/
	NIMChatRoomOnlineState	state_;		/**<成员是否处于在线状态, 仅特殊成员才可能离线, 对游客/匿名用户而言只能是在线*/
	NIMChatRoomGuestFlag	guest_flag_;/**<是否是普通游客类型,0:不是游客,1:是游客; 游客身份在聊天室中没有持久化, 只有在线时才会有内存状态*/
	int64_t			enter_timetag_;		/**<进入聊天室的时间点,对于离线成员该字段为空*/
	bool			is_blacklist_;		/**<是黑名单*/
	bool			is_muted_;			/**<是禁言用户*/
	bool			is_valid_;			/**<记录有效标记位*/
	int64_t			update_timetag_;	/**<固定成员的记录更新时间,用于固定成员列表的排列查询*/
	bool			temp_muted_;		/**<临时禁言*/
	int64_t			temp_muted_duration_;/**<临时禁言的解除时长,单位秒*/
	/** 构造函数 */
	ChatRoomMemberInfo() : room_id_(0),
		type_(-2),
		level_(0),
		state_(kNIMChatRoomOnlineStateOffline),
		guest_flag_(kNIMChatRoomGuestFlagGuest),
		enter_timetag_(0),
		is_blacklist_(false),
		is_muted_(false),
		is_valid_(false),
		update_timetag_(0),
		temp_muted_(false),
		temp_muted_duration_(0){}

	/** 构造函数 */
	void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &value)
	{
		if (value.isMember(kNIMChatRoomMemberInfoKeyRoomID))
			room_id_ = value[kNIMChatRoomMemberInfoKeyRoomID].asUInt64();
		if (value.isMember(kNIMChatRoomMemberInfoKeyAccID))
			account_id_ = value[kNIMChatRoomMemberInfoKeyAccID].asString();
		if (value.isMember(kNIMChatRoomMemberInfoKeyType))
			type_ = value[kNIMChatRoomMemberInfoKeyType].asInt();
		if (value.isMember(kNIMChatRoomMemberInfoKeyLevel))
			level_ = value[kNIMChatRoomMemberInfoKeyLevel].asInt();
		if (value.isMember(kNIMChatRoomMemberInfoKeyNick))
			nick_ = value[kNIMChatRoomMemberInfoKeyNick].asString();
		if (value.isMember(kNIMChatRoomMemberInfoKeyAvatar))
			avatar_ = value[kNIMChatRoomMemberInfoKeyAvatar].asString();
		if (value.isMember(kNIMChatRoomMemberInfoKeyExt))
			ext_ = value[kNIMChatRoomMemberInfoKeyExt].asString();
		if (value.isMember(kNIMChatRoomMemberInfoKeyOnlineState))
			state_ = (NIMChatRoomOnlineState)value[kNIMChatRoomMemberInfoKeyOnlineState].asInt();
		if (value.isMember(kNIMChatRoomMemberInfoKeyGuestFlag))
			guest_flag_ = (NIMChatRoomGuestFlag)value[kNIMChatRoomMemberInfoKeyGuestFlag].asInt();
		if (value.isMember(kNIMChatRoomMemberInfoKeyEnterTimetag))
			enter_timetag_ = value[kNIMChatRoomMemberInfoKeyEnterTimetag].asUInt64();
		if (value.isMember(kNIMChatRoomMemberInfoKeyBlacklist))
			is_blacklist_ = value[kNIMChatRoomMemberInfoKeyBlacklist].asInt() == 1;
		if (value.isMember(kNIMChatRoomMemberInfoKeyMuted))
			is_muted_ = value[kNIMChatRoomMemberInfoKeyMuted].asInt() == 1;
		if (value.isMember(kNIMChatRoomMemberInfoKeyValid))
			is_valid_ = value[kNIMChatRoomMemberInfoKeyValid].asInt() == 1;
		if (value.isMember(kNIMChatRoomMemberInfoKeyUpdateTimetag))
			update_timetag_ = value[kNIMChatRoomMemberInfoKeyUpdateTimetag].asUInt64();
		if (value.isMember(kNIMChatRoomMemberInfoKeyTempMute))
			temp_muted_ = value[kNIMChatRoomMemberInfoKeyTempMute].asInt() == 1;
		if (value.isMember(kNIMChatRoomMemberInfoKeyTempMuteRestDuration))
			temp_muted_duration_ = value[kNIMChatRoomMemberInfoKeyTempMuteRestDuration].asInt64();
	}

	/** @fn std::string ToJsonString() const
	  * @brief 组装Json Value字符串
	  * @return void
      */
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		values[kNIMChatRoomMemberInfoKeyRoomID] = room_id_;
		values[kNIMChatRoomMemberInfoKeyAccID] = account_id_;
		values[kNIMChatRoomMemberInfoKeyType] = type_;
		values[kNIMChatRoomMemberInfoKeyLevel] = level_;
		values[kNIMChatRoomMemberInfoKeyNick] = nick_;
		values[kNIMChatRoomMemberInfoKeyAvatar] = avatar_;
		values[kNIMChatRoomMemberInfoKeyExt] = ext_;
		values[kNIMChatRoomMemberInfoKeyOnlineState] = state_;
		values[kNIMChatRoomMemberInfoKeyGuestFlag] = guest_flag_;
		values[kNIMChatRoomMemberInfoKeyEnterTimetag] = enter_timetag_;
		values[kNIMChatRoomMemberInfoKeyBlacklist] = is_blacklist_ ? 1 : 0;
		values[kNIMChatRoomMemberInfoKeyMuted] = is_muted_ ? 1 : 0;
		values[kNIMChatRoomMemberInfoKeyValid] = is_valid_ ? 1 : 0;
		values[kNIMChatRoomMemberInfoKeyUpdateTimetag] = update_timetag_;
		values[kNIMChatRoomMemberInfoKeyTempMute] = temp_muted_ ? 1 : 0;
		values[kNIMChatRoomMemberInfoKeyTempMuteRestDuration] = temp_muted_duration_;
		return nim::GetJsonStringWithNoStyled(values);
	}

};

/** @brief 聊天室麦序队列元素*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomQueueElement
{
	std::string key_;		/**<元素的UniqKey,长度限制128字节 */
	std::string value_;		/**<元素的内容，长度限制4096字节 */
};

/** @brief 聊天室麦序队列*/
typedef std::list<ChatRoomQueueElement> ChatRoomQueue;

/** @brief 聊天室退出时的信息*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API NIMChatRoomExitReasonInfo
{
	std::string notify_ext_;/**<string 附加信息长度限制2048字节 */
	NIMChatRoomExitReason code_;/**< enum 退出原因的代码 */
	operator NIMChatRoomExitReason() const {return code_;}
};

/** 重载符号= */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool operator == (const NIMChatRoomExitReasonInfo& info,NIMChatRoomExitReason code);

/** @fn bool ParseChatRoomEnterCallbackResultInfo(const std::string& result, ChatRoomInfo& room_info, ChatRoomMemberInfo& my_info)
  * @brief 解析聊天室登录结果
  * @param[in] result 
  * @param[out] room_info 
  * @param[out] my_info 
  * @return bool 解析成功 或失败
  */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseChatRoomEnterCallbackResultInfo(const std::string& result, ChatRoomInfo& room_info, ChatRoomMemberInfo& my_info);

/** @fn bool ParseChatRoomMemberInfos(const std::string& infos_json_str, std::list<ChatRoomMemberInfo>& infos)
  * @brief 解析聊天室成员信息
  * @param[in] infos_json_str 
  * @param[out] infos 
  * @return bool 解析成功 或失败
  */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseChatRoomMemberInfos(const std::string& infos_json_str, std::list<ChatRoomMemberInfo>& infos);

/** @fn bool ParseChatRoomMsgs(const std::string& msgs_json_str, std::list<ChatRoomMessage>& msgs)
  * @brief 解析聊天室成员信息
  * @param[in] msgs_json_str 
  * @param[out] msgs 
  * @return bool 解析成功 或失败
  */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseChatRoomMsgs(const std::string& msgs_json_str, std::list<ChatRoomMessage>& msgs);

/** @brief 机器人信息 */
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API RobotInfo
{
public:
	/** 设置机器人云信id */
	void SetAccid(const std::string& accid)
	{
		robot_info_[kNIMRobotInfoKeyAccid] = accid;
	}

	/** 获取机器人云信id */
	std::string GetAccid() const
	{
		return robot_info_[kNIMRobotInfoKeyAccid].asString();
	}

	/** 设置机器人名字 */
	void SetName(const std::string& name)
	{
		robot_info_[kNIMRobotInfoKeyName] = name;
	}

	/** 获取机器人名字 */
	std::string GetName() const
	{
		return robot_info_[kNIMRobotInfoKeyName].asString();
	}

	/** 设置机器人头像 */
	void SetIcon(const std::string& icon)
	{
		robot_info_[kNIMRobotInfoKeyIcon] = icon;
	}

	/** 获取机器人头像 */
	std::string GetIcon() const
	{
		return robot_info_[kNIMRobotInfoKeyIcon].asString();
	}

	/** 设置机器人介绍 */
	void SetIntro(const std::string& intro)
	{
		robot_info_[kNIMRobotInfoKeyIntro] = intro;
	}

	/** 获取机器人介绍 */
	std::string GetIntro() const
	{
		return robot_info_[kNIMRobotInfoKeyIntro].asString();
	}

	/** 设置机器人波特平台ID */
	void SetRobotID(const std::string& id)
	{
		robot_info_[kNIMRobotInfoKeyRobotId] = id;
	}

	/** 获取机器人波特平台ID */
	std::string GetRobotID() const
	{
		return robot_info_[kNIMRobotInfoKeyRobotId].asString();
	}

	/** 设置机器人创建时间 */
	void SetCreateTime(const uint64_t time)
	{
		robot_info_[kNIMRobotInfoKeyCreateTime] = time;
	}

	/** 获取机器人创建时间 */
	uint64_t GetCreateTime() const
	{
		return robot_info_[kNIMRobotInfoKeyCreateTime].asUInt64();
	}

	/** 设置机器人更新时间 */
	void SetUpdateTime(const uint64_t time)
	{
		robot_info_[kNIMRobotInfoKeyUpdateTime] = time;
	}

	/** 获取机器人更新时间 */
	uint64_t GetUpdateTime() const
	{
		return robot_info_[kNIMRobotInfoKeyUpdateTime].asUInt64();
	}

private:
	nim_cpp_wrapper_util::Json::Value robot_info_;
};

/**< 机器人信息 */
typedef std::list<RobotInfo> RobotInfos;

/** @fn bool ParseRobotInfosStringToRobotInfos(const std::string& infos_json, RobotInfos &infos)
  * @brief 解析机器人信息
  * @param[in] infos_json 机器人信息（Json Value数据字符串）
  * @param[out] infos 机器人信息
  * @return bool 解析成功或失败 
  */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseRobotInfosStringToRobotInfos(const std::string &infos_json, RobotInfos &infos);

/** @fn bool ParseRobotInfoStringToRobotInfo(const std::string &info_json, RobotInfo &info)
  * @brief 解析机器人信息
  * @param[in] info_json 机器人信息
  * @param[out] info 机器人信息
  * @return bool 解析成功或失败 
  */
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseRobotInfoStringToRobotInfo(const std::string &info_json, RobotInfo &info);

typedef ChatRoomQueueElement ChatRoomMember;
/** @brief 聊天室批量更新成员*/
struct NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API ChatRoomBatchMembers
{

	std::map < std::string,std::string>members_values_;
	/** 构造函数 */
	ChatRoomBatchMembers(){ members_values_.clear(); }
	ChatRoomBatchMembers(const std::map < std::string, std::string>& members) :members_values_(members)
	{
		
	}
	
	void ParseFromJsonValue(const nim_cpp_wrapper_util::Json::Value &json_str)
	{
		assert(false);
	}

	/** @fn std::string ToJsonString() const
	* @brief 组装Json Value字符串
	* @return void
	*/
	std::string	ToJsonString() const
	{
		nim_cpp_wrapper_util::Json::Value values;
		int i = 0;
		for (auto it = members_values_.begin(); it != members_values_.end(); ++it)
		{
			values[i][kNIMChatRoomQueueElementKey] = it->first.c_str();
			values[i][kNIMChatRoomQueueElementValue] = it->second.c_str();
			i++;
		}
		return nim::GetJsonStringWithNoStyled(values);
	}


};
/** @fn bool ParseBatchInfosStringToNotMembers(const std::string &infos_json, std::list<std::string> &lst_members)
* @brief 解析成员列表
 * @param[in] info_json 成员列表信息 json格式字符串
 * @param[out] lst_members 成员列表
 * @return bool 解析成功或失败
*/
NIM_CHATROOM_SDK_CPPWRAPPER_DLL_API bool ParseBatchInfosStringToNotMembers(const std::string &infos_json, std::list<std::string> &lst_members);


} //namespace nim_chatroom

#endif //_NIM_CHATROOM_SDK_CPP_HELPER_H_

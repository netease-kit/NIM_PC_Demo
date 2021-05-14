/** @file nim_session_helper.h
  * @brief Session 辅助方法和数据结构定义
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/10/22
  */

#ifndef _NIM_SDK_CPP_SESSION_HELPER_H_
#define _NIM_SDK_CPP_SESSION_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "include/depend_lib/include/nim_json_util.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @brief 会话数据 */
	struct NIM_SDK_CPPWRAPPER_DLL_API StickTopSessionInfo
	{
		bool top_;/**< 是否置顶 */
		std::string		id_;					/**< 会话ID */
		NIMSessionType	type_;		/**< 会话类型 */
		std::string		ext_;				/**< 扩展信息*/
		int64_t			create_time_;	/**< 创建时间*/
		int64_t			update_time_;	/**< 更新时间*/
		/** 构造函数 */
		StickTopSessionInfo() : 
			top_(false),
			id_(""), 
			type_(NIMSessionType::kNIMSessionTypeP2P), 
			ext_(""), 
			create_time_(0), 
			update_time_(0) 
		{
		}

		static std::string ToJsonString(const StickTopSessionInfo& stickTopSessionInfo);
		static void ToJsonObject(const StickTopSessionInfo& stickTopSessionInfo, nim_cpp_wrapper_util::Json::Value& value);
	};

/** @brief 会话数据 */
struct NIM_SDK_CPPWRAPPER_DLL_API SessionData
{
	std::string		id_;					/**< 会话ID */
	NIMSessionType	type_;					/**< 会话类型 */
	int				unread_count_;			/**< 当前会话消息未读数 */
	NIMSessionCommand	command_;			/**< 会话修改命令 */
	std::string		msg_id_;				/**< 当前会话最新一条消息ID */
	std::string		msg_sender_accid_;		/**< 当前会话最新一条消息发送方ID */
	int64_t			msg_timetag_;			/**< 当前会话最新一条消息时间戳（毫秒） */
	NIMMessageType	msg_type_;				/**< 当前会话最新一条消息类型 */
	std::string		msg_content_;			/**< 当前会话最新一条消息内容 */
	std::string		msg_attach_;			/**< 当前会话最新一条消息附件 */
	NIMMsgLogStatus	msg_status_;			/**< 当前会话最新一条消息状态 */
	NIMMsgLogSubStatus	msg_sub_status_;	/**< 当前会话最新一条消息子状态 */
	bool			last_updated_msg_;		/**< (批量)消息变更或增加时是否是最后一条变更的信息 */
	bool			placed_on_top_;			/**< 置顶标识 */
	std::string		extend_data_;			/**< 本地扩展字段,限制4096 */
	bool			is_robot_session_;		/**< 是否为机器人会话, 默认为false */
	StickTopSessionInfo stick_top_info_;	/**< 置顶信息 v7.6添加,推荐使用此字段代替 "placed_on_top_" 字段*/
	/** 构造函数 */
	SessionData() : unread_count_(0)
				, msg_timetag_(0)
				, type_(kNIMSessionTypeP2P)
				, command_(kNIMSessionCommandAdd)
				, msg_type_(kNIMMessageTypeUnknown)
				, msg_status_(kNIMMsgLogStatusNone)
				, msg_sub_status_(kNIMMsgLogSubStatusNone)
				, last_updated_msg_(true)
				, placed_on_top_(false)
				, is_robot_session_(false){}

	static std::string ToJsonString(const SessionData& sessionData);
	static void ToJsonObject(const SessionData& sessionData, nim_cpp_wrapper_util::Json::Value& value);
};

/** @brief 会话列表数据 */
struct NIM_SDK_CPPWRAPPER_DLL_API SessionDataList
{
	int			count_;					/**< 会话列表项数量 */
	int			unread_count_;			/**< 会话列表总的未读消息数 */
	std::list<SessionData> sessions_;	/**< 会话列表项数据 */

	SessionDataList() : count_(0), unread_count_(0) {}
	static std::string ToJsonString(const SessionDataList& sessionData);
};
struct NIM_SDK_CPPWRAPPER_DLL_API StickTopSession
{
	StickTopSessionInfo stick_top_info_;	/**< 置顶信息 */
	SessionData session_data_; /**< 会话信息*/
	static std::string ToJsonString(const StickTopSession& stick_session);
	static void ToJsonObject(const StickTopSession& stick_session,nim_cpp_wrapper_util::Json::Value& value);
};

struct NIM_SDK_CPPWRAPPER_DLL_API MultiUnreadCountZeroInfo
{
	std::string		id_;					/**< 会话ID */
	NIMSessionType	type_;					/**< 会话类型 */
};
struct NIM_SDK_CPPWRAPPER_DLL_API StickTopSessionList
{
	std::list<StickTopSession> sessions_;	/**< 置顶会话列表项数据 */
	static std::string ToJsonString(const StickTopSessionList& stick_session_list);
};
/** @fn bool ParseSession(const std::string& session_json, SessionData& session)
  * @brief 解析会话信息
  * @param[in] session_json 会话信息(Json Value数据字符串)
  * @param[out] session 会话信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSession(const std::string& session_json, SessionData& session);

/** @fn void ParseSession(const nim_cpp_wrapper_util::Json::Value& session_json, SessionData& session)
  * @brief 解析会话信息
  * @param[in] session_json 会话信息(Json Value数据)
  * @param[out] session 会话信息
  * @return void 
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseSession(const nim_cpp_wrapper_util::Json::Value& session_json, SessionData& session);

/** @fn bool ParseSessionList(const std::string& sessions_json, SessionDataList& session_list)
  * @brief 解析会话列表信息
  * @param[in] sessions_json 会话列表信息(Json Value数据字符串)
  * @param[out] session_list 会话信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseSessionList(const std::string& sessions_json, SessionDataList& session_list);

/** @fn bool ParseStickTopSession(const std::string& session_json, StickTopSession& session);
  * @brief 解析置顶会话信息
  * @param[in] session_json 置顶会话信息(Json Value数据字符串)
  * @param[out] session 置顶会话信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseStickTopSession(const std::string& session_json, StickTopSession& session);

/** @fn void ParseStickTopSession(const nim_cpp_wrapper_util::Json::Value& session_json, StickTopSession& session)
  * @brief 解析置顶会话信息
  * @param[in] session_json 置顶会话信息(Json Value数据)
  * @param[out] session 置顶会话信息
  * @return void
  */
NIM_SDK_CPPWRAPPER_DLL_API void ParseStickTopSession(const nim_cpp_wrapper_util::Json::Value& session_json, StickTopSession& session);

/** @fn bool ParseStickTopSessionList(const std::string& sessions_json, StickTopSessionList& stick_top_session_list)
  * @brief 解析置顶会话列表信息
  * @param[in] sessions_json 置顶会话列表信息(Json Value数据字符串)
  * @param[out] stick_top_session_list 置顶会话信息
  * @return bool 解析成功 或失败
  */
NIM_SDK_CPPWRAPPER_DLL_API bool ParseStickTopSessionList(const std::string& sessions_json, StickTopSessionList& stick_top_session_list);

} //namespace nim

#endif //_NIM_SDK_CPP_SESSION_HELPER_H_
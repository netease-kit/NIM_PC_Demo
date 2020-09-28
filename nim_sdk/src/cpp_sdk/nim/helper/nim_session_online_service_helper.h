/** @file nim_session_online_service_helper.h
  * @brief SessionOnLineService 辅助方法和数据结构定义
  * @copyright (c) 2015-2019, NetEase Inc. All rights reserved
  * @date 2019/10/22
  */

#ifndef _NIM_CPP_SESSION_ONLINE_SERVICE_HELPER_H_
#define _NIM_CPP_SESSION_ONLINE_SERVICE_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "include/depend_lib/include/nim_json_util.h"
#include "public_define/nim_sdk_define_include.h"
#include "src/cpp_sdk/nim/helper/nim_msg_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
	class NIM_SDK_CPPWRAPPER_DLL_API SessionOnLineServiceHelper
	{
	public:
		/** @brief 会话服务 会话数据定义 */
		struct NIM_SDK_CPPWRAPPER_DLL_API SessionInfo
		{
			SessionInfo() : last_message_(""), last_message_type_(0) {}
			std::string		id_;					/**< 会话ID */
			NIMSessionType	type_;					/**< 会话类型 */
			std::string		ext_;			/**< 自定的扩展字段 */
			std::string		last_message_;			/**< 最后一条会话 json string */
			uint64_t	update_time_;/**< 最后更新时间戳 */
			int last_message_type_; /**< 最后一条消息的类型 0表示普通消息，1表示消息撤回通知*/
			//返回普通消息 auto last_msg = SessionInfo.GetLastMessage<0>();
			template <int type>
			auto GetLastMessage() const -> typename std::enable_if<type == 0, IMMessage>::type {
				IMMessage msg;
				ParseReceiveMessage(last_message_, msg);
				return msg;
			}
			//返回消息撤回通知auto last_msg = SessionInfo.GetLastMessage<1>();
			template <int type>
			auto GetLastMessage() const -> typename std::enable_if<type == 1, RecallMsgNotify>::type  {
				RecallMsgNotify recall_notify_msg;
				ParseRecallMsgNotify(last_message_, recall_notify_msg);
				return recall_notify_msg;
			}
		};
		typedef std::list<SessionInfo> SessionList; /**< 会话列表 */

		/** @brief 会话服务 查询会话列表应答数据定义 */
		struct NIM_SDK_CPPWRAPPER_DLL_API QuerySessionListResult
		{
			nim::NIMResCode		res_code;					/**< 返回的错误码 成功:200 */
			SessionList	session_list_; /**< 会话列表 */
			bool has_more_;/**<是否还有会话数据*/
		};
		struct NIM_SDK_CPPWRAPPER_DLL_API DeleteSessionParam
		{
			void AddSession(nim::NIMSessionType /*to_type*/, std::string /*session_id*/);
			std::list<std::pair<nim::NIMSessionType /*to_type*/, std::string /*session_id*/>> delete_list_;/**< list 要删除的会话 */
		};
		/** @fn bool ParseSessionOnLineInfo(const std::string& session_json, SessionOnLineInfo& session)
		  * @brief 解析会话信息
		  * @param[in] session_json 会话信息(Json Value数据字符串)
		  * @param[out] session 会话信息
		  * @return bool 解析成功 或失败
		  */
		static bool ParseSessionInfo(const std::string& session_json, SessionInfo& session);

		/** @fn bool ParseSessionList(const std::string& sessions_json, SessionDataList& session_list)
		  * @brief 解析查询会话列表结果
		  * @param[in] sessions_json 查询结果 (Json Value数据字符串)
		  * @param[out] result 会话信息
		  * @return bool 解析成功 或失败
		  */
		static bool ParseQuerySessionListResult(const std::string& sessions_json, QuerySessionListResult& result);
		
		/** @fn std::string DeleteSessionParamToJsonString(const DeleteSessionParam& param)
		  * @brief 解析查询会话列表结果
		  * @param[in] param DeleteSessionParam
		  * @return string json string
		  */
		static std::string DeleteSessionParamToJsonString(const DeleteSessionParam& param);
	};
} //namespace nim

#endif //_NIM_CPP_SESSION_ONLINE_SERVICE_HELPER_H_
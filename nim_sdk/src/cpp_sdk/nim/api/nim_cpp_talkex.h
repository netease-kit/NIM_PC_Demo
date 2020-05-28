/** @file nim_cpp_talkex.h
  * @brief 收藏、置顶会话、快捷回复、PinMessage、ThreadMessage
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/4/20
  */

#ifndef _NIM_SDK_CPP_TALKEX_H_
#define _NIM_SDK_CPP_TALKEX_H_

#include <string>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_collect.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_quick_comment.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_pin_message.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

	/** @class TalkEx
	  * @brief 聊天功能；主要包括收藏、快捷回复、PinMessage、ThreadMessage、置顶会话
	  */
	class NIM_SDK_CPPWRAPPER_DLL_API TalkEx
	{
	public:
		class NIM_SDK_CPPWRAPPER_DLL_API Collect
		{
		public:
			using AddCollectCallback = std::function<void(int code, const CollectInfo&)>;/**< 添加收藏回调模板 */
			using RemoveCollectsCallback = std::function<void(int code, int count)>;/**< 删除收藏回调模板 */
			using UpdateCollectCallback = std::function<void(int, const CollectInfo&)>;/**< 更新收藏回调模板 */
			using QueryCollectsCallback = std::function<void(int, int,const CollectInfoList&)>;/**< 分页查询回调模板 */
			/** @fn static void AddCollect(const CollectInfo& collect_info, const AddCollectCallback& cb)
			* 添加收藏
			* @param[in] collect_info	收藏内容
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void AddCollect(const CollectInfo& collect_info, const AddCollectCallback& cb);
			/** @fn static void RemoveCollects(const RemoveCollectsParm& collect_list, const RemoveCollectsCallback& cb)
			* 批量删除收藏
			* @param[in] collect_list	要删除的收藏列表
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void RemoveCollects(const RemoveCollectsParm& collect_list, const RemoveCollectsCallback& cb);
			/** @fn static void UpdateCollectExt(const MatchCollectParm& collect_match_param, const std::string& ext, const UpdateCollectCallback& cb)
			* 更新收藏扩展字段
			* @param[in] collect_match_param	根据收藏的id 与 create time去匹配收藏内容
			* @param[in] ext	收藏的扩展字段内容
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void UpdateCollectExt(const MatchCollectParm& collect_match_param, const std::string& ext, const UpdateCollectCallback& cb);
			/** @fn static void QueryCollectList(const QueryCollectsParm& query_collect_list_param, const QueryCollectsCallback& cb)
			* 分页查询收藏列表
			* @param[in] query_collect_list_param	 查询参数
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void QueryCollectList(const QueryCollectsParm& query_collect_list_param, const QueryCollectsCallback& cb);
		};
		class NIM_SDK_CPPWRAPPER_DLL_API QuickComment
		{
		public:
			using AddQuickCommentCallback = std::function < void(int, const QuickCommentInfo&)>;/**< 添加快捷回复回调模板 */
			using RemoveQuickCommentCallback = std::function < void(int, const std::string& id)>;/**< 删除快捷回复回调模板 */
			using QueryQuickCommentCallback = std::function<void(int, const QueryQuickCommentsResponse&)>;/**< 查询快捷回复回调模板 */
			using AddQuickCommentNotifyCallback = 
				std::function<void(	const std::string & session, NIMSessionType type,\
												const std::string & msg_client_id, \
												const QuickCommentInfo&)>;/**< 添加快捷回复通知回调*/
			using RemoveQuickCommentNotifyCallback = \
				std::function<void(	const std::string&session,NIMSessionType type,\
												const std::string& msg_client_id,\
												const std::string& quick_comment_id,\
												const std::string& ext)>;/**< 删除快捷回复通知回调*/

			/** @fn void UnregAllCb()
			* 反注册提供的所有回调
			* @return void 无返回值
			*/
			static void UnregAllCb();
			/** @fn static void RegAddQuickCommentNotify(const AddQuickCommentNotifyCallback& cb)
			* 注册添加快捷回复能知
			* @param[in] cb		收到通知时的回调函数
			* @return void 无返回值
			*/
			static void RegAddQuickCommentNotify(const AddQuickCommentNotifyCallback& cb);

			/** @fn static void RegRemoveQuickCommentNotify(const RemoveQuickCommentNotifyCallback& cb)
			* 注册删除快捷回复能知
			* @param[in] cb		收到通知时的回调函数
			* @return void 无返回值
			*/
			static void RegRemoveQuickCommentNotify(const RemoveQuickCommentNotifyCallback& cb);
			
			/** @fn static void AddQuickComment(const IMMessage &msg, const AddQuickCommentCallback& cb)
			* 添加快捷回复
			* @param[in] msg	被回复的消息
			* @param[in] info	回复的内容及设置
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void AddQuickComment(const IMMessage &msg, const QuickCommentInfo& info,const AddQuickCommentCallback& cb);
			/** @fn static void RemoveQuickComment(const IMMessage &msg, const AddQuickCommentCallback& cb)
			* 添加快捷回复
			* @param[in] msg	被回复的消息
			* @param[in] info	回复的内容及设置
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void RemoveQuickComment(const IMMessage& msg, const RemoveQuickCommentParam& param, const RemoveQuickCommentCallback& cb);
		
			/** @fn void QueryQuickCommentList(const QueryQuickCommentsParam& query_param, const QueryQuickCommentCallback& cb)
			* 添加快捷回复
			* @param[in] query_param	查询参数，一次最多只能查询20条消息的快捷回复
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void QueryQuickCommentList(const QueryQuickCommentsParam& query_param, const QueryQuickCommentCallback& cb);
		};
		class NIM_SDK_CPPWRAPPER_DLL_API PinMsg
		{
		public:
			using PinMessageCallback = std::function<void(int code, const std::string& session, int to_type, const PinMessageInfo&)>;
			using UnPinMessageCallback = std::function<void(int code, const std::string& session, int to_type, const std::string&)>;
			using UpdatePinMessageCallback = std::function<void(int code, const std::string & session, int to_type, const PinMessageInfo&)>;
			using QueryPinMessageCallback = std::function<void(int code, const std::string & session, int to_type, const QueryAllPinMessageResponse&)>;
			using AddPinMessageNotifyCallback = std::function<void( const std::string & session, int to_type, const PinMessageInfo&)>;
			using UnPinMessageNotifyCallback = std::function<void(const std::string& session, int to_type, const std::string& id)>;
			using UpdatePinMessageNotifyCallback = std::function<void(const std::string & session, int to_type, const PinMessageInfo&)>;
			/** @fn void AddPinMessage(const IMMessage& msg,const PinMessageInfo& pin_info, const PinMessageCallback& cb)
			* Pin某条消息
			* @param[in] msg	要Pin的消息
			* @param[in] pin_info	Pin的内容 只需赋值 ext参数,其它参数SDK来补充
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void AddPinMessage(const IMMessage& msg,const PinMessageInfo& pin_info, const PinMessageCallback& cb);
		
			/** @fn void UnPinMessage(const UnPinMessageParam& unpin_param, const UnPinMessageCallback& cb)
			* Pin某条消息
			* @param[in] modify_param	UnPin Message 参数
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void UnPinMessage(const ModifyPinMessageParam& modify_param, const UnPinMessageCallback& cb);

			/** @fn void UpdatePinMessage(const ModifyPinMessageParam& modify_param, const UpdatePinMessageCallback& cb)
			* 更新 Pin Message ext字段
			* @param[in] modify_param	更新 Pin Message 参数
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void UpdatePinMessage(const ModifyPinMessageParam& modify_param, const UpdatePinMessageCallback& cb);

			/** @fn void QueryAllPinMessage(const std::string& session,int to_type, const QueryPinMessageCallback& cb)
			* 查询会话的所有 Pin Message
			* @param[in] session	会话ID
			* @param[in] to_type	会话类型
			* @param[in] cb		执行结果回调函数
			* @return void 无返回值
			*/
			static void QueryAllPinMessage(const std::string& session,int to_type, const QueryPinMessageCallback& cb);
			
			/** @fn void UnregAllCb()
			* 反注册提供的所有回调
			* @return void 无返回值
			*/
			static void UnregAllCb();

			/** @fn void RegAddPinMessage(const AddPinMessageNotifyCallback& cb)
			* 注册添加 Pin Message 通知回调
			* @param[in] cb	 收到通知时的回调函数
			* @return void 无返回值
			*/
			static void RegAddPinMessage(const AddPinMessageNotifyCallback& cb);

			/** @fn void RegUnPinMessage(const UnPinMessageNotifyCallback& cb)
			* 注册 UnPin Message 通知回调
			* @param[in] cb	 收到通知时的回调函数
			* @return void 无返回值
			*/
			static void RegUnPinMessage(const UnPinMessageNotifyCallback& cb);

			/** @fn void RegUpdatePinMessage(const UpdatePinMessageNotifyCallback& cb)
			* 注册 更新Pin Message 通知回调
			* @param[in] cb	 收到通知时的回调函数
			* @return void 无返回值
			*/
			static void RegUpdatePinMessage(const UpdatePinMessageNotifyCallback& cb);
		};
	};
}

#endif //_NIM_SDK_CPP_TALKEX_H_
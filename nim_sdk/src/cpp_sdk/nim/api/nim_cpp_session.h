/** @file nim_cpp_session.h
  * @brief 最近会话列表
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_SESSION_H_
#define _NIM_SDK_CPP_SESSION_H_

#include <string>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_session_helper.h"
#include "src/cpp_sdk/nim/helper/nim_talk_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class Session
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  */

class NIM_SDK_CPPWRAPPER_DLL_API Session
{

public:
    typedef std::function<int32_t(const std::string&)> BadgeCountCallback;    /**< app角标未读计数通知回调模板 (仅iOS有效）*/
	typedef std::function<void(nim::NIMResCode, const SessionData&, int)> ChangeCallback;	/**< 会话变更通知回调模板 */
	typedef std::function<void(int, const SessionDataList&)> QuerySessionListCallabck;		/**< 查询会话列表回调模板 */
	typedef ChangeCallback DeleteRecentSessionCallabck;										/**< 删除会话回调模板 */
	typedef ChangeCallback DeleteAllRecentSessionCallabck;									/**< 删除全部会话回调模板 */
	typedef ChangeCallback SetUnreadCountZeroCallback;										/**< 会话未读消息数清零回调模板 */
	typedef std::function<void(nim::NIMResCode, const std::list<SessionData>& session_data_list, int unread_count)> SetMultiUnreadCountZeroCallback;/**< 多条会话未读消息数清零回调模板 */
	typedef std::function<void(nim::NIMResCode, const SessionData&)> QuerySessionDataCallback; /**< 会话信息查询结果回调模板 */
	typedef std::function<void(const StickTopSession&)> SetToStickTopSessionNotifyCallback;/**< 置顶会话通知回调模板 */
	typedef std::function<void(const std::string& session_id,nim::NIMSessionType)> CancelStickTopSessionNotifyCallback;/**< 取消置顶会话通知回调模板 */
	typedef std::function<void(const StickTopSession&)> UpdateStickTopSessionNotifyCallback;/**< 更新置顶会话通知回调模板 */
	typedef std::function<void(nim::NIMResCode, const StickTopSession&)> SetToStickTopSessionCallback;/**< 置顶会话回调模板 */
	typedef std::function<void(nim::NIMResCode, const std::string & session_id, nim::NIMSessionType)> CancelStickTopSessionCallback;/**< 取消置顶会话回调模板 */
	typedef std::function<void(nim::NIMResCode, const StickTopSession&)> UpdateStickTopSessionCallback;/**< 更新置顶会话回调模板 */
	typedef std::function<void(nim::NIMResCode, const StickTopSessionList&)> QueryStickTopSessionListCallback;/**< 查询置顶会话列表回调模板 */

	typedef std::function<void(nim::NIMResCode, const SessionRoamMsgHasMoreTagInfo& info)> QueryHasmoreRoammsgCallback;/**< 查询会话是漫游消息未拉取信息回调模板*/
	typedef std::function<void(nim::NIMResCode, const std::list<SessionRoamMsgHasMoreTagInfo>& info_list)> QueryAllHasmoreRoammsgCallback;/**< 查询所有漫游消息未拉取完全的会话回调模板*/
	typedef std::function<void(nim::NIMResCode)> UpdateHasmoreRoammsgCallback;/**< 更新会话是漫游消息未拉取信息回调模板*/
	typedef std::function<void(nim::NIMResCode)> DeleteHasmoreRoammsgCallback;/**< 删除会话是漫游消息未拉取信息回调模板*/
	typedef std::function<void(nim::NIMResCode,nim::NIMSessionType,const std::string& session_id)> DeleteSessionRoammsgCallback;/**< 删除会话是漫游消息未拉取信息回调模板*/
	/** @fn static void RegChangeCb(const ChangeCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册最近会话列表项变更通知
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegChangeCb(const ChangeCallback& cb, const std::string& json_extension = "");
    
    /** @fn static void RegBadgeCountCb(const BadgeCountCallback& cb, const std::string& json_extension = "")
    * (全局回调)注册app角标计数回调（仅iOS有效）
    * @param[in] json_extension json扩展参数（备用，目前不需要）
    * @param[in] cb    app角标未读计数回调函数
    * @return void 无返回值
    * @note 错误码    200:成功
    */
    static void RegBadgeCountCb(const BadgeCountCallback& cb, const std::string& json_extension = "");
	
	/** @fn static void QueryLastFewSessionAsync(int limit, const QuerySessionListCallabck& cb, const std::string& json_extension = "")
	* 查询指定数量的最后会话数据
	* @param[in] limit		要返回的最大数量
	* @param[in] cb			查询会话列表的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void QueryLastFewSessionAsync(int limit, const QuerySessionListCallabck& cb, const std::string& json_extension = "");

	/** @fn static void QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension = "")
	* 查询会话列表
	* @param[in] cb			查询会话列表的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension = "");

	/** @fn static void QueryAllRecentSessionAsync(NIMMessageType last_msg_excluded_type,const QuerySessionListCallabck& cb, const std::string& json_extension = "")
	* 查询会话列表,可指定最后一条会话消息要排除掉的类型
	* @param[in] last_msg_excluded_type 最后一条会话消息要排除掉的类型,如果不排除任何消息，参数请传入NIMMessageType::kNIMMessageTypeUnknown
	* @param[in] cb			查询会话列表的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）	
	* @return void 无返回值
	*/
	static void QueryAllRecentSessionAsync(NIMMessageType last_msg_excluded_type,const QuerySessionListCallabck& cb, const std::string& json_extension = "");

/** @fn static void QueryAllRecentSessionAsyncEx(const std::list<NIMMessageType>& last_msg_excluded_type, const QuerySessionListCallabck& cb, const std::string& json_extension = "");
* 查询会话列表,可指定最后一条会话消息要排除掉的类型(列表)
* @param[in] last_msg_excluded_type 最后一条会话消息要排除掉的类型(列表),如果不排除任何消息,传入空列表
* @param[in] cb			查询会话列表的回调函数
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @return void 无返回值
*/
	static void QueryAllRecentSessionAsyncEx(const std::list<NIMMessageType>& last_msg_excluded_type, const QuerySessionListCallabck& cb, const std::string& json_extension = "");

	/** @fn static bool DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension = "")
	* 删除最近联系人
	* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension = "");

/** @fn static bool DeleteRecentSessionEx(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, bool delete_roaming)
* 删除最近联系人
* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
* @param[in] id			对方的account id或者群组tid。
* @param[in] delete_roaming 是否同时删除漫游消息
* @param[in] cb			最近会话列表项变更的回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
	static bool DeleteRecentSessionEx(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, bool delete_roaming);

	/** @fn static bool DeleteSessionRoamingMessage(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension = "")
* 删除某会话的漫游消息
* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
* @param[in] id			对方的account id或者群组tid。
* @param[in] json_extension json扩展参数（备用，目前不需要）
* @param[in] cb			删除结果 回调回调函数
* @return bool 检查参数如果不符合要求则返回失败
* @note 错误码	200:成功
*				0:失败
*/
	static bool DeleteSessionRoamingMessage(nim::NIMSessionType to_type, const std::string& id, const DeleteSessionRoammsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb, const std::string& json_extension = "")
	* 删除全部最近联系人
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb, const std::string& json_extension = "");

	/** @fn static bool SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension = "")
	* 最近联系人项未读数清零
	* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetMultiUnreadCountZeroAsync(bool super_team,const std::list< MultiUnreadCountZeroInfo>& unread_zero_info_list, const SetMultiUnreadCountZeroCallback& cb)
	* 最近联系人项未读数清零
	* @param[in] super_team	会话列表中的会话是否是超大群会话
	* @param[in] unread_zero_info_list	清零的会话信息列表。
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool SetMultiUnreadCountZeroAsync(bool super_team,const std::list< MultiUnreadCountZeroInfo>& unread_zero_info_list, const SetMultiUnreadCountZeroCallback& cb);

	/** @fn static bool SetSessionTop(enum NIMSessionType to_type, const std::string& id, bool top, const ChangeCallback& cb, const std::string& json_extension = "");
	* 设置会话项是否置顶(置顶属性只保存在本地)
	* @param[in] to_type		会话类型
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] top			true - 置顶 false - 取消置顶
	* @param[in] cb			回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool SetSessionTop(enum NIMSessionType to_type, const std::string& id, bool top, const ChangeCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetSessionExtendData(enum NIMSessionType to_type, const std::string& id, const std::string& data, const ChangeCallback& cb, const std::string& json_extension = "")
	* 设置会话项扩展数据(扩展数据只保存在本地)
	* @param[in] to_type		会话类型
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] data			扩展数据,建议使用灵活可扩展的数据结构,例如Json
	* @param[in] cb			回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool SetSessionExtendData(enum NIMSessionType to_type, const std::string& id, const std::string& data, const ChangeCallback& cb, const std::string& json_extension = "");

	/** @fn static bool SetAllUnreadCountZeroAsync(const SetUnreadCountZeroCallback& cb, const std::string& json_extension = "")
	* 最近联系人项全部未读数清零
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				0:失败
	*/
	static bool SetAllUnreadCountZeroAsync(const SetUnreadCountZeroCallback& cb, const std::string& json_extension = "");

	/** @fn void QuerySessionDataById(NIMSessionType to_type, const std::string& id,const QuerySessionDataCallback& cb, const std::string& json_extension = "")
  	* 根据给定的id查询相应会话的信息
	* @param[in] to_type		会话类型
  	* @param[in] id			对方的account id或者群组tid。
  	* @param[in] cb			会话信息查询结果的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
  	* @return void 无返回值
  	* @note 错误码	200:成功
  	*				0:失败
  	*/
	static void QuerySessionDataById(NIMSessionType to_type, const std::string& id,const QuerySessionDataCallback& cb, const std::string& json_extension = "");
	
	/** @fn static void RegSetToStickTopSessionNotifyCB(const SetToStickTopSessionNotifyCallback& cb)
	* (全局回调)注册置顶会话通知回调
	* @param[in] cb			置顶会话通知回调模板
	* @return void 无返回值
	*/
	static void RegSetToStickTopSessionNotifyCB(const SetToStickTopSessionNotifyCallback& cb);

	/** @fn static void RegCancelStickTopSessionNotifyCB(const CancelStickTopSessionNotifyCallback& cb)
	* (全局回调)注册取消置顶会话通知回调
	* @param[in] cb			取消置顶会话通知回调模板
	* @return void 无返回值
	*/
	static void RegCancelStickTopSessionNotifyCB(const CancelStickTopSessionNotifyCallback& cb);

	/** @fn static void RegUpdateStickTopSessionNotifyCB(const UpdateStickTopSessionNotifyCallback& cb)
	* (全局回调)注册更新置顶会话通知回调
	* @param[in] cb			更新置顶会话通知回调模板
	* @return void 无返回值
	*/
	static void RegUpdateStickTopSessionNotifyCB(const UpdateStickTopSessionNotifyCallback& cb);
	
	/** @fn void QueryStickTopSessionList(const QueryStickTopSessionListCallback& cb)
	* 查询置顶会话列表
	* @param[in] cb			置顶会话列表查询结果的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void QueryStickTopSessionList(const QueryStickTopSessionListCallback& cb);

	/** @fn void SetToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext,const SetToStickTopSessionCallback& cb)
	* 设置置顶会话
	* @param[in] session_id			会话id
	* @param[in] to_type			会话类型
	* @param[in] ext			扩展字段
	* @param[in] cb			置顶会话设置结果的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void SetToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext,const SetToStickTopSessionCallback& cb);

	/** @fn void UpdateToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext, const UpdateStickTopSessionCallback& cb)
	* 更新置顶会话列表
	* @param[in] session_id			会话id
	* @param[in] to_type			会话类型
	* @param[in] ext			扩展字段
	* @param[in] cb			更新置顶会话设置结果的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void UpdateToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type, const std::string& ext, const UpdateStickTopSessionCallback& cb);

	/** @fn void CancelToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type,const CancelStickTopSessionCallback& cb)
	* 取消置顶会话列表
	* @param[in] session_id			会话id
	* @param[in] to_type			会话类型
	* @param[in] cb			取消置顶会话设置结果的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void CancelToStickTopSession(const std::string& session_id, nim::NIMSessionType to_type,const CancelStickTopSessionCallback& cb);

	/** @fn void QueryHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const QueryHasmoreRoammsgCallback& cb)
	* 查询会话是漫游消息未拉取信息
	* @param[in] session_id			会话id
	* @param[in] to_type			会话类型
	* @param[in] cb			查询会话是漫游消息未拉取信息回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				0:失败
	*/
	static void QueryHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const QueryHasmoreRoammsgCallback& cb);

    /** @fn void QueryAllHasmoreRoammsg(const QueryAllHasmoreRoammsgCallback& cb)
    * 查询所有漫游消息未拉取完全的会话
    * @param[in] cb	结果回调
    * @return void 无返回值
    */
	static void QueryAllHasmoreRoammsg(const QueryAllHasmoreRoammsgCallback& cb);

	/** @fn void UpdateHasmoreRoammsg(const IMMessage& msg,const UpdateHasmoreRoammsgCallback& cb)
	* 查询会话是漫游消息未拉取信息
	* @param[in] msg	对应的消息内容
	* @param[in] cb	结果回调
	* @return void 无返回值
	*/
	static void UpdateHasmoreRoammsg(const IMMessage& msg,const UpdateHasmoreRoammsgCallback& cb);

	/** @fn void DeleteHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const DeleteHasmoreRoammsgCallback& cb)
	* 删除会话是漫游消息未拉取信息
	* @param[in] session_id			会话id
	* @param[in] to_type			会话类型
	* @param[in] cb	结果回调
	* @return void 无返回值
	*/
	static void DeleteHasmoreRoammsg(const std::string& session_id, nim::NIMSessionType to_type, const DeleteHasmoreRoammsgCallback& cb);

	/** @fn void UnregSessionCb()
	* 反注册Session提供的所有回调
	* @return void 无返回值
	*/
	static void UnregSessionCb();

};

} 

#endif //_NIM_SDK_CPP_SESSION_H_

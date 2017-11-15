/** @file nim_cpp_session.h
  * @brief 最近会话列表
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_SESSION_H_
#define _NIM_SDK_CPP_SESSION_H_

#include <string>
#include <functional>
#include "nim_session_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class Session
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  */

class Session
{

public:
	typedef std::function<void(nim::NIMResCode, const SessionData&, int)> ChangeCallback;	/**< 会话变更通知回调模板 */
	typedef std::function<void(int, const SessionDataList&)> QuerySessionListCallabck;		/**< 查询会话列表回调模板 */
	typedef ChangeCallback DeleteRecentSessionCallabck;										/**< 删除会话回调模板 */
	typedef ChangeCallback DeleteAllRecentSessionCallabck;									/**< 删除全部会话回调模板 */
	typedef ChangeCallback SetUnreadCountZeroCallback;										/**< 会话未读消息数清零回调模板 */

	/** @fn static void RegChangeCb(const ChangeCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册最近会话列表项变更通知
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegChangeCb(const ChangeCallback& cb, const std::string& json_extension = "");

	/** @fn static void QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension = "")
	* 查询会话列表
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询会话列表的回调函数
	* @return void 无返回值
	*/
	static void QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension = "");

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

	/** @fn static bool SetSessionExtendData(enum NIMSessionType to_type, const std::string& id, const std::string& data, const ChangeCallback& cb, const std::string& json_extension = "");
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

	/** @fn void UnregSessionCb()
	* 反注册Session提供的所有回调
	* @return void 无返回值
	*/
	static void UnregSessionCb();

};

} 

#endif //_NIM_SDK_CPP_SESSION_H_
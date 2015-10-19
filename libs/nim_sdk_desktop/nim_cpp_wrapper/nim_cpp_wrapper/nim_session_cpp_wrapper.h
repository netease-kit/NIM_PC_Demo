#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_session_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_res_code_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_msglog_def.h"
#include "nim_global_define.h"

/** @class Session
  * @brief 会话列表管理功能；主要包括查询会话列表、删除会话列表等功能
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Session
{

public:
	typedef std::function<void(nim::NIMResCode, const std::string&, int)> ChangeCallback;
	typedef std::function<void(int, const std::string&)> QuerySessionListCallabck;
	typedef std::function<void(nim::NIMResCode, const std::string&, int)> DeleteRecentSessionCallabck;
	typedef std::function<void(nim::NIMResCode, const std::string&, int)> DeleteAllRecentSessionCallabck;
	typedef std::function<void(nim::NIMResCode, const std::string&, int)> SetUnreadCountZeroCallback;

	/** 
	* 注册最近会话列表项变更通知
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	*/
	static void RegChangeCb(const ChangeCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询会话列表
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询会话列表的回调函数
	* @return void 无返回值
	*/
	static void QueryAllRecentSessionAsync(const QuerySessionListCallabck& cb, const std::string& json_extension = "");

	/** 
	* 删除最近联系人
	* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	*/
	static void DeleteRecentSession(nim::NIMSessionType to_type, const std::string& id, const DeleteRecentSessionCallabck& cb, const std::string& json_extension = "");

	/** 
	* 删除全部最近联系人
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	*/
	static void DeleteAllRecentSession(const DeleteAllRecentSessionCallabck& cb, const std::string& json_extension = "");

	/** 
	* 最近联系人项未读数清零
	* @param[in] to_type		会话类型，双人0，群组1 (nim_msglog_def.h)
	* @param[in] id			对方的account id或者群组tid。
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			最近会话列表项变更的回调函数
	* @return void 无返回值
	*/
	static void SetUnreadCountZeroAsync(nim::NIMSessionType to_type, const std::string& id, const SetUnreadCountZeroCallback& cb, const std::string& json_extension = "");
};

} 
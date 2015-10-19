#pragma once


#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_sysmsg_def.h"
#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_res_code_def.h"

/** @class SystemMsg
  * @brief 系统消息接口；主要包括查询系统消息、删除系统消息等功能
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class SystemMsg
{

public:
	typedef std::function<void(const std::string&)>					SysmsgCallback;
	typedef std::function<void(int count, const std::string& result)>	QueryMsgCallback;
	typedef std::function<void(nim::NIMResCode res_code, int unread_count)>		QuerySysmsgUnreadCallback;
	typedef std::function<void(nim::NIMResCode res_code, __int64 msg_id, int unread_count)>		SetStatusCallback;
	typedef std::function<void(nim::NIMResCode res_code, int unread_count)>						ReadAllCallback;
	typedef std::function<void(nim::NIMResCode res_code, __int64 msg_id, int unread_count)>		DeleteCallback;
	typedef std::function<void(nim::NIMResCode res_code, int unread_count)>						DeleteAllCallback;

	/** 
	* 注册接收系统通知回调接口
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			操作结果的回调函数
	* @return void 无返回值
	*/
	static void RegSysmsgCb(const SysmsgCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询本地系统消息
	* @param[in] limit_count	一次查询数量，建议20
	* @param[in] last_time	上次查询最后一条消息的时间戳
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询本地系统消息的回调函数
	* @return void 无返回值
	*/
	static void QueryMsgAsync(int limit_count, __int64 last_time, const QueryMsgCallback& cb, const std::string& json_extension = "");

	/** 
	* 查询未读消息数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			查询未读消息数的回调函数
	* @return void 无返回值
	*/
	static void QueryUnreadCount(const QuerySysmsgUnreadCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置消息状态
	* @param[in] msg_id		消息id
	* @param[in] status		消息状态
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			设置消息状态的回调函数
	* @return void 无返回值
	*/
	static void SetStatusAsync(__int64 msg_id, nim::NIMSysMsgStatus status, const SetStatusCallback& cb, const std::string& json_extension = "");

	/** 
	* 设置全部消息为已读
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			设置全部消息为已读的回调函数
	* @return void 无返回值
	*/
	static void ReadAllAsync(const ReadAllCallback& cb, const std::string& json_extension = "");

	/** 
	* 删除消息
	* @param[in] msg_id		消息id
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			删除消息的回调函数
	* @return void 无返回值
	*/
	static void DeleteAsync(__int64 msg_id, const DeleteCallback& cb, const std::string& json_extension = "");

	/** 
	* 全部删除
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb			删除消息的回调函数
	* @return void 无返回值
	*/
	static void DeleteAllAsync(const DeleteAllCallback& cb, const std::string& json_extension = "");
};

} 
/** @file nim_cpp_subscribe_event.h
  * @brief 事件订阅
  * @copyright (c) 2017, NetEase Inc. All rights reserved
  * @date 2017/03/23
  */

#ifndef _NIM_SDK_CPP_SUBSCRIBE_EVENT_H_
#define _NIM_SDK_CPP_SUBSCRIBE_EVENT_H_

#include <string>
#include <list>
#include <functional>
#include "nim_subscribe_event_helper.h"
#include "nim_sdk_cpp_wrapper_dll.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{
/** @class SubscribeEvent
  * @brief NIM 订阅事件相关接口
  */
class NIM_SDK_CPPWRAPPER_DLL_API SubscribeEvent
{
public:
	typedef std::function<void(NIMResCode res_code, const nim::EventData& event_data)> PushEventCallback;	/**< 接收订阅事件回调模板 */
	typedef std::function<void(NIMResCode res_code, const std::list<nim::EventData>& event_list)> BatchPushEventCallback;		/**< 批量接收订阅事件回调模板 */

	typedef std::function<void(NIMResCode res_code, int event_type, const nim::EventData& event_data)> PublishEventCallback;	/**< 发布事件回调模板 */
	typedef std::function<void(NIMResCode res_code, int event_type, const std::list<std::string>& faild_list)> SubscribeEventCallback;	/**< 订阅事件回调模板 */
	typedef std::function<void(NIMResCode res_code, int event_type, const std::list<std::string>& faild_list)> UnSubscribeEventCallback;/**< 按账号取消指定事件的订阅关系回调模板 */
	typedef std::function<void(NIMResCode res_code, int event_type)> BatchUnSubscribeEventCallback;			/**< 取消指定事件的全部订阅关系回调模板 */
	typedef std::function<void(NIMResCode res_code, int event_type, const std::list<nim::EventSubscribeData>& subscribe_list)> QuerySubscribeEventCallback;			/**< 按帐号查询指定事件的订阅关系回调模板 */
	typedef std::function<void(NIMResCode res_code, int event_type, const std::list<nim::EventSubscribeData>& subscribe_list)> BatchQuerySubscribeEventCallback;	/**< 查询指定事件的全部订阅关系回调模板 */

public:
	/** @fn static void RegPushEventCb(const PushEventCallback &cb, const std::string& json_extension = "")
	* (全局回调)统一注册接收订阅的事件的回调函数
	* @param[in] cb	接收订阅事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegPushEventCb(const PushEventCallback &cb, const std::string& json_extension = "");

	/** @fn static void RegBatchPushEventCb(const BatchPushEventCallback &cb, const std::string& json_extension = "")
	*  (全局回调)统一注册批量接收订阅的事件的回调函数
	* @param[in] cb	批量接收订阅事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegBatchPushEventCb(const BatchPushEventCallback &cb, const std::string& json_extension = "");

	/** @fn static bool Publish(const EventData &event_data, const PublishEventCallback &cb, const std::string& json_extension = "")
	* 发布事件
	* @param[in] event_data	事件
	* @param[in] cb			发布事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				403:应用没开通事件服务
	*				404:禁用了特定事件
	*				414:参数错误
	*				500:未知错误
	*/
	static bool Publish(const EventData &event_data, const PublishEventCallback &cb, const std::string& json_extension = "");
	
	/** @fn static bool Subscribe(int event_type, int64_t ttl, NIMEventSubscribeSyncEventType sync_type, const SubscribeEventCallback &cb, const std::string& json_extension = "")
	* 订阅事件
	* @param[in] event_type		事件类型
	* @param[in] ttl			订阅有效期，单位：秒，范围：60s到30天
	* @param[in] sync_type		订阅后是否立即同步最新事件
	* @param[in] accid_list		要订阅的用户列表
	* @param[in] cb				订阅事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*				500:未知错误
	*/
	static bool Subscribe(int event_type, int64_t ttl, NIMEventSubscribeSyncEventType sync_type, const std::list<std::string> &accid_list, const SubscribeEventCallback &cb, const std::string& json_extension = "");

	/** @fn static bool UnSubscribe(int event_type, const std::list<str::string> &accid_list, const UnSubscribeEventCallback &cb, const std::string& json_extension = "")
	* 按账号取消指定事件的订阅关系
	* @param[in] event_type		事件类型
	* @param[in] accid_list		要取消订阅的用户列表
	* @param[in] cb				取消订阅事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*				500:未知错误
	*/
	static bool UnSubscribe(int event_type, const std::list<std::string> &accid_list, const UnSubscribeEventCallback &cb, const std::string& json_extension = "");

	/** @fn static bool BatchUnSubscribe(int event_type, const BatchUnSubscribeEventCallback &cb, const std::string& json_extension = "")
	* 取消指定事件的全部订阅关系
	* @param[in] event_type		事件类型
	* @param[in] cb				取消订阅事件回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*				500:未知错误
	*/
	static bool BatchUnSubscribe(int event_type, const BatchUnSubscribeEventCallback &cb, const std::string& json_extension = "");

	/** @fn static bool QuerySubscribe(int event_type, const std::list<str::string> &accid_list, const QuerySubscribeEventCallback &cb, const std::string& json_extension = "")
	* 按账号查询指定事件订阅关系
	* @param[in] event_type		事件类型
	* @param[in] accid_list		要查询订阅关系的用户列表
	* @param[in] cb				查询订阅关系回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	* @note 错误码	200:成功
	*				414:参数错误
	*				500:未知错误
	*/
	static bool QuerySubscribe(int event_type, const std::list<std::string> &accid_list, const QuerySubscribeEventCallback &cb, const std::string& json_extension = "");
};

}

#endif //_NIM_SDK_CPP_SUBSCRIBE_EVENT_H_
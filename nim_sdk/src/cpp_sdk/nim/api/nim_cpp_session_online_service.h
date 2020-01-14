/** @file nim_cpp_session_online_service.h
* @brief 会话服务
* @copyright (c) 2015-2019, NetEase Inc. All rights reserved
* @date 2019/10/22
*/

#ifndef _NIM_CPP_SESSION_ONLINE_SERVICE_H_
#define _NIM_CPP_SESSION_ONLINE_SERVICE_H_

#include <string>
#include <functional>
#include "src/cpp_sdk/nim/helper/nim_session_online_service_helper.h"
#include "src/cpp_sdk/nim/nim_sdk_cpp_wrapper_dll.h"
/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

class NIM_SDK_CPPWRAPPER_DLL_API SessionOnLineService
{
public:
typedef std::function<void(nim::NIMResCode, const SessionOnLineServiceHelper::SessionInfo&)> QuerySessionInfoCallback;	/**< 查询会话回调模板 */
typedef std::function<void(const SessionOnLineServiceHelper::QuerySessionListResult&)> QuerySessionListCallabck;		/**< 查询会话列表回调模板 */
typedef std::function<void(nim::NIMResCode)> UpdateSessionInfoCallback;	/**< 更新会话回调模板 */
typedef std::function<void(nim::NIMResCode)> DeleteSessionInfoCallback;	/**< 删除会话回调模板 */
typedef std::function<void(const SessionOnLineServiceHelper::SessionInfo&)> SessionChangedCallback;	/**< 会话变更回调模板 */

/** @fn static void QuerySessionList(uint64_t min_time, uint64_t max_time, bool need_last_msg, int limit, const QuerySessionListCallabck& cb)
* 会话服务 查询会话列表
* @param[in] min_time 最小时间戳,未知可以填0,表示到最早时间
* @param[in] max_time 最大时间戳,未知可以填0,表示从当前时间
* @param[in] need_last_msg 是否需要返回lastMsg，默认true
* @param[in] limit 结果集limit，最大100，缺省值100
* @param[in] cb 结果回调  可查看QuerySessionListCallabck定义
* @return void 无返回值
*/
static void QuerySessionList(uint64_t min_time, uint64_t max_time, bool need_last_msg, int limit, const QuerySessionListCallabck& cb);

/** @fn static void QuerySession(nim::NIMSessionType to_type, const std::string& session_id, const QuerySessionInfoCallback& cb)
	* 会话服务 查询会话
	* @param[in] to_type 会话类型
	* @param[in] session_id 会话ID
	* @param[in] cb 结果回调  可查看QuerySessionInfoCallback定义
	* @return void 无返回值
	*/
static void QuerySession(nim::NIMSessionType to_type, const std::string& session_id, const QuerySessionInfoCallback& cb);

/** @fn static void UpdateSession(nim::NIMSessionType to_type, const std::string& session_id, const std::string& ext, const UpdateSessionInfoCallback& cb)
	* 会话服务 更新会话
	* @param[in] to_type 会话类型
	* @param[in] session_id 会话ID
	* @param[in] ext 会话的扩展字段
	* @param[in] cb 结果回调  可查看UpdateSessionInfoCallback定义
	* @return void 无返回值
	*/
static void UpdateSession(nim::NIMSessionType to_type, const std::string& session_id, const std::string& ext, const UpdateSessionInfoCallback& cb);

/** @fn static void DeleteSession(const DeleteSessionParam& param, const DeleteSessionInfoCallback& cb)
	* 会话服务 删除会话
	* @param[in] param 要删除的会话列
	* @param[in] cb 结果回调  可查看DeleteSessionInfoCallback定义
	* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
static void DeleteSession(const SessionOnLineServiceHelper::DeleteSessionParam& param, const DeleteSessionInfoCallback& cb);

/** @fn static void RegSessionChanged(const SessionChangedCallback& cb)
	* 会话服务 注册会话变更回调
	* @param[in] cb 结果回调  可查看SessionChangedCallback定义
	* @return void 无返回值
	*/
static void RegSessionChanged(const SessionChangedCallback& cb);

/** @fn void UnregSessionOnLineServiceCb()
* 反注册 SessionOnLineService 所有回调
* @return void 无返回值
*/
static void UnregSessionOnLineServiceCb();
};

}

#endif//_NIM_CPP_SESSION_ONLINE_SERVICE_H_
/** @file nim_cpp_client.h
  * @brief 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/09/21
  */

#ifndef _NIM_SDK_CPP_CLIENT_H_
#define _NIM_SDK_CPP_CLIENT_H_

#include "nim_client_helper.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

/** @class Client
  * @brief 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能
  */

class Client
{

public:
	typedef std::function<void(const LoginRes&)> LoginCallback; /**< 登录回调模板 */
	typedef std::function<void(NIMResCode)> LogoutCallback;		/**< 登出回调模板 */
	typedef std::function<void(const KickoutRes&)> KickoutCallback;	/**< 被踢通知回调模板 */
	typedef std::function<void(void)> DisconnectCallback;		/**< 断网通知回调模板 */
	typedef std::function<void(const MultiSpotLoginRes&)> MultiSpotLoginCallback;	/**< 多端登录通知回调模板 */
	typedef std::function<void(const KickOtherRes&)> KickOtherCallback;	/**< 将多端下线回调模板 */
	typedef std::function<void(int, bool)> MultiportPushConfigCallback;	/**< 多端推送控制开关回调模板 */

public:
	/** @fn bool Init(const std::string& app_data_dir, const std::string& app_install_dir, const SDKConfig &config)
	* NIM SDK初始化
	* @param[in] app_data_dir 使用默认路径时只需传入单个目录名（不以反斜杠结尾)，使用自定义路径时需传入完整路径（以反斜杠结尾，并确保有正确的读写权限！）
	* @param[in] app_install_dir 目前不需要传入（SDK可以自动获取）
	* @param[in] config 初始化参数
	* @return bool 返回值true: 成功， false: 失败
	*/
	static bool Init(const std::string& app_data_dir
		, const std::string& app_install_dir
		, const SDKConfig &config);

	/** @fn void Cleanup(const std::string& json_extension = "")
	* NIM SDK清理
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Cleanup(const std::string& json_extension = "");

	/** @fn bool Login(const std::string& app_key, const std::string& account, const std::string& password, const LoginCallback& cb, const std::string& json_extension = "")
	* NIM客户端登录
	* @param[in] app_key
	* @param[in] account 帐号
	* @param[in] password 密码
	* @param[in] cb 登录流程的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool Login(const std::string& app_key
		, const std::string& account
		, const std::string& password
		, const LoginCallback& cb
		, const std::string& json_extension = "");

	/** @fn void Relogin(const std::string& json_extension = "")
	* NIM客户端手动重连（注意 APP需要统一处理自动重连/手动重连的回调，因为如果处于某次自动重连的过程中调用手动重连接口，不起作用！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Relogin(const std::string& json_extension = "");

	/** @fn void Logout(nim::NIMLogoutType logout_type, const LogoutCallback& cb, const std::string& json_extension = "")
	* NIM客户端注销/退出
	* @param[in] logout_type Logout操作类型，其定义见nim_client_def.h
	* @param[in] cb 注销/退出的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Logout(nim::NIMLogoutType logout_type
		, const LogoutCallback& cb
		, const std::string& json_extension = "");

	/** @fn bool KickOtherClient(const std::list<std::string>& client_ids)
	* 将本帐号的其他端踢下线
	* @param[in] client_ids 设备标识
	* @return bool 检查参数如果不符合要求则返回失败, 接口调用成功后的结果通过注册RegKickOtherClientCb回调得到
	*/
	static bool KickOtherClient(const std::list<std::string>& client_ids);

	/** @fn void RegReloginCb(const LoginCallback& cb, const std::string& json_extension = "")
	* 注册NIM客户端自动重连回调。重连失败时，如果不是网络错误引起的（网络相关的错误号为kNIMResTimeoutError和kNIMResConnectionError），而是服务器返回了非kNIMResSuccess的错误号，
	* 则说明重连的机制已经失效，需要APP层调用nim_client_logout执行注销操作并退回到登录界面后进行重新登录。
	* @param[in] cb 自动重连的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegReloginCb(const LoginCallback& cb, const std::string& json_extension = "");

	/** @fn void RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension = "")
	* 注册NIM客户端被踢回调
	* @param[in] cb 被踢的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension = "");

	/** @fn void RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension = "")
	* 注册NIM客户端掉线回调
	* @param[in] cb 掉线的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension = "");

	/** @fn void RegMultispotLoginCb(const MultiSpotLoginCallback& cb, const std::string& json_extension = "")
	* 注册NIM客户端多点登录通知回调
	* @param[in] cb 多点登录通知的回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegMultispotLoginCb(const MultiSpotLoginCallback& cb, const std::string& json_extension = "");

	/** @fn void RegKickOtherClientCb(const KickOtherCallback& cb, const std::string& json_extension = "")
	* 注册NIM客户端将本帐号的其他端踢下线结果回调
	* @param[in] cb 踢下线结果回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegKickOtherClientCb(const KickOtherCallback& cb, const std::string& json_extension = "");

	/** @fn void RegSyncMultiportPushConfigCb(const MultiportPushConfigCallback& cb, const std::string& json_extension = "")
	* 注册多端推送开关同步回调
	* @param[in] cb 回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void RegSyncMultiportPushConfigCb(const MultiportPushConfigCallback& cb, const std::string& json_extension = "");

	/** @fn void SetMultiportPushConfigAsync(bool switch_on, const MultiportPushConfigCallback& cb, const std::string& json_extension = "")
	* 设置多端推送
	* @param[in] switch_on 开关
	* @param[in] cb 回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void
	*/
	static void SetMultiportPushConfigAsync(bool switch_on, const MultiportPushConfigCallback& cb, const std::string& json_extension = "");

	/** @fn void GetMultiportPushConfigAsync(const MultiportPushConfigCallback& cb, const std::string& json_extension = "")
	* 获得多端推送设置
	* @param[in] cb 回调函数
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void
	*/
	static void GetMultiportPushConfigAsync(const MultiportPushConfigCallback& cb, const std::string& json_extension = "");

	/** @fn void UnregClientCb()
	* 反注册Client提供的所有回调
	* @return void 无返回值
	*/
	static void UnregClientCb();
};

} 

#endif	//_NIM_SDK_CPP_CLIENT_H_

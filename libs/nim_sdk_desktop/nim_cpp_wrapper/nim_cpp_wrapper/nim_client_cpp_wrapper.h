#pragma once


#include <string>
#include <functional>

namespace nim
{

#include "libs/nim_sdk_desktop/nim_c_sdk/include/nim_client_def.h"

/** @class Client
  * @brief 全局管理功能；主要包括SDK初始化/清理、客户端登录/退出等功能
  * 接口命名说明: nim_***(模块)_***(功能)，如nim_client_init
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/2/1
  */
class Client
{

public:
	typedef std::function<void(const std::string&)> LogoutCallback;
	typedef std::function<void(const std::string&)> ReloginCallback;
	typedef std::function<void(const std::string&)> KickoutCallback;
	typedef std::function<void(const std::string&)> DisconnectCallback;
	typedef std::function<void(const std::string&)> MultispotLoginCallback;
	typedef std::function<void(const std::string&)> KickoutOtherClientCallback;
	
	/** 
	* NIM SDK初始化，传入的JSON参数定义见nim_client_def.h
	* @param[in] app_data_dir 使用默认路径时只需传入单个目录名（不以反斜杠结尾)，使用自定义路径时需传入完整路径（以反斜杠结尾，并确保有正确的读写权限！）
	* @param[in] app_install_dir 目前不需要传入（SDK可以自动获取）
	* @param[in] json_extension json扩展参数。目前可以用于支持一些全局配置（必填，如数据库的加密密钥等）和私有服务器的设置（选填）
	* @return bool 返回值true: 成功， false: 失败
	*/
	static bool Init(const std::string& app_data_dir, const std::string& app_install_dir, const std::string& json_extension = "");

	/** 
	* NIM SDK清理，传入的JSON参数定义见nim_client_def.h
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Cleanup(const std::string& json_extension = "");

	/** 
	* NIM客户端登录，传入的JSON参数定义见nim_client_def.h
	* @param[in] app_key APP key
	* @param[in] account APP account
	* @param[in] token 令牌 (在后台绑定的登录token)
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 登录流程的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
	* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
	* @return void 无返回值
	*/
	static void Login(const std::string& app_key, const std::string& account, const std::string& token, nim_json_transport_cb_func cb, const void* user_data, const std::string& json_extension = "");

	/** 
	* NIM客户端手动重连（注意 APP需要统一处理自动重连/手动重连的回调，因为如果处于某次自动重连的过程中调用手动重连接口，不起作用！）
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @return void 无返回值
	*/
	static void Relogin(const std::string& json_extension = "");

	/** 
	* NIM客户端注销/退出，传入的JSON参数定义见nim_client_def.h
	* @param[in] logout_type Logout操作类型，其定义见nim_client_def.h
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 注销/退出的回调函数
	* @return void 无返回值
	*/
	static void Logout(nim::NIMLogoutType logout_type, const LogoutCallback& cb, const std::string& json_extension = "");

	/**
	* 将本帐号的其他端踢下线
	* @param[in] client_id 设备标识
	* @return void 无返回值 通过注册RegKickOtherClientCb回调得到结果
	*/
	static void KickOtherClient(const std::list<std::string>& client_ids);

	/** 
	* 注册NIM客户端自动重连回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 自动重连的回调函数
	* @return void 无返回值
	*/
	static void RegReloginCb(const ReloginCallback& cb, const std::string& json_extension = "");

	/** 
	* 注册NIM客户端被踢回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 被踢的回调函数
	* @return void 无返回值
	*/
	static void RegKickoutCb(const KickoutCallback& cb, const std::string& json_extension = "");

	/** 
	* 注册NIM客户端掉线回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 掉线的回调函数
	* @return void 无返回值
	*/
	static void RegDisconnectCb(const DisconnectCallback& cb, const std::string& json_extension = "");

	/** 
	* 注册NIM客户端多点登录通知回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 多点登录通知的回调函数
	* @return void 无返回值
	*/
	static void RegMultispotLoginCb(const MultispotLoginCallback& cb, const std::string& json_extension = "");

	/**
	* 注册NIM客户端将本帐号的其他端踢下线结果回调
	* @param[in] json_extension json扩展参数（备用，目前不需要）
	* @param[in] cb 踢下线结果回调函数
	* @return void 无返回值
	*/
	static void Client::RegKickOtherClientCb(const KickoutOtherClientCallback& cb, const std::string& json_extension = "");
};

} 
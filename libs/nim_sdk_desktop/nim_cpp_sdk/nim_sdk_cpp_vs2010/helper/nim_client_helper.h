/** @file nim_client_helper.h
  * @brief Client 辅助方法和数据结构定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/09/07
  */
#ifndef _NIM_SDK_CPP_CLIENT_HELPER_H_
#define _NIM_SDK_CPP_CLIENT_HELPER_H_

#include <string>
#include <list>
#include <functional>
#include "json.h"

/**
* @namespace nim
* @brief IM
*/
namespace nim
{
#include "nim_client_def.h"
#include "nim_res_code_def.h"

/** @brief SDK设置项 */
struct SDKConfig
{
	//global_config
	std::string		database_encrypt_key_;			/**< 数据库秘钥，必填，目前只支持最多32个字符的加密密钥！建议使用32个字符 */
	bool			preload_attach_;				/**< 必填，是否需要预下载附件缩略图，默认为true */
	NIMSDKLogLevel	sdk_log_level_;					/**< 定义见NIMSDKLogLevel，选填，SDK默认的内置级别为kNIMSDKLogLevelPro */

	//private_server_setting
	bool			use_private_server_;			/**< 选填，是否使用私有服务器 */
	std::string		lbs_address_;					/**< lbs地址，如果选择使用私有服务器，则必填 */
	std::string  	nos_lbs_address_;				/**< nos lbs地址，如果选择使用私有服务器，则必填 */
	std::list<std::string>	default_link_address_;			/**< 默认link服务器地址，如果选择使用私有服务器，则必填 */
	std::list<std::string>	default_nos_upload_address_;	/**< 默认nos 上传服务器地址，如果选择使用私有服务器，则必填 */
	std::list<std::string>	default_nos_download_address_;	/**< 默认nos 下载服务器地址，如果选择使用私有服务器，则必填 */
	std::list<std::string>	default_nos_access_address_;	/**< 默认nos access服务器地址，如果选择使用私有服务器，则必填 */
	std::string		rsa_public_key_module_;			/**< RSA public key，如果选择使用私有服务器，则必填 */
	int				rsa_version_;					/**< RSA version，如果选择使用私有服务器，则必填 */

	/** 构造函数 */
	SDKConfig() : preload_attach_(true)
				, use_private_server_(false)
				, rsa_version_(0)
				, sdk_log_level_(kNIMSDKLogLevelApp) {}
};

/** @brief 多端登陆客户端信息 */
struct OtherClientPres
{
	std::string	app_account_;			/**< 第三方账号 */
	NIMClientType	client_type_;		/**< 客户端类型, 见NIMClientType */
	std::string	client_os_;				/**< 登录系统类型,比如ios 6.0.1 */
	std::string	mac_address_;			/**< 登录设备的mac地址 */
	std::string	device_id_;				/**< 设备id，uuid */
	__int64		login_time_;			/**< 本次登陆时间, 精度到ms */

	/** 构造函数 */
	OtherClientPres() : login_time_(0) {}
};

/** @brief 登录结果回调信息 */
struct LoginRes
{
	NIMResCode res_code_;				/**< 返回的错误码NIMResCode */
	bool relogin_;						/**< 是否为重连过程 */
	NIMLoginStep	login_step_;		/**< 登录步骤NIMLoginStep */
	std::list<OtherClientPres> other_clients_;	/**< 其他端的在线状态列表，登录成功才会返回这部分内容 */

	/** 构造函数 */
	LoginRes() : relogin_(false) {}
};

/** @brief 被踢结果回调信息 */
struct KickoutRes
{
	NIMClientType client_type_;			/**< int, 客户端类型NIMClientType */
	NIMKickReason kick_reason_;			/**< 返回的被踢原因NIMKickReason */
};

/** @brief 多端登录回调信息 */
struct MultiSpotLoginRes
{
	NIMMultiSpotNotifyType	notify_type_;			/**< NIMMultiSpotNotifyType 多点登录通知类型 */
	std::list<OtherClientPres> other_clients_;		/**< 其他端的在线状态列表 */
};

/** @brief 踢人结果回调信息 */
struct KickOtherRes
{
	NIMResCode res_code_;					/**< 返回的错误码NIMResCode */
	std::list<std::string> device_ids_;		/**< 设备id，uuid */
};

/** @fn bool ParseOtherClientsPres(const Json::Value array_objs, std::list<OtherClientPres> &outs)
  * 解析多端登录客户端信息
  * @param[in] array_objs 多端登录客户端信息(Json Value数据)
  * @param[out] outs 多端登录客户端信息
  * @return bool 解析成功 或失败
  */
bool ParseOtherClientsPres(const Json::Value array_objs, std::list<OtherClientPres> &outs);

}

#endif //_NIM_SDK_CPP_CLIENT_HELPER_H_
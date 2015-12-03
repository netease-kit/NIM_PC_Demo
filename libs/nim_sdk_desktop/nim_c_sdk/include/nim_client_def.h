/** @file nim_client_def.h
  * @brief NIM SDK提供的Client相关定义（如登录、注销、被踢、掉线等功能）
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_CLIENT_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_CLIENT_DEF_H_

#include "nim_global_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @enum NIMLogoutType Logout类型 */
enum NIMLogoutType
{
	kNIMLogoutChangeAccout	= 1,	/**< 注销/切换帐号（返回到登录界面）*/
	kNIMLogoutKickout		= 2,	/**< 被踢（返回到登录界面）*/
	kNIMLogoutAppExit		= 3,	/**< 程序退出*/
	kNIMLogoutRelogin		= 4,	/**< 重连操作，包括保存密码时启动程序伪登录后的重连操作以及掉线后的重连操作（帐号未变化）*/
};

/** @enum NIMKickReason 被踢原因 */
enum NIMKickReason
{
	kNIMKickReasonSameGeneric			=   1,  /**< 互斥类型的客户端互踢*/
	kNIMKickReasonServerKick			=   2,  /**< 服务器端发起踢客户端指令*/
	kNIMKickReasonKickBySelfOtherClient	=   3,  /**< 被自己的其他端踢掉*/
};

/** @enum NIMClientType 客户端类型 */
enum NIMClientType
{
	kNIMClientTypeAndroid		= 1,  /**< Android*/
	kNIMClientTypeiOS			= 2,  /**< iOS*/
	kNIMClientTypePCWindows		= 4,  /**< PC Windows*/
	kNIMClientTypeWindowsPhone	= 8,  /**< Windows Phone*/
	kNIMClientTypeWeb			= 16, /**< Web*/
};

/** @enum NIMLoginStep 登录步骤 */
enum NIMLoginStep
{
	kNIMLoginStepLinking		= 0,	/**< 正在连接*/
	kNIMLoginStepLink			= 1,	/**< 连接服务器*/
	kNIMLoginStepLogining		= 2,	/**< 正在登录*/
	kNIMLoginStepLogin			= 3,	/**< 登录验证*/        
};

/** @enum NIMMultiSpotNotiyType 多点登录通知类型 */
enum NIMMultiSpotNotiyType
{
	kNIMMultiSpotNotiyTypeImIn	=   2,		/**< 通知其他在线端自己登录了*/
	kNIMMultiSpotNotiyTypeImOut	=   3,		/**< 通知其他在线端自己退出*/
};

/** @enum NIMSDKLogLevel NIM SDK log级别，级别越高，log越详细 */
enum NIMSDKLogLevel
{
	kNIMSDKLogLevelApp		= 5,	/**< SDK应用级别Log，正式发布时为了精简sdk log，可采用此级别*/
	kNIMSDKLogLevelPro		= 6,	/**< SDK过程级别Log，更加详细，更有利于开发调试*/
};

/** @name json extension params for nim_client_init API
  * for example: 
  * {"global_config":{"db_encrypt_key":"xxxxx","preload_attach":true},
  *  "private_server_setting":{"lbs":"xxxxx","nos_lbs":"xxxxx","default_link":["xxxxx"],"default_nos_upload":["xxxxx"],"default_nos_download":["xxxxx"],"rsa_public_key_module":"xxxxx"}
  * }
  * @{
  */ 
static const char *kNIMGlobalConfig				= "global_config";			/**< json object, Global Config */
static const char *kNIMDataBaseEncryptKey		= "db_encrypt_key";			/**< string, （必填，目前只支持最多32个字符的加密密钥！建议使用32个字符） */
static const char *kNIMPreloadAttach			= "preload_attach";			/**< bool, （必填，是否需要预下载附件缩略图） */
static const char *kNIMSDKLogLevel				= "sdk_log_level";			/**< int，定义见NIMSDKLogLevel（选填，SDK默认的内置级别为kNIMSDKLogLevelPro） */

static const char *kNIMPrivateServerSetting		= "private_server_setting";	/**< json object, Private Server Setting（一旦设置了私有服务器，则全部连私有服务器，必须确保配置正确！） */
static const char *kNIMLbsAddress				= "lbs";					/**< string, （必填，lbs地址） */
static const char *kNIMNosLbsAddress			= "nos_lbs";				/**< string, （必填，nos lbs地址） */
static const char *kNIMDefaultLinkAddress		= "default_link";			/**< string array, （必填，默认link服务器地址） */
static const char *kNIMDefaultNosUploadAddress	= "default_nos_upload";		/**< string array, （必填，默认nos 上传服务器地址） */
static const char *kNIMDefaultNosDownloadAddress= "default_nos_download";	/**< string array, （必填，默认nos 下载服务器地址） */
static const char *kNIMDefaultNosAccessAddress	= "default_nos_access";		/**< string array, （必填，默认nos access服务器地址） */
static const char *kNIMRsaPublicKeyModule		= "rsa_public_key_module";	/**< string, （必填，RSA public key） */
static const char *kNIMRsaVersion				= "rsa_version";			/**< int, （必填，RSA version） */
/** @}*/ //json extension params for nim_client_init API

/** @name json extension params for nim_client_kick_other_client API
  * for example: 
  * {
  *  "device_ids":["1","2"]
  * }
  * @{
  */ 
static const char *kNIMKickoutOtherDeviceIDs				= "device_ids";		/**< string array, 设备id，uuid */
/** @}*/ //json extension params for nim_client_kick_other_client API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_login and nim_client_reg_auto_relogin_cb API
  * for example: 
  * {"err_code": 200, "relogin": false, "login_step": 3,
  *  "other_clients_pres":[{"app_account":"abc","client_os":"***","client_type":1,"device_id":"***","mac":"***","login_time":"123456789"}]
  * }
  * @{
  */
static const char *kNIMErrorCode		= kNIMResCode;			/**< int, 返回的错误码NIMResCode */
static const char *kNIMRelogin			= "relogin";			/**< bool, 是否重连 */
static const char *kNIMLoginStep		= "login_step";			/**< int, 登录步骤NIMLoginStep */
static const char *kNIMOtherClientsPres	= "other_clients_pres";	/**< json object array， 其他端的在线状态列表，Keys SEE MORE in "kNIMPres***" Json Keys（登录成功才会返回这部分内容） */

static const char *kNIMPresAppAccount	= "app_account";		/**< string, 第三方帐号 */
static const char *kNIMPresClientType	= "client_type";		/**< int, 客户端类型, 见NIMClientType */
static const char *kNIMPresOS			= "client_os";			/**< string, 登录系统类型,比如ios 6.0.1 */
static const char *kNIMPresMac			= "mac";				/**< string, 登录设备的mac地址 */
static const char *kNIMPresDeviceID		= "device_id";			/**< string, 设备id，uuid */
static const char *kNIMPresLoginTime	= "login_time";			/**< long, 本次登录时间, 精度到ms */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_login and nim_client_reg_auto_relogin_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_reg_multispot_login_notify_cb API
  * for example: 
  * {"multi_spot_notiy_type":2,
  *  "other_clients_pres":[{"app_account":"abc","client_os":"***","client_type":1,"device_id":"***","mac":"***","login_time":"123456789"}]
  * }
  * @{
  */
static const char *kNIMMultiSpotNotiyType		= "multi_spot_notiy_type";	/**< int，多点登录通知类型NIMMultiSpotNotiyType，其他端的状态列表json格式同kNIMOtherClientsPres */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_multispot_login_notify_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_logout API
  * for example: 
  * {"err_code":200
  * }
  * @{
  */
static const char *kNIMLogoutErrorCode		= kNIMResCode;		/**< int, 返回的错误码NIMResCode */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_logout API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_cb API
  * for example: 
  * {"client_type":2,
  *  "reason_code":1
  * }
  * @{
  */
static const char *kNIMKickoutClientType	= "client_type";	/**< int, 客户端类型NIMClientType */
static const char *kNIMKickoutReasonCode	= "reason_code";	/**< int, 返回的被踢原因NIMKickReason */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_other_client_cb API
  * for example: 
  * {"err_code":200,
  *  "device_ids":["1","2"]
  * }
  * @{
  */
static const char *kNIMKickoutOtherResErrorCode				= kNIMResCode;		/**< int, 返回的错误码NIMResCode */
static const char *kNIMKickoutOtherResDeviceIDs				= "device_ids";		/**< string array, 设备id，uuid */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_other_client_cb API
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_CLIENT_DEF_H_
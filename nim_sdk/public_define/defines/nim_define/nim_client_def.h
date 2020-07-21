/** @file nim_client_def.h
  * @brief SDK接入 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
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

/** @typedef void (*nim_client_multiport_push_config_cb_func)(const char *content, const char *json_params, const void *user_data)
* 多端推送设置/同步回调
* @param[out] rescode
* @param[out] content
* @param[out] json_extension	json扩展数据（备用）
* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_client_multiport_push_config_cb_func)(int rescode, const char *content, const char *json_params, const void *user_data);

/** @typedef void (*nim_client_dnd_cb_func)(const char *content, const char *json_params, const void *user_data)
* (PC SDK该接口无效)ios免打扰设置/同步回调
* @param[out] rescode
* @param[out] content
* @param[out] json_extension	json扩展数据（备用）
* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_client_dnd_cb_func)(int rescode,const char *content, const char *json_params, const void *user_data);

/** @typedef void(*nim_client_get_server_current_time_cb_func)(int rescode, bool calc_local, uint64_t time,const char *json_params, const void *user_data)
* 获取服务端时间回调
* @param[out] rescode
* @param[out] calc_local 是否为本地计算
* @param[out] time 当前服务器时间(ms)
* @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void 无返回值
*/
typedef void(*nim_client_get_server_current_time_cb_func)(int rescode, bool calc_local, uint64_t time, const void *user_data);

/** @name 多端推送设置/同步 内容Json key
  * @{
  */ 
static const char *kNIMMultiportPushConfigContentKeyOpen	= "switch_open";		/**< int, 1开启，即桌面端在线时移动端不需推送；2关闭，即桌面端在线时移动端需推送 */
/** @}*/ //多端推送设置/同步 内容Json key

/** @name (PC SDK该设置无效)免打扰设置 内容Json key 
* 客户端传入的属性（如果开启免打扰，请让第三方确保把时间转成东八区，即北京时间，小时是24小时制)
* @{
*/
static const char *kNIMDndShowDetail = "show_detail"; /**< 是否显示详情，1显示详情，2不显示详情，其它按1处理(Integer)*/
static const char *kNIMDndOpened = "switch_open"; /**< 是否开启免打扰，1开启，2关闭，其它按2处理(Integer)*/
static const char *kNIMDndFromH = "fromh"; /**< 如果开启免打扰，开始小时数(Integer)*/
static const char *kNIMDndFromM = "fromm"; /**< 如果开启免打扰，开始分钟数(Integer)*/
static const char *kNIMDndToH = "toh"; /**< 如果开启免打扰，截止小时数(Integer)*/
static const char *kNIMDndToM = "tom"; /**< 如果开启免打扰，截止分钟数(Integer)*/
/** @}*/ //免打扰设置 内容Json key

/** @enum NIMLoginState 登录状态 */
enum NIMLoginState
{
	kNIMLoginStateLogin = 1,		/**< 登录状态*/
	kNIMLoginStateUnLogin = 2,		/**< 未登录状态*/
};

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
	kNIMClientTypeDefault		= 0,  /**< Default, unset*/
	kNIMClientTypeAndroid		= 1,  /**< Android*/
	kNIMClientTypeiOS			= 2,  /**< iOS*/
	kNIMClientTypePCWindows		= 4,  /**< PC Windows*/
	kNIMClientTypeWeb			= 16, /**< Web*/
	kNIMClientTypeRestAPI		= 32, /**< RestAPI*/
	kNIMClientTypeMacOS			= 64, /**< Mac*/
};

/** @enum NIMLoginStep 登录步骤 */
enum NIMLoginStep
{
	kNIMLoginStepLinking		= 0,	/**< 正在连接*/
	kNIMLoginStepLink			= 1,	/**< 连接服务器*/
	kNIMLoginStepLogining		= 2,	/**< 正在登录*/
	kNIMLoginStepLogin			= 3,	/**< 登录验证*/        
};

/** @enum NIMMultiSpotNotifyType 多点登录通知类型 */
enum NIMMultiSpotNotifyType
{
	kNIMMultiSpotNotifyTypeImIn	= 2,		/**< 通知其他在线端自己登录了*/
	kNIMMultiSpotNotifyTypeImOut= 3,		/**< 通知其他在线端自己退出*/
};

/** @name json extension params for nim_client_init API
  * for example: 
  * {"app_key":"45c6af3c98409b18a84451215d0bdd6e",
  *  "global_config":{"db_encrypt_key":"xxxxx","preload_attach":true,"sdk_log_level":1,"push_cer_name":"xxxxx"},
  *  "private_server_setting":{"lbs":"xxxxx","nos_lbs":"xxxxx","default_link":["xxxxx"],"default_nos_upload":["xxxxx"],"default_nos_download":["xxxxx"],"rsa_public_key_module":"xxxxx"}
  * }
  * @{
  */ 
static const char *kNIMAppKey							= "app_key";				/**< string, （必填，APP KEY） */
static const char *kNIMGlobalConfig						= "global_config";			/**< json object, Global Config */
static const char *kNIMDataBaseEncryptKey				= "db_encrypt_key";			/**< string, 必填，目前只支持最多32个字符的加密密钥！建议使用32个字符 */
static const char *kNIMPreloadAttach					= "preload_attach";			/**< bool, 是否需要预下载附件(图片和语音),选填,默认为true,如果有对带宽流量有较高要求的请关闭该选项，改为上层开发者按需下载附件文件 */
static const char *kNIMPreloadImageQuality				= "preload_image_quality";	/**< int, 预下载图片质量,选填,范围0-100 */
static const char *kNIMPreloadImageResize				= "preload_image_resize";	/**< string, 预下载图片基于长宽做内缩略,选填,比如宽100高50,则赋值为100x50,中间为字母小写x */
static const char *kNIMPreloadAttachImageNameTemplate = "preload_image_name_template";			/**< 预下载图片命名规则，以{filename}为token进行替换 */
static const char *kNIMSDKLogLevel						= "sdk_log_level";			/**< int，定义见NIMSDKLogLevel（选填，SDK默认的内置级别为kNIMSDKLogLevelPro） */
static const char *kNIMPushCerName						= "push_cer_name";			/**< string，推送证书名（选填，iOS端需要,PC SDK该设置无效） */
static const char *kNIMPushToken                        = "push_token";            /**< string，推送设备token（选填，iOS端需要,PC SDK该设置无效） */
static const char *kNIMSyncSessionAck					= "sync_session_ack";		/**< bool，设置是否已读未读状态多端同步，默认true */
static const char *kNIMLoginRetryMaxTimes				= "login_retry_max_times";	/**< int，登录重试最大次数，如需设置建议设置大于3次，默认填0，SDK默认设置次数 */
static const char *kNIMUseHttps							= "use_https";				/**< bool，是否启用HTTPS协议，默认为true */
static const char *kNIMTeamNotificationUnreadCount		= "team_notification_unread_count";		/**< bool，群通知是否计入未读数，默认为false */
static const char *kNIMVChatMissUnreadCount				= "vchat_miss_unread_count";		/**< bool，语音消息未接通消息是否计入未读数，默认为false */
static const char *kNIMResetUnreadCountWhenRecall		= "reset_unread_count_when_recall";		/**< bool，撤回消息是否重新计算未读消息计数，默认为false */
static const char *kNIMUploadSDKEventsAfterLogin		= "upload_sdk_events_after_login";	/**< bool，在调用 Login 接口后无论成功是否上报历史错误日志到服务器（目前支持 408、415、500）默认为 false */
static const char *kNIMAnimatedImageThumbnailEnabled	= "animated_image_thumbnail_enabled";	/**< bool，开启对动图缩略图的支持，默认为false，开启后获取的缩略图为原格式，关闭后获取的缩略图为第一帧静态图 */
static const char *kNIMClientAntispam					= "client_antispam";		/**< bool，客户端反垃圾，默认为false，如需开启请提前咨询技术支持或销售 */
static const char *kNIMTeamMessageAckEnabled			= "team_msg_ack";			/**< bool，群消息已读功能开关，默认为false，如需开启请提前咨询技术支持或销售  */
static const char *kNIMNeedUpdateLBSBeforRelogin 		= "need_update_lbs_befor_relogin";  /**< bool 在进行重新登录前是否先刷新一下lbs,对于切换网络的场景适用  */
static const char *kNIMCustomClientType = "custom_client_type";  /**< (可选)int 自定义客户端类型字段,大于0 否则初始化会失败*/
static const char *kNIMServerConfFilePath				= "server_conf_file_path";	/**< string,  私有云服务器相关地址配置文件本地绝对路径，如果不填默认执行文件目录下的nim_server.conf */
static const char *kNIMPrivateServerSetting				= "private_server_setting";	/**< json object, 私有服务器配置（设置方法有两种，一个是配置以下信息，一个是通过配置kNIMServerConfFilePath地址，信息从文件中读取） */
static const char *kNIMPrivateEnableHttps 				= "https_enabled";			/**< bool，【7.0.0版本后已废弃,统一由kNIMUseHttps来设置】（必填，私有化配置是否启用HTTPS协议，启用私有化配置时会覆盖 kNIMUseHttps，为true时kNIMDefaultNosUploadHost必填） */
static const char *kNIMLbsAddress						= "lbs";					/**< string, （必填，lbs地址） */
static const char* kNIMLbsBackupAddress = "lbs.backup";					/**< string list, （私有化参数 选填，lbs备用地址） */
static const char *kNIMNosLbsAddress					= "nos_lbs";				/**< string, （必填，nos lbs地址） */
static const char *kNIMDefaultLinkAddress				= "link";					/**< string array, （默认link 服务器地址 kNIMIPProtVersion == 0 or auto (ipv4）必填 */
static const char *kNIMDefaultLinkAddressIPV6			= "link_ipv6";			    /**< string array, （默认link ipv6服务器地址 kNIMIPProtVersion == 1(ipv6）必填 */
static const char *kNIMDefaultNosUploadAddress			= "nos_uploader";			/**< string array, （必填，默认nos 上传服务器地址） */
static const char *kNIMDefaultNosUploadHost				= "nos_uploader_host";		/**< string array, （kNIMPrivateEnableHttps设置为true 时必填，默认nos 上传服务器主机地址，用作 https 上传时的域名校验及 http header host 字段填充） */
static const char *kNIMRsaPublicKeyModule				= "module";					/**< string, （必填，RSA public key 【6.9.0版本后已废弃】） */
static const char *kNIMRsaVersion						= "version";				/**< int, （必填，RSA version 【6.9.0版本后已废弃】） */
static const char *kNIMDownloadAddressTemplate			= "nos_downloader";			/**< string, 下载地址拼接模板，用于拼接最终得到的下载地址 */
static const char *kNIMAccelerateHost					= "nos_accelerate_host";	/**< string, 需要被加速主机名*/
static const char *kNIMAccelerateAddressTemplate		= "nos_accelerate";			/**< string, 加速地址拼接模板，用于获得加速后的下载地址*/
//static const char* kNIMMarkreadAfterSaveDBEnabled 	= "enable_markread_after_save_db_";/**< bool, 是否开启消保存在本地DB以后再向服务端发送"已接收回执" 缺省 false 关闭*/
static const char *kNIMCachingMarkreadEnabled			= "caching_markread_enabled";	/**< 是否开启缓存式“已接收回执”发送，程序可能收到大量消息以至触发频控时可以考虑开启此开关 缺省 false 关闭*/
static const char *kNIMCachingMarkreadTime				= "caching_markread_time";	/**< caching_markread_ == true 时有效 缓存时间 单位ms 缺省 1000 */
static const char *kNIMCachingMarkreadCount				= "caching_markread_count";	/**< caching_markread_ == true 时有效 缓存的最大消息条数  缺省 10 */
static const char *kNIMUserDataFileLocalBackupFolder	= "user_datafile_localbackup_folder";		/**< string 用户数据文件备份（本地）目录，缺省在数据文件所在目录创建一个db_file.back目录 */
static const char *kNIMEnableUserDataFileLocalBackup	= "enable_user_datafile_backup";			/**< bool 是否开启用户数据备份(本地)功能  缺省true*/
static const char *kNIMEnableUserDataFileLocalRestore	= "enable_user_datafile_restore";			/**< bool 是否开启用户数据恢复(本地)功能  缺省false*/
static const char *kNIMEnableUserDataFileDefRestoreProc	= "enable_user_datafile_defrestoreproc";	/**< bool 是否使用缺省的用户数据恢复(本地)方案  缺省false enable_user_datafile_restore_ == true 生效*/
static const char *kNIMDedicatedClusteFlag 				= "dedicated_cluste_flag";/**< int 是否为专属集群 1:是 0: 否*/
static const char* kNIMHandShakeType 					= "hand_shake_type"; /**< int 登录时使用的握手协议类型 0:支持配置多种对称与非对称加密算法,1:只支持RAS + RC4，缺省 1 */
static const char *kNIMNegoKeyNECA 						= "nego_key_neca"; /**< int  "交换密钥"协议加密算法 {1(RSA),2(SM2)}, def:1(RSA) 非对称加密 */
static const char *kNIMNegoKeyNECAKeyPA 				= "nego_key_enca_key_parta"; /**< string  非对称加密算法key 自定义时则必填 十六进制大整数 不带0x*/
static const char *kNIMNegoKeyNECAKeyPB 				= "nego_key_enca_key_partb"; /**< string  非对称加密算法key2 RSA:EXP,SM2: SM2Y 自定义时则必填 十六进制大整数 不带0x*/
static const char *kNIMNegoKeyNECAKeyV 					= "nego_key_enca_key_version";   /**< int  非对称加密算法的 key version 自定义时则必填 */
static const char *kNIMCommNECA 						= "comm_enca";     /**< int 通信加密算法 {1(RC4),2(AES128),4(SM4)} def:1(RC4) 对称加密*/

static const char *kNIMIPProtVersion = "ip_protocol_version";   /**< int IP协议版本{0(IPV4),1(IPV6),2(Auto,SDK测试后自行选择,存在性能损耗)}。def:0(IPV4) */
static const char *kNIMProbeIPV4URL = "probe_ipv4_url";/**< string 探测ipv4地址类型使用的url,IPProtVersion == 2(Auto) 时生效*/
static const char *kNIMProbeIPV6URL = "probe_ipv6_url";/**< string 探测ipv6地址类型使用的url,IPProtVersion == 2(Auto) 时生效*/

static const char *kNIMSyncDataTypeConfig = "sync_data_type_config";/**< json array {"sync_data_type_config":[{"key":28,"type":0}]} 同步数据类型配置*/
static const char *kNIMSyncDataTypeConfigKey = "key";/**< int 同步数据类型配置的具体数据kye 28:置顶会话*/
static const char *kNIMSyncDataTypeConfigValue = "value";/**< int 同步数据类型配置的具体数据value 0:不更新 1:自动更新*/

static const char* kNIMHttpDNSServerInterface = "httpdns_server_interface";/**< string list 私有化参数,选填 如果有特定的地址可以配置在这个参数中*/

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
  * {"err_code": 200, "relogin": false, "login_step": 3, "retrying": false, 
  *  "other_clients_pres":[{"app_account":"abc","client_os":"***","client_type":1,"device_id":"***","mac":"***","login_time":"123456789"}]
  * }
  * @{
  */
static const char *kNIMErrorCode		= "err_code";			/**< int, 返回的错误码NIMResCode */
static const char *kNIMRelogin			= "relogin";			/**< bool, 是否属于重连 */
static const char *kNIMLoginStep		= "login_step";			/**< int, 登录步骤NIMLoginStep */
static const char *kNIMRetrying			= "retrying";			/**< bool, SDK是否在重试，如果为false，开发者需要检查登录步骤和错误码，明确问题后调用手动重连接口进行登录操作 */
static const char *kNIMOtherClientsPres	= "other_clients_pres";	/**< json object array， 其他端的在线状态列表，Keys SEE MORE in "kNIMPres***" Json Keys（登录成功才会返回这部分内容） */

static const char *kNIMPresAppAccount	= "app_account";		/**< string, 第三方帐号 */
static const char *kNIMPresClientType	= "client_type";		/**< int, 客户端类型, 见NIMClientType */
static const char *kNIMPresOS			= "client_os";			/**< string, 登录系统类型,比如ios 6.0.1 */
static const char *kNIMPresMac			= "mac";				/**< string, 登录设备的mac地址 */
static const char *kNIMPresDeviceID		= "device_id";			/**< string, 设备id，uuid */
static const char *kNIMPresLoginTime	= "login_time";			/**< long, 本次登录时间, 精度到ms */
static const char *kNIMPresCustomTag	= "custom_tag";			/**< string, 本次登录用户自定义字段 */
static const char *kNIMPresCustomClientType = "custom_client_type";/**< int, 自定义客户端类型字段,大于0 */

/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_login and nim_client_reg_auto_relogin_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_reg_multispot_login_notify_cb API
  * for example: 
  * {"multi_spot_notiy_type":2,
  *  "other_clients_pres":[{"app_account":"abc","client_os":"***","client_type":1,"device_id":"***","mac":"***","login_time":"123456789"}]
  * }
  * @{
  */
static const char *kNIMMultiSpotNotifyType		= "multi_spot_notiy_type";	/**< int，多点登录通知类型NIMMultiSpotNotifyType，其他端的状态列表json格式同kNIMOtherClientsPres */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_multispot_login_notify_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_logout API
  * for example: 
  * {"err_code":200
  * }
  * @{
  */
static const char *kNIMLogoutErrorCode		= "err_code"/*kNIMResCode*/;		/**< int, 返回的错误码NIMResCode */
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
static const char* kNIMKickoutReasonDescription = "reason_description";	/**< string, 返回的被踢描述 */
static const char* kNIMKickoutReasonCustomClientType = "custom_client_type";	/**< int 自定义客户端类型 > 0 */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_cb API

/** @name json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_other_client_cb API
  * for example: 
  * {"err_code":200,
  *  "device_ids":["1","2"]
  * }
  * @{
  */
static const char *kNIMKickoutOtherResErrorCode				= "err_code"/*kNIMResCode*/;		/**< int, 返回的错误码NIMResCode */
static const char *kNIMKickoutOtherResDeviceIDs				= "device_ids";		/**< string array, 设备id，uuid */
/** @}*/ //json params for nim_json_transport_cb_func that has been registered in nim_client_reg_kickout_other_client_cb API
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_CLIENT_DEF_H_

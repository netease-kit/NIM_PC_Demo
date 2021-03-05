/** @file nim_client.h
  * @brief SDK接入 接口头文件，主要包括SDK初始化/清理、客户端登录/退出/重连/掉线/被踢等流程
  * NIM SDK所有接口命名说明: nim_***(模块)_***(功能)，如nim_client_init
  * NIM SDK所有接口参数说明: 所有与字符串相关的参数(char *)全部为UTF-8编码。
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */

#ifndef NIM_SDK_DLL_API_NIM_CLIENT_H_
#define NIM_SDK_DLL_API_NIM_CLIENT_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/nim_util_include.h"
#include "public_define/defines/nim_define/nim_client_def.h"
#include "public_define/defines/nim_define/nim_global_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn bool nim_client_init(const char *app_data_dir, const char *app_install_dir, const char *json_extension)    
  * NIM SDK初始化，传入的JSON参数定义见nim_client_def.h    
  * @param[in] app_data_dir 使用默认路径时只需传入单个目录名（不以反斜杠结尾)，使用自定义路径时需传入完整路径（以反斜杠结尾，并确保有正确的读写权限！）
  * @param[in] app_install_dir 目前不需要传入（SDK可以自动获取）
  * @param[in] json_extension json扩展参数。目前可以用于支持一些全局配置（必填，如APP_KEY、数据库的加密密钥等）和私有服务器的设置（选填）
  * @return bool 返回值true: 成功， false: 失败
  */ 
NIM_SDK_DLL_API	bool nim_client_init(const char *app_data_dir, const char *app_install_dir, const char *json_extension);

/** @fn void nim_client_cleanup(const char *json_extension)   
  * NIM SDK清理，传入的JSON参数定义见nim_client_def.h    
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_client_cleanup(const char *json_extension);

/** @fn void nim_client_cleanup2(nim_json_transport_cb_func cb,const char *json_extension, const void *user_data)
  * NIM SDK清理，传入的JSON参数定义见nim_client_def.h
  * @param[in] cb 清理的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_client_cleanup2(nim_json_transport_cb_func cb,const char *json_extension, const void *user_data);

/** @fn void nim_client_update_apns_token(const char *device_token)   
  * (PC SDK该接口无效)更新Device Token，iOS推送使用    
  * @param[in] device_token device token
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_client_update_apns_token(const char *device_token);

/** @fn void nim_client_login(const char *app_key, const char *account, const char *token, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * NIM客户端登录，传入的JSON参数定义见nim_client_def.h    
  * @param[in] app_key APP key
  * @param[in] account APP account
  * @param[in] token 令牌 (在后台绑定的登录token)
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 登录流程的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				302:账号或密码错误
  *				403:禁止操作
  *				408:请求超时
  *				414:参数错误
  *				415:网络连接出现错误
  *				416:频率超限
  *				422:账号被禁用
  *				20002:账号已经处于登录状态
  */
NIM_SDK_DLL_API	void nim_client_login(const char *app_key, const char *account, const char *token, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn int nim_client_get_login_state(const char *json_extension)
  * 获取NIM客户端登录状态
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return int 登录状态见nim_client_def.h 中 NIMLoginState
  */
NIM_SDK_DLL_API int nim_client_get_login_state(const char *json_extension);

/** @fn void nim_client_relogin(const char *json_extension)
  * NIM客户端手动重连（注意 APP需要统一处理自动重连/手动重连的回调，因为如果处于某次自动重连的过程中调用手动重连接口，不起作用！）   
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_client_relogin(const char *json_extension);

/** @fn void nim_client_logout(enum NIMLogoutType logout_type, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * NIM客户端注销/退出，传入的JSON参数定义见nim_client_def.h 
  * @param[in] logout_type Logout操作类型，其定义见nim_client_def.h
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 注销/退出的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				500:未知错误
  */
NIM_SDK_DLL_API	void nim_client_logout(enum NIMLogoutType logout_type, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_client_kick_other_client(const char *json_extension)
  * 将本帐号的其他端踢下线
  * @param[in] json_extension 设备标识，其定义见nim_client_def.h
  * @return void 无返回值 通过注册nim_client_reg_kickout_other_client_cb回调得到结果
  */
NIM_SDK_DLL_API void nim_client_kick_other_client(const char *json_extension);

/** @fn void nim_client_reg_auto_relogin_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * (全局回调)注册NIM客户端自动重连回调。重连失败时，如果不是网络错误引起的（网络相关的错误号为kNIMResTimeoutError和kNIMResConnectionError），而是服务器返回了非kNIMResSuccess的错误号，
  * 则说明重连的机制已经失效，需要APP层调用nim_client_logout执行注销操作并退回到登录界面后进行重新登录。
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 自动重连的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h。
  *				  如果返回错误号kNIMResExist，说明无法继续重连，App层必须调用nim_client_logout退出到登录界面，以便用户重新进行登录。
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  *				403:禁止操作
  *				408:请求超时
  *				414:参数错误
  *				415:网络连接出现错误
  *				416:频率超限
  *				422:账号被禁用
  */
NIM_SDK_DLL_API	void nim_client_reg_auto_relogin_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_client_reg_relogin_request_token_cb(const char* json_extension, nim_relogin_request_token_cb_func cb, const void* user_data)
 *
 * @param[in] json_extension json扩展参数（备用，目前不需要）
 * @param[in] cb 如果登录类型模式默认类型，则注册该回调用于在重登陆时获取新的登录鉴权 token
 * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
 * @return void 无返回值
 * @note 错误码	无
 */
NIM_SDK_DLL_API void nim_client_reg_relogin_request_token_cb(const char* json_extension, nim_relogin_request_token_cb_func cb, const void* user_data);

/** @fn void nim_client_reg_kickout_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * (全局回调)注册NIM客户端被踢回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 被踢的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_client_reg_kickout_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn nim_client_reg_disconnect_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * (全局回调)注册NIM客户端掉线回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 掉线的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_client_reg_disconnect_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_client_reg_multispot_login_notify_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * (全局回调)注册NIM客户端多点登录通知回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 多点登录通知的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_client_reg_multispot_login_notify_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_client_reg_kickout_other_client_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * (全局回调)注册NIM客户端将本帐号的其他端踢下线结果回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  * @note 错误码	200:成功
  */
NIM_SDK_DLL_API void nim_client_reg_kickout_other_client_cb(const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_client_reg_sync_multiport_push_config_cb(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
  * (全局回调)注册多端推送设置同步回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_client_reg_sync_multiport_push_config_cb(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);

/** @fn nim_client_set_multiport_push_config(const char *switch_content, const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
  * 设置多端推送控制开关
  * @param[in] switch_content 开关内容
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果的回调函数
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值 通过注册nim_client_reg_kickout_other_client_cb回调得到结果
  * @note 错误码	200:成功
  */
NIM_SDK_DLL_API void nim_client_set_multiport_push_config(const char *switch_content, const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);

/** @fn nim_client_get_multiport_push_config(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);
  * 获取多端推送控制开关
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果的回调函数
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值 通过注册nim_client_reg_kickout_other_client_cb回调得到结果
  */
NIM_SDK_DLL_API void nim_client_get_multiport_push_config(const char *json_extension, nim_client_multiport_push_config_cb_func cb, const void *user_data);

/** @fn char* nim_client_version()
* 获取NIM SDK 版本号
* @return const char* const 版本号
*/
NIM_SDK_DLL_API const char* const nim_client_version();

/** @fn char* nim_client_current_user_account()
* 获取当前登录用户的accid
* @return const char* const  accid
*/
NIM_SDK_DLL_API const char* const nim_client_current_user_account();

/** @fn void nim_client_get_server_current_time(bool calc_local,nim_client_get_server_current_time_cb_func cb, const char *json_extension,const void *user_data)
* 获取当前服务器时间
* @param[in] calc_local 是否在本地计算，false:直接到服务端查询 ,true:根据上次查询到的服务端时间与本地系统启动时间来计算，不会到服务端查询 
* @param[in] cb 操作结果的回调函数
* @param[in] json_extension json扩展参数(备用，目前不需要）
* @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
* @return void
 * @note 由于网络上/下行的原因，返回的时间会存在一定误差，
 当 calc_local == false 时,如果跟上次调用该方法的时间间隔小于1000ms，SDK会采用 calc_local == true 时的方案以减少服务端的压力，并会在回调中指明返回的时间是由本地计算的。
 如果返回 code != 200,同样会返回一个本地计算结果
 */
NIM_SDK_DLL_API void nim_client_get_server_current_time(bool calc_local,nim_client_get_server_current_time_cb_func cb, const char *json_extension,const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_CLIENT_H_
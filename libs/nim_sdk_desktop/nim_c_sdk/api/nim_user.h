/** @file nim_user.h
  * @brief NIM SDK提供的用户相关接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Harrison
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_API_NIM_USER_H_
#define NIM_SDK_DLL_API_NIM_USER_H_

#include "nim_sdk_dll.h"
#include "nim_user_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_user_set_stat(const unsigned int user_stat, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data)
  * 顺风环境专用更新用户自定义在线状态（公网版不开发该接口），传入的JSON参数json_extension定义见nim_user_def.h
  * @param[in] user_stat 自定义用户状态，值意义由第三方自定义
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 更新用户自定义在线状态的回调函数， nim_json_transport_cb_func回调函数定义见nim_global_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_set_stat(const unsigned int user_stat, const char *json_extension, nim_json_transport_cb_func cb, const void *user_data);

/** @fn void nim_user_reg_special_relationship_changed_cb(const char *json_extension, nim_user_special_relationship_change_cb_func cb, const void *user_data)
  * 统一注册用户属性变更通知回调函数（本地、多端同步黑名单、静音名单变更）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_reg_special_relationship_changed_cb(const char *json_extension, nim_user_special_relationship_change_cb_func cb, const void *user_data);

/** @fn void nim_user_set_black(const char *accid, bool set_black, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data)
  * 设置、取消设置黑名单
  * @param[in] accid 好友id
  * @param[in] set_black 取消或设置
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_set_black(const char *accid, bool set_black, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);

/** @fn void nim_user_set_mute(const char *accid, bool set_mute, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data)
  * 设置、取消设置静音名单
  * @param[in] accid 好友id
  * @param[in] set_mute 取消或设置
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_set_mute(const char *accid, bool set_mute, const char *json_extension, nim_user_opt_cb_func cb, const void *user_data);

/** @fn void nim_user_get_mute_blacklist(const char *json_extension, nim_user_sync_muteandblacklist_cb_func cb, const void *user_data)
  * 获得黑名单/静音名单
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_get_mute_blacklist(const char *json_extension, nim_user_sync_muteandblacklist_cb_func cb, const void *user_data);

/** @fn void nim_user_reg_user_name_card_changed_cb(const char *json_extension, nim_user_name_card_change_cb_func cb, const void *user_data)
  * 统一注册用户名片变更通知回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_reg_user_name_card_changed_cb(const char *json_extension, nim_user_name_card_change_cb_func cb, const void *user_data);

/** @fn void nim_user_get_user_name_card(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data)
  * 获取本地的指定帐号的用户名片
  * @param[in] accids 例 ["litianyi01", "litianyi02", ...]
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_get_user_name_card(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data);

/** @fn void nim_user_get_user_name_card_online(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data)
  * 在线查询指定帐号的用户名片
  * @param[in] accids 例 ["litianyi01", "litianyi02", ...]
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_get_user_name_card_online(const char *accids, const char *json_extension, nim_user_get_user_name_card_cb_func cb, const void *user_data);

/** @fn void nim_user_update_user_name_card(const char *info_json, const char *json_extension, nim_user_update_name_card_cb_func cb, const void *user_data)
  * 更新用户名片
  * @param[in] info_json 用户信息json
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb 操作结果回调
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API	void nim_user_update_user_name_card(const char *info_json, const char *json_extension, nim_user_update_name_card_cb_func cb, const void *user_data);


#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_USER_H_
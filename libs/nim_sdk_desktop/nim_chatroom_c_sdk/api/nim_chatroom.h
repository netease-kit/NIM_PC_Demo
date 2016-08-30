/** @file nim_chatroom.h
  * @brief NIM 聊天室 SDK提供的接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/12/28
  */

#ifndef NIM_CHATROOM_SDK_DLL_API_H_
#define NIM_CHATROOM_SDK_DLL_API_H_

#include "nim_chatroom_sdk_dll.h"
#include "nim_chatroom_def.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @fn void nim_chatroom_reg_enter_cb(const char *json_extension, nim_chatroom_enter_cb_func cb, const void *user_data)
  * 注册全局进入聊天室的回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_enter_cb(const char *json_extension, nim_chatroom_enter_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_reg_exit_cb(const char *json_extension, nim_chatroom_exit_cb_func cb, const void *user_data)
  * 注册全局登出、被踢回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_exit_cb(const char *json_extension, nim_chatroom_exit_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_reg_link_condition_cb(const char *json_extension, nim_chatroom_link_condition_cb_func cb, const void *user_data)
  * 注册聊天室链接状况回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_link_condition_cb(const char *json_extension, nim_chatroom_link_condition_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_reg_send_msg_ack_cb(const char *json_extension, nim_chatroom_sendmsg_arc_cb_func cb, const void *user_data)
  * 注册全局发送消息回执回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_send_msg_ack_cb(const char *json_extension, nim_chatroom_sendmsg_arc_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_reg_receive_msg_cb(const char *json_extension, nim_chatroom_receive_msg_cb_func cb, const void *user_data)
  * 注册全局接收消息回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_receive_msg_cb(const char *json_extension, nim_chatroom_receive_msg_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_reg_receive_notification_cb(const char *json_extension, nim_chatroom_receive_notification_cb_func cb, const void *user_data)
  * 注册全局接收通知回调
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			  回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data	  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_reg_receive_notification_cb(const char *json_extension, nim_chatroom_receive_notification_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_init(const char *json_extension)
  * 聊天室模块初始化
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void
  */
NIM_SDK_DLL_API void nim_chatroom_init(const char *json_extension);

/** @fn bool nim_chatroom_enter(const __int64 room_id, const char *request_enter_data, const char *enter_info, const char *json_extension)
  * 聊天室进入
  * @param[in] room_id			  聊天室ID
  * @param[in] request_enter_data 聊天室进入信息(NIM SDK请求聊天室返回的数据)
  * @param[in] enter_info		  聊天室可选信息
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return bool 进入信息是否正确,返回失败则不会促发进入回调
  */
NIM_SDK_DLL_API bool nim_chatroom_enter(const __int64 room_id, const char *request_enter_data, const char *enter_info, const char *json_extension);

/** @fn void nim_chatroom_exit(const __int64 room_id, const char *json_extension)
  * 聊天室登出
  * @param[in] room_id			  聊天室ID
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_exit(const __int64 room_id, const char *json_extension);

/** @fn void nim_chatroom_cleanup(const char *json_extension)
  * 聊天室模块清理
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_cleanup(const char *json_extension);

/** @fn void nim_chatroom_send_msg(const __int64 room_id, const char *msg, const char *json_extension)
  * 发送消息
  * @param[in] room_id			  聊天室ID
  * @param[in] msg				  消息json string
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_send_msg(const __int64 room_id, const char *msg, const char *json_extension);

/** @fn void nim_chatroom_get_members_online_async(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data)
  * 异步获取聊天室成员信息
  * @param[in] room_id				聊天室ID
  * @param[in] parameters_json_str	参数json string
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_get_members_online_async(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_get_msg_history_online_async(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_msg_cb_func cb, const void *user_data)
  * 异步获取消息历史
  * @param[in] room_id				聊天室ID
  * @param[in] parameters_json_str	参数json string
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_get_msg_history_online_async(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_get_msg_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_set_member_attribute_async(const __int64 room_id, const char *parameters_json_str, const char *json_extension, nim_chatroom_set_member_attribute_cb_func cb, const void *user_data)
  * 异步修改成员身份标识
  * @param[in] room_id				聊天室ID
  * @param[in] parameters_json_str	参数json string
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_set_member_attribute_async(const __int64 room_id, const char *notify_ext, const char *json_extension, nim_chatroom_set_member_attribute_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_get_info_async(const __int64 room_id, const char *json_extension, nim_chatroom_get_info_cb_func cb, const void *user_data)
  * 异步获取当前聊天室信息
  * @param[in] room_id				聊天室ID
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_get_info_async(const __int64 room_id, const char *json_extension, nim_chatroom_get_info_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_get_members_by_ids_online_async(const __int64 room_id, const char *ids_json_array_string, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data)
  * 异步获取指定成员信息
  * @param[in] room_id				聊天室ID
  * @param[in] ids_json_array_string json array string
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_get_members_by_ids_online_async(const __int64 room_id, const char *ids_json_array_string, const char *json_extension, nim_chatroom_get_members_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_kick_member_async(const __int64 room_id, const char *id, const char *notify_ext, const char *json_extension, nim_chatroom_kick_member_cb_func cb, const void *user_data)
  * 异步踢掉指定成员
  * @param[in] room_id				聊天室ID
  * @param[in] id					成员id
  * @param[in] notify_ext			放到事件通知中的扩展字段
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_kick_member_async(const __int64 room_id, const char *id, const char *notify_ext, const char *json_extension, nim_chatroom_kick_member_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_set_proxy(NIMChatRoomProxyType type, const char *host, int port, const char *user, const char *password)
* 设置Chatroom SDK统一的网络代理。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。
* @param[in] type 代理类型，见NIMProxyType定义
* @param[in] host 代理地址
* @param[in] port 代理端口
* @param[in] user 代理用户名
* @param[in] password 代理密码
* @return void 无返回值
*/
NIM_SDK_DLL_API void nim_chatroom_set_proxy(NIMChatRoomProxyType type, const char *host, int port, const char *user, const char *password);

/** @fn void nim_chatroom_temp_mute_member_async(const __int64 room_id, const char *accid, const __int64 duration, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_temp_mute_member_cb_func cb, const void *user_data)
  * 异步临时禁言/解禁成员
  * @param[in] room_id				聊天室ID
  * @param[in] accid				成员ID
  * @param[in] duration				临时禁言时长（秒），解禁填0
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_temp_mute_member_async(const __int64 room_id, const char *accid, const __int64 duration, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_temp_mute_member_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_update_room_info_async(const __int64 room_id, const char *room_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_room_info_cb_func cb, const void *user_data)
  * 更新聊天室信息，目前只支持更新kNIMChatRoomInfoKeyName,kNIMChatRoomInfoKeyAnnouncement,kNIMChatRoomInfoKeyBroadcastUrl,kNIMChatRoomInfoKeyExt四个字段
  * @param[in] room_id				聊天室ID
  * @param[in] room_info_json_str	聊天室信息
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_update_room_info_async(const __int64 room_id, const char *room_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_room_info_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_update_my_role_async(const __int64 room_id, const char *role_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_my_role_cb_func cb, const void *user_data)
  * 更新我的信息，目前只支持更新kNIMChatRoomMemberInfoKeyNick,kNIMChatRoomMemberInfoKeyAvatar,kNIMChatRoomMemberInfoKeyExt三个字段
  * @param[in] room_id				聊天室ID
  * @param[in] role_info_json_str	我的信息
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_update_my_role_async(const __int64 room_id, const char *role_info_json_str, bool need_notify, const char *notify_ext, const char *json_extension, nim_chatroom_update_my_role_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_queue_offer_async(const __int64 room_id, const char *element_key, const char *element_value, const char *json_extension, nim_chatroom_queue_offer_cb_func cb, const void *user_data)
  * (聊天室管理员权限)新加(更新)麦序队列元素,如果element_key对应的元素已经在队列中存在了，那就是更新操作，如果不存在，就放到队列尾部 
  * @param[in] room_id				聊天室ID
  * @param[in] element_key			新元素的UniqKey,长度限制128字节 
  * @param[in] element_value		新元素内容，长度限制4096字节 
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_queue_offer_async(const __int64 room_id, const char *element_key, const char *element_value, const char *json_extension, nim_chatroom_queue_offer_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_queue_poll_async(const __int64 room_id, const char *element_key, const char *json_extension, nim_chatroom_queue_poll_cb_func cb, const void *user_data)
  * (聊天室管理员权限)取出麦序头元素 
  * @param[in] room_id				聊天室ID
  * @param[in] element_key			需要取出的元素的UniqKey,长度限制128字节,传空传表示取出第一个元素
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_queue_poll_async(const __int64 room_id, const char *element_key, const char *json_extension, nim_chatroom_queue_poll_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_queue_list_async(const __int64 room_id, const char *json_extension, nim_chatroom_queue_list_cb_func cb, const void *user_data)
  * 排序列出所有麦序元素 
  * @param[in] room_id				聊天室ID
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_queue_list_async(const __int64 room_id, const char *json_extension, nim_chatroom_queue_list_cb_func cb, const void *user_data);

/** @fn void nim_chatroom_queue_drop_async(const __int64 room_id, const char *json_extension, nim_chatroom_queue_drop_cb_func cb, const void *user_data)
  * (聊天室管理员权限)删除麦序队列
  * @param[in] room_id				聊天室ID
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @param[in] cb					回调函数, 定义见nim_chatroom_def.h
  * @param[in] user_data			APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_chatroom_queue_drop_async(const __int64 room_id, const char *json_extension, nim_chatroom_queue_drop_cb_func cb, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_CHATROOM_SDK_DLL_API_H_
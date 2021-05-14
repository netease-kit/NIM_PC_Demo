/** @file nim_talkex.h
  * @brief 收藏、置顶聊天、PinMessage、ThreadMessage、快捷回复接口头文件
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#ifndef NIM_SDK_DLL_API_NIM_TALKEX_H_
#define NIM_SDK_DLL_API_NIM_TALKEX_H_

#include "public_define/defines/nim_sdk_dll.h"
#include "public_define/defines/nim_define/nim_talk_def.h"
#include "public_define/defines/nim_define/nim_talkex_def_collect.h"
#include "public_define/defines/nim_define/nim_talkex_def_quick_comment.h"
#include "public_define/defines/nim_define/nim_talkex_def_pin_message.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn void nim_talkex_collect_add_collect(const char* collect_info, nim_talkex_collect_add_collect_cb_func cb, const void* user_data)
  * 添加收藏
  * @param[in] collect_info 收藏的内容
  * @param[in] cb	添加收藏回调函数定义 详见 nim_talkex_def_collect.h
  * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
    NIM_SDK_DLL_API void nim_talkex_collect_add_collect(const char* collect_info, nim_talkex_collect_add_collect_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_collect_remove_collects(const char* collect_list, nim_talkex_collect_remove_collects_cb_func cb, const void* user_data)
  * 添加收藏
  * @param[in] collect_list 要删除的收藏列表
  * @param[in] cb	删除收藏回调函数定义 详见 nim_talkex_def_collect.h
  * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
    NIM_SDK_DLL_API void nim_talkex_collect_remove_collects(const char* collect_list, nim_talkex_collect_remove_collects_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_collect_update_collect_ext(const char* collect_match_param, const char* ext, nim_talkex_collect_update_collect_ext_cb_func cb, const void* user_data)
    * 更新收藏
    * @param[in] collect_match_param 收藏匹配参数
    * @param[in] ext 收藏扩展参数
    * @param[in] cb	更新收藏回调函数定义 详见 nim_talkex_def_collect.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_collect_update_collect_ext(const char* collect_match_param, const char* ext, nim_talkex_collect_update_collect_ext_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_collect_query_collects(const char* query_collect_list_param, nim_talkex_collect_query_collects_cb_func cb, const void* user_data)
    * 分页查询收藏列表
    * @param[in] query_collect_list_param 查询参数
    * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_collect.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_collect_query_collects(const char* query_collect_list_param, nim_talkex_collect_query_collects_cb_func cb, const void* user_data);
   
    /** @fn void nim_talkex_quick_comment_add_quick_comment(const char* msg, const char* info, nim_talkex_quick_comment_add_quick_comment_cb_func cb, const void* user_data)
    * 添加快捷回复
    * @param[in] msg 被回复的消息
    * @param[in] info 回复的内容及设置
    * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_quick_comment.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_quick_comment_add_quick_comment(const char* msg, const char* info, nim_talkex_quick_comment_add_quick_comment_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_quick_comment_remove_quick_comment(const char* msg, const char* remove_param, nim_talkex_quick_comment_remove_quick_comment_cb_func cb, const void* user_data)
    * 删除快捷回复
    * @param[in] msg 被回复的消息
    * @param[in] info 回复的内容及设置
    * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_quick_comment.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_quick_comment_remove_quick_comment(const char* msg, const char* remove_param, nim_talkex_quick_comment_remove_quick_comment_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_quick_comment_query_quick_comment(const char* query_param, nim_talkex_quick_comment_query_quick_comment_cb_func cb, const void* user_data)
    * 查询快捷回复
    * @param[in] query_param	查询参数，一次最多只能查询20条消息的快捷回复
    * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_quick_comment.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_quick_comment_query_quick_comment(const char* query_param, nim_talkex_quick_comment_query_quick_comment_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_quick_comment_reg_add_quick_comment(nim_talkex_quick_comment_add_quick_comment_notify_cb_func cb, const void* user_data)
    * 注册添加快捷回复通知回调
    * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_quick_comment.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_quick_comment_reg_add_quick_comment(nim_talkex_quick_comment_add_quick_comment_notify_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_quick_comment_reg_remove_quick_comment(nim_talkex_quick_comment_remove_quick_comment_notify_cb_func cb, const void* user_data)
   * 注册添加快捷回复通知回调
   * @param[in] cb	查询结果回调函数定义 详见 nim_talkex_def_quick_comment.h
   * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
   * @return void 无返回值
   */
    NIM_SDK_DLL_API void nim_talkex_quick_comment_reg_remove_quick_comment(nim_talkex_quick_comment_remove_quick_comment_notify_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_pin_message_add_pin_message(const char* msg, const char* pin_info, nim_talkex_pin_message_add_pin_message_cb_func cb, const void* user_data)
    * Pin某条消息
	* @param[in] msg	要Pin的消息
	* @param[in] pin_info	Pin的内容 只需赋值 ext参数,其它参数SDK来补充
	* @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
  */
    NIM_SDK_DLL_API void nim_talkex_pin_message_add_pin_message(const char* msg, const char* pin_info, nim_talkex_pin_message_add_pin_message_cb_func cb, const void* user_data);
    
    /** @fn void nim_talkex_pin_message_unpin_message(const char* modify_param, nim_talkex_pin_message_unpin_message_cb_func cb, const void* user_data)
    * UnPin某条消息
    * @param[in] modify_param	UnPin Message 参数
    * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_pin_message_unpin_message(const char* modify_param, nim_talkex_pin_message_unpin_message_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_pin_message_update_pin_message(const char* modify_param, nim_talkex_pin_message_update_pin_message_cb_func cb, const void* user_data)
    * 更新 Pin Message ext字段
    * @param[in] modify_param	更新 Pin Message 参数
    * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_pin_message_update_pin_message(const char* modify_param, nim_talkex_pin_message_update_pin_message_cb_func cb, const void* user_data);
    
    /** @fn void nim_talkex_pin_message_query_all_pin_message(const char* session, int to_type, const nim_talkex_pin_message_query_all_pin_message_cb_func cb, const void* user_data)
    * 查询会话的所有 Pin Message
    * @param[in] session	会话ID
    * @param[in] to_type	会话类型
    * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_pin_message_query_all_pin_message(const char* session, int to_type, const nim_talkex_pin_message_query_all_pin_message_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_pin_message_reg_add_pin_message(nim_talkex_pin_message_add_pin_message_notify_cb_func cb, const void* user_data)
   * 注册添加 Pin Message 通知回调
   * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
   * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
   * @return void 无返回值
   */
    NIM_SDK_DLL_API void nim_talkex_pin_message_reg_add_pin_message(nim_talkex_pin_message_add_pin_message_notify_cb_func cb, const void* user_data);

    /** @fn void nim_talkex_pin_message_reg_unpin_message(nim_talkex_pin_message_unpin_message_notify_cb_func cb, const void* user_data)
    * 注册添加 UnPin Message 通知回调
    * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_pin_message_reg_unpin_message(nim_talkex_pin_message_unpin_message_notify_cb_func cb, const void* user_data);
    
    /** @fn void nim_talkex_pin_message_reg_update_pin_message(nim_talkex_pin_message_update_pin_message_notify_cb_func cb, const void* user_data)
    * 注册添加 Pin Message 通知回调
    * @param[in] cb		执行结果回调函数 详见 nim_talkex_def_pin_message.h
    * @param[in] user_data  APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
    * @return void 无返回值
    */
    NIM_SDK_DLL_API void nim_talkex_pin_message_reg_update_pin_message(nim_talkex_pin_message_update_pin_message_notify_cb_func cb, const void* user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_API_NIM_TALKEX_H_

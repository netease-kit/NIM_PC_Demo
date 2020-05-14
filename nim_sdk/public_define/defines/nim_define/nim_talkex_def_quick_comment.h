/** @file nim_talkex_def_quick_comment.h
  * @brief 快捷回复接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2020/04/19
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_QUICK_COMMENT_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_QUICK_COMMENT_H_

#include "public_define/nim_util_include.h"

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_talkex_quick_comment_add_quick_comment_cb_func)(int code, const char *quick_comment_info, const void *user_data)
  * 添加快捷回复回调函数定义
  * @param[out] code	执行结果
  * @param[out] quick_comment_info	添加快捷回复成功后会返回收藏的内容
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_quick_comment_add_quick_comment_cb_func)(int code, const char *quick_comment_info, const void *user_data);

/** @typedef void (*nim_talkex_quick_comment_remove_quick_comment_cb_func)(int code, const char *id, const void *user_data)
  * 删除快捷回复回调函数定义
  * @param[out] code	执行结果
  * @param[out] id	被删除的快捷回复ID
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_quick_comment_remove_quick_comment_cb_func)(int code, const char* id, const void* user_data);

/** @typedef void (*nim_talkex_quick_comment_query_quick_comment_cb_func)(int code, const char *response, const void *user_data)
  * 删除快捷回复回调函数定义
  * @param[out] code	执行结果
  * @param[out] response	查询结果 
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_quick_comment_query_quick_comment_cb_func)(int code, const char *response, const void *user_data);

/** @typedef void (*nim_talkex_quick_comment_add_quick_comment_notify_cb_func)(const char *session, int type, const char *msg_client_id, const char *info, const void *user_data)
  * 添加快捷通知回调函数定义
  * @param[out] session	查询消息属于哪个会话ID
  * @param[out] type	消息会话类型
  * @param[out] msg_client_id	消息ID
   * @param[out] info	回复的内容
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_quick_comment_add_quick_comment_notify_cb_func)(const char *session, int type, const char *msg_client_id, const char *info, const void *user_data);

/** @typedef void (*nim_talkex_quick_comment_remove_quick_comment_notify_cb_func)(const char *session, int type, const char *msg_client_id, const char *id, const char *ext, const void *user_data)
  * 添加快捷通知回调函数定义
  * @param[out] session	查询消息属于哪个会话ID
  * @param[out] type	消息会话类型
  * @param[out] msg_client_id	消息ID
  * @param[out] id	被删除的快捷回复id
  * @param[out] ext 扩展字段
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_talkex_quick_comment_remove_quick_comment_notify_cb_func)(const char *session, int type, const char *msg_client_id, const char *id, const char *ext, const void *user_data);


/** @name 快捷回复Json Keys
  * @{
  */

static const char *kNIMQuickCommentInfoKeyID	= "id"; /**< string 服务端没有这个字段，本地为了好处理记录加上的(message serverid + from_accid + type)*/
static const char *kNIMQuickCommentInfoKeyFromAccID = "from_account"; /**< string回复者的 accid */
static const char *kNIMQuickCommentInfoKeyReplyType = "reply_type"; /**< int64_t 开发者自定义的回复类型 不可使用0*/
static const char *kNIMQuickCommentInfoKeyClinetID = "client_id"; /**< string 被回复消息的客户端ID*/
static const char *kNIMQuickCommentInfoKeyServerID = "server_id";/**< uint64_t被回复消息的服务端ID*/
static const char *kNIMQuickCommentInfoKeyTime = "time";/**< uint64_t 回复的时间戳*/
static const char *kNIMQuickCommentInfoKeyExt = "ext";/**< string 自定义扩展字段，最大32字符*/
static const char *kNIMQuickCommentInfoKeyNeedPush = "need_push";/**< int 是否需要推送，0表示不需要，1表示需要，默认0*/
static const char *kNIMQuickCommentInfoKeyNeedBadge = "need_badge";/**<  int 是否需要角标，0表示不需要，1表示需要，默认0*/
static const char *kNIMQuickCommentInfoKeyPushTitle = "push_title";/**<  string 推送标题*/
static const char *kNIMQuickCommentInfoKeyPushContent = "push_content";/**<  string 推送文案*/
static const char *kNIMQuickCommentInfoKeyPushPayload = "push_payload";/**<  string 推送自定义字段JSON*/

/** @}*/ //快捷回复Json Keys

/** @name 删除快捷回复参数Json Keys
  * @{
  */
static const char *kNIMQuickCommentRemoveParamKeyID=  "id"; /**< string 服务端没有这个字段，本地为了好处理记录加上的*/
static const char *kNIMQuickCommentRemoveParamKeyReplyType = "reply_type";	/**< int64_t 开发者自定义的回复类型*/
static const char *kNIMQuickCommentRemoveParamKeyExt = "ext";	/**< string 取消操作的扩展字段*/
/** @}*/ //删除快捷回复参数Json Keys

/** @name 查询快捷回复应答参数Json Keys
  * @{
  */
static const char* kNIMQuickCommentQueryRSPKeyMsgClientID = "client_id"; /**< string 消息ID*/
static const char* kNIMQuickCommentQueryRSPKeyInfoList = "info_list";	/**< list 快捷回复列表*/
/** @}*/ //查询快捷回复应答参数Json Keys

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_TALKEX_DEF_QUICK_COMMENT_H_
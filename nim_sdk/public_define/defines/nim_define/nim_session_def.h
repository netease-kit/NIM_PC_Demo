/** @file nim_session_def.h
  * @brief 最近会话列表 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_SESSION_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_SESSION_DEF_H_

#include "public_define/nim_util_include.h"
#ifdef __cplusplus
extern"C"
{
#endif

/** @enum NIMSessionType 会话类型 */
enum NIMSessionType
{ 
    kNIMSessionTypeP2P            = 0,            /**< 个人，即点对点 */
    kNIMSessionTypeTeam            = 1,            /**< 群组 */
    kNIMSessionTypeSuperTeam    = 5,            /**< 超大群组 */
};

/** @enum NIMSessionCommand 会话操作命令 */
enum NIMSessionCommand
{
    kNIMSessionCommandAdd                = 0,    /**< 添加会话项 */
    kNIMSessionCommandRemove            = 1,    /**< 删除单个会话项 */
    kNIMSessionCommandRemoveAll            = 2,    /**< 删除所有会话项 */
    kNIMSessionCommandRemoveAllP2P        = 3,    /**< 删除所有点对点的会话项 */
    kNIMSessionCommandRemoveAllTeam        = 4,    /**< 删除所有群的会话项 */
    kNIMSessionCommandMsgDeleted        = 5,    /**< 单个会话项的消息已删除 */
    kNIMSessionCommandAllMsgDeleted        = 6,    /**< 所有会话项的消息已删除 */
    kNIMSessionCommandAllP2PMsgDeleted    = 7,    /**< 所有点对点会话项的消息已删除 */
    kNIMSessionCommandAllTeamMsgDeleted    = 8,    /**< 所有群会话项的消息已删除 */
    kNIMSessionCommandUpdate            = 9,    /**< 更新会话项 */
    kNIMSessionCommandRemoveAllSuperTeam = 10,    /**< 删除所有超大群的会话项 */
    kNIMSessionCommandAllSuperTeamMsgDeleted = 11,    /**< 所有超大群会话项的消息已删除 */

};

typedef struct
{
    char session_id[64];
    enum NIMSessionType to_type;
}SessionMainTagInfo;

typedef struct
{
    SessionMainTagInfo session_tag_info;
    uint64_t message_time_tag;
    uint64_t message_server_id;
}SessionRoamMsgHasMoreTagInfo;


/** @typedef void (*nim_session_change_cb_func)(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data)
  * 最近会话项更新通知的回调函数定义
  * @param[out] rescode		操作结果，成功200
  * @param[out] result			最近会话Json string (『会话列表的Json Keys』 as follows 不包含消息的字段)
  * @param[out] total_unread_counts 总的未读数目
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_session_change_cb_func)(int rescode, const char *result, int total_unread_counts, const char *json_extension, const void *user_data);

/** @typedef void (*nim_session_delete_session_roaming_cb_func)(int rescode, const char *to_type, const char *id, const void *user_data)
  * 最近会话项更新通知的回调函数定义
  * @param[out] rescode		操作结果，成功200
  * @param[out] to_type			会话类型
  * @param[out] id 对方的account id或者群组tid
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_session_delete_session_roaming_cb_func)(int rescode, const char *to_type, const char *id, const void *user_data);

/** @typedef void (*nim_session_query_recent_session_cb_func)(int total_unread_count, const char *result, const char *json_extension, const void *user_data)
  * 查询所有最近会话项的回调函数定义
  * @param[out] total_unread_counts 总的未读数目
  * @param[out] result			最近会话列表Json string (『会话列表的Json Keys』 as follows)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_session_query_recent_session_cb_func)(int total_unread_count, const char *result, const char *json_extension, const void *user_data);

/** @typedef void(*nim_session_query_sessiondata_by_id_cb_func)(int rescode,const char *result, const void *user_data)
  * 查询所有最近会话项的回调函数定义
  * @param[out] rescode		操作结果，成功200
  * @param[out] result			会话信息 (『会话信息的Json Keys』 as follows)
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void(*nim_session_query_sessiondata_by_id_cb_func)(int rescode,const char *result, const void *user_data);

/** @typedef int(*nim_session_badge_count_cb_func)(const char *json_extension, const void *user_data)
* 获取badge回调（仅支持iOS）
* @param[out] json_extension    json扩展数据（备用）
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return int badge数量
*/
typedef int (*nim_session_badge_count_cb_func)(const char *json_extension, const void *user_data);

/** @typedef void (*nim_session_query_stick_top_session_list_cb_func)(int rescode, const char *result,const void *user_data)
* 获取置顶会话列表回调
* @param[out] rescode		操作结果，成功200
* @param[out] result			置顶会话信息列表　jsonarray
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_query_stick_top_session_list_cb_func)(int rescode, const char *result,const void *user_data);

/** @typedef void (*nim_session_set_to_stick_top_cb_func)(int rescode, const char *result, const void *user_data)
* 会话置顶回调
* @param[out] rescode		操作结果，成功200
* @param[out] stick_session			置顶会话信息
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_set_to_stick_top_cb_func)(int rescode, const char *stick_session, const void *user_data);

/** @typedef void (*nim_session_cancel_stick_top_cb_func)(int rescode, const char *session_id, enum NIMSessionType to_type,const void *user_data)
* 取消会话置顶回调
* @param[out] rescode		操作结果，成功200
* @param[out] session_id		会话id
* @param[out] to_type			会话类型
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_cancel_stick_top_cb_func)(int rescode, const char *session_id, enum NIMSessionType to_type,const void *user_data);

/** @typedef void (*nim_session_update_stick_top_cb_func)(int rescode, const char *stick_session, const void *user_data)
* 更新会话置顶ext字段回调
* @param[out] rescode		操作结果，成功200
* @param[out] stick_session			置顶会话信息
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_update_stick_top_cb_func)(int rescode, const char *stick_session, const void *user_data);

/** @typedef void (*nim_session_set_to_stick_top_notify_cb_func)(const char *stick_session, const void *user_data)
* 会话置顶通知回调
* @param[out] stick_session			置顶会话信息
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_set_to_stick_top_notify_cb_func)(const char *stick_session, const void *user_data);

/** @typedef void (*nim_session_cancel_stick_top_notify_cb_func)(const char *session_id, enum NIMSessionType to_type, const void *user_data)
* 取消会话置顶通知回调
* @param[out] session_id			被取消息的会话id
* @param[out] to_type			会话类型
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_cancel_stick_top_notify_cb_func)(const char *session_id, enum NIMSessionType to_type, const void *user_data);

/** @typedef void (*nim_session_update_stick_top_notify_cb_func)(const char *stick_session, const void *user_data)
* 更新置顶会话信息通知回调
* @param[out] stick_session			置顶会话信息
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void (*nim_session_update_stick_top_notify_cb_func)(const char *stick_session, const void *user_data);

/** @name 会话列表的Json Keys
  * @{
  */

static const char *kNIMSessionListCount		= "count";				/**< int, 会话列表项数量 */
static const char *kNIMSessionListUnreadCount = "unread_counts";	/**< int, 总未读数 */
static const char *kNIMSessionListContent	= "content";			/**< json object (Keys as follows), 会话列表项内容 */
static const char *kNIMSessionId			= "id";					/**< string, 会话id */
static const char *kNIMSessionType			= "type";				/**< int, 会话类型(见NIMSessionType 个人0 群组1 超大群5 etc) */
static const char *kNIMSessionUnreadCount	= "unread_count";		/**< int, 本地消息历史中该会话未读消息数量 */
static const char *kNIMSessionCommand		= "command";			/**< int, 会话项操作命令NIMSessionCommand */
static const char *kNIMSessionMsgClientID	= "msg_client_id";		/**< string, 本地消息历史中该会话最后一条消息的客户端ID唯一标识，即消息uuid */
static const char *kNIMSessionMsgFromAccount= "msg_from_account";	/**< string, 本地消息历史中该会话最后一条消息的发送方帐号 */
static const char *kNIMSessionMsgTime		= "msg_time";			/**< long, 本地消息历史中该会话最后一条消息的时间 */
static const char *kNIMSessionMsgType		= "msg_type";			/**< int, 本地消息历史中该会话最后一条消息的类型 见NIMMessageType*/
static const char *kNIMSessionMsgBody		= "msg_body";			/**< string, 本地消息历史中该会话最后一条消息的内容 */
static const char *kNIMSessionMsgAttach		= "msg_attach";			/**< string, 本地消息历史中该会话最后一条消息的attach */
static const char *kNIMSessionMsgStatus		= "msg_status";			/**< int, 本地消息历史中该会话最后一条消息的状态，见NIMMsgLogStatus */
static const char *kNIMSessionMsgSubStatus	= "msg_sub_status";		/**< int, 本地消息历史中该会话最后一条消息的子状态，见NIMMsgLogSubStatus */
static const char *kNIMSessionLastUpdatedMsg= "last_updated_msg";	/**< bool, (批量)消息变更或增加时是否是最后一条变更的信息 */
static const char *kNIMSessionOnTop			= "top";				/**< bool, 是否置顶 */
static const char *kNIMSessionExtendedData	= "extend_data";		/**< string, 本地扩展字段, 限4096 */
static const char *kNIMSessionRobotFlag		= "robot_session";		/**< bool, 是否为机器人会话, 默认为false */
static const char* kNIMSessionStickTopInfo = "stick_top_info";		/**< json object, 置顶信息 (v7.6.0添加),该置顶信息会进行多端同步,不建议再使用 kNIMSessionOnTop*/
/** @}*/ //会话列表的Json Keys

/** @name 会话漫游消息未完待取的Json Keys(v7.6.0添加)
  * @{
  */
static const char* kNIMSessionRoamMsgHasMoreId = "id";					/**< string, 会话id */
static const char* kNIMSessionRoamMsgHasMoreType = "type";				/**< int, 会话类型(见NIMSessionType 个人0 群组1 超大群5 etc) */
static const char* kNIMSessionRoamMsgHasMoreMsgServerID = "msg_server_id";		/**< string, 界定消息的服务端ID */
static const char* kNIMSessionRoamMsgHasMoreMsgTime = "msg_time";			/**< uint64_t, 最早一条漫游消息的时间戳 */
/** @}*/ //会话漫游消息未完待取的Json Keys

/** @name 置顶会话Json Keys(v7.6.0添加)
  * @{
  */
static const char *kNIMSessionStickTopInfoTop = "top";				/**< bool, 是否置顶 */
static const char* kNIMSessionStickTopInfoSessionId = "id";					/**< string, 会话id */
static const char* kNIMSessionStickTopInfoType = "type";				/**< int, 会话类型(见NIMSessionType 个人0 群组1 超大群5 etc) */
static const char* kNIMSessionStickTopInfoExt = "ext";		/**< string,  扩展字段*/
static const char* kNIMSessionStickTopInfoCreateTime = "create_time";			/**< uint64_t 设置置顶的时间戳 */
static const char* kNIMSessionStickTopInfoUpdateTime = "update_time";			/**< uint64_t 更新置顶的时间戳 */
static const char* kNIMSessionStickTopSessionData = "session_data";			/**< json object 会话信息 */
/** @}*/ //置顶会话Json Keys

/** @typedef void(*nim_session_query_session_hasmore_roammsg_cb_func)(int rescode, const SessionRoamMsgHasMoreTagInfo *tag_info, const void *user_data)
* 查询会话漫游消息未完待拉信息结果回调
* @param[out] rescode		操作结果，成功200
* @param[out] tag_info	会话漫游消息未完待拉信息
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void(*nim_session_query_session_hasmore_roammsg_cb_func)(int rescode, const SessionRoamMsgHasMoreTagInfo *tag_info, const void *user_data);

/** @typedef void(*nim_session_query_all_session_hasmore_roammsg_cb_func)(int rescode, const SessionRoamMsgHasMoreTagInfo *tag_info_list, int size, const void *user_data)
* 查询所有漫游消息未完待拉会话结果回调
* @param[out] rescode		操作结果，成功200
* @param[out] tag_info_list	会话漫游消息未完待拉信息头指针，与size一起配合使用例如 for(int index = 0;index < size;index++ )tag_info_list++;
* @param[out] size	 结果列表的长度，
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void(*nim_session_query_all_session_hasmore_roammsg_cb_func)(int rescode, const SessionRoamMsgHasMoreTagInfo *tag_info_list, int size, const void *user_data);

/** @typedef void(*nim_session_update_session_hasmore_roammsg_cb_func)(int rescode)
* 更新会话漫游消息未完待拉信息结果回调
* @param[out] rescode		操作结果，成功200
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void(*nim_session_update_session_hasmore_roammsg_cb_func)(int rescode, const void* user_data);

/** @typedef void(*nim_session_delete_session_hasmore_roammsg_cb_func)(int rescode)
* 删除会话漫游消息未完待拉信息结果回调
* @param[out] rescode		操作结果，成功200
* @param[out] user_data        APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
* @return void
*/
typedef void(*nim_session_delete_session_hasmore_roammsg_cb_func)(int rescode, const void* user_data);
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_SESSION_DEF_H_

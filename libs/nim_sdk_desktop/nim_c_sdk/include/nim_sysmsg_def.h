/** @file nim_sysmsg_def.h
  * @brief system msg define
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/2/1
  */
#ifndef NIM_SDK_DLL_EXPORT_HEADERS_SYSMSG_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_SYSMSG_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif
/** @typedef void (*nim_custom_sysmsg_arc_cb_func)(const char *result, const void *user_data)
  * 透传系统消息回执的回调函数定义
  * @param[out] result json string (Keys SEE MORE 『发送透传的系统消息回执Json Keys』 as follows)
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_custom_sysmsg_arc_cb_func)(const char *result, const void *user_data);

/** @typedef void (*nim_sysmsg_receive_cb_func)(const char *content, const char *json_extension, const void *user_data)
  * 接收系统消息的回调函数定义
  * @param[out] content			json string (Keys SEE MORE 『接收系统消息Json Keys』 as follows)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_sysmsg_receive_cb_func)(const char *content, const char *json_extension, const void *user_data);

/** @typedef void (*nim_sysmsg_query_cb_func)(int count, const char *result, const char *json_extension, const void *user_data)
  * 查询系统消息的回调函数定义
  * @param[out] count			查询结果的实际数量
  * @param[out] result			查询结果json object array string SEE MORE 『查询系统消息历史结果Json Keys』
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_sysmsg_query_cb_func)(int count, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_sysmsg_res_cb_func)(int res_code, int unread_count, const char *json_extension, const void *user_data)
  * 系统消息历史操作结果的回调函数定义
  * @param[out] res_code		操作结果，成功200
  * @param[out] unread_count	总的未读数
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_sysmsg_res_cb_func)(int res_code, int unread_count, const char *json_extension, const void *user_data);

/** @typedef void (*nim_sysmsg_res_ex_cb_func)(int res_code, __int64 msg_id, int unread_count, const char *json_extension, const void *user_data)
  * 系统消息历史操作结果的回调函数定义
  * @param[out] res_code	操作结果，成功200
  * @param[out] msg_id		消息id
  * @param[out] unread_count	总的未读数
  * @param[out] json_extension json扩展数据（备用）
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_sysmsg_res_ex_cb_func)(int res_code, __int64 msg_id, int unread_count, const char *json_extension, const void *user_data);

/** @name 查询系统消息历史结果Json Keys
  * for example: 
  * {"count": 20,
  *  "unread_count": 20,
  *  "content":[***]
  * }
  * @{
  */
static const char *kNIMSysMsglogQueryKeyCount	= "count";				/**< int, 查询得到的数量 */
static const char *kNIMSysMsglogQueryKeyUnreadCount	= "unread_count";	/**< int, 每次查询带回的是当前的总的未读数 */
static const char *kNIMSysMsglogQueryKeyContent = "content";			/**< json object array 『系统消息结构 Json Keys』 as follows，查询得到的sysmsglog内容 */
/** @}*/ //查询系统消息历史结果Json Keys

/** @name 发送透传的系统消息回执Json Keys
  * for example: 
  * {"msg_id": "123456",
  *  "talk_id": "abc",
  *  "rescode":200
  * }
  * @{
  */
static const char *kNIMSendSysMsgArcKeyMsgId		= "msg_id";				/**< string, msg id */
static const char *kNIMSendSysMsgArcKeyTalkId		= "talk_id";			/**< string, talk id */
static const char *kNIMSendSysMsgArcKeyRescode		= "rescode";			/**< int, 返回的错误码，见NIMResCode */
/** @}*/ //发送透传的系统消息回执Json Keys

/** @name 接收系统消息Json Keys，本地定义
  * for example: 
  * {"rescode": 200
  *  "feature": 1,
  *  "unread_count": 20,
  *  "content": "***",
  * }
  * @{
  */
static const char *kNIMSysMsgKeyLocalRescode	= "rescode";			/**< int, 返回的错误码，见NIMResCode */ 
static const char *kNIMSysMsgKeyLocalFeature	= "feature";			/**< int, Sysmsg Feature，见NIMMessageFeature */ 
static const char *kNIMSysMsgKeyLocalUnreadCount= "unread_count";		/**< int, 每次带回的是当前的总的未读数 */ 
static const char *kNIMSysMsgKeyLocalContent	= "content";			/**< json string, 协议定义的消息结构 */ 
/** @}*/ //接收系统消息Json Keys，本地定义

/** @name 系统消息结构 Json Keys，协议定义
  * @{
  */
static const char *kNIMSysMsgKeyTime			= "msg_time";			/**< long,		时间戳(可选) */
static const char *kNIMSysMsgKeyType			= "msg_type";			/**< int,		通知类型 */
static const char *kNIMSysMsgKeyToAccount		= "to_account";			/**< string,	tid或accid */
static const char *kNIMSysMsgKeyFromAccount		= "from_account";		/**< string,	accid */
static const char *kNIMSysMsgKeyMsg				= "msg";				/**< string,	附言 */
static const char *kNIMSysMsgKeyAttach			= "attach";				/**< string,	附件 */
static const char *kNIMSysMsgKeyMsgId			= "msg_id";				/**< long,		msg id（自定义系统通知即透传消息，必须填0）*/
static const char *kNIMSysMsgKeyCustomSaveFlag	= "custom_save_flag";	/**< int,		透传消息是否存离线:0-不存（只发给在线用户）,1-存（可发给离线用户）*/
static const char *kNIMSysMsgKeyCustomApnsText	= "custom_apns_text";	/**< string,	透传消息推送文本，不填则不推送*/
static const char *kNIMSysMsgKeyLocalStatus		= "log_status";			/**< int,		系统消息状态，见NIMSysMsgStatus，本地定义*/
/** @}*/ //接收系统消息Json Keys，协议定义

/** @enum NIMSysMsgStatus 系统消息状态 */
enum NIMSysMsgStatus
{
	kNIMSysMsgStatusNone        = 0,		/**< 默认,未读 */
	kNIMSysMsgStatusPass		= 1,		/**< 收到消息,通过 */
	kNIMSysMsgStatusDecline		= 2,		/**< 收到消息,拒绝 */
	kNIMSysMsgStatusRead		= 3,		/**< 收到消息,已读 */
	kNIMSysMsgStatusDeleted		= 4,		/**< 已删 */
	kNIMSysMsgStatusInvalid		= 5,		/**< 已失效 */
};

/** @enum NIMSysMsgType 系统消息内容类型 */
enum NIMSysMsgType
{
	kNIMSysMsgTypeTeamApply			= 0,		/**< 申请入群  */
	kNIMSysMsgTypeTeamReject		= 1,		/**< 拒绝入群申请 */
	kNIMSysMsgTypeTeamInvite		= 2,		/**< 邀请进群 kNIMSysMsgKeyAttach : {"team_info":team_info} //群组信息(Keys SEE MORE `nim_team_def.h` 『群组信息 Json Keys』) */
	kNIMSysMsgTypeTeamInviteReject	= 3,		/**< 拒绝邀请 */
	kNIMSysMsgTypeFriendAdd			= 5,		/**< 加好友, attach: {"vt":verifyType} */
	kNIMSysMsgTypeFriendDel			= 6,		/**< 删除好友 */
	kNIMSysMsgTypeCustomP2PMsg		= 100,		/**< 点对点透传消息（透传消息的内容放到kNIMSysMsgKeyAttach） */
	kNIMSysMsgTypeCustomTeamMsg		= 101,		/**< 群透传消息（透传消息的内容放到kNIMSysMsgKeyAttach） */
	kNIMSysMsgTypeUnknown			= 1000,		/**< 未知类型，作为默认 */
};

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_SYSMSG_DEF_H_
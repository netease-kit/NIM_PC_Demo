/** @file nim_chatroom_def.h
  * @brief NIM 聊天室 SDK 定义
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/12/24
  */
#ifndef NIM_CHATROOM_SDK_DLL_EXPORT_HEADERS_DEF_H_
#define NIM_CHATROOM_SDK_DLL_EXPORT_HEADERS_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif

/** @typedef void (*nim_chatroom_enter_cb_func)(__int64 room_id, int enter_step, int error_code, const char *result, const void *user_data)
  * 进入的回调函数定义
  * @param[out] room_id		聊天室ID
  * @param[out] enter_step	进入聊天室的过程(NIMChatRoomEnterStep)
  * @param[out] error_code	错误码
  * @param[out] result		聊天室信息
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_enter_cb_func)(__int64 room_id, int enter_step, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_exit_cb_func)(__int64 room_id, int error_code, int exit_type, const char *json_extension, const void *user_data)
  * 退出、被踢的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] exit_type		退出原因(ChatRoomExitReason)
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_exit_cb_func)(__int64 room_id, int error_code, int exit_type, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_link_condition_cb_func)(__int64 room_id, int condition, const void *user_data)
  * 聊天室链接状况的回调函数定义
  * @param[out] room_id		聊天室ID
  * @param[out] condition	网络状况
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_link_condition_cb_func)(__int64 room_id, int condition, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_sendmsg_arc_cb_func)(__int64 room_id, int error_code, const char *result, const void *user_data)
  * 发送消息回执的回调函数定义
  * @param[out] room_id		聊天室ID
  * @param[out] error_code	错误码
  * @param[out] result		json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data	APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_sendmsg_arc_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_receive_msg_cb_func)(__int64 room_id, const char *result, const char *json_extension, const void *user_data)
  * 接收消息的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_receive_msg_cb_func)(__int64 room_id, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_receive_notification_cb_func)(__int64 room_id, const char *result, const char *json_extension, const void *user_data)
  * 聊天室通知的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_receive_notification_cb_func)(__int64 room_id, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_get_members_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
  * 获取指定/分页获取成员列表的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_get_members_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_get_msg_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
  * 获取历史消息的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_get_msg_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_set_member_attribute_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
  * 设定聊天室成员标记身份的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_set_member_attribute_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_get_info_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data)
  * 获取当前聊天室信息的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] result			json string
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_get_info_cb_func)(__int64 room_id, int error_code, const char *result, const char *json_extension, const void *user_data);

/** @typedef void (*nim_chatroom_kick_member_cb_func)(__int64 room_id, int error_code, const char *json_extension, const void *user_data)
  * 踢掉指定成员的回调函数定义
  * @param[out] room_id			聊天室ID
  * @param[out] error_code		错误码
  * @param[out] json_extension	json扩展数据（备用）
  * @param[out] user_data		APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_chatroom_kick_member_cb_func)(__int64 room_id, int error_code, const char *json_extension, const void *user_data);

/** @name 进入聊天室的可选填信息Json Keys
* @{
*/
static const char *kNIMChatRoomEnterKeyNick		= "nick";		/**< string, 进入聊天室后展示的昵称,选填 */
static const char *kNIMChatRoomEnterKeyAvatar	= "avatar";		/**< string, 进入聊天室后展示的头像,选填 */
static const char *kNIMChatRoomEnterKeyExt		= "ext";		/**< string, 聊天室可用的扩展字段,必须为可以解析为Json的非格式化的字符串,选填 */
static const char *kNIMChatRoomEnterKeyNotifyExt= "notify_ext";	/**< string, 进入聊天室通知开发者扩展字段,必须为可以解析为Json的非格式化的字符串,选填*/
/** @}*/ //登录Json Keys


/** @name 消息结构 Json Keys
  * @{
  */
//协议定义
static const char *kNIMChatRoomMsgKeyType			= "msg_type";			/**< int, 消息类型(NIMChatRoomMsgType) */
static const char *kNIMChatRoomMsgKeyAttach			= "msg_attach";			/**< string, 消息内容,如果约定的是json字符串，必须为可以解析为json的非格式化的字符串 */
static const char *kNIMChatRoomMsgKeyClientMsgid	= "client_msg_id";		/**< string,客户端消息id */
static const char *kNIMChatRoomMsgKeyResendFlag		= "resend_flag";		/**< int,消息重发标记位,第一次发送0,重发1 */
static const char *kNIMChatRoomMsgKeyExt			= "ext";				/**< string, 第三方扩展字段, 长度限制4096, 必须为可以解析为Json的非格式化的字符串*/

//以下定义对于客户端只读
static const char *kNIMChatRoomMsgKeyRoomID			= "room_id";			/**< long, 消息所属的聊天室id,服务器填写,发送方不需要填写 */
static const char *kNIMChatRoomMsgKeyFromAccount	= "from_id";			/**< string, 消息发送者的账号,服务器填写,发送方不需要填写 */
static const char *kNIMChatRoomMsgKeyTime			= "time";				/**< long, 消息发送的时间戳(毫秒),服务器填写,发送方不需要填写 */
static const char *kNIMChatRoomMsgKeyFromClientType = "from_client_type";	/**< int,消息发送方客户端类型,服务器填写,发送方不需要填写 */
static const char *kNIMChatRoomMsgKeyFromNick		= "from_nick";			/**< string, 消息发送方昵称,服务器填写,发送方不需要填写*/
static const char *kNIMChatRoomMsgKeyFromAvatar		= "from_avator";		/**< string, 消息发送方头像,服务器填写,发送方不需要填写*/
static const char *kNIMChatRoomMsgKeyFromExt		= "from_ext";			/**< string, 消息发送方身份扩展字段,服务器填写,发送方不需要填写*/

//本地定义
static const char *kNIMChatRoomMsgKeyLocalFilePath	= "local_res_path";		/**< string, 暂时不用,多媒体消息资源本地绝对路径,SDK本地维护,发送多媒体消息时必填 */
static const char *kNIMChatRoomMsgKeyLocalResId		= "res_id";				/**< string, 暂时不用,多媒体资源id,发送方选填,接收方收到的是客户端消息id */
/** @}*/ //消息结构 Json Keys

/** @enum NIMChatRoomMsgType 聊天室消息类型*/
enum NIMChatRoomMsgType
{
	kNIMChatRoomMsgTypeText = 0,		/**< 文本类型消息*/
	kNIMChatRoomMsgTypeImage = 1,		/**< 图片类型消息*/
	kNIMChatRoomMsgTypeAudio = 2,		/**< 声音类型消息*/
	kNIMChatRoomMsgTypeVideo = 3,		/**< 视频类型消息*/
	kNIMChatRoomMsgTypeLocation = 4,	/**< 位置类型消息*/
	kNIMChatRoomMsgTypeNotification = 5,/**< 活动室通知*/
	kNIMChatRoomMsgTypeFile = 6,		/**< 文件类型消息*/

	kNIMChatRoomMsgTypeTips = 10,		/**< 提醒类型消息*/
	kNIMChatRoomMsgTypeCustom = 100,	/**< 自定义消息*/

	kNIMChatRoomMsgTypeUnknown = 1000,	/**< 未知类型消息，作为默认值*/
};

/** @name 进入聊天室回调结果Json Keys
  * @{
  */
static const char *kNIMChatRoomEnterCallbackKeyRoomInfo		= "room_info";		/**< json value, 见聊天室Info Json Keys */
static const char *kNIMChatRoomEnterCallbackKeyMyInfo		= "my_info";		/**< json value, 见聊天室个人Info Json Keys */

/** @}*/ //进入回调结果Json Keys

/** @name 聊天室Info Json Keys
  * @{
  */
static const char *kNIMChatRoomInfoKeyID			= "id";				/**< long, 聊天室ID */
static const char *kNIMChatRoomInfoKeyName			= "name";			/**< string, 聊天室名称 */
static const char *kNIMChatRoomInfoKeyAnnouncement	= "announcement";	/**< string, 聊天室公告 */
static const char *kNIMChatRoomInfoKeyBroadcastUrl	= "broadcast_url";	/**< string, 视频直播拉流地址 */
static const char *kNIMChatRoomInfoKeyCreatorID		= "creator_id";		/**< string, 聊天室创建者账号 */
static const char *kNIMChatRoomInfoKeyValidFlag		= "valid_flag";		/**< int, 聊天室有效标记, 1:有效,0:无效 */
static const char *kNIMChatRoomInfoKeyExt			= "ext";			/**< string, 第三方扩展字段, 必须为可以解析为Json的非格式化的字符串, 长度4k */
static const char *kNIMChatRoomInfoKeyOnlineCount	= "online_count";	/**< int, 当前在线用户数量 */
/** @}*/ //聊天室Info Json Keys

/** @enum NIMChatRoomExitReason 聊天室离开原因 */
enum NIMChatRoomExitReason
{
	kNIMChatRoomExitReasonExit					= 0,	/**< 自行退出,重登前需要重新请求进入 */
	kNIMChatRoomExitReasonRoomInvalid			= 1,	/**< 聊天室已经被解散,重登前需要重新请求进入 */
	kNIMChatRoomExitReasonKickByManager			= 2,	/**< 被管理员踢出,重登前需要重新请求进入 */
	kNIMChatRoomExitReasonKickByMultiSpot		= 3,	/**< 多端被踢 */
	kNIMChatRoomExitReasonIllegalState			= 4,	/**< 当前链接状态异常 */
	kNIMChatRoomExitReasonBeBlacklisted			= 5,	/**< 被加黑了 */
};

/** @name 聊天室个人Info Json Keys
  * @{
  */
static const char *kNIMChatRoomMemberInfoKeyRoomID		= "room_id";	/**<long 聊天室id */
static const char *kNIMChatRoomMemberInfoKeyAccID		= "account_id"; /**<string 成员账号 */
static const char *kNIMChatRoomMemberInfoKeyType		= "type";		/**<int 成员类型, -1:受限用户; 0:普通;1:创建者;2:管理员*/
static const char *kNIMChatRoomMemberInfoKeyLevel		= "level";		/**<int 成员级别: >=0表示用户开发者可以自定义的级别*/
static const char *kNIMChatRoomMemberInfoKeyNick		= "nick";		/**<string 聊天室内的昵称字段,预留字段, 可从Uinfo中取 */
static const char *kNIMChatRoomMemberInfoKeyAvatar		= "avatar";		/**<string 聊天室内的头像,预留字段, 可从Uinfo中取icon */
static const char *kNIMChatRoomMemberInfoKeyExt			= "ext";		/**<string 开发者扩展字段, 长度限制2k, 必须为可以解析为Json的非格式化的字符串*/
//以下字段即时生成,仅在在线状态中存储
static const char *kNIMChatRoomMemberInfoKeyOnlineState	= "online_state";	/**<NIMChatRoomOnlineState 成员是否处于在线状态, 仅特殊成员才可能离线, 对游客/匿名用户而言只能是在线*/
static const char *kNIMChatRoomMemberInfoKeyGuestFlag	= "guest_flag";		/**<NIMChatRoomGuestFlag 是否是普通游客类型,0:不是游客,1:是游客; 游客身份在聊天室中没有持久化, 只有在线时才会有内存状态*/
static const char *kNIMChatRoomMemberInfoKeyEnterTimetag= "enter_timetag";	/**<long 进入聊天室的时间点,对于离线成员该字段为空*/
//以下字段仅在返回前端时使用
static const char *kNIMChatRoomMemberInfoKeyBlacklist	= "is_blacklist";	/**<int 是黑名单*/
static const char *kNIMChatRoomMemberInfoKeyMuted		= "is_muted";		/**<int 是禁言用户*/
static const char *kNIMChatRoomMemberInfoKeyValid		= "valid";			/**<int 记录有效标记位*/
static const char *kNIMChatRoomMemberInfoKeyUpdateTimetag= "update_timetag";/**<long 固定成员的记录更新时间,用于固定成员列表的排列查询*/
/** @}*/ //聊天室个人Info Json Keys

/** @enum NIMChatRoomOnlineState 在线状态 */
enum NIMChatRoomOnlineState
{
	kNIMChatRoomOnlineStateOffline	= 0, /**<不在线 */
	kNIMChatRoomOnlineStateOnline	= 1, /**<在线 */
};

/** @enum NIMChatRoomGuestFlag 成员类型 */
enum NIMChatRoomGuestFlag
{
	kNIMChatRoomGuestFlagNoGuest	= 0, /**< 非游客 */
	kNIMChatRoomGuestFlagGuest		= 1, /**< 游客 */
};

/** @enum NIMChatRoomGetMemberType 查询成员的成员类型 */
enum NIMChatRoomGetMemberType
{
	kNIMChatRoomGetMemberTypeSolid	= 0, /**< 固定成员,固定成员,包括创建者,管理员,普通等级用户,受限用户(禁言+黑名单),即使非在线也可以在列表中看到,有数量限制*/
	kNIMChatRoomGetMemberTypeTemp	= 1, /**< 非固定成员,非固定成员,又称临时成员,只有在线时才能在列表中看到,数量无上限*/
};

/** @enum NIMChatRoomMemberAttribute 设置成员的成员身份 */
enum NIMChatRoomMemberAttribute
{
	kNIMChatRoomMemberAttributeAdminister	= 1, /**< 管理员,operator必须是创建者 */
	kNIMChatRoomMemberAttributeNomalMember	= 2, /**< 普通成员,operator必须是创建者或管理员 */
	kNIMChatRoomMemberAttributeBlackList	= -1, /**< 黑名单,operator必须是创建者或管理员 */
	kNIMChatRoomMemberAttributeMuteList		= -2, /**< 禁言,operator必须是创建者或管理员 */
};

/** @name 分页获取成员列表条件Keys
  * @{
  */
static const char *kNIMChatRoomGetMembersKeyType	= "type";	/**<NIMChatRoomGetMemberType 成员类型 */
static const char *kNIMChatRoomGetMembersKeyOffset	= "offset"; /**<long 时间戳,填0会使用当前服务器最新时间开始查询，即第一页  */
static const char *kNIMChatRoomGetMembersKeyLimit	= "limit";	/**<int 数量*/
/** @}*/ //分页获取成员列表条件Keys

/** @name 分获取历史消息条件Keys
  * @{
  */
static const char *kNIMChatRoomGetMsgHistoryKeyStartTime = "start";	/**<long 开始时间,单位毫秒 */
static const char *kNIMChatRoomGetMsgHistoryKeyLimit	 = "limit";	/**<int 本次返回的消息数量*/
/** @}*/ //分获取历史消息条件Keys

/** @name 设定聊天室成员标记身份条件Keys
  * @{
  */
static const char *kNIMChatRoomSetMemberAttributeKeyAccoutID	= "account_id";	/**<string 成员ID */
static const char *kNIMChatRoomSetMemberAttributeKeyAttribute	= "attribute";	/**<NIMChatRoomMemberAttribute */
static const char *kNIMChatRoomSetMemberAttributeKeyOpt			= "opt";		/**<boolean: true:是,false:否*/
static const char *kNIMChatRoomSetMemberAttributeKeyNotifyExt	= "notify_ext";	/**<string 自定义的事件通知扩展字段, 必须为可以解析为Json的非格式化的字符串*/
/** @}*/ //设定聊天室成员标记身份条件Keys

/** @name 聊天室通知Keys
  * @{
  */
static const char *kChatRoomNotificationKeyData		=	"data";			/**<string 通知内容*/
static const char *kChatRoomNotificationKeyID		=	"id";			/**<string 通知类型ID */
static const char *kChatRoomNotificationDataKeyExt	=	"ext";			/**<string 上层开发自定义的事件通知扩展字段, 必须为可以解析为Json的非格式化的字符串 */
static const char *kChatRoomNotificationDataKeyOpt	=	"operator";		/**<string 操作者的账号accid */
static const char *kChatRoomNotificationDataKeyOptNick		=	"opeNick";		/**<string 操作者的账号nick */
static const char *kChatRoomNotificationDataKeyTargetNick	=	"tarNick";	/**<string json array 被操作者的nick列表 */
static const char *kChatRoomNotificationDataKeyTarget=	"target";		/**<string json array 被操作者的accid */
/** @}*/ //聊天室通知Keys

/** @enum NIMChatRoomNotificationId 聊天室通知类型 */
enum NIMChatRoomNotificationId
{
	kNIMChatRoomNotificationIdMemberIn			= 301, /**< 成员进入聊天室*/
	kNIMChatRoomNotificationIdMemberExit		= 302, /**< 成员离开聊天室*/
	kNIMChatRoomNotificationIdAddBlack			= 303, /**< 成员被加黑*/
	kNIMChatRoomNotificationIdRemoveBlack		= 304, /**< 成员被取消黑名单*/
	kNIMChatRoomNotificationIdAddMute			= 305, /**< 成员被设置禁言*/
	kNIMChatRoomNotificationIdRemoveMute		= 306, /**< 成员被取消禁言*/
	kNIMChatRoomNotificationIdAddManager		= 307, /**< 设置为管理员*/
	kNIMChatRoomNotificationIdRemoveManager		= 308, /**< 取消管理员*/
	kNIMChatRoomNotificationIdAddFixed			= 309, /**< 成员设定为固定成员*/
	kNIMChatRoomNotificationIdRemoveFixed		= 310, /**< 成员取消固定成员*/
	kNIMChatRoomNotificationIdClosed			= 311, /**< 聊天室被关闭了*/
	kNIMChatRoomNotificationIdInfoUpdated		= 312, /**< 聊天室信息被更新了*/
	kNIMChatRoomNotificationIdMemberKicked		= 313, /**< 成员被踢了*/
};

/** @enum NIMChatRoomLinkCondition 聊天室链接情况，一般都是有本地网路情况引起 */
enum NIMChatRoomLinkCondition
{
	kNIMChatRoomLinkConditionAlive			= 0,	/**< 链接正常*/
	kNIMChatRoomLinkConditionDeadAndRetry	= 1,	/**< 链接失败,sdk尝试重链*/
	kNIMChatRoomLinkConditionDead			= 2,	/**< 链接失败,开发者需要重新申请聊天室进入信息*/
};

/** @enum NIMChatRoomEnterStep 聊天室进入状态 */
enum NIMChatRoomEnterStep
{
	kNIMChatRoomEnterStepInit				= 1,	/**< 本地服务初始化*/
	kNIMChatRoomEnterStepServerConnecting	= 2,	/**< 服务器连接中*/
	kNIMChatRoomEnterStepServerConnectOver	= 3,	/**< 服务器连接结束,连接结果见error_code*/
	kNIMChatRoomEnterStepRoomAuthing		= 4,	/**< 聊天室鉴权中*/
	kNIMChatRoomEnterStepRoomAuthOver		= 5,	/**< 聊天室鉴权结束,鉴权结果见error_code, error_code非408则需要开发者重新请求聊天室进入信息*/
};

/** @enum NIMChatRoomProxyType 代理类型 */
enum NIMChatRoomProxyType
{
	kNIMChatRoomProxyNone = 0,	/**< 不使用代理*/
	kNIMChatRoomProxyHttp11 = 1,	/**< HTTP 1.1 Proxy（暂不支持）*/
	kNIMChatRoomProxySocks4 = 4,	/**< Socks4 Proxy*/
	kNIMChatRoomProxySocks4a = 5,	/**< Socks4a Proxy*/
	kNIMChatRoomProxySocks5 = 6,	/**< Socks5 Proxy*/
};

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_CHATROOM_SDK_DLL_EXPORT_HEADERS_DEF_H_
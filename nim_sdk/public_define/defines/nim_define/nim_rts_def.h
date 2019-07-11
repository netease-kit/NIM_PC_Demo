/** @file nim_rts_def.h
  * @brief 实时会话（数据通道） 接口相关的常量函数等定义头文件
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/6/30
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_RTS_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_RTS_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif

/** @enum NIMRtsChannelType rts通道类型 */
enum NIMRtsChannelType
{
	kNIMRtsChannelTypeNone	= 0,	/**< 无通道 */
	kNIMRtsChannelTypeTcp	= 1,	/**< tcp通道 */
	kNIMRtsChannelTypeUdp	= 2,	/**< udp通道 暂不支持 */
	kNIMRtsChannelTypeVchat	= 4,	/**< 音视频通道*/
};

/** @enum NIMRtsMemberStatus 成员变化类型 */
enum NIMRtsMemberStatus
{
	kNIMRtsMemberStatusJoined           = 0,	/**< 成员进入 */
	kNIMRtsMemberStatusLeaved           = 1,	/**< 成员退出 */
};
/** @enum NIMRtsMemberLeftType 成员退出类型 */
enum NIMRtsMemberLeftType{
	kNIMRtsMemberLeftTimeout	= -1,		/**< 成员超时掉线 */
	kNIMRtsMemberLeftNormal		= 0,		/**< 成员离开 */
};

/** @enum NIMRtsVideoChatMode 音视频通话类型 */
enum NIMRtsVideoChatMode
{
	kNIMRtsVideoChatModeAudio	=	1,	/**< 语音通话模式 */
	kNIMRtsVideoChatModeVideo	=	2	/**< 视频通话模式 */
};

/** @enum NIMRtsConnectStatus 音视频服务器连接状态类型 */
enum NIMRtsConnectStatus
{
	kNIMRtsConnectStatusDisconn			= 0,	/**< 断开连接 */
	kNIMRtsConnectStatusStartFail		= 1,	/**< 启动失败 */
	kNIMRtsConnectStatusTimeout			= 101,	/**< 超时 */
	kNIMRtsConnectStatusSuccess			= 200,	/**< 成功 */
	kNIMRtsConnectStatusInvalidParam	= 400,	/**< 错误参数 */
	kNIMRtsConnectStatusDesKey			= 401,	/**< 密码加密错误 */
	kNIMRtsConnectStatusInvalidRequst	= 417,	/**< 错误请求 */
	kNIMRtsConnectStatusServerUnknown	= 500,	/**< 服务器内部错误 */
	kNIMRtsConnectStatusLogout			= 1001,	/**< 退出 */
};

/** @name json extension params for nim_rts_start and nim_rts_ack
  * for example: 参数可选，例子中是不填时的默认值
  * {"mode": 1, "custom_video":0, "custom_audio":0, "data_record":0, "session_id":"b76e2b7ae065224499e4d7138d643961" }
  * @{
  */
static const char *kNIMRtsSessionId			= "session_id";		/**< string 发起会话的标识id，将在创建点对点和多人通话时，如果填写了此参数优先使用用户填写的session_id */
static const char *kNIMRtsVChatMode			= "mode";			/**< int NIMRtsVideoChatMode，非视频模式时不会发送视频数据 */
static const char *kNIMRtsVChatCustomVideo	= "custom_video";	/**< int 是否用自主的视频数据 >0表示是 */
static const char *kNIMRtsVChatCustomAudio	= "custom_audio";	/**< int 是否用自主的音频数据 >0表示是 */
static const char *kNIMRtsDataRecord		= "data_record";	/**< int 是否需要服务器录制白板数据 >0表示是 */
static const char *kNIMRtsAudioRecord		= "audio_record";	/**< int 是否需要服务器录制音频数据 >0表示是 */
static const char *kNIMRtsApnsText			= "apns";			/**< string 推送用的文本 nim_rts_ack无效 */
static const char *kNIMRtsCreateCustomInfo	= "custom_info";	/**< string 自定义数据，透传给被邀请方 nim_rts_ack无效 */
static const char *kNIMRtsPushEnable		= "push_enable";	/**< int 是否需要推送 >0表示是 默认是 */
static const char *kNIMRtsNeedBadge			= "need_badge";		/**< int 是否需要角标计数 >0表示是 默认是 */
static const char *kNIMRtsNeedFromNick		= "need_nick";		/**< int 是否需要推送昵称 >0表示是 默认是 */
static const char *kNIMRtsApnsPayload		= "payload";		/**< string JSON格式,推送payload */
static const char *kNIMRtsSound				= "sound";			/**< string 推送声音 */
static const char *kNIMRtsKeepCalling		= "keepcalling";	/**< int, 是否强制持续呼叫（对方离线也会呼叫）,1表示是，0表示否。默认是 */
//无效已经默认支持 static const char *kNIMRtsWebrtc			= "webrtc";			/**< int, 是否支持webrtc互通（针对点对点中的音频通话）,1表示是，0表示否。默认否 */
/** @}*/ //json extension params for nim_rts_start and nim_rts_ack

/** @name json extension params for nim_rts_start_cb_func nim_rts_connect_notify_cb_func
  * for example: 
  * {"channel_id": 1231 }
  * @{
  */
static const char *kNIMRtsChannelId			= "channel_id";			/**< int64 通话的通道id */
/** @}*/ //json extension params for nim_rts_start_cb_func nim_rts_connect_notify_cb_func

/** @name json extension params for nim_rts_join_cb_func
  * for example: 
  * {"channel_id": 1231, "custom_info":"hello world" }
  * @{
  */
static const char *kNIMRtsCustomInfo		= "custom_info";		/**< string 自定义数据 */
/** @}*/ //json extension params for nim_rts_join_cb_func

/** @name json extension params for nim_rts_send_data
* for example:
* {"uid": "abc" }
* @{
*/
static const char *kNIMRtsUid				= "uid";			/**< string 用户账号uid */
/** @}*/ //json extension params for nim_rts_send_data


/** @name json extension params for nim_rts_connect_notify_cb_func nim_rts_connect_notify_cb_func
  * for example: 
  * {"record_addr": "", "record_file":"" }
  * @{
  */
static const char *kNIMRtsRecordAddr		= "record_addr";		/**< string 录制地址（服务器开启录制时有效） */
static const char *kNIMRtsRecordFile		= "record_file";		/**< string 录制的文件名（服务器开启录制时有效,音视频类型时为音频文件名） */
static const char *kNIMRtsVideoRecordFile	= "video_record_file";	/**< string 录制的视频文件名（服务器开启录制时有效） */
/** @}*/ //json extension params for nim_rts_connect_notify_cb_func

/** @name json extension params for nim_rts_sync_ack_notify_cb_func
  * for example: 
  * {"client_type": 1 }
  * @{
  */
static const char *kNIMRtsClientType	= "client_type";	/**< int 客户端类型NIMClientType，见nim_client_def.h */
/** @}*/ //json extension params for nim_rts_sync_ack_notify_cb_func

/** @name json extension params for nim_rts_member_change_cb_func
  * for example: 
  * {"client_type": 1 }
  * @{
  */
static const char *kNIMRtsLeaveType		= "leave_type";		/**< int 客户端类型NIMRtsMemberLeftType，见nim_client_def.h */
/** @}*/ //json extension params for nim_rts_member_change_cb_func
	
/** @typedef void (*nim_rts_start_cb_func)(int code, const char *session_id, int channel_type, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 创建通道返回结果
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型 如要tcp+音视频，则channel_type=kNIMRtsChannelTypeTcp|kNIMRtsChannelTypeVchat
  * @param[out] uid 对方帐号
  * @param[out] json_extension 扩展字段，若成功见kNIMRtsChannelId， ，如{"channel_id": 123213213123 }
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_start_cb_func)(int code, const char *session_id, int channel_type, const char *uid, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_start_notify_cb_func)(const char *session_id, int channel_type, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 收到对方创建通道通知
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型 如要tcp+音视频，则channel_type=kNIMRtsChannelTypeTcp|kNIMRtsChannelTypeVchat
  * @param[out] uid 对方帐号
  * @param[out] json_extension 扩展字段，若有透传数据见kNIMRtsCreateCustomInfo，通道id见kNIMRtsChannelId，如{"custom_info": "","channel_id": 123213213123 }
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_start_notify_cb_func)(const char *session_id, int channel_type, const char *uid, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_create_cb_func)(int code, const char *session_id, const char *json_extension, const void *user_data)
  * NIM RTS 创建多人rts返回结果
  * @param[out] code 调用结果
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_create_cb_func)(int code, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_join_cb_func)(int code, const char *session_id, const char *json_extension, const void *user_data)
  * NIM RTS 加入多人rts返回结果
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] json_extension 扩展字段，若成功返回创建的kNIMRtsCustomInfo及kNIMRtsChannelId，如{"channel_id": 1231, "custom_info":"hello world" }
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_join_cb_func)(int code, const char *session_id, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_ack_res_cb_func)(int code, const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data)
  * NIM RTS 回复收到邀请的结果
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型
  * @param[out] accept 是否接受
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_ack_res_cb_func)(int code, const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_ack_notify_cb_func)(const char *session_id, int channel_type, bool accept, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 收到对方回复邀请的通知
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型
  * @param[out] accept 是否接受
  * @param[out] uid 对方帐号
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_ack_notify_cb_func)(const char *session_id, int channel_type, bool accept, const char *uid, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_sync_ack_notify_cb_func)(const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data)
  * NIM RTS 收到本人在其他端回复邀请的同步通知
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型
  * @param[out] accept 是否接受
  * @param[out] json_extension 带kNIMRtsClientType，客户端类型
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_sync_ack_notify_cb_func)(const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data);

//状态回调相关
/** @typedef void (*nim_rts_connect_notify_cb_func)(const char *session_id, int channel_type, int code, const char *json_extension, const void *user_data)
  * NIM RTS 通道连接状态通知
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型
  * @param[out] code 状态码见NIMRtsConnectStatus
  * @param[out] json_extension 成功时带录制的地址和文件名，服务器开启录制时有效
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_connect_notify_cb_func)(const char *session_id, int channel_type, int code, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_member_change_cb_func)(const char *session_id, int channel_type, int type, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 通道连接成员变化通知
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型
  * @param[out] type 成员变化类型见NIMRtsMemberStatus
  * @param[out] uid 对方帐号
  * @param[out] json_extension 如果是成员离开，返回kNIMRtsLeaveType
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_member_change_cb_func)(const char *session_id, int channel_type, int type, const char *uid, const char *json_extension, const void *user_data);

//控制接口
/** @typedef void (*nim_rts_control_res_cb_func)(int code, const char *session_id, const char *info, const char *json_extension, const void *user_data)
  * NIM RTS 控制接口调用结果
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] info 透传数据
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_control_res_cb_func)(int code, const char *session_id, const char *info, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_control_notify_cb_func)(const char *session_id, const char *info, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 控制消息通知回调
  * @param[out] session_id 会话id
  * @param[out] info 透传数据
  * @param[out] uid 对方帐号
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_control_notify_cb_func)(const char *session_id, const char *info, const char *uid, const char *json_extension, const void *user_data);

//结束相关
/** @typedef void (*nim_rts_hangup_res_cb_func)(int code, const char *session_id, const char *json_extension, const void *user_data)
  * NIM RTS 挂断会话调用结果
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_hangup_res_cb_func)(int code, const char *session_id, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_hangup_notify_cb_func)(const char *session_id, const char *uid, const char *json_extension, const void *user_data)
  * NIM RTS 挂断会话通知回调
  * @param[out] session_id 会话id
  * @param[out] uid 对方帐号
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_hangup_notify_cb_func)(const char *session_id, const char *uid, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_rec_data_cb_func)(const char *session_id, int channel_type, const char *uid, const char *data, unsigned int size, const char *json_extension, const void *user_data)
  * NIM RTS 数据监听回调
  * @param[out] data 接受的数据
  * @param[out] size data的数据长度
  * @param[out] uid 对方帐号
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_rec_data_cb_func)(const char *session_id, int channel_type, const char *uid, const char *data, unsigned int size, const char *json_extension, const void *user_data);

/** @typedef void (*nim_rts_opt_cb_func)(int code, const char *session_id, int channel_type, const char *json_extension, const void *user_data)
  * NIM RTS 操作结果回调
  * @param[out] code 调用结果
  * @param[out] session_id 会话id
  * @param[out] channel_type 通道类型，暂时只支持白板类型
  * @param[out] json_extension 无效的扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数，不做任何处理！
  * @return void 无返回值
  */ 
typedef void (*nim_rts_opt_cb_func)(int code, const char *session_id, int channel_type, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_RTS_DEF_H_
/** @file nim_vchat_def.h
  * @brief NIM VChat提供的音视频接口定义，
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/5/5
  */

#ifndef NIM_SDK_DLL_EXPORT_HEADERS_NIM_VCHAT_DEF_H_
#define NIM_SDK_DLL_EXPORT_HEADERS_NIM_VCHAT_DEF_H_

#ifdef __cplusplus
extern"C"
{
#endif
/** @enum NIMVideoChatSessionType 音视频通话状态通知类型 */
enum NIMVideoChatSessionType{
	kNIMVideoChatSessionTypeStartRes		= 1,		/**< 创建通话结果 */
	kNIMVideoChatSessionTypeInviteNotify	= 2,		/**< 通话邀请 */
	kNIMVideoChatSessionTypeCalleeAckRes	= 3,		/**< 确认通话，接受拒绝结果 */
	kNIMVideoChatSessionTypeCalleeAckNotify	= 4,		/**< 确认通话，接受拒绝通知 */
	kNIMVideoChatSessionTypeControlRes		= 5,		/**< NIMVChatControlType 结果 */
	kNIMVideoChatSessionTypeControlNotify	= 6,		/**< NIMVChatControlType 通知 */
	kNIMVideoChatSessionTypeConnect			= 7,		/**< 通话中链接状态通知 */
	kNIMVideoChatSessionTypePeopleStatus	= 8,		/**< 通话中成员状态 */
	kNIMVideoChatSessionTypeNetStatus		= 9,		/**< 通话中网络状态 */
	kNIMVideoChatSessionTypeHangupRes		= 10,		/**< 通话挂断结果 */
	kNIMVideoChatSessionTypeHangupNotify	= 11,		/**< 通话被挂断通知 */
	kNIMVideoChatSessionTypeSyncAckNotify	= 12,		/**< 通话接听挂断同步通知 */
	kNIMVideoChatSessionTypeMp4Notify		= 13,		/**< 通知MP4录制状态，包括开始录制和结束录制 */
	kNIMVideoChatSessionTypeInfoNotify		= 14,		/**< 通知实时音视频数据状态 */
};

/** @enum NIMVChatControlType 音视频通话控制类型 */
enum NIMVChatControlType
{
	kNIMTagControlOpenAudio             = 1,    /**< 打开音频 */
	kNIMTagControlCloseAudio            = 2,    /**< 关闭音频 */
	kNIMTagControlOpenVideo             = 3,    /**< 打开视频 */
	kNIMTagControlCloseVideo            = 4,    /**< 关闭视频 */
	kNIMTagControlAudioToVideo          = 5,    /**< 请求从音频切换到视频 */
	kNIMTagControlAgreeAudioToVideo     = 6,    /**< 同意从音频切换到视频 */
	kNIMTagControlRejectAudioToVideo    = 7,    /**< 拒绝从音频切换到视频 */
	kNIMTagControlVideoToAudio          = 8,    /**< 从视频切换到音频 */
	kNIMTagControlBusyLine              = 9,    /**< 占线 */
	kNIMTagControlCamaraNotAvailable    = 10,   /**< 告诉对方自己的摄像头不可用 */
	kNIMTagControlEnterBackground       = 11,   /**< 告诉对方自已处于后台 */
	kNIMTagControlReceiveStartNotifyFeedback = 12,   /**< 告诉发送方自己已经收到请求了（用于通知发送方开始播放提示音） */
	kNIMTagControlMp4StartRecord		= 13,   /**< 告诉发送方自己开始录制 */
	kNIMTagControlMp4StopRecord			= 14,   /**< 告诉发送方自己结束录制 */
};

/** @enum NIMVideoChatMode 音视频通话类型 */
enum NIMVideoChatMode{
	kNIMVideoChatModeAudio	=	1,	/**< 语音通话模式 */
	kNIMVideoChatModeVideo	=	2	/**< 视频通话模式 */
};

/** @enum NIMVChatVideoQuality 视频通话分辨率，最终长宽比不保证 */
enum NIMVChatVideoQuality{
	kNIMVChatVideoQualityNormal		= 0,		/**< 视频默认分辨率 480x320*/
	kNIMVChatVideoQualityLow		= 1,		/**< 视频低分辨率 176x144*/
	kNIMVChatVideoQualityMedium		= 2,		/**< 视频中分辨率 352x288*/
	kNIMVChatVideoQualityHigh		= 3,		/**< 视频高分辨率 480x320*/
	kNIMVChatVideoQualitySuper		= 4,		/**< 视频超高分辨率 640x480*/
	kNIMVChatVideoQuality720p		= 5,		/**< 用于桌面分享级别的分辨率1280x720，需要使用高清摄像头并指定对应的分辨率，或者自定义通道传输 */
};

/** @enum NIMVChatVideoFrameRate 视频通话帧率，实际帧率因画面采集频率和机器性能限制可能达不到期望值 */
enum NIMVChatVideoFrameRate{
	kNIMVChatVideoFrameRateNormal	= 0,		/**< 视频通话帧率默认值,最大取每秒15帧 */
	kNIMVChatVideoFrameRate5		= 1,		/**< 视频通话帧率 最大取每秒5帧 */
	kNIMVChatVideoFrameRate10		= 2,		/**< 视频通话帧率 最大取每秒10帧 */
	kNIMVChatVideoFrameRate15		= 3,		/**< 视频通话帧率 最大取每秒15帧 */
	kNIMVChatVideoFrameRate20		= 4,		/**< 视频通话帧率 最大取每秒20帧 */
	kNIMVChatVideoFrameRate25		= 5,		/**< 视频通话帧率 最大取每秒25帧 */
};

/** @enum NIMVideoChatSessionStatus 音视频通话成员变化类型 */
enum NIMVideoChatSessionStatus{
    kNIMVideoChatSessionStatusJoined           = 0,	/**< 成员进入 */
    kNIMVideoChatSessionStatusLeaved           = 1,	/**< 成员退出 */
};

/** @enum NIMVideoChatSessionNetStat 音视频通话网络变化类型 */
enum NIMVideoChatSessionNetStat{
    kNIMVideoChatSessionNetStatVeryGood    = 0,	/**< 网络状态很好 */
    kNIMVideoChatSessionNetStatGood        = 1,	/**< 网络状态较好 */
    kNIMVideoChatSessionNetStatBad         = 2,	/**< 网络状态较差 */
    kNIMVideoChatSessionNetStatVeryBad     = 3,	/**< 网络状态很差 */
};

/** @enum NIMVChatConnectErrorCode 音视频服务器连接状态类型 */
enum NIMVChatConnectErrorCode{
	kNIMVChatConnectDisconn				= 0,		/**< 断开连接 */
	kNIMVChatConnectStartFail			= 1,		/**< 启动失败 */
    kNIMVChatConnectTimeout				= 101,		/**< 超时 */
	kNIMVChatConnectMeetingModeError	= 102,		/**< 会议模式错误 */
	kNIMVChatConnectRtmpModeError		= 103,		/**< 非rtmp用户加入rtmp频道 */
	kNIMVChatConnectRtmpNodesError		= 104,		/**< 超过频道最多rtmp人数限制 */
	kNIMVChatConnectRtmpHostError		= 105,		/**< 已经存在一个主播 */
	kNIMVChatConnectRtmpCreateError		= 106,		/**< 需要旁路直播, 但频道创建者非主播 */
    kNIMVChatConnectSuccess				= 200,		/**< 成功 */
    kNIMVChatConnectInvalidParam		= 400,		/**< 错误参数 */
	kNIMVChatConnectDesKey				= 401,		/**< 密码加密错误 */
    kNIMVChatConnectInvalidRequst		= 417,		/**< 错误请求 */
    kNIMVChatConnectServerUnknown		= 500,		/**< 服务器内部错误 */
	kNIMVChatConnectLogout				= 1001,		/**< 退出 */
	kNIMVChatChannelStartFail			= 11000,	/**< 发起失败 */
	kNIMVChatChannelDisconnected		= 11001,	/**< 断开连接 */
	kNIMVChatVersionSelfLow				= 11002,	/**< 本人SDK版本太低不兼容 */
	kNIMVChatVersionRemoteLow			= 11003,	/**< 对方SDK版本太低不兼容 */
};

/** @enum NIMVChatMp4RecordCode mp4录制状态 */
enum NIMVChatMp4RecordCode{
	kNIMVChatMp4RecordClose				= 0,		/**< MP4结束 */
	kNIMVChatMp4RecordVideoSizeError	= 1,		/**< MP4结束，视频画面大小变化 */
	kNIMVChatMp4RecordOutDiskSpace		= 2,		/**< MP4结束，磁盘空间不足 */
	kNIMVChatMp4RecordCreate			= 200,		/**< MP4文件创建 */
	kNIMVChatMp4RecordExsit				= 400,		/**< MP4文件已经存在 */
	kNIMVChatMp4RecordCreateError		= 403,		/**< MP4文件创建失败 */
	kNIMVChatMp4RecordInvalid			= 404,		/**< 通话不存在 */
};

/** @name json extension params for start or ack accept
  * @{
  */
static const char *kNIMVChatUids			= "uids";			/**< StrArray 帐号, ack无效 */
static const char *kNIMVChatSessionId		= "session_id";		/**< string 发起会话的标识id，将在创建通话及结束通话时有效，帮助针对无channelid的情况下进行映射 */
static const char *kNIMVChatCustomVideo		= "custom_video";	/**< int 是否用自主的视频数据 >0表示是 */
static const char *kNIMVChatCustomAudio		= "custom_audio";	/**< int 是否用自主的音频数据 >0表示是 */
static const char *kNIMVChatRecord			= "record";			/**< int 是否需要录制音频数据 >0表示是 （需要服务器配置支持，本地录制直接调用接口函数） */
static const char *kNIMVChatVideoRecord		= "video_record";	/**< int 是否需要录制视频数据 >0表示是 （需要服务器配置支持，本地录制直接调用接口函数）*/
static const char *kNIMVChatMaxVideoRate	= "max_video_rate";	/**< int 视频发送编码码率 >=100000 <=5000000有效 */
static const char *kNIMVChatVideoQuality	= "video_quality";	/**< int 视频聊天分辨率选择 NIMVChatVideoQuality */
static const char *kNIMVChatVideoFrameRate	= "frame_rate";		/**< int 视频画面帧率 NIMVChatVideoFrameRate */
static const char *kNIMVChatRtmpUrl			= "rtmp_url";		/**< string 直播推流地址(加入多人时有效)，非空代表主播旁路直播，此时kNIMVChatBypassRtmp无效 */
static const char *kNIMVChatBypassRtmp		= "bypass_rtmp";	/**< int 是否是旁路直播观众(加入多人时有效)， >0表示是 */
static const char *kNIMVChatPushEnable		= "push_enable";	/**< int 是否需要推送 >0表示是 默认是 */
static const char *kNIMVChatNeedBadge		= "need_badge";		/**< int 是否需要角标计数 >0表示是 默认是 */
static const char *kNIMVChatNeedFromNick	= "need_nick";		/**< int 是否需要推送昵称 >0表示是 默认是 */
static const char *kNIMVChatApnsPayload		= "payload";		/**< string JSON格式,推送payload */
static const char *kNIMVChatSound			= "sound";			/**< string 推送声音 */
/** @}*/ //json extension params

/** @name json extension params for nim_vchat_cb_func
  * @{
  */
static const char *kNIMVChatUid				= "uid";				/**< string 帐号 */
static const char *kNIMVChatStatus			= "status";				/**< int 状态 */
static const char *kNIMVChatRecordAddr		= "record_addr";		/**< string 录制地址（服务器开启录制时有效） */
static const char *kNIMVChatRecordFile		= "record_file";		/**< string 服务器音频录制文件名（服务器开启录制时有效） */
static const char *kNIMVChatVideoRecordFile = "video_record_file";	/**< string 服务器视频录制文件名（服务器开启录制时有效） */
static const char *kNIMVChatType			= "type";				/**< int 状态 */
static const char *kNIMVChatTime			= "time";				/**< int64 时间 单位毫秒 */
static const char *kNIMVChatAccept			= "accept";				/**< int 是否接受 >0表示接受 */
static const char *kNIMVChatClient			= "client";				/**< int 客户端类型 NIMClientType见nim_client_def.h */
static const char *kNIMVChatMp4Start		= "mp4_start";			/**< key Mp4写入数据开始 kNIMVChatMp4File kNIMVChatTime(本地时间点) */
static const char *kNIMVChatMp4Close		= "mp4_close";			/**< key 结束Mp4录制，返回时长及原因 kNIMVChatStatus(NIMVChatMp4RecordCode) kNIMVChatTime(时长) kNIMVChatMp4File */
static const char *kNIMVChatMp4File			= "mp4_file";			/**< string mp4录制地址 */
static const char *kNIMVChatCustomInfo		= "custom_info";		/**< string 自定义数据 */
static const char *kNIMVChatVideo			= "video";				/**< key 视频 */
static const char *kNIMVChatAudio			= "audio";				/**< key 音频 */
static const char *kNIMVChatStaticInfo		= "static_info";		/**< key 音视频实时状态 */
static const char *kNIMVChatFPS				= "fps";				/**< int 每秒帧率或者每秒发包数 */
static const char *kNIMVChatKBPS			= "KBps";				/**< int 每秒流量，单位为“千字节” */
/** @}*/ //json extension params

/** @typedef void (*nim_vchat_cb_func)(NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json_extension, const void *user_data)
  * NIM VChat 	音视频通话中状态返回回调接口   \n
  * 			根据NIMVideoChatSessionType的具体参数说明如下： \n
  * 			kNIMVideoChatSessionTypeStartRes,			//创建通话结果 code=200成功，json 返回kNIMVChatSessionId \n
  * 			kNIMVideoChatSessionTypeInviteNotify,		//通话邀请 code无效,json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVideoChatMode, kNIMVChatTime, kNIMVChatCustomInfo \n
  * 			kNIMVideoChatSessionTypeCalleeAckRes,		//确认通话，接受拒绝结果 json 无效 code: 200:成功 9103 : 已经在其他端接听 / 拒绝过这通电话 \n
  * 			kNIMVideoChatSessionTypeCalleeAckNotify,	//确认通话，接受拒绝通知 code=无效，json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVideoChatMode, kNIMVChatAccept \n
  * 			kNIMVideoChatSessionTypeControlRes,			//NIMVChatControlType 结果  code=200成功，json 返回kNIMVChatType对应NIMVChatControlType \n
  * 			kNIMVideoChatSessionTypeControlNotify,		//NIMVChatControlType 通知	code=无效，json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVChatControlType \n
  * 			kNIMVideoChatSessionTypeConnect,			//通话中链接状态通知 code对应NIMVChatConnectErrorCode， 非200均失败并底层结束,如果成功json中带kNIMVChatVideoRecordFile和kNIMVChatRecordFile \n
  * 			kNIMVideoChatSessionTypePeopleStatus,		//通话中成员状态 code对应NIMVideoChatSessionStatus, json返回kNIMVChatUid \n
  * 			kNIMVideoChatSessionTypeNetStatus,			//通话中网络状态 code对应NIMVideoChatSessionNetStat, json返回kNIMVChatUid \n
  * 			kNIMVideoChatSessionTypeHangupRes,			//通话主动结果 code=200成功，json无效 \n
  * 			kNIMVideoChatSessionTypeHangupNotify,		//通话被挂断通知 code无效，json无效 \n
  * 			kNIMVideoChatSessionTypeSyncAckNotify,		//其他端接听挂断后的同步通知 json 返回 kNIMVChatTime，kNIMVChatType对应NIMVideoChatMode，kNIMVChatAccept，kNIMVChatClient  \n
  *				kNIMVideoChatSessionTypeMp4Notify			//通知MP4录制状态，包括开始录制和结束录制 code无效，json 返回如下 \n
  *															//	MP4开始 	{"mp4_start":{ "mp4_file": "d:\\test.mp4", "time": 14496477000000 }} \n
  *															//	MP4结束 	{"mp4_close":{ "mp4_file": "d:\\test.mp4", "time": 120000, "status": 0 }} \n
  *				kNIMVideoChatSessionTypeInfoNotify			//实时状态		{"static_info":{ "video": {"fps":20, "KBps":200, "width":1280,"height":720}, "audio": {"fps":17, "KBps":3}}} \n
  * @param[out] type NIMVideoChatSessionType
  * @param[out] channel_id 通话的通道id
  * @param[out] code 结果类型或错误类型
  * @param[out] json_extension Json string 扩展
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  *
  */
typedef void (*nim_vchat_cb_func)(NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json_extension, const void *user_data);

/** @typedef void (*nim_vchat_mp4_record_opt_cb_func)(bool ret, int code, const char *file, __int64 time, const char *json_extension, const void *user_data)
  * NIM MP4操作回调，实际的开始录制和结束都会在nim_vchat_cb_func中返回
  * @param[out] ret 结果代码，true表示成功
  * @param[out] code 对应NIMVChatMp4RecordCode，用于获得失败时的错误原因
  * @param[out] file 文件路径
  * @param[out] time 录制结束时有效，对应毫秒级的录制时长
  * @param[out] json_extension Json string 无效扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_vchat_mp4_record_opt_cb_func)(bool ret, int code, const char *file, __int64 time, const char *json_extension, const void *user_data);

/** @typedef void (*nim_vchat_opt_cb_func)(bool ret, int code, const char *json_extension, const void *user_data)
  * NIM 操作回调，通用的操作回调接口
  * @param[out] ret 结果代码，true表示成功
  * @param[out] code 暂时无效
  * @param[out] json_extension Json string 扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_vchat_opt_cb_func)(bool ret, int code, const char *json_extension, const void *user_data);

/** @typedef void (*nim_vchat_opt2_cb_func)(int code, __int64 channel_id, const char *json_extension, const void *user_data)
  * NIM 操作回调，通用的操作回调接口
  * @param[out] code 结果代码，code==200表示成功
  * @param[out] channel_id 通道id
  * @param[out] json_extension Json string 扩展字段
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
typedef void (*nim_vchat_opt2_cb_func)(int code, __int64 channel_id, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_VCHAT_DEF_H_
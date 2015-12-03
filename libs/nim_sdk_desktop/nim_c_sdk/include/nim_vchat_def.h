/** @file nim_vchat_def.h
  * @brief NIM VChat提供的音视频接口定义，
  * @copyright (c) 2015, NetEase Inc. All rights reserved
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
};

/** @enum NIMVideoChatMode 音视频通话类型 */
enum NIMVideoChatMode{
	kNIMVideoChatModeAudio	=	1,	/**< 语音通话模式 */
	kNIMVideoChatModeVideo	=	2	/**< 视频通话模式 */
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
	kNIMVChatConnectDisconn			= 0,	/**< 断开连接 */
	kNIMVChatConnectStartFail		= 1,	/**< 启动失败 */
    kNIMVChatConnectTimeout			= 101,	/**< 超时 */
    kNIMVChatConnectSuccess			= 200,	/**< 成功 */
    kNIMVChatConnectInvalidParam	= 400,	/**< 错误参数 */
	kNIMVChatConnectDesKey			= 401,	/**< 密码加密错误 */
    kNIMVChatConnectInvalidRequst	= 417,	/**< 错误请求 */
    kNIMVChatConnectServerUnknown	= 500,	/**< 服务器内部错误 */
	kNIMVChatConnectLogout			= 1001,	/**< 退出 */
};

/** @name json extension params for nim_vchat_cb_func
  * @{
  */
static const char *kNIMVChatUids			= "uids";			/**< StrArray 帐号 */
static const char *kNIMVChatUid				= "uid";			/**< string 帐号 */
static const char *kNIMVChatStatus			= "status";			/**< int 状态 */
static const char *kNIMVChatRecordAddr		= "record_addr";	/**< string 录制地址（服务器开启录制时有效） */
static const char *kNIMVChatRecordFile		= "record_file";	/**< string 录制文件名（服务器开启录制时有效） */
static const char *kNIMVChatType			= "type";			/**< int 状态 */
static const char *kNIMVChatTime			= "time";			/**< int64 时间 单位毫秒 */
static const char *kNIMVChatAccept			= "accept";			/**< int 是否接受 >0表示接受 */
static const char *kNIMVChatClient			= "client";			/**< int 客户端类型 NIMClientType见nim_client_def.h */
static const char *kNIMVChatCustomVideo		= "custom_video";	/**< int 是否用自主的视频数据 >0表示是 */
static const char *kNIMVChatCustomAudio		= "custom_audio";	/**< int 是否用自主的音频数据 >0表示是 */
static const char *kNIMVChatRecord			= "record";			/**< int 是否需要录制数据 >0表示是 */
static const char *kNIMVChatAecTime			= "aec_time";		/**< int 回音消除延迟参数 >=0有效 */
static const char *kNIMVChatMaxVideoRate	= "max_video_rate";	/**< int 视频发送编码码率 >=100000 <=600000有效 */
/** @}*/ //json extension params

/** @typedef void (*nim_vchat_cb_func)(NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json_extension, const void *user_data)
  * NIM VChat 	音视频通话中状态返回回调接口   \n
  * 			根据NIMVideoChatSessionType的具体参数说明如下： \n
  * 			kNIMVideoChatSessionTypeStartRes,			//创建通话结果 code=200成功，json无效 \n
  * 			kNIMVideoChatSessionTypeInviteNotify,		//通话邀请 code无效,json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVideoChatMode, kNIMVChatTime \n
  * 			kNIMVideoChatSessionTypeCalleeAckRes,		//确认通话，接受拒绝结果 json 无效 code: 200:成功 9103 : 已经在其他端接听 / 拒绝过这通电话 \n
  * 			kNIMVideoChatSessionTypeCalleeAckNotify,	//确认通话，接受拒绝通知 code=无效，json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVideoChatMode, kNIMVChatAccept \n
  * 			kNIMVideoChatSessionTypeControlRes,			//NIMVChatControlType 结果  code=200成功，json 返回kNIMVChatType对应NIMVChatControlType \n
  * 			kNIMVideoChatSessionTypeControlNotify,		//NIMVChatControlType 通知	code=无效，json 返回kNIMVChatUid发起者，kNIMVChatType对应NIMVChatControlType \n
  * 			kNIMVideoChatSessionTypeConnect,			//通话中链接状态通知 code对应NIMVChatConnectErrorCode， 非200均失败并底层结束,如果成功json中带kNIMVChatRecordAddr和kNIMVChatRecordFile \n
  * 			kNIMVideoChatSessionTypePeopleStatus,		//通话中成员状态 code对应NIMVideoChatSessionStatus, json返回kNIMVChatUid \n
  * 			kNIMVideoChatSessionTypeNetStatus,			//通话中网络状态 code对应NIMVideoChatSessionNetStat \n
  * 			kNIMVideoChatSessionTypeHangupRes,			//通话主动结果 code=200成功，json无效 \n
  * 			kNIMVideoChatSessionTypeHangupNotify,		//通话被挂断通知 code无效，json无效 \n
  * 			kNIMVideoChatSessionTypeSyncAckNotify,		//其他端接听挂断后的同步通知 json 返回 kNIMVChatTime，kNIMVChatType对应NIMVideoChatMode，kNIMVChatAccept，kNIMVChatClient  \n
  * @param[out] type NIMVideoChatSessionType
  * @param[out] channel_id 通话的通道id
  * @param[out] code 结果类型或错误类型
  * @param[out] json_extension Json string 扩展
  * @param[out] user_data APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  *
  */
typedef void (*nim_vchat_cb_func)(NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json_extension, const void *user_data);

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //NIM_SDK_DLL_EXPORT_HEADERS_NIM_VCHAT_DEF_H_
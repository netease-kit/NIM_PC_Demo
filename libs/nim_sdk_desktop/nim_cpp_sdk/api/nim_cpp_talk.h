/** @file nim_cpp_talk.h
  * @brief P2P和群组聊天
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_TALK_H_
#define _NIM_SDK_CPP_TALK_H_

#include <string>
#include <functional>
#include "nim_talk_helper.h"
#include "nim_msg_helper.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

/** @class Talk
  * @brief 聊天功能；主要包括发送消息、接收消息等功能
  */
class Talk
{

public:
	typedef std::function<void(const SendMessageArc&)>	SendMsgAckCallback;	/**< 发送消息回执通知回调模板 */
	typedef std::function<void(const IMMessage&)>	ReceiveMsgCallback;	/**< 接收消息通知回调模板 */
	typedef std::function<void(const std::list<IMMessage>&)>	ReceiveMsgsCallback;	/**< 批量接收消息通知回调模板 */
	typedef std::function<void(int64_t, int64_t)>	FileUpPrgCallback;	/**< 发送多媒体消息文件上传过程回调模板 */
	typedef std::function<bool(const IMMessage&)> TeamNotificationFilter; /**< 群通知过滤器 */
	typedef std::function<void(const NIMResCode, const std::list<RecallMsgNotify>&)>	RecallMsgsCallback;	/**< 消息撤回通知回调模板 */
	typedef std::function<void(const BroadcastMessage&)>	ReceiveBroadcastMsgCallback;	/**< 接收广播消息通知回调模板 */
	typedef std::function<void(const std::list<BroadcastMessage>&)>	ReceiveBroadcastMsgsCallback;	/**< 批量接收广播消息通知回调模板 */

	/** @fn static void RegSendMsgCb(const SendMsgCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册发送消息回调函数 （必须全局注册,统一接受回调后分发消息到具体的会话。注意：客户端发包之后,服务器不一定会返回！！！）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb		发送消息的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				408:请求过程超时
	*				414:参数错误
	*				802:没有权限(群错误码)
	*				811:强推列表中帐号数量超限(群错误码)
	*				812:群禁言(群错误码)
	*				10200:发送文件消息，NOS上传暂停
	*				10404:本地资源不存在
	*				10414:本地错误码，参数错误
	*				10502:发送消息，上传NOS失败
	*/
	static void RegSendMsgCb(const SendMsgAckCallback& cb, const std::string& json_extension = "");

	/** 
	* 发送消息
	* @param[in] json_msg		消息体Json字符串,可以通过CreateXXXMessage方法自动创建
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] pcb		上传进度的回调函数, 如果发送的消息里包含了文件资源,则通过此回调函数通知上传进度
	* @return void 无返回值
	*/
	static void SendMsg(const std::string& json_msg, const std::string& json_extension = "", FileUpPrgCallback* pcb = nullptr);

	/** @fn static bool StopSendMsg(const IMMessageBase& msg, const std::string& json_extension = "")
	* 停止正在发送中的消息（目前只支持发送文件消息时的终止）
	* @param[in] client_msg_id	停止发送的消息客户端id
	* @param[in] type			停止发送的消息类型
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @return void 无返回值
	*/
	static bool StopSendMsg(const std::string& client_msg_id, const NIMMessageType& type, const std::string& json_extension = "");

	/** @fn static void RegReceiveCb(const ReceiveMsgCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册接收消息回调 （建议全局注册,统一接受回调后分发消息到具体的会话）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb		接收消息的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				10414:本地错误码，参数错误
	*				10417:本地错误码，对象已经存在/重复操作
	*/
	static void RegReceiveCb(const ReceiveMsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void RegReceiveMessagesCb(const ReceiveMsgsCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册批量接收消息回调 （建议全局注册,统一接受回调后分发消息到具体的会话）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb		接收消息的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegReceiveMessagesCb(const ReceiveMsgsCallback& cb, const std::string& json_extension = "");

	/** @fn static std::string CreateTextMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const std::string& content, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成文字消息内容,生成的字符串在调用SendMsg时直接传入 
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] content 文本内容
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 文字消息Json字符串
	*/
	static std::string CreateTextMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const std::string& content
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateImageMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const IMImage& image, const std::string& file_path, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成图片消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] image IMImage,图片文件信息
	*  @param[in] file_path 文件本地绝对路径
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 图片消息Json字符串
	*/
	static std::string CreateImageMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const IMImage& image
		, const std::string& file_path
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateFileMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const IMFile& file, const std::string& file_path, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成文件消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] file IMFile,文件信息
	*  @param[in] file_path 文件本地绝对路径
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 文件消息Json字符串
	*/
	static std::string CreateFileMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const IMFile& file
		, const std::string& file_path
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateAudioMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const IMAudio& audio, const std::string& file_path, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成语音消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] audio IMAudio,语音文件信息
	*  @param[in] file_path 文件本地绝对路径
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 语音消息Json字符串
	*/
	static std::string CreateAudioMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const IMAudio& audio
		, const std::string& file_path
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateVideoMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const IMVideo& video, const std::string& file_path, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成视频消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] video IMVideo,视频文件信息
	*  @param[in] file_path 文件本地绝对路径
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 视频消息Json字符串
	*/
	static std::string CreateVideoMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const IMVideo& video
		, const std::string& file_path
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateLocationMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const IMLocation& location, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成位置消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] location IMLocation,位置信息
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 位置消息Json字符串
	*/
	static std::string CreateLocationMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const IMLocation& location
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateTipMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const std::string& tip_content, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成Tip消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] tip_content Tip文本内容
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 位置消息Json字符串
	*/
	static std::string CreateTipMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const std::string& tip_content
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateBotRobotMessage(const std::string& receiver_id, const NIMSessionType session_type, const std::string& client_msg_id, const std::string& content, const IMBotRobot& bot_msg, const MessageSetting& msg_setting, int64_t timetag  = 0)
	* 生成Tip消息内容,生成的字符串在调用SendMsg时直接传入
	*  @param[in] receiver_id 聊天对象的 ID,如果是单聊,为用户帐号,如果是群聊,为群组 ID
	*  @param[in] session_type NIMSessionType,聊天类型,单聊或群组
	*  @param[in] client_msg_id 客户端消息id,建议uuid
	*  @param[in] content 内容
	*  @param[in] bot_msg 机器人消息信息
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 位置消息Json字符串
	*/	
	static std::string CreateBotRobotMessage(const std::string& receiver_id
		, const NIMSessionType session_type
		, const std::string& client_msg_id
		, const std::string& content
		, const IMBotRobot& bot_msg
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static std::string CreateRetweetMessage(const std::string& src_msg_json	, const std::string& client_msg_id	, const NIMSessionType retweet_to_session_type	, const std::string& retweet_to_session_id	, const MessageSetting& msg_setting	, int64_t timetag = 0)
	* 由其他消息生成转发消息
	*  @param[in] src_msg_json 原消息json
	*  @param[in] client_msg_id 新的客户端消息id,建议uuid
	*  @param[in] retweet_to_session_type 转发目标会话类型 NIMSessionType
	*  @param[in] retweet_to_session_id 转发目标ID
	*  @param[in] msg_setting 消息属性设置
	*  @param[in] timetag 消息时间
	*  @return std::string 位置消息Json字符串
	*/
	static std::string CreateRetweetMessage(const std::string& src_msg_json
		, const std::string& client_msg_id
		, const NIMSessionType retweet_to_session_type
		, const std::string& retweet_to_session_id
		, const MessageSetting& msg_setting
		, int64_t timetag = 0);

	/** @fn static bool ParseIMMessage(const std::string& json_msg, IMMessage& msg)
	* 解析消息Json字符串
	*  @param[in] json_msg 消息内容Json字符串
	*  @param[out] msg 消息内容
	*  @return bool 解析是否成功
	*/
	static bool ParseIMMessage(const std::string& json_msg, IMMessage& msg);

	/** @fn static bool ParseImageMessageAttach(const IMMessage& msg, IMImage& image)
	* 通过得到的消息获得图片信息
	*  @param[in] msg 消息
	*  @param[out] image 图片信息
	*  @return bool 解析是否成功
	*/
	static bool ParseImageMessageAttach(const IMMessage& msg, IMImage& image);

	/** @fn static bool ParseFileMessageAttach(const IMMessage& msg, IMFile& file)
	* 通过得到的消息获得文件信息
	*  @param[in] msg 消息
	*  @param[out] file 文件信息
	*  @return bool 解析是否成功
	*/
	static bool ParseFileMessageAttach(const IMMessage& msg, IMFile& file);

	/** @fn static bool ParseAudioMessageAttach(const IMMessage& msg, IMAudio& audio)
	* 通过得到的消息获得语音信息
	*  @param[in] msg 消息
	*  @param[out] audio 语音信息
	*  @return bool 解析是否成功
	*/
	static bool ParseAudioMessageAttach(const IMMessage& msg, IMAudio& audio);

	/** @fn static bool ParseVideoMessageAttach(const IMMessage& msg, IMVideo& video)
	* 通过得到的消息获得视频信息
	*  @param[in] msg 消息
	*  @param[out] video 视频信息
	*  @return bool 解析是否成功
	*/
	static bool ParseVideoMessageAttach(const IMMessage& msg, IMVideo& video);

	/** @fn static bool ParseLocationMessageAttach(const IMMessage& msg, IMLocation& location)
	* 通过得到的消息获得位置信息
	*  @param[in] msg 消息
	*  @param[out] location 位置信息
	*  @return bool 解析是否成功
	*/
	static bool ParseLocationMessageAttach(const IMMessage& msg, IMLocation& location);

	/** @fn static bool ParseBotRobotMessageAttach(const IMMessage& msg, IMBotRobot& robot_msg)
	* 通过得到的消息获得机器人消息
	*  @param[in] msg 消息
	*  @param[out] robot_msg 机器人消息
	*  @return bool 解析是否成功
	*/
	static bool ParseBotRobotMessageAttach(const IMMessage& msg, IMBotRobot& robot_msg);

	/** @fn void UnregTalkCb()
	* 反注册Talk提供的所有回调
	* @return void 无返回值
	*/
	static void UnregTalkCb();

	/** @fn static void RegTeamNotificationFilter(const TeamNotificationFilter& filter, const std::string& json_extension = "")
	* (全局回调)注册群通知过滤接口 （堵塞线程，谨慎使用，避免耗时行为）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] filter	过滤接口
	* @return void 无返回值
	*/
	static void RegTeamNotificationFilter(const TeamNotificationFilter& filter, const std::string& json_extension = "");

	/** @fn static void RegRecallMsgsCallback(const RecallMsgsCallback& cb, const std::string& json_extension = "");
	* (全局回调)注册消息回调通知接口
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb	回调
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegRecallMsgsCallback(const RecallMsgsCallback& cb, const std::string& json_extension = "");

	/** @fn static void RecallMsg(const IMMessage &msg, const std::string &notify, const RecallMsgsCallback& cb, const std::string& json_extension = "");
	* 撤回消息
	* @param[in] msg 消息
	* @param[in] notify 自定义通知消息
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb	回调
	* @return void 无返回值
	* @note 错误码	200:成功
	*				414:参数错误
	*				508:撤回时间超过配制有效期，默认是2分钟
	*				10414:本地错误码，参数错误
	*				10508:本地错误码,超过配置有效期或者所需参数不存在
	*/
	static void RecallMsg(const IMMessage &msg, const std::string &notify, const RecallMsgsCallback& cb, const std::string& json_extension = "");
	
	/** @fn static std::string GetAttachmentPathFromMsg(const IMMessage& msg)
	*  从消息体中获取附件（图片、语音、视频等）的本地路径
	*  @param[in]  msg	消息
	*  @return char *	消息如果有附件，不管是否已下载，返回附件的本地路径；消息如果没有附件，返回空字符串。
	*/
	static std::string GetAttachmentPathFromMsg(const IMMessage& msg);

	/** @fn static void RegReceiveBroadcastMsgCb(const ReceiveBroadcastMsgCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册接收广播消息回调 （全局注册）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb		接收消息的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*				10414:本地错误码，参数错误
	*				10417:本地错误码，对象已经存在/重复操作
	*/
	static void RegReceiveBroadcastMsgCb(const ReceiveBroadcastMsgCallback& cb, const std::string& json_extension = "");

	/** @fn static void RegReceiveBroadcastMsgsCb(const ReceiveBroadcastMsgsCallback& cb, const std::string& json_extension = "")
	* (全局回调)注册批量接收广播消息回调 （全局注册）
	* @param[in] json_extension json扩展参数（备用,目前不需要）
	* @param[in] cb		接收消息的回调函数
	* @return void 无返回值
	* @note 错误码	200:成功
	*/
	static void RegReceiveBroadcastMsgsCb(const ReceiveBroadcastMsgsCallback& cb, const std::string& json_extension = "");

};

} 

#endif //_NIM_SDK_CPP_TALK_H_
/** @file nim_chatroom_cpp.h
  * @brief 聊天室SDK
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author Oleg, Harrison
  * @date 2015/12/29
  */

#ifndef _NIM_CHATROOM_SDK_CPP_H_
#define _NIM_CHATROOM_SDK_CPP_H_

#include <string>
#include <functional>
#include "nim_chatroom_helper.h"

/**
* @namespace nim_chatroom
* @brief 聊天室
*/
namespace nim_chatroom
{
/** @class ChatRoom
  * @brief 聊天室
  */
class ChatRoom
{
public:
	typedef std::function<void(int64_t room_id, const NIMChatRoomEnterStep step, int error_code, const ChatRoomInfo& info, const ChatRoomMemberInfo& my_info)>	EnterCallback;	/**< 登录回调, 如果错误码为kResRoomLocalNeedRequestAgain，聊天室重连机制结束，则需要向IM服务器重新请求进入该聊天室权限 */
	typedef std::function<void(int64_t room_id, int error_code, NIMChatRoomExitReason exit_reason)>	ExitCallback;	/**< 登出、被踢回调 */
	typedef std::function<void(int64_t room_id, int error_code, const ChatRoomMessage& result)>	SendMsgAckCallback;	/**< 发送消息回执 */
	typedef std::function<void(int64_t room_id, const ChatRoomMessage& result)>	ReceiveMsgCallback;	/**< 接收消息回调 */
	typedef std::function<void(int64_t room_id, const ChatRoomNotification& notification)> NotificationCallback; /**< 通知回调 */
	typedef std::function<void(int64_t room_id, int error_code, const std::list<ChatRoomMemberInfo>& infos)> GetMembersCallback; /**< 在线查询成员列表回调 */
	typedef std::function<void(int64_t room_id, int error_code, const std::list<ChatRoomMessage>& msgs)> GetMsgHistoryCallback; /**< 在线查询消息历史回调 */
	typedef std::function<void(int64_t room_id, int error_code, const ChatRoomMemberInfo& info)> SetMemberAttributeCallback; /**< 设置成员身份标识回调*/
	typedef std::function<void(int64_t room_id, int error_code, const ChatRoomInfo& info)> GetChatRoomInfoCallback; /**< 获取当前聊天室信息回调*/
	typedef std::function<void(int64_t room_id, int error_code)> KickMemberCallback; /**< 踢掉指定成员回调*/
	typedef std::function<void(int64_t room_id, const NIMChatRoomLinkCondition condition)> LinkConditionCallback; /**< 服务连接情况回调*/
	typedef SetMemberAttributeCallback TempMuteMemberCallback; /**< 临时禁言/解禁回调*/
	typedef KickMemberCallback UpdateRoomInfoCallback; /**< 更新聊天室信息回调*/
	typedef KickMemberCallback UpdateMyRoomRoleCallback; /**< 更新我的信息回调*/
	typedef KickMemberCallback QueueOfferCallback; /**< 新加(更新)麦序队列元素回调*/
	typedef std::function<void(int64_t room_id, int error_code, const ChatRoomQueueElement& element)> QueuePollCallback; /**< 取出麦序队列元素回调*/
	typedef std::function<void(int64_t room_id, int error_code, const ChatRoomQueue& queue)> QueueListCallback; /**< 排序列出麦序队列所有元素回调*/
	typedef KickMemberCallback QueueDropCallback; /**< 删除麦序队列元素回调*/
	typedef QueuePollCallback QueueHeaderCallback; /**< 查看麦序队列头元素回调*/
	typedef std::function<void(int rescode, const RobotInfos& infos)> RobotQueryCallback;		/**< 获取机器人信息事件通知回调模板 */

public:
/** @fn void RegEnterCb(const EnterCallback& cb, const std::string& json_extension = "")
  * 注册全局登录回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegEnterCb(const EnterCallback& cb, const std::string& json_extension = "");

/** @fn void RegExitCb(const ExitCallback& cb, const std::string& json_extension = "")
  * 注册全局登出、被踢回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegExitCb(const ExitCallback& cb, const std::string& json_extension = "");

/** @fn void RegSendMsgAckCb(const SendMsgAckCallback& cb, const std::string& json_extension = "")
  * 注册全局发送消息回执回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegSendMsgAckCb(const SendMsgAckCallback& cb, const std::string& json_extension = "");

/** @fn void RegReceiveMsgCb(const ReceiveMsgCallback& cb, const std::string& json_extension = "")
  * 注册全局接收消息回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegReceiveMsgCb(const ReceiveMsgCallback& cb, const std::string& json_extension = "");

/** @fn void RegNotificationCb(const NotificationCallback& cb, const std::string& json_extension = "")
  * 注册全局接收通知回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegNotificationCb(const NotificationCallback& cb, const std::string& json_extension = "");

/** @fn void RegLinkConditionCb(const LinkConditionCallback& cb, const std::string& json_extension = "")
  * 注册全局聊天室链接情况回调
  * @param[in] cb			  回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void RegLinkConditionCb(const LinkConditionCallback& cb, const std::string& json_extension = "");

/** @fn void Init(const std::string& app_install_dir, const std::string& json_extension = "")
  * 聊天室模块初始化(SDK初始化时调用一次)
  * @param[in] app_install_dir SDK动态库所在的目录全路径（如果传入为空，则按照默认规则搜索该动态库）  
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return bool 模块加载结果
  */
static bool Init(const std::string& app_install_dir, const std::string& json_extension = "");

/** @fn void Cleanup(const std::string& json_extension = "")
  * 聊天室模块清理(SDK卸载前调用一次)
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void Cleanup(const std::string& json_extension = "");

#if NIMAPI_UNDER_WIN_DESKTOP_ONLY

/** @fn bool AnonymousEnter(const int64_t room_id, const ChatRoomAnoymityEnterInfo& anonymity_info, const ChatRoomEnterInfo& info, const std::string& json_extension = "")
  * 聊天室匿名进入
  * @param[in] room_id			  聊天室ID
  * @param[in] anonymity_info	  匿名登录相关信息
  * @param[in] enter_info		  聊天室进入信息
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return bool 进入信息是否正确,返回失败则不会触发进入回调
  */
static bool AnonymousEnter(const int64_t room_id, const ChatRoomAnoymityEnterInfo& anonymity_info, const ChatRoomEnterInfo& info, const std::string& json_extension = "");

#endif

/** @fn bool Enter(const int64_t room_id, const std::string& request_login_data, const ChatRoomEnterInfo& info = ChatRoomEnterInfo(), const std::string& json_extension = "")
  * 聊天室登录
  * @param[in] room_id			  聊天室ID
  * @param[in] request_login_data 聊天室登录信息(NIM SDK请求聊天室返回的数据)
  * @param[in] info				  聊天室可选信息
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return bool 登录信息是否正确,返回失败则不会促发登录回调
  */
static bool Enter(const int64_t room_id, const std::string& request_login_data, const ChatRoomEnterInfo& info = ChatRoomEnterInfo(), const std::string& json_extension = "");

/** @fn void Exit(const int64_t room_id, const std::string& json_extension = "")
  * 聊天室登出
  * @param[in] room_id			  聊天室ID
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void Exit(const int64_t room_id, const std::string& json_extension = "");

/** @fn NIMChatRoomLoginState GetLoginState(const int64_t room_id, const std::string& json_extension = "")
  * 获取聊天室登录状态
  * @param[in] room_id			  聊天室ID
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static NIMChatRoomLoginState GetLoginState(const int64_t room_id, const std::string& json_extension = "");

/** @fn void SendMsg(const int64_t room_id, const std::string& json_msg, const std::string& json_extension = "")
  * 发送消息
  * @param[in] room_id			  聊天室ID
  * @param[in] json_msg			  消息json string(可以通过CreateTextMessage生成)
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void SendMsg(const int64_t room_id, const std::string& json_msg, const std::string& json_extension = "");

/** @fn std::string CreateRoomMessage(const NIMChatRoomMsgType msg_type, const std::string& client_msg_id, const std::string& attach, const ChatRoomMessageSetting& msg_setting, int64_t timetag = 0)
  * 生成消息内容（所有支持的消息类型的内容）
  * @param[in] msg_type			消息类型
  * @param[in] client_msg_id	消息ID
  * @param[in] attach			消息内容（包含多媒体的消息类型，此处传入的是约定的可以解析为json的非格式化的字符串，如图片、文件消息等）
  * @param[in] msg_body			文本消息内容（目前只用到聊天室机器人文本消息）
  * @param[in] msg_setting		消息属性
  * @param[in] timetag			消息时间
  * @return std::string 消息json string
  */
static std::string CreateRoomMessage(const NIMChatRoomMsgType msg_type
	, const std::string& client_msg_id
	, const std::string& attach
	, const std::string& msg_body
	, const ChatRoomMessageSetting& msg_setting
	, int64_t timetag = 0);

/** @fn void GetMembersOnlineAsync(const int64_t room_id, const ChatRoomGetMembersParameters &parameters, const GetMembersCallback &callback, const std::string& json_extension = "")
  * 异步查询成员列表
  * @param[in] room_id			  聊天室ID
  * @param[in] parameters		  查询参数
  * @param[in] callback			  回调函数
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void GetMembersOnlineAsync(const int64_t room_id, 
	const ChatRoomGetMembersParameters &parameters, 
	const GetMembersCallback &callback,
	const std::string& json_extension = "");

/** @fn void GetMessageHistoryOnlineAsync(const int64_t room_id, const ChatRoomGetMembersParameters &parameters, const GetMembersCallback &callback, const std::string& json_extension = "")
  * 异步查询消息历史
  * @param[in] room_id			  聊天室ID
  * @param[in] parameters		  查询参数
  * @param[in] callback			  回调函数
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void GetMessageHistoryOnlineAsync(const int64_t room_id, 
	const ChatRoomGetMsgHistoryParameters &parameters, 
	const GetMsgHistoryCallback &callback,
	const std::string& json_extension = "");

/** @fn void SetMemberAttributeOnlineAsync(const int64_t room_id, const ChatRoomSetMemberAttributeParameters &parameters, const GetMembersCallback &callback, const std::string& json_extension = "")
  * 异步设置成员身份标识
  * @param[in] room_id			  聊天室ID
  * @param[in] parameters		  查询参数
  * @param[in] callback			  回调函数
  * @param[in] json_extension	  json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void SetMemberAttributeOnlineAsync(const int64_t room_id, 
	const ChatRoomSetMemberAttributeParameters &parameters, 
	const SetMemberAttributeCallback &callback,
	const std::string& json_extension = "");

/** @fn void GetInfoAsync(const int64_t room_id, const GetChatRoomInfoCallback& callback ,const std::string &json_extension)
  * 异步获取当前聊天室信息
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void GetInfoAsync(const int64_t room_id, 
	const GetChatRoomInfoCallback& callback ,
	const std::string &json_extension = "");

/** @fn void GetMemberInfoByIDsAsync(const int64_t room_id, const std::list<std::string>& ids, const GetMembersCallback& callback ,const std::string &json_extension)
  * 异步获取指定成员信息
  * @param[in] room_id				聊天室ID
  * @param[in] ids					
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void GetMemberInfoByIDsAsync(const int64_t room_id, 
	const std::list<std::string>& ids, 
	const GetMembersCallback& callback,
	const std::string &json_extension = "");

/** @fn void KickMemberAsync(const int64_t room_id, const std::string& id, const std::string& notify_ext, const KickMemberCallback &callback ,const std::string &json_extension)
  * 异步踢掉指定成员
  * @param[in] room_id				聊天室ID
  * @param[in] id					成员id
  * @param[in] notify_ext			放到事件通知中的扩展字段
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void KickMemberAsync(const int64_t room_id, 
	const std::string& id, 
	const std::string& notify_ext, 
	const KickMemberCallback &callback, 
	const std::string &json_extension = "");

/** @fn void SetProxy(NIMChatRoomProxyType type, const std::string& host, int port, const std::string& user, const std::string& password)
* 设置SDK统一的网络代理。不需要代理时，type设置为kNIMProxyNone，其余参数都传空字符串（端口设为0）。有些代理不需要用户名和密码，相应参数也传空字符串。
* @param[in] type 代理类型，见NIMChatRoomProxyType定义
* @param[in] host 代理地址
* @param[in] port 代理端口
* @param[in] user 代理用户名
* @param[in] password 代理密码
* @return void 无返回值
*/
static void SetProxy(NIMChatRoomProxyType type, 
	const std::string& host, 
	int				   port, 
	const std::string& user, 
	const std::string& password);

/** @fn static void TempMuteMemberAsync(const int64_t room_id, const std::string& accid, const int64_t duration, bool need_notify, std::string& notify_ext, const TempMuteMemberCallback& callback, const std::string &json_extension = "");
  * 异步临时禁言/解禁成员
  * @param[in] room_id				聊天室ID
  * @param[in] accid				成员ID
  * @param[in] duration				临时禁言时长（秒），解禁填0
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void TempMuteMemberAsync(const int64_t room_id
	, const std::string& accid
	, const int64_t duration
	, bool need_notify
	, const std::string& notify_ext
	, const TempMuteMemberCallback& callback
	, const std::string &json_extension = "");

/** @fn static void UpdateRoomInfoAsync(const int64_t room_id, const ChatRoomInfo& info, bool need_notify, std::string& notify_ext, const UpdateRoomInfoCallback& callback, const std::string &json_extension = "");
  * 更新聊天室信息
  * @param[in] room_id				聊天室ID
  * @param[in] info					聊天室信息
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void UpdateRoomInfoAsync(const int64_t room_id
	, const ChatRoomInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateRoomInfoCallback& callback
	, const std::string &json_extension = "");

/** @fn static void UpdateMyRoomRoleAsync(const int64_t room_id, const ChatRoomMemberInfo& info, bool need_notify, std::string& notify_ext, const UpdateMyRoomRoleCallback& callback, const std::string &json_extension = "");
  * 更新我的信息
  * @param[in] room_id				聊天室ID
  * @param[in] info					我的信息
  * @param[in] need_notify			是否聊天室内广播通知
  * @param[in] notify_ext			通知中的自定义字段，长度限制2048
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数，针对固定成员，可配置更新的信息是否需要持久化，默认不持久化，{"need_save" : false}
  * @return void 无返回值
  */
static void UpdateMyRoomRoleAsync(const int64_t room_id
	, const ChatRoomMemberInfo& info
	, bool need_notify
	, const std::string& notify_ext
	, const UpdateMyRoomRoleCallback& callback
	, const std::string &json_extension = "");

/** @fn static void QueueOfferAsync(const int64_t room_id, const ChatRoomQueueElement& element, const QueueOfferCallback& callback, const std::string &json_extension = "");
  * 新加(更新)麦序队列元素，如果element.key_对应的元素已经在队列中存在了，那就是更新操作，如果不存在，就放到队列尾部
  * @param[in] room_id				聊天室ID
  * @param[in] element				麦序队列元素
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void QueueOfferAsync(const int64_t room_id
	, const ChatRoomQueueElement& element
	, const QueueOfferCallback& callback
	, const std::string &json_extension = "{\"transient\":true}");

/** @fn static void QueuePollAsync(const int64_t room_id, const std::string& element_key, const QueuePollCallback& callback, const std::string &json_extension = "");
  * 取出麦序元素
  * @param[in] room_id				聊天室ID
  * @param[in] element_key			需要取出的元素的UniqKey, 传空传表示取出第一个元素
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void QueuePollAsync(const int64_t room_id
	, const std::string& element_key
	, const QueuePollCallback& callback
	, const std::string &json_extension = "");

/** @fn static void QueueListAsync(const int64_t room_id, const QueueListCallback& callback, const std::string &json_extension = "");
  * 排序列出所有元素
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void QueueListAsync(const int64_t room_id
	, const QueueListCallback& callback
	, const std::string &json_extension = "");

/** @fn static void QueueHeaderAsync(const int64_t room_id, const QueueHeaderCallback& callback, const std::string &json_extension = "");
  * 查看麦序头元素
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void QueueHeaderAsync(const int64_t room_id
	, const QueueHeaderCallback& callback
	, const std::string &json_extension = "");

/** @fn static void QueueDropAsync(const int64_t room_id, const QueueDropCallback& callback, const std::string &json_extension = "");
  * (管理员权限)删除麦序队列
  * @param[in] room_id				聊天室ID
  * @param[in] callback				回调函数
  * @param[in] json_extension		json扩展参数（备用，目前不需要）
  * @return void 无返回值
  */
static void QueueDropAsync(const int64_t room_id
	, const QueueDropCallback& callback
	, const std::string &json_extension = "");

/** @fn void UnregChatroomCb()
* 反注册Chatroom提供的所有回调
* @return void 无返回值
*/
static void UnregChatroomCb();

#if NIMAPI_UNDER_WIN_DESKTOP_ONLY

/** @fn char *QueryAllRobotInfosBlock(const int64_t room_id, const std::string &json_extension = "")
  * 获取全部机器人信息(同步接口，堵塞NIM内部线程)
  * @param[in] room_id				聊天室ID
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return char 机器人信息 json string array
  */
static RobotInfos QueryAllRobotInfosBlock(const int64_t room_id, const std::string &json_extension = "");

/** @fn char *QueryRobotInfoByAccidBlock(const int64_t room_id, const std::string &accid, const std::string &json_extension = "")
  * 获取指定机器人信息(同步接口，堵塞NIM内部线程)
  * @param[in] room_id				聊天室ID
  * @param[in] accid 机器人accid
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return char 机器人信息 json string
  */
static RobotInfo QueryRobotInfoByAccidBlock(const int64_t room_id, const std::string &accid, const std::string &json_extension = "");

/** @fn void GetRobotInfoAsync(const int64_t room_id, const __int64 timetag, const RobotQueryCallback &callback, const std::string &json_extension = "");
  * 获取机器人信息
  * @param[in] room_id				聊天室ID
  * @param[in] timetag 时间戳
  * @param[in] callback		回调函数
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @return  void
  */
static void GetRobotInfoAsync(const int64_t room_id, const int64_t timetag, const RobotQueryCallback &callback, const std::string &json_extension = "");

#endif
};
}
#endif //_NIM_CHATROOM_SDK_CPP_H_

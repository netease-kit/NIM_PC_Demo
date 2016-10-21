#pragma once

namespace nim_comp
{
/** @class TalkCallback
  * @brief 普通消息相关回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TalkCallback
{
public:
	/**
	* 发送消息的结果回调，需要在程序开始运行时就注册好。
	* @param[in] arc 发送消息回执
	* @return void	无返回值
	*/
	static void OnSendMsgCallback(const nim::SendMessageArc& arc);

	/**
	* 收到普通消息的回调，需要在程序开始运行时就注册好。
	* @param[in] message 接收到的消息内容及其他相关信息
	* @return void	无返回值
	*/
	static void OnReceiveMsgCallback(const nim::IMMessage& message);

	/**
	* 收到批量普通消息的回调，需要在程序开始运行时就注册好。
	* @param[in] messages 接收到的消息内容及其他相关信息
	* @return void	无返回值
	*/
	static void OnReceiveMsgsCallback(const std::list<nim::IMMessage>& messages);

	/**
	* 发送消息的已读结果回调，需要在程序开始运行时就注册好。
	* @param[in] res 发送消息已读回执
	* @return void	无返回值
	*/
	static void OnMsgStatusChangedCallback(const nim::MessageStatusChangedResult& res);

	/**
	* 发送自定义消息的结果回调，需要在程序开始运行时就注册好。
	* @param[in] arc 发送消息回执
	* @return void	无返回值
	*/
	static void OnSendCustomSysmsgCallback(const nim::SendMessageArc& arc);

	/**
	* 向sdk查询某个会话的消息历史的结果回调。
	* @param[in] code 返回码
	* @param[in] query_id 查询消息的会话id（群id或好友id）
	* @param[in] query_type 会话类型（P2P或群聊）
	* @param[in] result 查询到的消息历史结果
	* @return void	无返回值
	*/
	static void OnQueryMsgCallback(nim::NIMResCode code, const std::string& query_id, nim::NIMSessionType query_type, const nim::QueryMsglogResult& result);
	
	/**
	* 向sdk查询会话列表的结果回调。
	* @param[in] unread_count 未读消息条数
	* @param[in] session_list 会话列表数据
	* @return void	无返回值
	*/
	static void OnQuerySessionListCallback(int unread_count, const nim::SessionDataList& session_list);

	/**
	* 消息撤回的结果回调
	* @param[in] code 返回码
	* @param[in] message 撤回消息的通知列表
	* @return void 无返回值
	*/
	static void OnReceiveRecallMsgCallback(nim::NIMResCode code, const std::list<nim::RecallMsgNotify>& message);
};

}
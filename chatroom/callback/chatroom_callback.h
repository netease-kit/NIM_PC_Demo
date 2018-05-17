#pragma once
#include "nim_chatroom_helper.h"

namespace nim_chatroom
{
/** @class ChatroomCallback
  * @brief 聊天室相关回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author redrain
  * @date 2016/1/6
  */
class ChatroomCallback
{
public:

	/**
	* 注册聊天室所需回调函数
	* @return void	无返回值
	*/
	static void InitChatroomCallback();

	/**
	* 收到普通消息的回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] result 消息内容及其他相关信息
	* @return void	无返回值
	*/
	static void OnReceiveMsgCallback(__int64 room_id, const ChatRoomMessage& result);

	/**
	* 收到普通消息的回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] result 消息内容及其他相关信息
	* @return void	无返回值
	*/
	static void OnReceiveMsgsCallback(__int64 room_id, const std::list<ChatRoomMessage>& result);

	/**
	* 发送消息的结果回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] result 消息内容及其他相关信息
	* @return void	无返回值
	*/
	static void OnSendMsgCallback(__int64 room_id, int error_code, const ChatRoomMessage& result);

	/**
	* 进入某聊天室的结果回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] error_code 错误码
	* @param[in] info 聊天室相关信息
	* @return void	无返回值
	*/
	static void OnEnterCallback(__int64 room_id, const NIMChatRoomEnterStep step, int error_code, const ChatRoomInfo& info, const ChatRoomMemberInfo& my_info);

	/**
	* 退出某聊天室的结果回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] error_code 错误码
	* @param[in] exit_info 退出时的信息
	* @return void	无返回值
	*/
	static void OnExitCallback(__int64 room_id, int error_code, nim_chatroom::NIMChatRoomExitReasonInfo exit_info);

	/**
	* 收到聊天室通知的结果回调，需要在程序开始运行时就注册好。
	* @param[in] room_id 消息所属房间号
	* @param[in] notification 通知信息
	* @return void	无返回值
	*/
	static void OnNotificationCallback(__int64 room_id, const ChatRoomNotification& notification);

	/**
	* 注册全局聊天室链接情况回调
	* @param[in] room_id 消息所属房间号
	* @param[in] condition 聊天室链接情况，一般都是有本地网路情况引起
	* @return void 无返回值
	*/
	static void OnRegLinkConditionCallback(__int64 room_id, const NIMChatRoomLinkCondition condition);
};

}
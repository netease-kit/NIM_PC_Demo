#pragma once

namespace nim_comp
{
/** @class RtsCallback
  * @brief 白板消息事件回调类
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/13
  */
class RtsCallback
{
public:
	/**
	* 收到白板邀请的通知的回调
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型 如要tcp+音视频，则channel_type=kNIMRtsChannelTypeTcp|kNIMRtsChannelTypeVchat
	* @param[in] uid 对方帐号
	* @param[in] custom_info 扩展字段，若有透传数据见kNIMRtsCreateCustomInfo，如{"custom_info": "" }
	* @return void 无返回值
	*/
	static void StartNotifyCb(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info);

	/**
	* 收到对方回复邀请的通知
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	static void AckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);

	/**
	* 收到本人在其他端回复邀请的同步通知
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @return void 无返回值
	*/
	static void SyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);

	/**
	* 通道连接状态通知
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] code 状态码见NIMRtsConnectStatus
	* @param[in] json 成功时带录制的地址和文件名，服务器开启录制时有效
	* @return void 无返回值
	*/
	static void ConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);

	/**
	* 通道连接成员变化通知
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] code 成员变化类型见NIMRtsMemberStatus
	* @param[in] leave_type 成员离开类型见NIMRtsMemberLeftType
	* @return void 无返回值
	*/
	static void MemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code, int leave_type);

	/**
	* 挂断会话通知回调
	* @param[in] session_id 会话id
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	static void HangupNotifyCallback(const std::string& session_id, const std::string& uid);

	/**
	* 控制消息通知回调
	* @param[in] session_id 会话id
	* @param[in] info 透传数据
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	static void ControlNotifyCallback(const std::string& session_id, const std::string& info, const std::string& uid);

	/**
	* 数据监听回调
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] data 接受的数据
	* @return void 无返回值
	*/
	static void RecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);
};
}











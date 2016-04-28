/** @file nim_cpp_rts.h
  * @brief NIM Rts提供的相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/7/3
  */

#ifndef _NIM_SDK_CPP_RTS_H_
#define _NIM_SDK_CPP_RTS_H_

#include <string>
#include <functional>

/**
* @namespace nim
* @brief IM
*/
namespace nim
{

#include "nim_rts_def.h"
#include "nim_res_code_def.h"

/** @class Rts
  * @brief NIM Rts提供的相关接口
  */
class Rts
{
public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& session_id, int channel_type, const std::string& uid)> StartChannelCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info)> StartNotifyCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& session_id, int channel_type, bool accept)> AckCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, bool accept, const std::string& uid)> AckNotifyCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, bool accept)> SyncAckNotifyCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, int code, const std::string& json)> ConnectNotifyCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, const std::string& uid, int code)> MemberNotifyCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& session_id)> HangupCallback;
	typedef std::function<void(const std::string& session_id, const std::string& uid)> HangupNotifyCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& session_id, const std::string& info)> ControlCallback;
	typedef std::function<void(const std::string& session_id, const std::string& info, const std::string& uid)> ControlNotifyCallback;
	typedef std::function<void(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data)> RecDataCallback;

	//发起相关
	//NIM 创建rts会话，传入的JSON参数定义见nim_rts_def.h
	static void StartChannel(int channel_type, const std::string& uid, const std::string& apns, const std::string& custom_info, const StartChannelCallback& cb);

	//NIM 设置收到会话邀请的通知的回调
	static void SetStartNotifyCb(const StartNotifyCallback& cb);

	//NIM 回复收到的邀请
	static void Ack(const std::string& session_id, int channel_type, bool accept, const AckCallback& cb);

	//NIM 设置收到对方回复的通知回调
	static void SetAckNotifyCb(const AckNotifyCallback& cb);

	//NIM 设置收到本人其他端回复的同步通知回调
	static void SetSyncAckNotifyCb(const SyncAckNotifyCallback& cb);

	//状态回调相关
	//NIM 设置监听通道连接状态回调
	static void SetConnectNotifyCb(const ConnectNotifyCallback& cb);

	//NIM 设置监听通道成员状态回调
	static void SetMemberChangeCb(const MemberNotifyCallback& cb);

	//控制接口
	//NIM 会话控制（透传）
	static void Control(const std::string& session_id, const std::string& info, const ControlCallback& cb);

	//NIM 设置收到会话控制的通知回调
	static	void SetControlNotifyCb(const ControlNotifyCallback& cb);

	//NIM 设置音视频会话类型
	static void SetVChatMode(const std::string& session_id, int mode);

	//结束相关
	//NIM 结束会话
	static void Hangup(const std::string& session_id, const HangupCallback& cb);

	//NIM 设置结束会话通知回调
	static void SetHangupNotifyCb(const HangupNotifyCallback& cb);

	//数据相关
	//NIM 发送数据
	static void SendData(const std::string& session_id, int channel_type, const std::string& data);

	//NIM 设置监听数据接收回调
	static void SetRecDataCb(const RecDataCallback& cb);

};

}

#endif //_NIM_SDK_CPP_RTS_H_
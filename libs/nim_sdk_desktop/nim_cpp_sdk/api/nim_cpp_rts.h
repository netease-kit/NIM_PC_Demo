/** @file nim_cpp_rts.h
  * @brief NIM Rts提供的相关接口
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/7/3
  */

#ifndef _NIM_SDK_CPP_RTS_H_
#define _NIM_SDK_CPP_RTS_H_

#include <string>
#include <functional>

/**
* @namespace nim
* @brief namespace nim
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
	typedef std::function<void(nim::NIMResCode res_code)> CreateConfCallback;
	typedef std::function<void(nim::NIMResCode res_code, const std::string& session_id, __int64 channel_id, const std::string& custom_info)> JoinConfCallback;
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
	/** @fn void StartChannel(int channel_type, const std::string& uid, const std::string& apns, const std::string& custom_info, bool data_record, bool audio_record, const StartChannelCallback& cb)
	  * NIM 创建rts会话，传入的JSON参数定义见nim_rts_def.h    
	  * @param[in] channel_type 通道类型 如要tcp+音视频，则channel_type=kNIMRtsChannelTypeTcp|kNIMRtsChannelTypeVchat，同时整个SDK只允许一个音视频通道存在（包括vchat）
	  * @param[in] uid 对方帐号
	  * @param[in] apns json_extension Json string 扩展
	  * @param[in] custom_info json_extension Json string 扩展
	  * @param[in] data_record 服务器白板数据录制标记
	  * @param[in] audio_record 服务器音频数据录制标记
	  * @param[in] cb 结果回调见nim_rts_def.h
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  *				414:参数错误
	  *				501:数据库失败
	  *				509:通道失效
	  *				514:服务不可用 
	  *				11001:无可送达的被叫方,主叫方可直接挂断		
	  */ 
	static void StartChannel(int channel_type, const std::string& uid, const std::string& apns, const std::string& custom_info, bool data_record, bool audio_record, const StartChannelCallback& cb);

	/** @fn void SetStartNotifyCb(const StartNotifyCallback& cb)
	  * NIM 设置收到会话邀请的通知的回调   
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  *				404:对象不存在，发起记录不存在
	  *				509:通道失效
	  */ 
	static void SetStartNotifyCb(const StartNotifyCallback& cb);

	/** @fn void CreateConf(const std::string& name, const std::string& custom_info, const CreateConfCallback& cb)
	  * NIM VCHAT 创建一个多人数据通道房间（后续需要主动调用加入接口进入房间）
	  * @param[in] name 房间名
	  * @param[in] custom_info 自定义的房间信息（加入房间的时候会返回）
	  * @param[in] cb 结果回调见nim_rts_def.h，返回的json_extension无效
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  *				417:提示已经创建好频道
	  */
	static void CreateConf(const std::string& name, const std::string& custom_info, const CreateConfCallback& cb);

	/** @fn void JoinConf(const std::string& name, bool record, const JoinConfCallback& cb);
	  * NIM VCHAT 加入一个多人房间（进入房间后成员变化等，等同点对点nim_vchat_cb_func）
	  * @param[in] name 房间名
	  * @param[in] record json_extension 扩展可选参数kNIMRtsDataRecord， 如{"record":1}
	  * @param[in] cb 结果回调见nim_rts_def.h，回调的json_extension若成功返回创建的kNIMRtsCustomInfo及kNIMRtsChannelId，如{"channel_id": 1231, "custom_info":"hello world" }
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  */
	static void JoinConf(const std::string& name, bool record, const JoinConfCallback& cb);

	/** @fn void Ack(const std::string& session_id, int channel_type, bool accept, bool data_record, bool audio_record, const AckCallback& cb)
	  * NIM 回复收到的邀请  
	  * @param[in] session_id 会话id
	  * @param[in] channel_type 通道类型,暂时无效
	  * @param[in] accept 是否接受
	  * @param[in] data_record 服务器白板数据录制标记
	  * @param[in] audio_record 服务器音频数据录制标记
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void Ack(const std::string& session_id, int channel_type, bool accept, bool data_record, bool audio_record, const AckCallback& cb);

	/** @fn void SetAckNotifyCb(const AckNotifyCallback& cb)
	  * NIM 设置收到对方回复的通知回调
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  *				509:失效
	  */ 
	static void SetAckNotifyCb(const AckNotifyCallback& cb);

	/** @fn void SetSyncAckNotifyCb(const SyncAckNotifyCallback& cb)
	  * NIM 设置收到本人其他端回复的同步通知回调
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void SetSyncAckNotifyCb(const SyncAckNotifyCallback& cb);

	//状态回调相关
	/** @fn void SetConnectNotifyCb(const ConnectNotifyCallback& cb)
	  * NIM 设置监听通道连接状态回调
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void SetConnectNotifyCb(const ConnectNotifyCallback& cb);

	/** @fn void SetMemberChangeCb(const MemberNotifyCallback& cb)
	  * NIM 设置监听通道成员状态回调
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  * @note 错误码	200:成功
	  */ 
	static void SetMemberChangeCb(const MemberNotifyCallback& cb);

	//控制接口
	/** @fn void Control(const std::string& session_id, const std::string& info, const ControlCallback& cb)
	  * NIM 会话控制（透传），点对点有效
	  * @param[in] session_id 会话id
	  * @param[in] info 透传数据
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */
	static void Control(const std::string& session_id, const std::string& info, const ControlCallback& cb);

	/** @fn void SetControlNotifyCb(const ControlNotifyCallback& cb)
	  * NIM 设置收到会话控制的通知回调
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static	void SetControlNotifyCb(const ControlNotifyCallback& cb);

	/** @fn void SetVChatMode(const std::string& session_id, int mode)
	  * NIM 设置音视频会话类型  
	  * @param[in] session_id 会话id
	  * @param[in] mode 见NIMRtsVideoChatMode
	  * @return void 无返回值
	  */ 
	static void SetVChatMode(const std::string& session_id, int mode);

	//结束相关
	/** @fn void Hangup(const std::string& session_id, const HangupCallback& cb)
	  * NIM 结束会话  
	  * @param[in] session_id 会话id
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void Hangup(const std::string& session_id, const HangupCallback& cb);

	/** @fn void SetHangupNotifyCb(const HangupNotifyCallback& cb)
	  * NIM 设置结束会话通知回调，多人的时候结束后对方无通知。
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void SetHangupNotifyCb(const HangupNotifyCallback& cb);

	//数据相关
	/** @fn void SendData(const std::string& session_id, int channel_type, const std::string& data, const std::string& uid="")
	  * NIM 发送数据，暂时支持tcp通道，建议发送频率在20Hz以下
	  * @param[in] session_id 会话id
	  * @param[in] channel_type 通道类型, kNIMRtsChannelTypeVchat通道如果要自定义数据调用nim_device.h中nim_vchat_custom_audio_data和nim_vchat_custom_video_data
	  * @param[in] data 发送数据
	  * @param[in] size data的数据长度，现在只支持50k的长度
	  * @param[in] uid json_extension 可扩展kNIMRtsUid（指定发送某人，不填则群发）
	  * @return void 无返回值
	  */ 
	static void SendData(const std::string& session_id, int channel_type, const std::string& data, const std::string& uid="");

	/** @fn void SetRecDataCb(const RecDataCallback& cb)
	  * NIM 设置监听数据接收回调, kNIMRtsChannelTypeVchat通道要监听音视频数据，调用nim_device.h中nim_vchat_set_audio_data_cb和nim_vchat_set_video_data_cb
	  * @param[in] cb 见nim_rts_def.h
	  * @return void 无返回值
	  */ 
	static void SetRecDataCb(const RecDataCallback& cb);

};

}

#endif //_NIM_SDK_CPP_RTS_H_
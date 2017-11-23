#pragma once

#include "gui/rts/rts_form.h"

namespace nim_comp
{
/** @class RtsManager
  * @brief 白板管理器
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class RtsManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(RtsManager);
	RtsManager(){};
	~RtsManager(){};

	/**
	* 创建白板窗口
	* @param[in] channel_type 白板使用的通道
	* @param[in] uid 对方用户id
	* @param[in] session_id 会话id
	* @return bool true 成功，false 失败
	*/
	bool StartRtsForm(int channel_type, std::string uid, std::string session_id);

	/**
	* 判断白板窗口是否已经存在
	* @param[in] channel_type 白板使用的通道
	* @param[in] show 暂时无用
	* @return bool true 存在，false 不存在
	*/
	bool IsRtsFormExist(int channel_type, bool show);

	/**
	* 收到白板邀请的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型 如要tcp+音视频，则channel_type=kNIMRtsChannelTypeTcp|kNIMRtsChannelTypeVchat
	* @param[in] uid 对方帐号
	* @param[in] custom_info 扩展字段，若有透传数据见kNIMRtsCreateCustomInfo，如{"custom_info": "" }
	* @return void 无返回值
	*/
	void OnStartRtsNotify(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info);

	/**
	* 收到对方回复邀请的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnAckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);

	/**
	* 收到本人在其他端回复邀请的同步回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] accept 是否接受
	* @return void 无返回值
	*/
	void OnSyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);

	/**
	* 通道连接状态改变的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] code 状态码见NIMRtsConnectStatus
	* @param[in] json 成功时带录制的地址和文件名，服务器开启录制时有效
	* @return void 无返回值
	*/
	void OnConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);

	/**
	* 通道连接成员变化的回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] code 成员变化类型见NIMRtsMemberStatus
	* @param[in] leave_type 成员离开类型NIMRtsMemberLeftType
	* @return void 无返回值
	*/
	void OnMemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code, int leave_type);

	/**
	* 挂断会话通知回调函数
	* @param[in] session_id 会话id
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnHangupNotifyCallback(const std::string& session_id, const std::string& uid);

	/**
	* 控制消息通知回调函数
	* @param[in] session_id 会话id
	* @param[in] info 透传数据
	* @param[in] uid 对方帐号
	* @return void 无返回值
	*/
	void OnControlNotifyCallback(const std::string& session_id, const std::string& info, const std::string& uid);

	/**
	* 数据监听回调函数
	* @param[in] session_id 会话id
	* @param[in] channel_type 通道类型
	* @param[in] uid 对方帐号
	* @param[in] data 接受的数据
	* @return void 无返回值
	*/
	void OnRecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);

private:
	/**
	* 根据会话id查找对应的白板窗口
	* @param[in] session_id 会话id
	* @return WindowEx* 白板窗口指针
	*/
	WindowEx* GetRtsFromBySessionId(const std::string& session_id);

};
}
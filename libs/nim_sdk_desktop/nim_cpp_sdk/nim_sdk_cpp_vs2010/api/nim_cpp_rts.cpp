/** @file nim_cpp_rts.cpp
  * @brief NIM Rts提供的相关接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author gq
  * @date 2015/7/3
  */

#include "nim_cpp_rts.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"

namespace nim
{

//发起相关
typedef	void (*nim_rts_start)(int channel_type, const char* uid, const char *json_extension, nim_rts_start_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_start_notify_cb_func)(nim_rts_start_notify_cb_func cb, const void *user_data);
typedef void (*nim_rts_create_conf)(const char *name, const char *custom_info, const char *json_extension, nim_rts_create_cb_func cb, const void *user_data);
typedef void (*nim_rts_join_conf)(const char *name, const char *json_extension, nim_rts_join_cb_func cb, const void *user_data);
typedef	void (*nim_rts_ack)(const char *session_id, int channel_type, bool accept, const char *json_extension, nim_rts_ack_res_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_ack_notify_cb_func)(nim_rts_ack_notify_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_sync_ack_notify_cb_func)(nim_rts_sync_ack_notify_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_connect_notify_cb_func)(nim_rts_connect_notify_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_member_change_cb_func)(nim_rts_member_change_cb_func cb, const void *user_data);
typedef void (*nim_rts_control)(const char *session_id, const char* info, const char* json_extension, nim_rts_control_res_cb_func cb, const void *user_data);
typedef void (*nim_rts_set_control_notify_cb_func)(nim_rts_control_notify_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_vchat_mode)(const char *session_id, int mode, const char *json_extension);
typedef	void (*nim_rts_hangup)(const char *session_id, const char *json_extension, nim_rts_hangup_res_cb_func cb, const void *user_data);
typedef	void (*nim_rts_set_hangup_notify_cb_func)(nim_rts_hangup_notify_cb_func cb, const void *user_data);
typedef	void (*nim_rts_send_data)(const char *session_id, int channel_type, const char* data, unsigned int size, const char *json_extension);
typedef	void (*nim_rts_set_rec_data_cb_func)(nim_rts_rec_data_cb_func cb, const void *user_data);

void StartChannelCallbackWrapper(int code, const char *session_id, int channel_type, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::StartChannelCallback* cb_pointer = (Rts::StartChannelCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)code, PCharToString(session_id), channel_type, PCharToString(uid));
		}
		delete cb_pointer;
	}
}
void StartNotifyCallbackWrapper(const char *session_id, int channel_type, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::StartNotifyCallback* cb_pointer = (Rts::StartNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			std::string custom_info;
			std::string json(json_extension);
			Json::Value valus;
			Json::Reader reader;
			if (reader.parse(json, valus) && valus.isObject())
			{
				custom_info = valus[nim::kNIMRtsCreateCustomInfo].asString();
			}
			(*cb_pointer)(PCharToString(session_id), channel_type, PCharToString(uid), custom_info);
		}
	}
}
void CreateConfCallbackWrapper(int code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::CreateConfCallback* cb_pointer = (Rts::CreateConfCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)code);
		}
		delete cb_pointer;
	}
}
void JoinConfCallbackWrapper(int code, const char *session_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::JoinConfCallback* cb_pointer = (Rts::JoinConfCallback*)user_data;
		if (*cb_pointer)
		{
			__int64 channel_id = 0;
			std::string custom_info;
			Json::Value values;
			Json::Reader reader;
			std::string json(json_extension);
			if (reader.parse(json, values) && values.isObject())
			{
				channel_id = values[kNIMRtsChannelId].asInt64();
				custom_info = values[kNIMRtsCustomInfo].asString();
			}
			(*cb_pointer)((nim::NIMResCode)code, PCharToString(session_id), channel_id, custom_info);
		}
		delete cb_pointer;
	}
}
void AckCallbackWrapper(int code, const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::AckCallback* cb_pointer = (Rts::AckCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)code, PCharToString(session_id), channel_type, accept);
		}
		delete cb_pointer;
	}
}
void AckNotifyCallbackWrapper(const char *session_id, int channel_type, bool accept, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::AckNotifyCallback* cb_pointer = (Rts::AckNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), channel_type, accept, PCharToString(uid));
		}
	}
}
void SyncAckNotifyCallbackWrapper(const char *session_id, int channel_type, bool accept, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::SyncAckNotifyCallback* cb_pointer = (Rts::SyncAckNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), channel_type, accept);
		}
	}
}
void ConnectNotifyCallbackWrapper(const char *session_id, int channel_type, int code, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::ConnectNotifyCallback* cb_pointer = (Rts::ConnectNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), channel_type, code, PCharToString(json_extension));
		}
	}
}
void MemberNotifyCallbackWrapper(const char *session_id, int channel_type, int type, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::MemberNotifyCallback* cb_pointer = (Rts::MemberNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), channel_type, PCharToString(uid), type);
		}
	}
}
void HangupCallbackWrapper(int code, const char *session_id, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::HangupCallback* cb_pointer = (Rts::HangupCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)code, PCharToString(session_id));
		}
		delete cb_pointer;
	}
}
void HangupNotifyCallbackWrapper(const char *session_id, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::HangupNotifyCallback* cb_pointer = (Rts::HangupNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), PCharToString(uid));
		}
	}
}
void ControlCallbackWrapper(int code, const char *session_id, const char *info, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::ControlCallback* cb_pointer = (Rts::ControlCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)((nim::NIMResCode)code, PCharToString(session_id), PCharToString(info));
		}
		delete cb_pointer;
	}
}
void ControlNotifyCallbackWrapper(const char *session_id, const char *info, const char* uid, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::ControlNotifyCallback* cb_pointer = (Rts::ControlNotifyCallback*)user_data;
		if (*cb_pointer)
		{
			(*cb_pointer)(PCharToString(session_id), PCharToString(info), PCharToString(uid));
		}
	}
}
void RecDataCallbackWrapper(const char *session_id, int channel_type, const char* uid, const char* data, unsigned int size, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Rts::RecDataCallback* cb_pointer = (Rts::RecDataCallback*)user_data;
		if (*cb_pointer)
		{
			std::string data_temp(data, size);
			(*cb_pointer)(PCharToString(session_id), channel_type, PCharToString(uid), data_temp);
		}
	}
}

//发起相关
//NIM 创建rts会话，传入的JSON参数定义见nim_rts_def.h
void Rts::StartChannel(int channel_type, const std::string& uid, const std::string& apns, const std::string& custom_info, const StartChannelCallback& cb)
{
	StartChannelCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new StartChannelCallback(cb);
	}
	std::string json;
	Json::Value values_temp;
	values_temp[nim::kNIMRtsCreateCustomInfo] = custom_info;
	values_temp[nim::kNIMRtsApnsText] = apns;
	values_temp[nim::kNIMRtsDataRecord] = 1;
	Json::FastWriter fs;
	json = fs.write(values_temp);
	return NIM_SDK_GET_FUNC(nim_rts_start)(channel_type, uid.c_str(), json.c_str(), &StartChannelCallbackWrapper, cb_pointer);
}

//NIM 设置收到会话邀请的通知的回调
static Rts::StartNotifyCallback* g_start_notify_cb_pointer = nullptr;
void Rts::SetStartNotifyCb(const StartNotifyCallback& cb)
{
	delete g_start_notify_cb_pointer;
	if (cb)
	{
		g_start_notify_cb_pointer = new StartNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_start_notify_cb_func)(&StartNotifyCallbackWrapper, g_start_notify_cb_pointer);
}

//NIM 向服务器创建多人rts会话，实际加入会话还需要调用加入接口。
void Rts::CreateConf(const std::string& name, const std::string& custom_info, const CreateConfCallback& cb)
{
	CreateConfCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new CreateConfCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_create_conf)(name.c_str(), custom_info.c_str(), "", &CreateConfCallbackWrapper, cb_pointer);
}

//NIM 加入多人rts会话
void Rts::JoinConf(const std::string& name, bool record, const JoinConfCallback& cb)
{
	JoinConfCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new JoinConfCallback(cb);
	}
	std::string json;
	Json::Value values_temp;
	values_temp[nim::kNIMRtsDataRecord] = record ? 1 : 0;
	Json::FastWriter fs;
	json = fs.write(values_temp);
	return NIM_SDK_GET_FUNC(nim_rts_join_conf)(name.c_str(), json.c_str(), &JoinConfCallbackWrapper, cb_pointer);
}

//NIM 回复收到的邀请
void Rts::Ack(const std::string& session_id, int channel_type, bool accept, const AckCallback& cb)
{
	AckCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new AckCallback(cb);
	}
	std::string json;
	Json::Value values_temp;
	values_temp[nim::kNIMRtsDataRecord] = 1;
	Json::FastWriter fs;
	json = fs.write(values_temp);
	return NIM_SDK_GET_FUNC(nim_rts_ack)(session_id.c_str(), channel_type, accept, json.c_str(), &AckCallbackWrapper, cb_pointer);
}

//NIM 设置收到对方回复的通知回调
static Rts::AckNotifyCallback* g_ack_notify_cb_pointer = nullptr;
void Rts::SetAckNotifyCb(const AckNotifyCallback& cb)
{
	delete g_ack_notify_cb_pointer;
	if (cb)
	{
		g_ack_notify_cb_pointer = new AckNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_ack_notify_cb_func)(&AckNotifyCallbackWrapper, g_ack_notify_cb_pointer);
}

//NIM 设置收到本人其他端回复的同步通知回调
static Rts::SyncAckNotifyCallback* g_syncack_notify_cb_pointer = nullptr;
void Rts::SetSyncAckNotifyCb(const SyncAckNotifyCallback& cb)
{
	delete g_syncack_notify_cb_pointer;
	if (cb)
	{
		g_syncack_notify_cb_pointer = new SyncAckNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_sync_ack_notify_cb_func)(&SyncAckNotifyCallbackWrapper, g_syncack_notify_cb_pointer);
}

//状态回调相关
//NIM 设置监听通道连接状态回调
static Rts::ConnectNotifyCallback* g_connect_notify_cb_pointer = nullptr;
void Rts::SetConnectNotifyCb(const ConnectNotifyCallback& cb)
{
	delete g_connect_notify_cb_pointer;
	if (cb)
	{
		g_connect_notify_cb_pointer = new ConnectNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_connect_notify_cb_func)(&ConnectNotifyCallbackWrapper, g_connect_notify_cb_pointer);
}

//NIM 设置监听通道成员状态回调
static Rts::MemberNotifyCallback* g_member_notify_cb_pointer = nullptr;
void Rts::SetMemberChangeCb(const MemberNotifyCallback& cb)
{
	delete g_member_notify_cb_pointer;
	if (cb)
	{
		g_member_notify_cb_pointer = new MemberNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_member_change_cb_func)(&MemberNotifyCallbackWrapper, g_member_notify_cb_pointer);
}

//控制接口
//NIM 会话控制（透传）
void Rts::Control(const std::string& session_id, const std::string& info, const ControlCallback& cb)
{
	ControlCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new ControlCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_control)(session_id.c_str(), info.c_str(), "", &ControlCallbackWrapper, cb_pointer);
}

//NIM 设置收到会话控制的通知回调
static Rts::ControlNotifyCallback* g_control_notify_cb_pointer = nullptr;
void Rts::SetControlNotifyCb(const ControlNotifyCallback& cb)
{
	delete g_control_notify_cb_pointer;
	if (cb)
	{
		g_control_notify_cb_pointer = new ControlNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_control_notify_cb_func)(&ControlNotifyCallbackWrapper, g_control_notify_cb_pointer);
}

//NIM 设置音视频会话类型
void Rts::SetVChatMode(const std::string& session_id, int mode)
{
	return NIM_SDK_GET_FUNC(nim_rts_set_vchat_mode)(session_id.c_str(), mode, "");
}

//结束相关
//NIM 结束会话
void Rts::Hangup(const std::string& session_id, const HangupCallback& cb)
{
	HangupCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new HangupCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_hangup)(session_id.c_str(), "", &HangupCallbackWrapper, cb_pointer);
}

//NIM 设置结束会话通知回调
static Rts::HangupNotifyCallback* g_hangup_notify_cb_pointer = nullptr;
void Rts::SetHangupNotifyCb(const HangupNotifyCallback& cb)
{
	delete g_hangup_notify_cb_pointer;
	if (cb)
	{
		g_hangup_notify_cb_pointer = new HangupNotifyCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_hangup_notify_cb_func)(&HangupNotifyCallbackWrapper, g_hangup_notify_cb_pointer);
}

//数据相关
//NIM 发送数据
void Rts::SendData(const std::string& session_id, int channel_type, const std::string& data, const std::string& uid)
{
	std::string json;
	if (!uid.empty())
	{
		Json::Value values_temp;
		values_temp[nim::kNIMRtsUid] = uid;
		Json::FastWriter fs;
		json = fs.write(values_temp);
	}
	return NIM_SDK_GET_FUNC(nim_rts_send_data)(session_id.c_str(), channel_type, data.c_str(), data.size(), json.c_str());
}

//NIM 设置监听数据接收回调
static Rts::RecDataCallback* g_rec_data_cb_pointer = nullptr;
void Rts::SetRecDataCb(const RecDataCallback& cb)
{
	delete g_rec_data_cb_pointer;
	if (cb)
	{
		g_rec_data_cb_pointer = new RecDataCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_rts_set_rec_data_cb_func)(&RecDataCallbackWrapper, g_rec_data_cb_pointer);
}

}  // namespace nim

#include "rts_callback.h"
#include "module/rts/rts_manager.h"

namespace nim_comp
{
//callback----------------------
void RtsCallback::StartNotifyCb(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info)
{
	RtsManager::GetInstance()->OnStartRtsNotify(session_id, channel_type, uid, custom_info);
}
void RtsCallback::AckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid)
{
	RtsManager::GetInstance()->OnAckNotifyCallback(session_id, channel_type, accept, uid);
}
void RtsCallback::SyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept)
{
	RtsManager::GetInstance()->OnSyncAckNotifyCallback(session_id, channel_type, accept);
}
void RtsCallback::ConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json)
{
	RtsManager::GetInstance()->OnConnectNotifyCallback(session_id, channel_type, code, json);
}
void RtsCallback::MemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code, int leave_type)
{
	RtsManager::GetInstance()->OnMemberNotifyCallback(session_id, channel_type, uid, code, leave_type);
}
void RtsCallback::HangupNotifyCallback(const std::string& session_id, const std::string& uid)
{
	RtsManager::GetInstance()->OnHangupNotifyCallback(session_id, uid);
}
void RtsCallback::ControlNotifyCallback(const std::string& session_id, const std::string& info, const std::string& uid)
{
	RtsManager::GetInstance()->OnControlNotifyCallback(session_id, info, uid);
}
void RtsCallback::RecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data)
{
	RtsManager::GetInstance()->OnRecDataCallback(session_id, channel_type, uid, data);
}
}
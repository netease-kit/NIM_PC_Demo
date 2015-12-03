#pragma once

namespace nim_comp
{
class RtsCallback
{
public:
	//callback----------------------
	static void StartNotifyCb(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info);
	static void AckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);
	static void SyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);
	static void ConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);
	static void MemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code);
	static void HangupNotifyCallback(const std::string& session_id, const std::string& uid);
	static void ControlNotifyCallback(const std::string& session_id, const std::string& info, const std::string& uid);
	static void RecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);
};
}











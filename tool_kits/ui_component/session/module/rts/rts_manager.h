#pragma once

#include "gui/rts/rts_form.h"

namespace nim_comp
{
class RtsManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(RtsManager);

	RtsManager();
	~RtsManager();

	bool StartRtsForm(int channel_type, std::string uid, std::string session_id);
	bool IsRtsFormExist(int channel_type, bool show);
	void OnStartRtsNotify(const std::string& session_id, int channel_type, const std::string& uid, const std::string& custom_info);
	void OnAckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);
	void OnSyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);
	void OnConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);
	void OnMemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code);
	void OnHangupNotifyCallback(const std::string& session_id, const std::string& uid);
	void OnControlNotifyCallback(const std::string& session_id, const std::string& info, const std::string& uid);
	void OnRecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);

private:
	WindowEx* GetRtsFromBySessionId(const std::string& session_id);

};
}
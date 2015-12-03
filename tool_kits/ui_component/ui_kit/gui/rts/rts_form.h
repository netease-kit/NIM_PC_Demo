#pragma once

#include "util/window_ex.h"
#include "ui_board_control.h"
#include "shared/ui/msgbox.h"

namespace nim_comp
{
class RtsForm : public WindowEx
{
public:
	RtsForm(int type, std::string uid, std::string session_id);
	~RtsForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void InitWindow() override; 

	std::string GetSessionId() { return session_id_; }
	int GetType() { return type_; }

	void ShowStartUI(bool creater);
	void StartResult(nim::NIMResCode code, std::string session_id);

	//callback
	void OnAckNotifyCallback(const std::string& session_id, int channel_type, bool accept, const std::string& uid);
	void OnSyncAckNotifyCallback(const std::string& session_id, int channel_type, bool accept);
	void OnConnectNotifyCallback(const std::string& session_id, int channel_type, int code, const std::string& json);
	void OnMemberNotifyCallback(const std::string& session_id, int channel_type, const std::string& uid, int code);
	void OnHangupNotifyCallback(const std::string& session_id, const std::string& uid);
	void OnRecDataCallback(const std::string& session_id, int channel_type, const std::string& uid, const std::string& data);
	void OnControlNotify(const std::string& session_id, const std::string& uid, const std::string& text);

private:
	bool Notify(ui::EventArgs* msg);
	bool OnClicked(ui::EventArgs* msg);
	void OnStartRtsCb(std::string& old_id, nim::NIMResCode res_code, const std::string& session_id, int channel_type, const std::string& uid);
	void OnAckCallback(nim::NIMResCode res_code, const std::string& session_id, int channel_type, bool accept);

	void ShowBoardUI();
	void ShowHeader();
	void DelayClose();
	void NoActiveTimer();

	void ShowTip(std::wstring text);

	void OnDrawCallback(ui::DrawOpInfo info);
	void SendCurData();
	void OnParseTcpData(std::string data);

	bool InitAudio();
	void FreeAudio();
	bool InitMic();
	void FreeMic();

	void OnBtnClose();
	void OnQuitMsgBox(MsgBoxRet ret);

	void SetAudioStatus(bool show);

	void SendCreateMsg();
	void ShowEndMsg();
	void HideCtrlNotifyTip();


public:
	static const LPCTSTR kClassName; // 类名

private:
	bool show_endmsg_;
	bool need_ack_;
	bool talking_;
	bool closing_;
	int type_;
	std::string uid_;
	std::string session_id_;
	std::wstring uuid_;
	std::string cur_buffer_;

	ui::Label* chat_status_;
	ui::Label* chat_status2_;
	ui::Label* ctrl_notify_;
	ui::BoardControl* board_;
	ui::CheckBox* speak_;
	ui::ShowOpType show_type_;

};
}
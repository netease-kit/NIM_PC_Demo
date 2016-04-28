#ifndef NIM_WIN_GUI_VIDEO_FORM_H_
#define NIM_WIN_GUI_VIDEO_FORM_H_

#include "shared/base.h"
#include "util/window_ex.h"
//#include "video_setting.h"
//#include "video_mission.h"
#include "shared/ui/msgbox.h"
#include "module/session/ring.h"
#include "ui_bitmap_control.h"

namespace nim_comp
{

const int kDialTimeOut = 45;
const int kAnswerTimeOut = 60;
const int kConnectTimeOut = 17;

class VideoForm : public WindowEx
{
	friend class VideoManager;
public:
	//拨打界面，接听界面，视频界面
	enum StatusPage { SP_DIAL, SP_CONFIRM, SP_VIDEO };
	//各种状态
	enum StatusEnum 
	{ 
		STATUS_NONE, 
		STATUS_CALLING, STATUS_WAITING, //拨打
		STATUS_INVITING, STATUS_CONNECTING, //接听
		STATUS_TALKING, //通话
		STATUS_NO_RESPONSE, STATUS_BUSYING, STATUS_REJECT 
	};
	//结束通话原因
	enum EndCallEnum
	{
		END_CALL_NONE, 
		END_CALL_CLOSE, 
		END_CALL_HANGUP, 
		END_CALL_BE_HANGUP, 
		END_CALL_SYNC_ACCEPT, 
		END_CALL_SYNC_REFUSE,
		END_CALL_BAD_MICROPHONE, 
		END_CALL_VERSION, 
		END_CALL_CONNECTION,
		END_CALL_RESPONSE, 
		END_CALL_BUSYING, 
		END_CALL_REJECT,
		END_CALL_STARTFAIL,
	};
	enum VideoChatMsgStateType
	{
		kVideoChatMsgStateSuccess		= 0, //完美对话
		kVideoChatMsgStateHangup1		= 1, //我发起，我挂断：   无人接听
		kVideoChatMsgStateHangup2		= 2, //我发起，对方挂断： 对方正忙
		kVideoChatMsgStateHangup3		= 3, //对方发起，我挂断： 未接听
	};
public:
	VideoForm(std::string session_id);
	virtual ~VideoForm();
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnFinalMessage(HWND hWnd) override;
	virtual void InitWindow() override; 

	void ShowStatusPage(StatusPage sp);
	void AdjustWindowSize( bool video );
	void AllowWindowMaxsize( bool allow );

	void OnAutoCloseWnd();
	void OnCloseRingEnd( const int32_t mode ); //通话结束时的播放铃声
	void EnterEndCallPage( EndCallEnum why );
	void BeforeClose();
	void OnQuitMsgBox(MsgBoxRet ret);

	void DirectQuit();

	std::string GetSessionId(){	return session_id_; }
	//void ShowPosition( tool::WindowEx* session );
	void StartCalling(bool video_mode);
	void StartReceiving(uint64_t channel_id, bool video);
	void PrepareQuit(); //退出前的清理操作
	void OnRepeatRingSend( const int32_t mode );  //语音拨打，先“正在为您接通”，然后重复“叮叮叮”
	void SwitchStatus(StatusEnum status);
	void IntoVideo();
	void IntoAudio();

	void StartTalking(/*const nbiz::VideoChatStartCallbackParam& msg*/);

	void CheckTitle();
	void CheckHeadIcon();
	void CheckFriendName();

	bool  CheckAudioInputDevice();
	bool  CheckAudioOutputDevice( );
	int   CheckVideoInputDevice(); //0=有+开；1=有+没开；2=无
	void  InitSetting();
	void  InitVolumnSetting();

	void ClearBitmapControl( bool mine );
	//是否开启了通话
	bool IsStart(){ return is_start_; }
public:
	//virtual void AddListenEvents();
	//virtual void OnLocalSettingChange(const UTF8String &item_id);
	void OnAudioInDeviceChange(uint32_t status);
	void OnVideoInDeviceChange(uint32_t status);
private:	//ui
	bool Notify(::ui::EventArgs* msg);
	bool OnClicked(::ui::EventArgs* arg);
	bool OnSelect(::ui::EventArgs* arg);
	bool OnMouseMessage(::ui::EventArgs* arg);
	void OnMaxWindow(bool _max);
	bool OnMicrophoneVolumnKillFocus(::ui::EventArgs* arg);
	bool OnSpeakerVolumnKillFocus(::ui::EventArgs* arg);
private:	//logic
	bool InitVideo();
	void FreeVideo();

	bool InitAudio();
	void FreeAudio();

	//void OnVideoData(const std::string &data, ::ui::CSize size, uint64_t timestamp);
	//void OnRecVideo(const std::string &data, ::ui::CSize size, uint64_t timestamp);

	void StartDialWaitingTimer();

	void StartChat();
	void ChatStartCallback(bool success, int64_t channel_id);
	void ChangeToVideo();
	void ChangeToAudio();
	void SendControlCommand(bool audio, bool open);
	void OnControlModeChange(int64_t channel_id, nim::NIMVChatControlType type);
public:
	void OnRejected(int64_t channel_id);  //被对方拒绝
	void OnVideochatSyncNotify( bool accept, int64_t channel_id ); //其他端接收或拒绝后通知pc端

	void OnComeIn(uint64_t id);
	void OnComeOut(uint64_t id);
	void OnHangup(uint64_t channel_id);
	void EndSession();

	void OnNetstatChange(uint16_t stat);

	void OnLogin(bool success); //连接数据服务器
	void OnAudioDeviceStartCallback(bool result);
	void OnVideoDeviceStartCallback(bool result);

	void OnDialTimeOut();
	void OnAnswerTimeOut();
	void OnConnectTimeOut();

	std::wstring GetTickTime(long &sec);
	void OnTick();

	void ShowVideoTip( const std::wstring stringID ); 
	void OnVideoAudioTip();

	void OnMissionCallback(MsgBoxRet ret); //语音转视频的请求msg box回调处理

	void OnRecordMp4SelFileCb(BOOL ret, std::wstring path);
	//开始录制回调
	void StartRecordCb(bool ret, int code, const std::string& file, __int64 time);
	//结束录制回调，不需要处理，在结束的通知里处理即可
	void StopRecordCb(bool ret, int code, const std::string& file, __int64 time);
	//录制开始通知，正式开始录制数据
	void OnStartRecord(const std::string& file, __int64 time);
	//录制结束通知
	void OnStopRecord(int code, const std::string& file, __int64 time);

	void ShowRecordTip(std::wstring tip = L"", std::wstring tip2 = L"", std::wstring path = L"");
	void HideRecordTipTime();

	void CheckRecordDiskSpace(const std::wstring& file);

	void PaintVideo();
public:
	static const LPCTSTR kClassName; // 类名
private:
	::ui::Box*	status_page_;
	::ui::Box*	video_page_;
	::ui::Box*	volumn_page_;
	::ui::Box*	confirm_page_;
	
	::ui::Button*		headicon_btn_;
	::ui::Label*		friend_label_;
	::ui::Label*		status_label_;

	ui::CBitmapControl* video_ctrl_screen_;  // 视频渲染窗口，大窗口
	ui::CBitmapControl* video_ctrl_preview_; // 视频渲染窗口，小窗口
	::ui::Label*			time_tick_label_;    //视频窗口的计时
	::ui::Control*		video_border_ctrl_;
	::ui::Button*			swap_photo_btn_;

	::ui::Label*			camera_open_label_;  //等待对方开启摄像头
	::ui::Label*			camera_closed_label_; //对方关闭了摄像头

	::ui::Control*	camera_page_tip_;
	::ui::Control*	camera_page_tip2_;

	::ui::Button*		speaker_btn_;
	::ui::Button*		microphone_btn_;
	::ui::CheckBox*	camera_checkbox_;
	::ui::Button*		start_record_btn_;
	::ui::Button*		stop_record_btn_;

	::ui::Slider*		input_volumn_slider_;
	::ui::VBox*		vbox_of_input_;
	::ui::Control*	place_ctrl_1_;

	::ui::Slider*		output_volumn_slider_;
	::ui::VBox*		vbox_of_output_;
	::ui::Control*	place_ctrl_2_;

	::ui::Button*		into_video_btn_;
	::ui::Button*		into_audio_btn_;
	::ui::Button*		stop_chat_btn_;
	
	::ui::Button*		refuse_btn_;
	::ui::Button*		complete_btn_;

	::ui::Label*		netstat_label_;
	::ui::HBox*		hbox_camera_fail_;

	::ui::Box*		vbox_video_audio_tip_;

	::ui::Box*		record_tip_box_;
	::ui::Label*	record_tip_label0_;
	::ui::Label*	record_tip_label1_;
	::ui::Label*	record_tip_label2_;
	::ui::Label*	record_tip_label3_;
	::ui::Label*	recording_tip_label_;

	std::string		session_id_;
	StatusEnum		status_;
	bool			current_video_mode_; //当前聊天模式，true为视频，false为语音
	int64_t			channel_id_;

	EndCallEnum		end_call_;

	bool	screen_is_other_; //视频对调画面
	bool	camera_is_open_;  //摄像头是否主动关闭
	bool	camera_is_open_other_; //对方摄像头是否关闭

	bool	need_change_form_size_; //视频通话在接通以后调整窗口大小

	uint32_t  time_stamp_;
	nbase::WeakCallbackFlag tick_weakflag_; //通话过程计时

	nbase::WeakCallbackFlag dial_timeout_timer_;
	nbase::WeakCallbackFlag answer_timeout_timer_;
	nbase::WeakCallbackFlag connect_timeout_timer_;
	nbase::WeakCallbackFlag record_tip_timer_;
	nbase::WeakCallbackFlag record_check_disk_space_timer_;
	nbase::WeakCallbackFlag paint_video_timer_;

	bool is_self_; //是否是主动发起，true表示“是”

	bool send_camera_notify_; //视频聊天默认对方都是开启的，所以第一次开启不用告诉对方
	bool is_mode_changing_; //是否正处于 语音切视频模式

	//OnTimeTickNotify time_notify_;
	//std::weak_ptr<nbase::WeakFlag> time_weak_flag_;

	bool	  is_max_window_;

	Ring	  voip_ring_;

	bool	mic_in_open_;
	bool	speaker_out_open_;
	bool	mp4_recording_;
	bool	is_start_;
};
}
#endif  // NIM_WIN_GUI_VIDEO_FORM_H_

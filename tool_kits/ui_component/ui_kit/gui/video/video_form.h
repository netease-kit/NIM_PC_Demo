#ifndef NIM_WIN_GUI_VIDEO_FORM_H_
#define NIM_WIN_GUI_VIDEO_FORM_H_

#include "shared/base.h"
#include "util/window_ex.h"
//#include "video_setting.h"
//#include "video_mission.h"
#include "shared/ui/msgbox.h"
#include "module/session/ring.h"
#include "helper/ui_bitmap_control.h"

namespace nim_comp
{

const int kDialTimeOut = 45;
const int kAnswerTimeOut = 60;
const int kConnectTimeOut = 17;

/** @class VideoForm
  * @brief 音视频聊天窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author gaoqi
  * @date 2016/09/21
  */
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
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 处理窗口被销毁的消息
	* @param[in] hWnd 窗口句柄
	* @return void	无返回值
	*/
	virtual void OnFinalMessage(HWND hWnd) override;

	/**
	* 根据控件类名创建自定义控件
	* @param[in] pstrClass 控件类名
	* @return Control* 创建的控件的指针
	*/
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;

	/**
	* 设置显示的页面
	* @param[in] sp 页面
	* @return void	无返回值
	*/
	void ShowStatusPage(StatusPage sp);

	/**
	* 调整窗口大小
	* @param[in] video 是否需要视频功能
	* @return void	无返回值
	*/
	void AdjustWindowSize( bool video );

	/**
	* 设置是否允许最大化
	* @param[in] allow 是否允许
	* @return void	无返回值
	*/
	void AllowWindowMaxsize( bool allow );

	/**
	* 响应自动关闭窗口的回调函数
	* @return void	无返回值
	*/
	void OnAutoCloseWnd();

	/**
	* 通话结束时的播放铃声
	* @param[in] mode MCI_MODE类型
	* @return void	无返回值
	*/
	void OnCloseRingEnd( const int32_t mode );

	/**
	* 显示结束通话页面
	* @param[in] why 结束原因
	* @return void	无返回值
	*/
	void EnterEndCallPage( EndCallEnum why );

	/**
	* 用户请求关闭窗口时的提示询问
	* @return void	无返回值
	*/
	void BeforeClose();

	/**
	* 关闭窗口的提示窗的回调函数
	* @param[in] ret 提示窗返回值
	* @return void	无返回值
	*/
	void OnQuitMsgBox(MsgBoxRet ret);

	/**
	* 直接关闭窗口
	* @return void	无返回值
	*/
	void DirectQuit();

	/**
	* 获取会话id
	* @return std::string 会话id
	*/
	std::string GetSessionId(){	return session_id_; }
	
	/**
	* 开启请求通话
	* @param[in] video_mode 是否包含视频功能
	* @return void	无返回值
	*/
	void StartCalling(bool video_mode);

	/**
	* 收到通话请求
	* @param[in] channel_id 通道id
	* @param[in] video 是否包含视频功能
	* @return void	无返回值
	*/
	void StartReceiving(uint64_t channel_id, bool video);

	/**
	* 退出前的清理操作
	* @return void	无返回值
	*/
	void PrepareQuit();

	/**
	* 语音拨打，先“正在为您接通”，然后重复“叮叮叮”
	* @param[in] mode MCI_MODE值
	* @return void	无返回值
	*/
	void OnRepeatRingSend( const int32_t mode );

	/**
	* 切换到某个状态的界面显示效果
	* @param[in] status 状态值
	* @return void	无返回值
	*/
	void SwitchStatus(StatusEnum status);

	/**
	* 进入视频通话界面
	* @return void	无返回值
	*/
	void IntoVideo();

	/**
	* 进入语音通话界面
	* @return void	无返回值
	*/
	void IntoAudio();

	/*void StartTalking(const nbiz::VideoChatStartCallbackParam& msg);*/

	/**
	* 设置窗口标题
	* @return void	无返回值
	*/
	void CheckTitle();

	/**
	* 设置头像
	* @return void	无返回值
	*/
	void CheckHeadIcon();

	/**
	* 设置好友名字
	* @return void	无返回值
	*/
	void CheckFriendName();

	/**
	* 检查视频设备状态
	* @return int 0=有+开；1=有+没开；2=无
	*/
	int   CheckVideoInputDevice();
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
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 处理所有控件选中消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelect(::ui::EventArgs* arg);

	/**
	* 处理所有控件鼠标移入移出消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMouseMessage(::ui::EventArgs* arg);

	/**
	* 处理窗体最大化消息
	* @param[in] max 是否最大化
	* @return void 无返回值
	*/
	void OnMaxWindow(bool max);

	/**
	* 处理语音输入音量控件失去焦点消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMicrophoneVolumnKillFocus(::ui::EventArgs* arg);

	/**
	* 处理语音输出音量控件失去焦点消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSpeakerVolumnKillFocus(::ui::EventArgs* arg);
private:
	/**
	* 初始化视频设备
	* @return bool true 成功，false 失败
	*/
	bool InitVideo();

	/**
	* 结束视频设备
	* @return void	无返回值
	*/
	void FreeVideo();

	/**
	* 初始化音频设备
	* @return bool true 成功，false 失败
	*/
	bool InitAudio();

	/**
	* 结束音频设备
	* @return void	无返回值
	*/
	void FreeAudio();

	//void OnVideoData(const std::string &data, ::ui::CSize size, uint64_t timestamp);
	//void OnRecVideo(const std::string &data, ::ui::CSize size, uint64_t timestamp);

	/**
	* 设置等待通话的超时响应函数
	* @return void	无返回值
	*/
	void StartDialWaitingTimer();

	/**
	* 开始音视频通话
	* @return void	无返回值
	*/
	void StartChat();

	/**
	* 开始语音聊天的回调函数
	* @param[in] success 是否成功开启
	* @param[in] channel_id 通道id
	* @return void	无返回值
	*/
	void ChatStartCallback(bool success, int64_t channel_id);

	/**
	* 切换到视频通话
	* @return void	无返回值
	*/
	void ChangeToVideo();

	/**
	* 切换到音频通话
	* @return void	无返回值
	*/
	void ChangeToAudio();

	/**
	* 发送开启音视频功能的控制命令
	* @param[in] audio 是否发送语音命令
	* @param[in] open 开启或者关闭功能
	* @return void	无返回值
	*/
	void SendControlCommand(bool audio, bool open);

	/**
	* 响应音视频通话控制命令
	* @param[in] channel_id 通道id
	* @param[in] type 音视频通话控制类型
	* @return void	无返回值
	*/
	void OnControlModeChange(int64_t channel_id, nim::NIMVChatControlType type);
public:
	/**
	* 被对方拒绝
	* @param[in] channel_id 通道id
	* @return void	无返回值
	*/
	void OnRejected(int64_t channel_id);

	/**
	* 其他端接收或拒绝后通知pc端
	* @param[in] accept 是否接收
	* @param[in] channel_id 通道id
	* @return void	无返回值
	*/
	void OnVideochatSyncNotify( bool accept, int64_t channel_id );

	/**
	* 响应音视频通话控制命令，有成员进入
	* @param[in] id 用户id
	* @return void	无返回值
	*/
	void OnComeIn(uint64_t id);

	/**
	* 响应音视频通话控制命令，有成员退出
	* @param[in] id 用户id
	* @return void	无返回值
	*/
	void OnComeOut(uint64_t id);

	/**
	* 响应音视频通话控制命令，挂断通话
	* @param[in] channel_id 通道id
	* @return void	无返回值
	*/
	void OnHangup(uint64_t channel_id);

	/**
	* 结束会话时被调用
	* @return void	无返回值
	*/
	void EndSession();

	/**
	* 响应音视频通话控制命令，网络状态改变
	* @param[in] stat 状态吗
	* @return void	无返回值
	*/
	void OnNetstatChange(uint16_t stat);

	/**
	* 响应音视频通话控制命令，连接数据服务器
	* @param[in] success 是否连接成功
	* @return void	无返回值
	*/
	void OnLogin(bool success);

	/**
	* 响应启用音频设备的回调函数
	* @param[in] result 是否成功
	* @return void	无返回值
	*/
	void OnAudioDeviceStartCallback(bool result);

	/**
	* 响应启用视频设备的回调函数
	* @param[in] result 是否成功
	* @return void	无返回值
	*/
	void OnVideoDeviceStartCallback(bool result);

	/**
	* 等待通话超时的回调函数
	* @return void	无返回值
	*/
	void OnDialTimeOut();

	/**
	* 接收通话超时的回调函数
	* @return void	无返回值
	*/
	void OnAnswerTimeOut();

	/**
	* 链接通话超时的回调函数
	* @return void	无返回值
	*/
	void OnConnectTimeOut();

	/**
	* 得到当前时间
	* @param[in] sec 当前时间戳
	* @return wstring	格式化为字符串的时间
	*/
	std::wstring GetTickTime(long &sec);

	/**
	* 定时更新界面时间的回调函数
	* @return void	无返回值
	*/
	void OnTick();

	/**
	* 修改界面视频提示
	* @param[in] stringID 字符串id
	* @return void	无返回值
	*/
	void ShowVideoTip( const std::wstring stringID ); 

	/**
	* 自动隐藏视频提示的回调函数
	* @return void	无返回值
	*/
	void OnVideoAudioTip();

	/**
	* 语音转视频的请求提示框的回调处理
	* @param[in] ret 提示框返回值
	* @return void	无返回值
	*/
	void OnMissionCallback(MsgBoxRet ret);

	/**
	* 选择录制保存路径的回调处理
	* @param[in] ret 是否选择了路径
	* @param[in] mp4_path 保存MP4路径
	* @param[in] audio_path 保存音频路径
	* @return void	无返回值
	*/
	void OnRecordSelFileCb(BOOL ret, std::wstring mp4_path, std::wstring audio_path);

	/**
	* 开始录制的回调
	* @param[in] mp4 是否是MP4录制
	* @param[in] ret 是否成功
	* @param[in] code 视频录制错误码
	* @param[in] file 保存路径
	* @param[in] time 开始录制事件
	* @return void	无返回值
	*/
	void StartRecordCb(bool mp4, bool ret, int code, const std::string& file, __int64 time);

	/**
	* 结束录制的回调，不需要处理，在结束的通知里处理即可
	* @param[in] mp4 是否是MP4录制
	* @param[in] ret 是否成功
	* @param[in] code 视频录制错误码
	* @param[in] file 保存路径
	* @param[in] time 结束录制事件
	* @return void	无返回值
	*/
	void StopRecordCb(bool mp4, bool ret, int code, const std::string& file, __int64 time);

	/**
	* 结束录制的通知调，正式开始录制数据
	* @param[in] mp4 是否是MP4录制
	* @param[in] file 保存路径
	* @param[in] time 开始录制事件
	* @return void	无返回值
	*/
	void OnStartRecord(bool mp4, const std::string& file, __int64 time);

	/**
	* 结束录制的通知
	* @param[in] mp4 是否是MP4录制
	* @param[in] code 视频录制错误码
	* @param[in] file 保存路径
	* @param[in] time 结束录制事件
	* @return void	无返回值
	*/
	void OnStopRecord(bool mp4, int code, const std::string& file, __int64 time);

	/**
	* 显示录制视频时的提示
	* @param[in] tip 提示内容
	* @param[in] tip2 提示内容2
	* @param[in] path 提示内容3
	* @return void	无返回值
	*/
	void ShowRecordTip(std::wstring tip = L"", std::wstring tip2 = L"", std::wstring path = L"");

	/**
	* 隐藏录制视频提示的回调函数
	* @return void	无返回值
	*/
	void HideRecordTipTime();

	/**
	* 检测录制视频文件所在磁盘的空间
	* @param[in] file 文件路径
	* @return void	无返回值
	*/
	void CheckRecordDiskSpace(const std::wstring& file);

	/**
	* 绘制视频界面
	* @return void	无返回值
	*/
	void PaintVideo();

	/**
	* 设置自定义数据模式
	* @param[in] open 开启或关闭
	* @return void	无返回值
	*/
	void SetCustomVideoMode(bool open);
	/**
	* 将自定义数据处理后给底层发送
	* @return void	无返回值
	*/
	void SendCustomVideo();
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

	ui::BitmapControl* video_ctrl_screen_;  // 视频渲染窗口，大窗口
	ui::BitmapControl* video_ctrl_preview_; // 视频渲染窗口，小窗口
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
	::ui::Button*		face_open_btn_;
	::ui::Button*		face_close_btn_;

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
	bool			custom_video_mode_;//使用自定义数据模式，做数据处理用
	VideoFrameMng	video_frame_mng_;//使用自定义数据模式，做数据处理后，数据保存用于预览
	int64_t			channel_id_;
	nbase::NLock	capture_lock_;

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
	nbase::WeakCallbackFlag send_custom_video_;

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

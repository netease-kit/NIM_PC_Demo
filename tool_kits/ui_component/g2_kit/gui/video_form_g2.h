#pragma once

#include "shared/base.h"
// #include "video_setting.h"
#include "video_canvas_g2.h"
#include "shared/ui/msgbox.h"
#include "shared/ring.h"
#include "bitmap_control_g2.h"
#include "shared/business_action_gateway/business_manager/business_manager.h"
#include "nim_cpp_wrapper/helper/nim_talk_helper.h"
//#include "helper/ui_bitmap_control.h"
//#include "record_select.h"

namespace nim_comp
{

	//const int kDialTimeOut = 45;
	//const int kAnswerTimeOut = 60;
	//const int kConnectTimeOut = 17;

	/** @class VideoFormG2
	  * @brief 音视频聊天窗口,基于G2改造, 音视频通话使用信令SDK+G2
	  * @copyright (c) 2020, NetEase Inc. All rights reserved
	  * @author Martin
	  * @date 2020/11/02
	  */
	class VideoFormG2 : public ui::WindowImplBase
	{
		//friend class VideoManagerG2;
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
			END_CALL_TOAUDIO,
			END_CALL_OTHER_CLIENT_ACCEPT,
			END_CALL_OTHER_CLIENT_REJECT,
		};
		enum VideoChatMsgStateType
		{
			kVideoChatMsgStateSuccess = 0, //完美对话
			kVideoChatMsgStateHangup1 = 1, //我发起，我挂断：   无人接听
			kVideoChatMsgStateHangup2 = 2, //我发起，对方挂断： 对方正忙
			kVideoChatMsgStateHangup3 = 3, //对方发起，我挂断： 未接听
		};
		enum CallType {
			kVideoToAudio = 0,  //视频转音频
			kAudioToVideo = 1,  //音频转视频
		};
	public:
		VideoFormG2(const std::string& session_id);
		virtual ~VideoFormG2();

		std::string GetSessionId();
		//覆盖虚函数
		virtual std::wstring GetSkinFolder() override;
		virtual std::wstring GetSkinFile() override;
		virtual std::wstring GetWindowClassName() const override 
		{ return kClassName; };
		virtual UINT GetClassStyle() const override 
		{ return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

		/**
		* 窗口初始化函数
		* @return void	无返回值
		*/
		virtual void InitWindow();// override;
		virtual HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, bool isLayeredWindow, const ui::UiRect& rc) override;
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
		
		void OnMaxWindow(bool _max);
		
		/**
		* 根据控件类名创建自定义控件
		* @param[in] pstrClass 控件类名
		* @return Control* 创建的控件的指针
		*/
		virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;

		/**
		* 进入语音通话界面
		* @return void	无返回值
		*/
		void IntoAudio();

		/**
		* 切换到音频通话
		* @return void	无返回值
		*/
		void ChangeToAudio();
		
		//virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		//virtual LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		//virtual HWND GetCanvasWndHandle();
		//virtual void ShowCanvas();
		bool Notify(ui::EventArgs* msg);
		void ClearBitmapControl(bool mine);
		void StartDialWaitingTimer();
		bool OnClicked(ui::EventArgs* arg);
		bool OnSelect(ui::EventArgs* arg);
		bool OnMouseMessage(::ui::EventArgs* arg);
		bool OnSpeakerVolumnKillFocus(::ui::EventArgs* arg);
		bool OnMicrophoneVolumnKillFocus(::ui::EventArgs* arg);
		void AllowWindowMaxsize(bool allow);
		void CheckHeadIcon();
		void CheckFriendName();

		/**
		* 检查视频设备状态
		* @return int 0=有+开；1=有+没开；2=无
		*/
		int CheckVideoInputDevice();
		void InitSetting();
		void OnTick();
		void OnComeIn(uint64_t id);
		void BeforeClose();
		void OnQuitMsgBox(MsgBoxRet ret);
		bool IsStart() { return is_start_; }
		void CaptureLocalVideoFrame(uint64_t uid,   /**< uid */
			void* data,         /**< 数据指针 */
			uint32_t type,      /**< NERtcVideoType */
			uint32_t width,     /**< 宽度 */
			uint32_t height,    /**< 高度 */
			uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
			uint32_t offset[4], /**< 每类数据偏移 */
			uint32_t stride[4], /**< 每类数据步进 */
			uint32_t rotation,  /**< NERtcVideoRotation */
			void* user_data     /**< 用户透传数据 */);

		void CaptureRemoteVideoFrame(uint64_t uid,   /**< uid */
			void* data,         /**< 数据指针 */
			uint32_t type,      /**< NERtcVideoType */
			uint32_t width,     /**< 宽度 */
			uint32_t height,    /**< 高度 */
			uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
			uint32_t offset[4], /**< 每类数据偏移 */
			uint32_t stride[4], /**< 每类数据步进 */
			uint32_t rotation,  /**< NERtcVideoRotation */
			void* user_data     /**< 用户透传数据 */);

		void OnPeerCameraChanged(const std::string& userId, bool avaiable);
		
		void EnterEndCallPage(EndCallEnum why);
		void OnAutoCloseWnd();
		void DirectQuit();
		void AdjustWindowSize(bool video);
		void IntoVideo();
		//void IntoAudio();
		void CheckTitle();
		void SwitchStatus(StatusEnum status);
		void setVideoMode(bool mode);
		void PaintVideo();
		void StartPaintVideo();
		void ShowStatusPage(StatusPage sp);
		void InitVolumnSetting();
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
		//ui::VideoCanvasG2*  g2_canvas_;
		ui::BitmapControlG2* video_ctrl_screen_;  // 视频渲染窗口，大窗口
		ui::BitmapControlG2* video_ctrl_preview_; // 视频渲染窗口，小窗口
		::ui::Label* time_tick_label_;    //视频窗口的计时
		::ui::Control* video_border_ctrl_;
		::ui::Button* swap_photo_btn_;

		::ui::Label* camera_open_label_;  //等待对方开启摄像头
		::ui::Label* camera_closed_label_; //对方关闭了摄像头

		::ui::Control* camera_page_tip_;
		::ui::Control* camera_page_tip2_;

		::ui::Button* speaker_btn_;
		::ui::Button* microphone_btn_;
		::ui::CheckBox* camera_checkbox_;
		::ui::Button* start_record_btn_;
		::ui::Button* stop_record_btn_;
		::ui::Button* face_open_btn_;
		::ui::Button* face_close_btn_;

		::ui::Slider* input_volumn_slider_;
		::ui::VBox* vbox_of_input_;
		::ui::Control* place_ctrl_1_;

		::ui::Slider* output_volumn_slider_;
		::ui::VBox* vbox_of_output_;
		::ui::Control* place_ctrl_2_;

		::ui::Button* into_video_btn_;
		::ui::Button* into_audio_btn_;
		::ui::Button* stop_chat_btn_;

		::ui::Button* refuse_btn_;
		::ui::Button* complete_btn_;

		::ui::Label* netstat_label_;
		::ui::HBox* hbox_camera_fail_;

		::ui::Box* vbox_video_audio_tip_;

		::ui::Box* record_tip_box_;
		::ui::Label* record_tip_label0_;
		::ui::Label* record_tip_label1_;
		::ui::Label* record_tip_label2_;
		::ui::Label* record_tip_label3_;
		::ui::Label* recording_tip_label_;

		std::string		session_id_;
		StatusEnum		status_;
		bool			current_video_mode_; //当前聊天模式，true为视频，false为语音
		bool			custom_video_mode_;//使用自定义数据模式，做数据处理用
		//VideoFrameMng	video_frame_mng_;//使用自定义数据模式，做数据处理后，数据保存用于预览
		int64_t			channel_id_;
		nbase::NLock	capture_lock_;

		EndCallEnum		end_call_;

		std::atomic_bool	screen_is_other_; //视频对调画面
		bool				camera_is_open_;  //摄像头是否主动关闭
		bool				camera_is_open_other_; //对方摄像头是否关闭

		bool				need_change_form_size_; //视频通话在接通以后调整窗口大小
		bool				firstFrame_ = false;	// 对方第一帧画面
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

		shared::Ring	  voip_ring_;

		bool	mic_in_open_;
		bool	speaker_out_open_;
		bool	mp4_recording_;
		std::atomic_bool	is_start_; //是否处于通话界面

		//std::unique_ptr<RecordSelectForm> record_select_form_;
		std::weak_ptr<nbase::WeakFlag> record_select_form_flag_;
	};

#define SUB_SINGLE_NOTIFY(notify_name) _SUB_SINGLE_NOTIFY_(VideoFormG2, notify_name)


	/*cn_name = notify_name + "Cb"*/
#define _SUB_SINGLE_NOTIFY_(class_name, notify_name)										\
	{																						\
		std::vector<std::string> vec;														\
		vec.emplace_back(#notify_name);														\
		auto cb = nbase::Bind(&class_name::notify_name##Cb, this, std::placeholders::_1);	\
		nbase::BusinessManager::GetInstance()->SubNotify(vec, cb);\
	}
}

#pragma once
#include "util/window_ex.h"
#include "helper/ui_bitmap_control.h"
#include "module/session/ring.h"

namespace nim_comp
{
#define MULTIVCHATMEMBERCOUNT 8 
#define MULTIVCHATVIDEOCOUNT 4

	class MultiVideoChatForm :public WindowEx
	{
		enum VchatMemberStatus
		{
			kNone	=0,
			kWaiting, //等待
			kRefuse,	//未接听
			kConnected,	//已连接，正在通话中
			kDisConnect	//成员断开连接
		};
		//结束通话原因
		enum MultiEndCallEnum
		{
			END_CALL_CLOSE,
			END_CALL_HANGUP,
			END_CALL_CONNECTION,
		};

		class VideoUidsInfo
		{
		public:
			std::string uid;
			bool video;
			int32_t audio_volumn;
			VchatMemberStatus status;
			nbase::WeakCallbackFlag call_timer_;
			
			VideoUidsInfo()
			{
				uid = "";
				video = false;
				audio_volumn = 0;
				status = kNone;
			}
		};
	public:
		MultiVideoChatForm(std::string room_name, std::string session_id, int64_t channel_id, bool video = false, bool creator=false,std::string creator_id="");
		~MultiVideoChatForm();

		// 覆盖虚函数
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
		void OnVideoInDeviceChange(uint32_t status);
		void OnAudioInDeviceChange(uint32_t status);
		void OnVideoDeviceStartCallback(bool result);
		void OnAudioDeviceStartCallback(bool result);

		/**
		* 多人群聊人员进入
		* @param[in] uid 成员uid
		*/
		void OnPeopleJoin(std::string uid);

		/**
		* 多人群聊人员退出      
		* @param[in] uid 成员uid
		*/
		void OnPeopleLeave(std::string uid);

		/**
		* 设置成员uid的bitmap 信息
		* @param[in] uid 成员uid
		* @param[in] video 是否是视频模式
		* @param[in] refuse 是否拒绝了邀请
		*/
		void OnSetShowBitmap(std::string uid, bool video,bool refuse=false);

		/**
		* 音量回调信息
		* @param[in] map_volume uid对应的vol音量信息
		*/
		void OnVChatVolumeCallback(std::map<std::string, int32_t> map_volume);

		/**
		* @更新成员的音量信息
		*/
		void RefreshMemberAudioStatus();

		/**
		* 获取音量值的等级
		* @param[in] volume音量
		*/
		int32_t GetVolumeStep(uint32_t volume);

		/**
		* 获取已经邀请的成员的视频状态
		* @return map uid 对应的视频状态
		*/
		std::map<std::string,bool> GetInvitedMember();

		/**
		* 设置邀请成员是否开启了视频
		* @param uid 成员uid
		* @param video 是否是视频模式
		*/
		void SetInvitedMember(std::string uid,bool video);

		/**
		* 得到视频人数,当前房间的视频人数+允许视频通话的人数（还未进房间）
		* @return 返回当前群聊的开启视频的人数
		*/
		int GetVideoCount();

		/**
		* 获取正在群聊的成员集合
		* @return set<std::string> 已经进入音视频房间的成员信息
		*/
		std::set<std::string> GetTalkingMember() { return talking_members_; }

		/**
		* 设置邀请的成员集
		* @param[in]  邀请的成员uid集合
		*/
		void SetInvitingMember(std::set<std::string> inviting_members);


		/**
		* 初始音量，采集，置顶相关UI设置
		*/
		void InitVolumnSetting();

		/** 
		* 获取音视频房间名
		* @return std::string 音视频房间名
		*/
		std::string  GetRoomName() { return room_name_; }
		
		/** 
		* 缓存成员的视频信息。ps,防止对方的音视频模式通知消息（消息kMsgMultiVchatP2PChangeMode）先于该成员进入房间的通知上报，故需要缓存 
		* @param[in] uid 成员uid
		* @param[in] video true:视频 false:音频
		*/
		void SetCacheMemberVideo(std::string uid, bool video) { cache_members_info_[uid] = video; }
	private:
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
		* 显示结束通话页面
		* @param[in] end_reason 结束原因
		* @return void	无返回值
		*/
		void EnterEndCallPage(MultiEndCallEnum end_reason);

		/**
		* 直接关闭窗口
		* @return void	无返回值
		*/
		void DirectQuit();

		/**
		* 自动关闭窗口
		*/
		void OnAutoCloseWnd();
		
		/** 
		* 关闭视频设备
		*/
		void FreeVideo();

		/**
		* 关闭音频设备
		*/
		void FreeAudio();

		/**
		* 初始化摄像头控件
		*/
		void InitCbCameraSetting();

		/** 
		* 初始化音视频设置操作
		*/
		void InitSetting();

		/**
		* 检查视频输入设备
		*/
		int CheckVideoInputDevice();

		/** 
		*  麦克风音量控件失去焦点时的处理事件
		*/
		bool OnMicrophoneVolumnKillFocus(ui::EventArgs* arg);

		/**
		*  扬声器音量控件失去焦点时的处理事件
		*/
		bool OnSpeakerVolumnKillFocus(ui::EventArgs* arg);

		/** 
		* 关闭窗口前所做的操作
		*/
		void BeforeClose();

		/** 
		* 确定退出界面是的提示框
		*/
		void OnQuitMsgBox(MsgBoxRet ret);

		/**  
		* 最大化窗口，暂时没实现
		*/
		void OnMaxWindow(bool _max);

		/**
		* 处理所有控件选中消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnSelect(ui::EventArgs* arg);

		/**
		* 处理所有控件非选中消息
		* @param[in] msg 消息的相关信息
		* @return bool true 继续传递控件消息，false 停止传递控件消息
		*/
		bool OnUnSelect(ui::EventArgs* arg);

		/**
		* 初始化视频设备
		* @return bool true 成功，false 失败
		*/
		bool InitVideo();

		/**
		* 初始化音频设备
		* @return bool true 成功，false 失败
		*/
		bool InitAudio();

		/** 
		* 视频渲染控件
		*/
		void PaintVideo();
		
		/**
		* 处理鼠标事件
		* @param[in] msg 消息的相关信息
		*/
		bool OnMouseMessage(ui::EventArgs* arg);
		
		/** 
		* 给成员uid发送音视频切换通知
		* @param[in] 成员uid
		* @param[in] team true:群透传消息 false:p2p消息
		*/
		void SendNotifyToMember(std::string uid, bool team);
		
		/** 
		* bitmap渲染窗口分配
		* @param[in] uid 成员uid
		* @param[in] uid remove  true:增加,false:移除,ps.目前窗口中未移除，用文字提示
		* @param[in] video  true:视频 false:音频
		* @param[in] status uid的通话状态
		*/
		void AdjustVideoBitmap(std::string uid, bool remove, bool video, VchatMemberStatus status);
		
		/** 
		* bitmap渲染窗口分配
		* @param[in] uid 成员uid
		* @param[in] uid remove  true:移除,false:增加或更新
		* @param[in] video  true:视频 false:音频
		* @param[in] status uid的通话状态
		*/
		void AdjustVideoBitmap2(std::string uid, bool remove, bool video, VchatMemberStatus status);
		
		/** 
		* 检查视频群聊的人数
		*/
		int  CheckVChatMembers();

		/**
		* 限制弹框提示
		* @param[in] video true:代表视频受限提示 false:代表群聊人数受限提示
		*/
		void ShowLimitNotify(bool video=true);

		/** 
		* 邀请成员选择完处理事件
		*/
		void SelectedCompleted(const std::list<UTF8String>& friend_list, const std::list<UTF8String>& team_list, bool delete_enable);

		/** 
		* 与大窗口中的视频源进行交换
		* @param[in] bitmap_i_index  small_camera_page_tip_ 二维数组索引号
		* @param[in] bitmap_j_index  small_camera_page_tip_ 二维数组索引号
		*/
		void ExChangeBitmapControl(int bitmap_i_index, int bitmap_j_index);

		/** 
		* 置顶操作
		* @param[in] forever true:置顶 false:不置顶
		*/
		void SetTopMast(bool forever);

		/** 
		* 调整窗口大小
		*/
		void AdjustWindowSize();

		/** 
		* 刷新正在群聊的人员记录
		* @param[in] uid 成员uid remove true:移除 false 增加记录
		*/
		void RefreshTalkingMembers(std::string uid, bool remove);

		/** 
		* 启动摄像头设备
		*/
		void StartCameraDevice();

		/** 
		* 启动呼叫倒计时
		* @param[in] uid_info 成员相关信息
		*/
		void StartCallTimer(VideoUidsInfo& uid_info);

		/**
		* 关闭uid的呼叫定时器
		*/
		void EndCallTimer(std::string uid);

		/** 
		* 关闭聊天。ps 45s后无人进入那么就关闭会话
		*/
		void EndVChat();

		/** 
		* 调整群视频创建者的视频渲染位置 
		* @param[in] uid 成员uid
		*/
		void AdjustCreatorVideoBitmap(std::string uid);
		
		/** 
		* 用户信息修改回调
		* @param[in] uinfos 成员信息
		*/
		void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);

		std::wstring OnHandlerUserName(std::wstring user_name);
	public:
		static const LPCTSTR kClassName;
	private:
		std::string room_name_;
		std::string session_id_;
		uint64_t channel_id_;
		bool  creator_;//是否是群视频发起者
		std::string creator_id_;//
		bool	camera_is_open_;  //摄像头是否主动关闭
		bool is_max_window_;
		bool current_video_mode_;
		nbase::WeakCallbackFlag paint_video_timer_;
		nbase::WeakCallbackFlag start_video_timer_;//1s后开启摄像头
		nbase::WeakCallbackFlag start_call_timer_;
		VideoUidsInfo video_uids[2][4];
		VideoUidsInfo big_wnd_info_;
		nbase::NLock data_lock_;
		nbase::NLock invited_members_lock_;
		std::map<std::string, bool> invited_members_;
		std::set<std::string> talking_members_;
		std::map<std::string, bool> cache_members_info_;//缓存提前到达的消息通知
		std::set<std::string> members_info_error_;
		Ring	  voip_ring_;
		VchatMemberStatus current_status_;
		AutoUnregister	unregister_cb_;
	private:
		ui::Button* btn_microphone_;
		ui::Button* btn_speaker_;
		ui::Button* btn_camera_;
		ui::Button* btn_add_member_;
		ui::Button* btn_end_chat_;
		ui::Button*	btn_win_top_;
		ui::CheckBox* cb_camera_;
		ui::Slider*		input_volumn_slider_;
		ui::VBox*		vbox_of_input_;
		ui::Control*	place_ctrl_1_;
		ui::Slider*		output_volumn_slider_;
		ui::VBox*		vbox_of_output_;
		ui::Control*	place_ctrl_2_;
		ui::BitmapControl* video_ctrl_screen_;  // 视频渲染窗口，大窗口
		ui::Control*		camera_page_tip_;
		ui::Label*			lb_ctrl_screen_;
		ui::BitmapControl* video_ctrl_preview_[2][4];
		ui::Control*	small_camera_page_tip_[2][4];
		ui::Control*		video_border_ctrl_[2][4];
		ui::Button*			swap_photo_btn_[2][4];
		ui::Label*			lb_member_name_[2][4];
		ui::Label*			lb_member_status_prompt_[2][4];
		ui::Control*		member_volume_[2][4];
		ui::Control* big_wnd_member_volume_;
		ui::HBox*		hbox_camera_fail_;

		
	};
}
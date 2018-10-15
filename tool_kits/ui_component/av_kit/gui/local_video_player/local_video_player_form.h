#pragma once
#ifdef SUPPORTLOCALPLAYER

#include "controls/ui_video_player_bitmap_control.h"

using namespace nbase;

typedef enum 
{
	PlayerStateStarting = 1,
	PlayerStatePlaying,
	PlayerStatePaused,
	PlayerStateCompleted,
	PlayerStateStopping,
	PlayerStateStopped,
	PlayerStateError,
} PlayerState;

class LocalVideoPlayerForm : public ui::WindowImplBase
{
public:
	LocalVideoPlayerForm(const std::function<void()>& close_cb, const std::wstring& path_ = L"");
	~LocalVideoPlayerForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual UINT GetClassStyle() const override;

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

	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);
	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* msg);

	/**
	* 处理ESC键消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual void OnEsc(BOOL &bHandled);
	 /**
	 * 处理窗体最大化消息
	 * @param[in] max 是否最大化
	 * @return void 无返回值
	 */
	void OnMaxWindow(bool max);
	void Play(const std::wstring& path);
	bool SamePath(const std::wstring& path) const;
private:

	/**
	* 开始视频播放
	* @return void	无返回值
	*/
	bool StartPlay();
	/**
	* 开始视频播放
	* @return void	无返回值
	*/
	void Shutdown();
	/**
	* 刷新播放时间和进度条
	* @return void	无返回值
	*/
	void RefreshPlayTime();
	/**
	* 更新播放器状态
	* @return void	无返回值
	*/
	void ChangePlayerState(PlayerState new_state);
	/**
	* 播放结果回调
	* @return void	无返回值
	*/
	static void PlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	static void UIPlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	/**
	* 视频数据回调
	* @return void	无返回值
	*/
	static void VideoFrameCB(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame);

public:
	static const LPTSTR kClassName;
	static std::map<_HNLPSERVICE, LocalVideoPlayerForm*> InstFormMap;
	static unsigned int InstanceNum; //播放器窗口个数

private:
	std::wstring       path_; //当前窗口ID
	_HNLPSERVICE       nelp_handle_ = NULL;
	bool               is_max_window_ = false;
	bool               playing_ = false;
	long long          total_duration_ = 0; //总时长，可用于区分是直播开始点播
	float              volume_ = 20.0f;
	bool               muted_ = false;
	PlayerState        state_ = PlayerStateStopped;
	WeakCallbackFlag   refresh_time_flag_;
	std::function<void()> close_cb_;

	ui::VBox*		   volume_box_ = NULL;
	ui::TabBox*		   play_pause_box_ = NULL;
	ui::VideoPlayerBitmapControl* canvas_ = NULL;
	ui::Button*        btn_start_ = NULL;
	ui::Slider*        playing_progress_ = NULL;
	ui::Label*         label_duration_ = NULL;
	ui::Button*        btn_volume_ = NULL;
	ui::Button*        btn_mute_ = NULL;
};
#endif
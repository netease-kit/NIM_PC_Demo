#ifndef NIM_WIN_GUI_VIDEO_SETTING_H_
#define NIM_WIN_GUI_VIDEO_SETTING_H_

#include "util/window_ex.h"
#include "helper/ui_bitmap_control.h"

namespace nim_comp
{
#define AUDIO_VIDEO_VOLUMN	"audio_video_volumn"
#define AUDIO_VIDEO_DEVICE	"audio_video_device"

/** @class VideoSettingForm
  * @brief 音视频设置窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class VideoSettingForm : public WindowEx
{
public:
	VideoSettingForm();
	~VideoSettingForm();
	
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
	
	/**
	* 切换显示的页面
	* @param[in] video 是否切换到视频页面
	* @return void	无返回值
	*/
	void ShowPage(bool video);

	/**
	* 启动视频设备的回调函数
	* @param[in] result 是否成功
	* @return void	无返回值
	*/
	void OnVideoDeviceStartCallback(bool result);
private:
	/**
	* 初始化设备列表
	* @param[in] video 是否为视频设备
	* @param[in] input 是否为输入设备
	* @return void	无返回值
	*/
	void InitDeviceList(bool video, bool input);

	/**
	* 切换到音频输入设备
	* @param[in] device_path 设备路径
	* @return void	无返回值
	*/
	void SwitchInputAudioDevice(const std::string &device_path);

	/**
	* 切换到音频输出设备
	* @param[in] device_path 设备路径
	* @return void	无返回值
	*/
	void SwitchOutputAudioDevice(const std::string &device_path);

	/**
	* 切换到视频输入设备
	* @param[in] device_path 设备路径
	* @return void	无返回值
	*/
	void SwitchInputVideoDevice(const std::string &device_path);

	/**
	* 显示音频输入界面
	* @param[in] start 设备是否启动
	* @return void	无返回值
	*/
	void PrepareAudioInput(bool start);

	/**
	* 显示音频输出界面
	* @param[in] start 设备是否启动
	* @return void	无返回值
	*/
	void PrepareAudioOutput(bool start);

	/**
	* 显示视频输入界面
	* @param[in] start 设备是否启动
	* @return void	无返回值
	*/
	void PrepareVideoInput(bool start);
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
	* 处理所有控件的选中消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnSelect(ui::EventArgs* arg);
public:
	/**
	* 响应音频设备改变的回调函数
	* @param[in] status StatusEnum状态吗
	* @return void	无返回值
	*/
	void OnAudioInDeviceChange(uint32_t status);

	/**
	* 响应视频设备改变的回调函数
	* @param[in] status StatusEnum状态吗
	* @return void	无返回值
	*/
	void OnVideoInDeviceChange(uint32_t status);

	/**
	* 响应音量改变的回调函数
	* @return void	无返回值
	*/
	void AudioVolumnChange();

	/**
	* 绘制视频界面
	* @return void	无返回值
	*/
	void PaintVideo();
public:
	static const LPCTSTR kClassName;
private:
	nbase::WeakCallbackFlag paint_video_timer_;

	ui::TabBox*		tabbox_;
	ui::OptionBox*	option_audio_;
	ui::OptionBox*	option_video_;

	ui::CheckBox*	open_output_checkbox_;
	ui::Combo*      output_audio_device_;
	ui::CheckBox*	output_silent_checkbox_;
	ui::Slider*		output_volumn_slider_;

	ui::CheckBox*	open_input_checkbox_;
	ui::Combo*      input_audio_device_;
	ui::CheckBox*	input_silent_checkbox_;
	ui::Slider*		input_volumn_slider_;

	ui::CheckBox*	auto_input_volumn_checkbox_;

	ui::CheckBox*	open_video_checkbox_;
	ui::Combo*		input_video_device_;

	bool	camera_is_open_;

	ui::BitmapControl* video_show_ctrl_;
	ui::Control*	camera_fail_ctrl_;
	ui::Label*		error_notice_label_;

	//std::list<MEDIA_DEVICE_DRIVE_INFO> input_audios_;
	//std::list<MEDIA_DEVICE_DRIVE_INFO> output_audios_;
	//std::list<MEDIA_DEVICE_DRIVE_INFO> input_videos_;
};
}
using namespace nbase;
DISABLE_RUNNABLE_METHOD_REFCOUNT(nim_comp::VideoSettingForm);

#endif

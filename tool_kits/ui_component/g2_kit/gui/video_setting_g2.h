#ifndef NIM_WIN_GUI_VIDEO_SETTING_G2_H_
#define NIM_WIN_GUI_VIDEO_SETTING_G2_H_

#include "bitmap_control_g2.h"
#include "g2_kit/module/g2_device_def.h"


namespace nim_comp
{
#define AUDIO_VIDEO_VOLUMN	"audio_video_volumn"
#define AUDIO_VIDEO_DEVICE	"audio_video_device"

	/** @class VideoSettingForm
	  * @brief 音视频设置窗口
	  * @copyright (c) 2016, NetEase Inc. All rights reserved
	  * @date 2016/09/21
	  */
	class VideoSettingFormG2 : public ui::WindowImplBase
	{
		friend class VideoManagerG2;
	public:
		VideoSettingFormG2();
		~VideoSettingFormG2();

		// 覆盖虚函数
		virtual std::wstring GetSkinFolder() override;
		virtual std::wstring GetSkinFile() override;
		virtual std::wstring GetWindowClassName() const override { return kClassName; };
		virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };

		void DirectQuit();

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
		
		void StopVideoPreview();
		void StartVideoPreview();
		void StartAudioPreview();
		void StopAudioPreview();
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
		* 初始化视频可用分辨率列表
		* @return void	无返回值
		*/
		void InitVideoQualityList();

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

		/**
		* 显示视频输入界面
		* @param[in] start 设备是否启动
		* @return void	无返回值
		*/
		void PrepareVideoQuality(int current_video_quality);
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
		* @return void	无返回值 unsigned char value = 0, bool isRecord = false
		*/
		void AudioVolumnChange();

		void GetAudioStatusOnUI(bool& input, bool& output);
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

		//G2不支持自动音量
		//ui::CheckBox*	auto_input_volumn_checkbox_;

		ui::CheckBox*	open_video_checkbox_;
		ui::Combo*		input_video_device_;
		ui::Combo*		video_quality_list_;

		ui::VBox*		video_page_;

		bool	has_inite_;

		ui::BitmapControlG2* video_show_ctrl_;
		ui::Control*	camera_fail_ctrl_;
		ui::Label*		error_notice_label_;

		std::vector<G2_MEDIA_DEVICE_INFO> input_audios_;
		std::vector<G2_MEDIA_DEVICE_INFO> output_audios_;
		std::vector<G2_MEDIA_DEVICE_INFO> input_videos_;
	};
}

#endif

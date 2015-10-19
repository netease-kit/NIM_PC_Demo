#ifndef YIXIN_WIN_GUI_VIDEO_SETTING_H_
#define YIXIN_WIN_GUI_VIDEO_SETTING_H_

#include "util/window_ex.h"
#include "ui_bitmap_control.h"

namespace nim_comp
{
#define AUDIO_VIDEO_VOLUMN	"audio_video_volumn"
#define AUDIO_VIDEO_DEVICE	"audio_video_device"

class VideoSettingForm : public WindowEx
{
public:
	VideoSettingForm();
	~VideoSettingForm();
	
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
	
	void ShowPage(bool video);

	void OnInputVideoData(const std::string &data, ui::CSize size, uint64_t timestamp);
	void OnVideoDeviceStartCallback(bool result);
private:
	void InitDeviceList(bool video, bool input);
	void SwitchInputAudioDevice(const std::string &device_path);
	void SwitchOutputAudioDevice(const std::string &device_path);
	void SwitchInputVideoDevice(const std::string &device_path);
	void PrepareAudioInput(bool start);
	void PrepareAudioOutput(bool start);
	void PrepareVideoInput(bool start);
private:
	bool Notify(ui::EventArgs* msg); 
	bool OnClicked(ui::EventArgs* arg);
	bool OnSelect(ui::EventArgs* arg);
public:
	void OnAudioInDeviceChange(uint32_t status);
	void OnVideoInDeviceChange(uint32_t status);
	void AudioVolumnChange();
public:
	static const LPCTSTR kClassName;
private:
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

	ui::CBitmapControl* video_show_ctrl_;
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

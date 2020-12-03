#include "pch.h"
#include "video_setting_g2.h"
#include "module/video_manager_g2.h"
#include "g2_kit/component/avchat_component_def.h"
#include "g2_kit/component/wrapper/avchat_business_wrapper.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR VideoSettingFormG2::kClassName = L"VideoSettingFormG2";

VideoSettingFormG2::VideoSettingFormG2()
{
	has_inite_ = false;
}

VideoSettingFormG2::~VideoSettingFormG2()
{
}

std::wstring VideoSettingFormG2::GetSkinFolder()
{
	return L"video";
}

std::wstring VideoSettingFormG2::GetSkinFile()
{
	return L"video_setting_g2.xml";
}

ui::Control* VideoSettingFormG2::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BitmapControlG2"))
	{
		return new ui::BitmapControlG2(this);
	}
	return NULL;
}
void VideoSettingFormG2::CaptureLocalVideoFrame(uint64_t uid,   /**< uid */
	void* data,         /**< 数据指针 */
	uint32_t type,      /**< NERtcVideoType */
	uint32_t width,     /**< 宽度 */
	uint32_t height,    /**< 高度 */
	uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
	uint32_t offset[4], /**< 每类数据偏移 */
	uint32_t stride[4], /**< 每类数据步进 */
	uint32_t rotation,  /**< NERtcVideoRotation */
	void* user_data     /**< 用户透传数据 */)
{
	if (video_show_ctrl_)
		video_show_ctrl_->CaptureVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
}
void VideoSettingFormG2::StopVideoPreview()
{
	if (!paint_video_timer_.HasUsed())
		return;
	nbase::BatpPackPostRequest bp_canvas(kAvChatSetupLocalView, BatpTrailInfoFromHere);
	//bp_canvas.head_.action_name_ = kAvChatSetupLocalView;
	AvChatParams canvas_params;
	canvas_params.window = NULL;
	canvas_params.dataCb = nullptr;
	bp_canvas.body_.param_ = canvas_params;
	nbase::BusinessManager::GetInstance()->Request(bp_canvas, nullptr);

	VideoManagerG2::GetInstance()->StartVideoPreview(false);
	camera_fail_ctrl_->SetVisible(true);
	video_show_ctrl_->EnableCaptureVideoFrame(false);
	video_show_ctrl_->Clear();
	camera_fail_ctrl_->SetVisible(true);
	paint_video_timer_.Cancel();
}

void VideoSettingFormG2::StopAudioPreview()
{
	VideoManagerG2::GetInstance()->StartAudioPreview(false);
}

void VideoSettingFormG2::StartAudioPreview()
{
	auto* vm = VideoManagerG2::GetInstance();
	if (vm->IsTalking())
		return;

	vm->StartAudioPreview(true);
}
void VideoSettingFormG2::StartVideoPreview()
{
	//处于通话界面，不提供视频预览。已打开预览，然后视频通话，组件会停止预览
	if (VideoManagerG2::GetInstance()->IsTalking())
		return;
	if (!open_video_checkbox_->IsSelected())
		return;

	VideoManagerG2::GetInstance()->EnableVideoDevice(true);

	auto canvasDataCb = [](
		nertc::uid_t uid,   /**< uid */
		void* data,         /**< 数据指针 */
		uint32_t type,      /**< NERtcVideoType */
		uint32_t width,     /**< 宽度 */
		uint32_t height,    /**< 高度 */
		uint32_t count,     /**< 数据类型个数，即offset及stride的数目 */
		uint32_t offset[4], /**< 每类数据偏移 */
		uint32_t stride[4], /**< 每类数据步进 */
		uint32_t rotation,  /**< NERtcVideoRotation */
		void* user_data     /**< 用户透传数据 */) {

			auto video_setting = VideoManagerG2::GetInstance()->GetVideoSettingForm();
			if (video_setting)
				video_setting->CaptureLocalVideoFrame(uid, data, type, width, height, count, offset, stride, rotation, user_data);
	};
	//设置本地画布
	//batp 回调进不来，暂时直接使用RTC相关接口
	nbase::BatpPackPostRequest bp_canvas(kAvChatSetupLocalView, BatpTrailInfoFromHere);
	//bp_canvas.head_.action_name_ = kAvChatSetupLocalView;
	AvChatParams canvas_params;
	canvas_params.window = NULL;
	canvas_params.dataCb = canvasDataCb;
	bp_canvas.body_.param_ = canvas_params;

	nbase::BusinessManager::GetInstance()->Request(bp_canvas, [this](const nbase::BatpPack& response) {
		VideoManagerG2::GetInstance()->StartVideoPreview();
		video_show_ctrl_->EnableCaptureVideoFrame(true);
		camera_fail_ctrl_->SetVisible(false);
		paint_video_timer_.Cancel();
		StdClosure task = nbase::Bind([this]() {
			video_show_ctrl_->Refresh();
			});
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, paint_video_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(70));

	});
}
LRESULT VideoSettingFormG2::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		this->Close();
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void VideoSettingFormG2::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&VideoSettingFormG2::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&VideoSettingFormG2::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&VideoSettingFormG2::OnSelect, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&VideoSettingFormG2::OnSelect, this, std::placeholders::_1));

	tabbox_ = (TabBox*) FindControl(L"audio_tab_video");

	option_audio_ = (OptionBox*) FindControl(L"option_audio");
	option_video_ = (OptionBox*) FindControl(L"option_video");

	option_audio_->AttachSelect(nbase::Bind(&VideoSettingFormG2::OnClicked, this, std::placeholders::_1));
	option_video_->AttachSelect(nbase::Bind(&VideoSettingFormG2::OnClicked, this, std::placeholders::_1));

	open_output_checkbox_	= (CheckBox*) FindControl(L"open_output");
	output_audio_device_	= (Combo*)  FindControl(L"output_audio");
	output_silent_checkbox_ = (CheckBox*) FindControl(L"output_silent");
	output_volumn_slider_	= (Slider*) FindControl(L"output_volumn");

	open_input_checkbox_	= (CheckBox*) FindControl(L"open_input");
	input_audio_device_		= (Combo*)  FindControl(L"input_audio");
	input_silent_checkbox_  = (CheckBox*) FindControl(L"input_silent");
	input_volumn_slider_	= (Slider*) FindControl(L"input_volumn");

	//auto_input_volumn_checkbox_    = (CheckBox*) FindControl(L"auto_input_volumn");

	video_show_ctrl_ = (BitmapControlG2*) FindControl(L"video_show");
	video_show_ctrl_->SetAutoSize(true);

	camera_fail_ctrl_ = FindControl( L"camera_fail" );
	error_notice_label_ = (Label*) FindControl(L"error_notice");

	open_video_checkbox_ = (CheckBox*) FindControl(L"open_video");
	input_video_device_  = (Combo*) FindControl(L"input_video");
	video_quality_list_ = (Combo*)FindControl(L"video_quality");

	video_page_ = (ui::VBox*) FindControl(L"video_page");

	//第一次展示界面，控件状态设为默认值（设备全开、音量最大）

	auto* vm = VideoManagerG2::GetInstance();
	open_output_checkbox_->Selected(vm->IsAudioEnable(false), false);
	open_input_checkbox_->Selected(vm->IsAudioEnable(true), false);
	open_video_checkbox_->Selected(vm->IsVideoEnable(), false);
	
	output_volumn_slider_->SetValue(vm->GetAudioVolumn(false));
	input_volumn_slider_->SetValue(vm->GetAudioVolumn(true));
	
}

void VideoSettingFormG2::OnFinalMessage(HWND hWnd)
{
// 	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting);
// 	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting);
// 	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeSetting);
// 	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting);
// 	
	__super::OnFinalMessage(hWnd);
}

bool VideoSettingFormG2::Notify(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if(msg->Type == ui::kEventSelect)
	{
		if(name == L"output_audio")
		{
			int k = output_audio_device_->GetCurSel();
			ASSERT(k >= 0 && k < output_audio_device_->GetCount());

			ListContainerElement* label = (ListContainerElement*)output_audio_device_->GetItemAt(k);
			SwitchOutputAudioDevice( label->GetUTF8DataID() );
		}
		else if(name == L"input_audio")
		{
			int k = input_audio_device_->GetCurSel();
			ASSERT(k >= 0 && k < input_audio_device_->GetCount());

			ListContainerElement* label = (ListContainerElement*)input_audio_device_->GetItemAt(k);
			SwitchInputAudioDevice(label->GetUTF8DataID());
		}
		else if(name == L"input_video")
		{
			int k = input_video_device_->GetCurSel();
			ASSERT(k >= 0 && k < input_video_device_->GetCount());

			ListContainerElement* label = (ListContainerElement*)input_video_device_->GetItemAt(k);
			SwitchInputVideoDevice(label->GetUTF8DataID());
		}
		else if (name == L"video_quality")
		{
			int k = video_quality_list_->GetCurSel();
			ASSERT(k >= 0 && k < video_quality_list_->GetCount());

			ListContainerElement* label = (ListContainerElement*)video_quality_list_->GetItemAt(k);

			int video_quality = 0;
			nbase::StringToInt(label->GetDataID(), &video_quality);
			VideoManagerG2::GetInstance()->SetVideoQuality(video_quality);
		}
	}
	else if(msg->Type == ui::kEventValueChange)
	{
		if(name == L"output_volumn")
		{
			int v = output_volumn_slider_->GetValue();

			//VideoManager::GetInstance()->SetAudioVolumn(v, false);
			VideoManagerG2::GetInstance()->SetAudioVolumn(v, false);
		}
		else if(name == L"input_volumn")
		{
			int v = input_volumn_slider_->GetValue();

			//VideoManager::GetInstance()->SetAudioVolumn(v, true);
			VideoManagerG2::GetInstance()->SetAudioVolumn(v, true);
			//VideoManager::GetInstance()->SetAudioInputAutoVolumn(false);
		}
	}
	return true;
}

bool VideoSettingFormG2::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();		
	if(name == L"option_audio")
	{
		ShowPage(false);
	}
	else if(name == L"option_video")
	{
		ShowPage(true);
	}
	else if (name == L"closebtn")
	{
		StopVideoPreview();
		StopAudioPreview();
	}
	//else if(name == L"reset_setting")
	//{
	//	VideoManager* vm = VideoManager::GetInstance();

	//	vm->SetAudioOpenInput(true, false);
	//	vm->SetAudioInputSilent(false, false);
	//	vm->SetAudioInputVolumn(0xff, false);
	//	vm->SetAutoInputVolumn(true, false);

	//	vm->SetAudioOpenOutput(true, false);
	//	vm->SetAudioOutputSilent(false, false);
	//	vm->SetAudioOutputVolumn(0xff, false); 

	//	vm->SetDefaultAudioInput(L"", false);
	//	vm->SetDefaultAudioOutput(L"", true);

	//	InitDeviceList(false, true);
	//	InitDeviceList(false, false);

	//	PrepareAudioOutput(true);
	//	PrepareAudioInput(true);
	//}
	return true;
}

bool VideoSettingFormG2::OnSelect( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();	
	if(name == L"open_output")
	{
		bool select = open_output_checkbox_->IsSelected();
		std::string path;
		if( select )
		{
			int k = output_audio_device_->GetCurSel();
			ListContainerElement* label = (ListContainerElement*)output_audio_device_->GetItemAt(k);
			if (label)
			{
				SwitchOutputAudioDevice(label->GetUTF8DataID());
			}
		}
		else
		{
			//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting, true);
			//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeSetting, true);
		}
		//VideoManager::GetInstance()->SetSettingAudioOutStatus(select);
		VideoManagerG2::GetInstance()->EnableAudioDevice(select, false);
		select ? StartAudioPreview() : StopAudioPreview();
	}
	else if(name == L"output_silent")
	{
		if (output_silent_checkbox_->IsSelected())
		{
			//VideoManager::GetInstance()->SetAudioVolumn(0, false);
			VideoManagerG2::GetInstance()->SetAudioVolumn(0, false);
		}
	}
	else if(name == L"open_input")
	{
		bool select = open_input_checkbox_->IsSelected();
		if( select )
		{
			int k = input_audio_device_->GetCurSel();
			ListContainerElement* label = (ListContainerElement*)input_audio_device_->GetItemAt(k);
			if( label )
				SwitchInputAudioDevice(label->GetUTF8DataID());
		}
		else
		{
			//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting, true);
		}
		//VideoManager::GetInstance()->SetSettingAudioInStatus(select);
		VideoManagerG2::GetInstance()->EnableAudioDevice(select, true);
		select ? StartAudioPreview() : StopAudioPreview();
	}
	else if(name == L"input_silent")
	{	
		if (input_silent_checkbox_->IsSelected())
		{
			//VideoManager::GetInstance()->SetAudioVolumn(0, true);
			VideoManagerG2::GetInstance()->SetAudioVolumn(0, true);
		}
	}
	else if(name == L"auto_input_volumn")
	{
		//bool check = auto_input_volumn_checkbox_->IsSelected();
		//VideoManager::GetInstance()->SetAudioInputAutoVolumn(check);
	}
	else if(name == L"open_video")
	{
		bool select = open_video_checkbox_->IsSelected();
		if( select )
		{
			int k = input_video_device_->GetCurSel();
			ListContainerElement* label = (ListContainerElement*)input_video_device_->GetItemAt(k);
			if( label )
				SwitchInputVideoDevice(label->GetUTF8DataID());
		}
		else
		{
			//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting, true);
		}
		VideoManagerG2::GetInstance()->EnableVideoDevice(select);
		select ? StartVideoPreview() : StopVideoPreview();
	}
	return true;
}

void VideoSettingFormG2::InitDeviceList( bool video, bool input )
{
	Combo* combo = NULL;
	std::vector<G2_MEDIA_DEVICE_INFO> device;

	if(video)
	{
		combo = input_video_device_;

		device = VideoManagerG2::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeVideo);
		input_videos_.assign(device.begin(), device.end());
	}
	else if(input)
	{
		combo = input_audio_device_;

		device = VideoManagerG2::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeAudioIn);
		input_audios_.assign(device.begin(), device.end());
	}
	else
	{
		combo = output_audio_device_;

		device = VideoManagerG2::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeAudioOut);
		output_audios_.assign(device.begin(), device.end());
	}

	combo->RemoveAll();

	
	if( device.empty() )
	{
		std::wstring str;
		if(video)
			str = L"STRID_VIDEO_SET_CAMERA_NOT_FOUND";
		else if(input)
			str = L"STRID_VIDEO_SET_MICROPHONE_NOT_FOUND";
		else
			str = L"STRID_VIDEO_SET_PLAYER_NOT_FOUND";

		ListContainerElement* label = new ListContainerElement;
		label->SetFixedHeight(30);
		label->SetTextPadding(UiRect(10, 1, 30, 1));
		label->SetTextId(str);
		label->SetEnabled( false );

		combo->Add(label);
		combo->SelectItem(0);
		combo->SetEnabled(false);
	}
	else
	{
		for(auto i = device.begin(); i != device.end(); i++)
		{
			ListContainerElement* label = new ListContainerElement;
			label->SetClass( L"listitem" );
			label->SetFixedHeight(30);
			label->SetTextPadding(UiRect(10, 1, 30, 1));
			label->SetUTF8Text(nbase::UTF16ToUTF8(i->device_name_));
			label->SetUTF8DataID(nbase::UTF16ToUTF8(i->device_id_));

			combo->Add(label);
		}
		combo->SetEnabled(true);
	}
}

void VideoSettingFormG2::InitVideoQualityList()
{
	video_quality_list_->RemoveAll();

	// 不使用 map 的原因是 map 会打乱次序
	std::list<std::pair<std::wstring, int>> quality_list;
	quality_list.push_back(std::make_pair(L"720P（1280x720）", nertc::kNERtcVideoProfileHD720P));
	quality_list.push_back(std::make_pair(L"Low（160x90）", nertc::kNERtcVideoProfileLowest));
	quality_list.push_back(std::make_pair(L"Medium（320x180）", nertc::kNERtcVideoProfileLow));
	quality_list.push_back(std::make_pair(L"High（640x360）", nertc::kNERtcVideoProfileStandard));
	//quality_list.push_back(std::make_pair(L"480P（640x480）", nim::kNIMVChatVideoQualitySuper));
	//quality_list.push_back(std::make_pair(L"540P（960x540）", nim::kNIMVChatVideoQuality540p));
	quality_list.push_back(std::make_pair(L"720P（1280x720）", nertc::kNERtcVideoProfileHD720P));
	quality_list.push_back(std::make_pair(L"1080P（1920x1080）", nertc::kNERtcVideoProfileHD1080P));
	
	for (auto quality : quality_list)
	{
		ListContainerElement* label = new ListContainerElement;
		label->SetClass(L"listitem");
		label->SetFixedHeight(30);
		label->SetTextPadding(UiRect(10, 1, 30, 1));
		label->SetText(quality.first);
		label->SetDataID(nbase::Int64ToString16(quality.second));
		video_quality_list_->Add(label);
	}

	video_quality_list_->SelectItem(0);
}

void VideoSettingFormG2::ShowPage( bool video )
{
	//camera_is_open_ = false;
	//video_show_ctrl_->Clear( );

	if(video)
	{
		video_page_->SetVisible(true);

		//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting);
		//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting);
		option_video_->Selected(true, false);

		InitDeviceList(true, true);

		InitVideoQualityList();

		tabbox_->SelectItem(1);

		error_notice_label_->SetVisible(false);
		camera_fail_ctrl_->SetVisible( true );
		PrepareVideoInput(VideoManagerG2::GetInstance()->IsVideoEnable());
		//PrepareVideoQuality(VideoManager::GetInstance()->GetVideoQuality());
		StopAudioPreview();
	}
	else
	{
		video_page_->SetVisible(false);

		paint_video_timer_.Cancel();
		//VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting);

		option_audio_->Selected(true, false);
		
		InitDeviceList(false, false);

		InitDeviceList(false, true);

		tabbox_->SelectItem(0);

		PrepareAudioOutput(VideoManagerG2::GetInstance()->IsAudioEnable(false));
		PrepareAudioInput(VideoManagerG2::GetInstance()->IsAudioEnable(true));
		StopVideoPreview();
	}
}

void VideoSettingFormG2::SwitchInputAudioDevice( const std::string &id)
{
	VideoManagerG2::GetInstance()->SetAudioDevice(nbase::UTF8ToUTF16(id), true);
}

void VideoSettingFormG2::SwitchOutputAudioDevice( const std::string & id)
{
	VideoManagerG2::GetInstance()->SetAudioDevice(nbase::UTF8ToUTF16(id), false);
	//VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOut, device_path, kDeviceSessionTypeSetting);
	//VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOutChat, device_path, kDeviceSessionTypeSetting);
}

void VideoSettingFormG2::SwitchInputVideoDevice( const std::string &id )
{
	//camera_is_open_ = false;
	camera_fail_ctrl_->SetVisible( true );
	error_notice_label_->SetVisible(false);
	VideoManagerG2::GetInstance()->SetVideoDevice(nbase::UTF8ToUTF16(id));

	VideoManagerG2::GetInstance()->EnableVideoDevice(open_video_checkbox_->IsSelected());
	//StartVideoPreview();
	//VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, device_path, kDeviceSessionTypeSetting);
}

void VideoSettingFormG2::OnVideoDeviceStartCallback(bool result)
{
	//camera_is_open_ = result;
	if( result )
	{
		paint_video_timer_.Cancel();
		StdClosure task = nbase::Bind(&VideoSettingFormG2::PaintVideo, this);
		nbase::ThreadManager::PostRepeatedTask(kThreadUI, paint_video_timer_.ToWeakCallback(task), nbase::TimeDelta::FromMilliseconds(70));
		camera_fail_ctrl_->SetVisible( false );
		error_notice_label_->SetVisible( false );
	}
	else
	{
		camera_fail_ctrl_->SetVisible( true );
		error_notice_label_->SetVisible( true );
	}
}
void VideoSettingFormG2::AudioVolumnChange()
{
	//bool auto_volumn = nim::VChat::GetAudioInputAutoVolumn();
	//auto_input_volumn_checkbox_->Selected(auto_volumn, false);
	//int volumn = nim::VChat::GetAudioVolumn(true);
	int volumn = VideoManagerG2::GetInstance()->GetAudioVolumn(true);
	input_volumn_slider_->SetValue(volumn);
	input_silent_checkbox_->Selected(volumn==0, false);
	//volumn = nim::VChat::GetAudioVolumn(false);
	volumn = VideoManagerG2::GetInstance()->GetAudioVolumn(false);
	output_volumn_slider_->SetValue(volumn);
	output_silent_checkbox_->Selected(volumn == 0, false);
}

void VideoSettingFormG2::GetAudioStatusOnUI(bool& input, bool& output)
{
	input = open_input_checkbox_->IsSelected();
	output = open_output_checkbox_->IsSelected();
}

//视频时不支持音频设备测试，以及本地视频预览
void VideoSettingFormG2::PrepareAudioInput( bool start )
{
	VideoManagerG2* vm = VideoManagerG2::GetInstance();

	bool open_input = true;
	if (start)
	{
		open_input_checkbox_->Selected(open_input, false);

	}
	else
	{
		open_input = open_input_checkbox_->IsSelected();
	}

	//bool auto_volumn = nim::VChat::GetAudioInputAutoVolumn();
	//auto_input_volumn_checkbox_->Selected(auto_volumn, false);

	bool silent = false;
	input_silent_checkbox_->Selected(silent, false);

	if( silent )
		input_volumn_slider_->SetValue( 0 );
	else
	{
		//int volumn = nim::VChat::GetAudioVolumn(true);
		int volumn = VideoManagerG2::GetInstance()->GetAudioVolumn(true);
		input_volumn_slider_->SetValue( volumn );
		if( volumn == 0 )
			input_silent_checkbox_->Selected( true, false );
	}

	std::string device_path;
	int no = 0;
	auto devices = vm->GetDeviceInfo(nim::kNIMDeviceTypeAudioIn, &no);
	//int no = GetCurDeviceIndex(devices, vm->GetCurAudioDeviceId(true));
	bool can_use = devices.size() > 0;// vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeAudioIn);

	open_input_checkbox_->SetEnabled( can_use );

	if(can_use)
	{
		input_audio_device_->SelectItem(no);
		input_audio_device_->SetEnabled( open_input );

		if(open_input)
		{
			//UI设置结束，开始启动设备
			if(start)
				SwitchInputAudioDevice(nbase::UTF16ToUTF8(devices[no].device_id_));
		}
		return;
	}
	input_audio_device_->SetEnabled(false);
}

void VideoSettingFormG2::PrepareAudioOutput( bool start )
{
	VideoManagerG2* vm = VideoManagerG2::GetInstance();

	bool open_output = true;
	bool silent = false;
	if (start)
	{
		open_output_checkbox_->Selected(open_output, false);
		output_silent_checkbox_->Selected(silent, false);
		StartAudioPreview();
	}
	else
	{
		open_output = open_output_checkbox_->IsSelected();
		silent = output_silent_checkbox_->IsSelected();
		StopAudioPreview();
	}

	if (silent)
		output_volumn_slider_->SetValue(0);
	else
	{
		//int volumn = nim::VChat::GetAudioVolumn(false);
		int volumn = VideoManagerG2::GetInstance()->GetAudioVolumn(false);
		output_volumn_slider_->SetValue( volumn );
		if( volumn == 0 )
			output_silent_checkbox_->Selected( true, false );
	}

	std::string device_path;
	int no = 0;
	auto devices = vm->GetDeviceInfo(nim::kNIMDeviceTypeAudioOut, &no);
	bool can_use = devices.size() > 0;// vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeAudioOut);

	open_output_checkbox_->SetEnabled( can_use );

	if(can_use)
	{
		output_audio_device_->SelectItem(no);
		output_audio_device_->SetEnabled(open_output);

		if(open_output && start)
			SwitchOutputAudioDevice(nbase::UTF16ToUTF8(devices[no].device_id_));
		return;
	}
	output_audio_device_->SetEnabled(false);
}

void VideoSettingFormG2::PrepareVideoInput( bool start )
{
	//video_show_ctrl_->Clear();

	VideoManagerG2* vm = VideoManagerG2::GetInstance();

	bool open_video = true;
	if (start)
	{
		open_video_checkbox_->Selected(open_video, false);
	}
	else
	{
		open_video = open_video_checkbox_->IsSelected();
	}

	std::string device_path;
	int no = 0;
	auto devices = vm->GetDeviceInfo(nim::kNIMDeviceTypeVideo, &no);
	bool can_use = devices.size() > 0; //vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeVideo);

	//open_video_checkbox_->SetEnabled( can_use );

	if(can_use)
	{
		input_video_device_->SelectItem(no);
		input_video_device_->SetEnabled( open_video );

		if( !open_video )
		{
			error_notice_label_->SetVisible( true );
			camera_fail_ctrl_->SetVisible( true );
		}

		if(open_video && start)
		{
			SwitchInputVideoDevice(nbase::UTF16ToUTF8(devices[no].device_id_));
			StartVideoPreview();
		}

		return;
	}
	input_video_device_->SetEnabled(false);

	error_notice_label_->SetVisible(true);
	camera_fail_ctrl_->SetVisible( true );
}

void VideoSettingFormG2::PrepareVideoQuality(int current_video_quality)
{
	if (video_quality_list_)
	{
		for (int i = 0; i < video_quality_list_->GetCount(); i++)
		{
			auto label = video_quality_list_->GetItemAt(i);
			int video_quality = 0;
			nbase::StringToInt(label->GetDataID(), &video_quality);
			if (current_video_quality == video_quality)
			{
				video_quality_list_->SelectItem(i);
				return;
			}
		}
	}
}

void VideoSettingFormG2::PaintVideo()
{
	int cur = tabbox_->GetCurSel();
	//if (cur == 1 && camera_is_open_)
	{
		//video_show_ctrl_->Refresh(this);
	}
}

void VideoSettingFormG2::OnAudioInDeviceChange(uint32_t status)
{
	int cur = tabbox_->GetCurSel();
	if(cur == 0)
	{
		InitDeviceList(false, true);
		PrepareAudioInput(false);
	}
}

void VideoSettingFormG2::OnVideoInDeviceChange(uint32_t status)
{
	int cur = tabbox_->GetCurSel();
	if(cur == 1)
	{
		InitDeviceList(true, false);
		PrepareVideoInput(false);

		if (status & nim::kNIMDeviceStatusEnd)
		{
			//camera_is_open_ = false;
			camera_fail_ctrl_->SetVisible( true );
			error_notice_label_->SetVisible(true);
			open_video_checkbox_->Selected(false, false);
		}
		else if (status & nim::kNIMDeviceStatusStart)
		{
			//camera_is_open_ = true;
			camera_fail_ctrl_->SetVisible( false );
			error_notice_label_->SetVisible(false);
			open_video_checkbox_->Selected(true, false);
		}
		//video_show_ctrl_->Clear( );
	}
}
}
#include "video_setting.h"
#include "module/video/video_manager.h"

using namespace ui;

namespace nim_comp
{
const LPCTSTR VideoSettingForm::kClassName = L"VideoSettingForm";

VideoSettingForm::VideoSettingForm()
{
	camera_is_open_ = false;
}

VideoSettingForm::~VideoSettingForm()
{
}

std::wstring VideoSettingForm::GetSkinFolder()
{
	return L"video";
}

std::wstring VideoSettingForm::GetSkinFile()
{
	return L"video_setting.xml";
}

ui::Control* VideoSettingForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BitmapControl"))
	{
		return new ui::BitmapControl(&nim_comp::VideoManager::GetInstance()->video_frame_mng_);
	}
	return NULL;
}

LRESULT VideoSettingForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		this->Close();
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void VideoSettingForm::InitWindow()
{
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&VideoSettingForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&VideoSettingForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&VideoSettingForm::OnSelect, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventUnSelect, nbase::Bind(&VideoSettingForm::OnSelect, this, std::placeholders::_1));

	tabbox_ = (TabBox*) FindControl(L"audio_tab_video");

	option_audio_ = (OptionBox*) FindControl(L"option_audio");
	option_video_ = (OptionBox*) FindControl(L"option_video");

	option_audio_->AttachSelect(nbase::Bind(&VideoSettingForm::OnClicked, this, std::placeholders::_1));
	option_video_->AttachSelect(nbase::Bind(&VideoSettingForm::OnClicked, this, std::placeholders::_1));

	open_output_checkbox_	= (CheckBox*) FindControl(L"open_output");
	output_audio_device_	= (Combo*)  FindControl(L"output_audio");
	output_silent_checkbox_ = (CheckBox*) FindControl(L"output_silent");
	output_volumn_slider_	= (Slider*) FindControl(L"output_volumn");

	open_input_checkbox_	= (CheckBox*) FindControl(L"open_input");
	input_audio_device_		= (Combo*)  FindControl(L"input_audio");
	input_silent_checkbox_  = (CheckBox*) FindControl(L"input_silent");
	input_volumn_slider_	= (Slider*) FindControl(L"input_volumn");

	auto_input_volumn_checkbox_    = (CheckBox*) FindControl(L"auto_input_volumn");

	video_show_ctrl_ = (BitmapControl*) FindControl(L"video_show");

	camera_fail_ctrl_ = FindControl( L"camera_fail" );
	error_notice_label_ = (Label*) FindControl(L"error_notice");

	open_video_checkbox_ = (CheckBox*) FindControl(L"open_video");
	input_video_device_  = (Combo*) FindControl(L"input_video");
}

void VideoSettingForm::OnFinalMessage(HWND hWnd)
{
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting);
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting);
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeSetting);
	VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting);
	
	__super::OnFinalMessage(hWnd);
}

bool VideoSettingForm::Notify(ui::EventArgs* msg)
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
	}
	else if(msg->Type == ui::kEventValueChange)
	{
		if(name == L"output_volumn")
		{
			int v = output_volumn_slider_->GetValue();

			VideoManager::GetInstance()->SetAudioVolumn(v, false);
		}
		else if(name == L"input_volumn")
		{
			int v = input_volumn_slider_->GetValue();

			VideoManager::GetInstance()->SetAudioVolumn(v, true);
			VideoManager::GetInstance()->SetAudioInputAutoVolumn(false);
		}
	}
	return true;
}

bool VideoSettingForm::OnClicked( ui::EventArgs* arg )
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

bool VideoSettingForm::OnSelect( ui::EventArgs* arg )
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
			VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting, true);
			VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOutChat, kDeviceSessionTypeSetting, true);
		}
		VideoManager::GetInstance()->SetSettingAudioOutStatus(select);
	}
	else if(name == L"output_silent")
	{
		if (output_silent_checkbox_->IsSelected())
		{
			VideoManager::GetInstance()->SetAudioVolumn(0, false);
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
			VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting, true);
		}
		VideoManager::GetInstance()->SetSettingAudioInStatus(select);
	}
	else if(name == L"input_silent")
	{	
		if (input_silent_checkbox_->IsSelected())
		{
			VideoManager::GetInstance()->SetAudioVolumn(0, true);
		}
	}
	else if(name == L"auto_input_volumn")
	{
		bool check = auto_input_volumn_checkbox_->IsSelected();
		VideoManager::GetInstance()->SetAudioInputAutoVolumn(check);
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
			VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting, true);
		}
		VideoManager::GetInstance()->SetSettingVideoInStatus(select);
	}
	return true;
}

void VideoSettingForm::InitDeviceList( bool video, bool input )
{
	Combo* combo = NULL;
	std::list<MEDIA_DEVICE_DRIVE_INFO> device;

	if(video)
	{
		combo = input_video_device_;

		device = VideoManager::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeVideo);
		//input_videos_.assign(device.begin(), device.end());
	}
	else if(input)
	{
		combo = input_audio_device_;

		device = VideoManager::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeAudioIn);
		//input_audios_.assign(device.begin(), device.end());
	}
	else
	{
		combo = output_audio_device_;

		device = VideoManager::GetInstance()->GetDeviceInfo(nim::kNIMDeviceTypeAudioOut);
		//output_audios_.assign(device.begin(), device.end());
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
			label->SetUTF8Text(i->friendly_name_);
			label->SetUTF8DataID(i->device_path_);  

			combo->Add(label);
		}
		combo->SetEnabled(true);
	}
}

void VideoSettingForm::ShowPage( bool video )
{
	camera_is_open_ = false;
	video_show_ctrl_->Clear( );

	if(video)
	{
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioIn, kDeviceSessionTypeSetting);
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeAudioOut, kDeviceSessionTypeSetting);
		option_video_->Selected(true, false);

		InitDeviceList(true, true);

		tabbox_->SelectItem(1);

		error_notice_label_->SetVisible(false);
		camera_fail_ctrl_->SetVisible( true );
		PrepareVideoInput(VideoManager::GetInstance()->GetSettingVideoInStatus());
	}
	else
	{
		paint_video_timer_.Cancel();
		VideoManager::GetInstance()->EndDevice(nim::kNIMDeviceTypeVideo, kDeviceSessionTypeSetting);

		option_audio_->Selected(true, false);
		
		InitDeviceList(false, false);

		InitDeviceList(false, true);

		tabbox_->SelectItem(0);

		PrepareAudioOutput(VideoManager::GetInstance()->GetSettingAudioOutStatus());
		PrepareAudioInput(VideoManager::GetInstance()->GetSettingAudioInStatus());
	}
}

void VideoSettingForm::SwitchInputAudioDevice( const std::string &device_path )
{
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioIn, device_path, kDeviceSessionTypeSetting);
}

void VideoSettingForm::SwitchOutputAudioDevice( const std::string &device_path )
{
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOut, device_path, kDeviceSessionTypeSetting);
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeAudioOutChat, device_path, kDeviceSessionTypeSetting);
}

void VideoSettingForm::SwitchInputVideoDevice( const std::string &device_path )
{
	camera_is_open_ = false;
	camera_fail_ctrl_->SetVisible( true );
	error_notice_label_->SetVisible(false);
	VideoManager::GetInstance()->StartDevice(nim::kNIMDeviceTypeVideo, device_path, kDeviceSessionTypeSetting);
}

void VideoSettingForm::OnVideoDeviceStartCallback(bool result)
{
	camera_is_open_ = result;
	if( result )
	{
		paint_video_timer_.Cancel();
		StdClosure task = nbase::Bind(&VideoSettingForm::PaintVideo, this);
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
void VideoSettingForm::AudioVolumnChange()
{
	bool auto_volumn = nim::VChat::GetAudioInputAutoVolumn();
	auto_input_volumn_checkbox_->Selected(auto_volumn, false);
	int volumn = nim::VChat::GetAudioVolumn(true);
	input_volumn_slider_->SetValue(volumn);
	input_silent_checkbox_->Selected(volumn==0, false);
	volumn = nim::VChat::GetAudioVolumn(false);
	output_volumn_slider_->SetValue(volumn);
	output_silent_checkbox_->Selected(volumn == 0, false);
}

void VideoSettingForm::PrepareAudioInput( bool start )
{
	VideoManager* vm = VideoManager::GetInstance();

	bool open_input = true;
	if (start)
	{
		open_input_checkbox_->Selected(open_input, false);
	}
	else
	{
		open_input = open_input_checkbox_->IsSelected();
	}

	bool auto_volumn = nim::VChat::GetAudioInputAutoVolumn();
	auto_input_volumn_checkbox_->Selected(auto_volumn, false);

	bool silent = false;
	input_silent_checkbox_->Selected(silent, false);

	if( silent )
		input_volumn_slider_->SetValue( 0 );
	else
	{
		int volumn = nim::VChat::GetAudioVolumn(true);
		input_volumn_slider_->SetValue( volumn );
		if( volumn == 0 )
			input_silent_checkbox_->Selected( true, false );
	}

	std::string device_path;
	int no = 0;
	bool can_use = vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeAudioIn);

	open_input_checkbox_->SetEnabled( can_use );

	if(can_use)
	{
		input_audio_device_->SelectItem(no);
		input_audio_device_->SetEnabled( open_input );

		if(open_input)
		{
			//UI设置结束，开始启动设备
			if(start)
				SwitchInputAudioDevice(device_path);
		}
		return;
	}
	input_audio_device_->SetEnabled(false);
}

void VideoSettingForm::PrepareAudioOutput( bool start )
{
	VideoManager* vm = VideoManager::GetInstance();

	bool open_output = true;
	bool silent = false;
	if (start)
	{
		open_output_checkbox_->Selected(open_output, false);
		output_silent_checkbox_->Selected(silent, false);
	}
	else
	{
		open_output = open_output_checkbox_->IsSelected();
		silent = output_silent_checkbox_->IsSelected();
	}

	if (silent)
		output_volumn_slider_->SetValue(0);
	else
	{
		int volumn = nim::VChat::GetAudioVolumn(false);
		output_volumn_slider_->SetValue( volumn );
		if( volumn == 0 )
			output_silent_checkbox_->Selected( true, false );
	}

	std::string device_path;
	int no = 0;
	bool can_use = vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeAudioOut);

	open_output_checkbox_->SetEnabled( can_use );

	if(can_use)
	{
		output_audio_device_->SelectItem(no);
		output_audio_device_->SetEnabled(open_output);

		if(open_output && start)
			SwitchOutputAudioDevice(device_path);
		return;
	}
	output_audio_device_->SetEnabled(false);
}

void VideoSettingForm::PrepareVideoInput( bool start )
{
	video_show_ctrl_->Clear();

	VideoManager* vm = VideoManager::GetInstance();

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
	bool can_use = vm->GetDefaultDevicePath(no, device_path, nim::kNIMDeviceTypeVideo);

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
			SwitchInputVideoDevice(device_path);
		return;
	}
	input_video_device_->SetEnabled(false);

	error_notice_label_->SetVisible(true);
	camera_fail_ctrl_->SetVisible( true );
}
void VideoSettingForm::PaintVideo()
{
	int cur = tabbox_->GetCurSel();
	if (cur == 1 && camera_is_open_)
	{
		video_show_ctrl_->Refresh(this);
	}
}

void VideoSettingForm::OnAudioInDeviceChange(uint32_t status)
{
	int cur = tabbox_->GetCurSel();
	if(cur == 0)
	{
		InitDeviceList(false, true);
		PrepareAudioInput(false);
	}
}

void VideoSettingForm::OnVideoInDeviceChange(uint32_t status)
{
	int cur = tabbox_->GetCurSel();
	if(cur == 1)
	{
		InitDeviceList(true, false);
		PrepareVideoInput(false);

		if (status & nim::kNIMDeviceStatusEnd)
		{
			camera_is_open_ = false;
			camera_fail_ctrl_->SetVisible( true );
			error_notice_label_->SetVisible(true);
			open_video_checkbox_->Selected(false, false);
		}
		else if (status & nim::kNIMDeviceStatusStart)
		{
			camera_is_open_ = true;
			camera_fail_ctrl_->SetVisible( false );
			error_notice_label_->SetVisible(false);
			open_video_checkbox_->Selected(true, false);
		}
		video_show_ctrl_->Clear( );
	}
}
}

#include "video_manager.h"
#include "gui/video/video_form.h"
#include "gui/video/video_setting.h"
#include "util/windows_manager.h"
#include "callback/vchat/vchat_callback.h"
#include "../rts/rts_manager.h"


namespace nim_comp
{

VideoManager::VideoManager()
{
	for (int i = nim::kNIMDeviceTypeAudioIn; i <= nim::kNIMDeviceTypeVideo; i++)
	{
		device_session_type_[i] = kDeviceSessionTypeNone;
	}
}

VideoManager::~VideoManager()
{

}

void VideoManager::OnDeviceStatus(nim::NIMDeviceType type, UINT status, std::string path)
{
	if (device_session_type_[type] == kDeviceSessionTypeNone)
	{
		nim::VChat::RemoveDeviceStatusCb(type);
	}
	VideoForm *video_form = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
	if (video_form)
	{
		if (type == nim::kNIMDeviceTypeAudioIn)
		{
			video_form->OnAudioInDeviceChange(status);
		}
		else if (type == nim::kNIMDeviceTypeVideo)
		{
			video_form->OnVideoInDeviceChange(status);
		}
	}
	VideoSettingForm *setting_form = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
	if (setting_form)
	{
		if (type == nim::kNIMDeviceTypeAudioIn)
		{
			setting_form->OnAudioInDeviceChange(status);
		}
		else if (type == nim::kNIMDeviceTypeVideo)
		{
			setting_form->OnVideoInDeviceChange(status);
		}
	}
}
void VideoManager::OnStartDeviceCb(nim::NIMDeviceType type, bool ret)
{
	VideoForm *video_form = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
	VideoSettingForm *setting_form = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
	switch (type)
	{
	case nim::kNIMDeviceTypeVideo:
		if (video_form)
		{
			video_form->OnVideoDeviceStartCallback(ret);
		}
		if (setting_form)
		{
			setting_form->OnVideoDeviceStartCallback(ret);
		}
		break;
	case nim::kNIMDeviceTypeAudioIn:
		if (video_form)
		{
			video_form->OnAudioDeviceStartCallback(ret);
		}
		break;
	}
}
//void VideoManager::OnVideoDataCb(bool capture, const std::string& data, unsigned int width, unsigned int height, const std::string& json)
//{
//	ui::CSize size;
//	size.cx = width;
//	size.cy = height;
//	VideoForm *video_form = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
//	if (video_form && video_form->IsStart())
//	{
//		if (capture)
//		{
//			video_form->OnVideoData(data, size, 0);
//		} 
//		else
//		{
//			video_form->OnRecVideo(data, size, 0);
//		}
//	}
//	VideoSettingForm *setting_form = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
//	if (setting_form)
//	{
//		if (capture)
//		{
//			setting_form->OnInputVideoData(data, size, 0);
//		}
//	}
//	if (!capture && chatroom_videorec_cb_)
//	{
//		Json::Value valus;
//		Json::Reader reader;
//		std::string uid;
//		if (reader.parse(json, valus))
//		{
//			uid = valus[nim::kNIMDeviceDataAccount].asString();
//		}
//		//发送的成员id json
//		chatroom_videorec_cb_(data, size, uid);
//	}
//}
void VideoManager::OnVChatEvent(nim::NIMVideoChatSessionType type, uint64_t channel_id, int code, const std::string& json)
{
	if (type != nim::kNIMVideoChatSessionTypeInfoNotify
		&& type != nim::kNIMVideoChatSessionTypeVolumeNotify)
	{
		QLOG_APP(L"OnVChatEvent type={0}, channel_id={1}, code={2}, json={3}") << type << channel_id << code << json;
	}
	switch (type)
	{
	case nim::kNIMVideoChatSessionTypeStartRes:{
		VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
		if (window && window->IsStart())
		{
			window->ChatStartCallback(code == 200, channel_id);
		}
		else
		{
			QLOG_ERR(L"ChatStart no window");
			EndChat(json);
		}
	}break;
	case nim::kNIMVideoChatSessionTypeInviteNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			std::string uid = valus[nim::kNIMVChatUid].asString();
			int mode = valus[nim::kNIMVChatType].asInt();
			if (!ShowVideoChatForm(uid, mode == nim::kNIMRtsVideoChatModeVideo, channel_id))
			{
				VChatControl(channel_id, nim::kNIMTagControlBusyLine);
				VChatCalleeAck(channel_id, false, "");
			}
			else
			{
				VChatControl(channel_id, nim::kNIMTagControlReceiveStartNotifyFeedback);
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeCalleeAckRes:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			//std::string uid = valus[nim::kNIMVChatUid].asString();
			bool accept = valus[nim::kNIMVChatAccept].asInt() > 0;
			if (accept && code != 200)
			{
				VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
				if (window)
				{
					window->OnLogin(0);
				}
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeCalleeAckNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			//std::string uid = valus[nim::kNIMVChatUid].asString();
			bool accept = valus[nim::kNIMVChatAccept].asInt() > 0;
			VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
			if (window && window->IsStart())
			{
				if (!accept)
				{
					window->OnRejected(channel_id);
				}
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeControlRes:{
		//
	}break;
	case nim::kNIMVideoChatSessionTypeControlNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			//std::string uid = valus[nim::kNIMVChatUid].asString();
			int type = valus[nim::kNIMVChatType].asInt();
			VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
			if (window && window->IsStart())
			{
				window->OnControlModeChange(channel_id, (nim::NIMVChatControlType)type);
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeConnect:{
		VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
		if (window && window->IsStart())
		{
			window->OnLogin(code == 200);
			int bitrate = atoi(GetConfigValue("video_bitrate").c_str());
			if (bitrate > 0)
			{
				nim::VChat::SetVideoBitrate(bitrate);
			}
		}
		else if (chatroom_connect_cb_)
		{
			chatroom_connect_cb_(code);
		}
	}break;
	case nim::kNIMVideoChatSessionTypePeopleStatus:{
		VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
		if (window && window->IsStart())
		{
			if (code == nim::kNIMVideoChatSessionStatusJoined)
			{
				window->OnComeIn(0);
			}
			else if (code == nim::kNIMVideoChatSessionStatusLeaved)
			{
				window->OnComeOut(0);
			}
		}
		else if (chatroom_people_cb_)
		{
			Json::Value valus;
			Json::Reader reader;
			if (reader.parse(json, valus))
			{
				std::string uid = valus[nim::kNIMVChatUid].asString();
				if (!uid.empty())
				{
					chatroom_people_cb_(uid, code == nim::kNIMVideoChatSessionStatusJoined);
				}
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeNetStatus:{
		VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
		if (window && window->IsStart())
		{
			window->OnNetstatChange(code);
		}
	}break;
	case nim::kNIMVideoChatSessionTypeHangupRes:{
		//
	}break;
	case nim::kNIMVideoChatSessionTypeHangupNotify:{
		VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
		if (window)
		{
			window->OnHangup(channel_id);
		}
	}break;
	case nim::kNIMVideoChatSessionTypeSyncAckNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			bool accept = valus[nim::kNIMVChatAccept].asInt() > 0;
			VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
			if (window)
			{
				window->OnVideochatSyncNotify(accept, channel_id);
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeMp4Notify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
			if (window && window->IsStart())
			{
				if (valus[nim::kNIMVChatMp4Start].isObject())
				{
					std::string path = valus[nim::kNIMVChatMp4Start][nim::kNIMVChatMp4File].asString();
					int64_t time = valus[nim::kNIMVChatMp4Start][nim::kNIMVChatTime].asInt64();
					window->OnStartRecord(path, time);
				}
				else if (valus[nim::kNIMVChatMp4Close].isObject())
				{
					std::string path = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatMp4File].asString();
					int code = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatStatus].asInt();
					int64_t time = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatTime].asInt64();
					window->OnStopRecord(code, path, time);
				}
			}
		}
	}break;
	}
}

bool VideoManager::ShowVideoChatForm(std::string id, bool video, uint64_t channel_id)
{
	if (IsVideoChatFormExist(true) || RtsManager::GetInstance()->IsRtsFormExist(nim::kNIMRtsChannelTypeVchat, true))
	{
		return false;
	}
	else
	{
		VideoForm *window = new VideoForm(id);
		window->Create(NULL, L"音视频通话", WS_OVERLAPPEDWINDOW, 0, false);
		window->CenterWindow();
		window->ShowWindow();
		if (channel_id == 0)
		{
			window->StartCalling(video);
		}
		else
		{
			window->StartReceiving(channel_id, video);
		}
	}
	return true;
}
bool VideoManager::IsVideoChatFormExist(bool show)
{
	VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
	if (window)
	{
		if (show)
		{
			window->ShowWindow(true, true);
		}
		return true;
	}
	return false;
}
bool VideoManager::ShowVideoSetting(bool video)
{
	VideoSettingForm *window = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
	if (window)
	{
		window->ShowWindow(true, true);
		window->ShowPage(video);
		SetForegroundWindow(window->GetHWND());
		return false;
	}
	else
	{
		window = new VideoSettingForm();
		window->Create(NULL, L"音视频设置", WS_OVERLAPPEDWINDOW, 0);
		window->CenterWindow();
		window->ShowWindow();
		window->ShowPage(video);
	}
	return true;
}
void VideoManager::GetDeviceByJson(bool ret, nim::NIMDeviceType type, const char* json)
{
	device_info_list_[type].clear();
	if (ret)
	{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus) && valus.isArray())
		{
			int num = valus.size();
			for (int i=0;i<num;++i)
			{
				Json::Value device;
				device = valus.get(i, device);
				MEDIA_DEVICE_DRIVE_INFO info;
				info.device_path_ = device[nim::kNIMDevicePath].asString();
				info.friendly_name_ = device[nim::kNIMDeviceName].asString();
				device_info_list_[type].push_back(info);
			}
		}
	}
}

bool VideoManager::GetDefaultDevicePath(int &no, std::string &device_path, nim::NIMDeviceType type)
{
	if (type == nim::kNIMDeviceTypeAudioOutChat)
	{
		type = nim::kNIMDeviceTypeAudioOut;
	}
	no = 0;
	device_path.clear();
	GetDeviceInfo(type);
	if(device_info_list_[type].size() != 0)
	{
		if( !def_device_path_[type].empty() )
		{
			int k = 0;
			for(auto i = device_info_list_[type].begin(); i != device_info_list_[type].end(); i++, k++)
			{
				if(i->device_path_ == def_device_path_[type])
				{
					no = k;
					device_path = def_device_path_[type];
					break;
				}
			}
		}
		if( device_path.empty() )
		{
			no = 0;
			device_path = device_info_list_[type].begin()->device_path_;
			def_device_path_[type] = device_path;
		}
	}

	return !device_path.empty();
}

void VideoManager::SetDefaultDevicePath(std::string device_path, nim::NIMDeviceType type)
{
	if (type == nim::kNIMDeviceTypeAudioOutChat)
	{
		type = nim::kNIMDeviceTypeAudioOut;
	}
	def_device_path_[type] = device_path;
}
std::list<MEDIA_DEVICE_DRIVE_INFO> VideoManager::GetDeviceInfo(nim::NIMDeviceType type)
{
	nim::VChat::EnumDeviceDevpath(type, &VChatCallback::EnumDevCb);
	return device_info_list_[type];
}
//音频设置	
void VideoManager::SetAudioVolumn(unsigned char volumn, bool capture)
{
	nim::VChat::SetAudioVolumn(volumn, capture);
	VideoSettingForm *setting_form = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
	if (setting_form)
	{
		setting_form->AudioVolumnChange();
	}
	VideoForm *window = (VideoForm*)(WindowsManager::GetInstance()->GetWindow(VideoForm::kClassName, VideoForm::kClassName));
	if (window)
	{
		window->InitVolumnSetting();
	}
}
//自动调节麦克风音量
void VideoManager::SetAudioInputAutoVolumn(bool auto_volumn)
{
	nim::VChat::SetAudioInputAutoVolumn(auto_volumn);
	VideoSettingForm *setting_form = (VideoSettingForm*)(WindowsManager::GetInstance()->GetWindow(VideoSettingForm::kClassName, VideoSettingForm::kClassName));
	if (setting_form)
	{
		setting_form->AudioVolumnChange();
	}
}


void VideoManager::StartDevice(nim::NIMDeviceType type, std::string device_path, DeviceSessionType session_type)
{
	if (device_path.empty())
	{
		int num_no = 0;
		GetDefaultDevicePath(num_no, device_path, type);
	}
	SetDefaultDevicePath(device_path, type);
	int width = 1280;
	int height = 720;
	if (1)
	{
		width = atoi(GetConfigValue("video_width").c_str());
		height = atoi(GetConfigValue("video_height").c_str());
	}
	nim::VChat::StartDevice(type, device_path, 50, width, height, &VChatCallback::StartDeviceCb);
	if (device_session_type_[type] == kDeviceSessionTypeNone)
	{
		nim::VChat::AddDeviceStatusCb(type, &VChatCallback::DeviceStatusCb);
	}
	device_session_type_[type] |= session_type;
}
void VideoManager::EndDevice(nim::NIMDeviceType type, DeviceSessionType session_type, bool forced)
{
	device_session_type_[type] &= ~session_type;
	if (device_session_type_[type] == kDeviceSessionTypeNone || forced)
	{
		nim::VChat::EndDevice(type);
	}
}
//通话
bool VideoManager::StartChat(nim::NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& uid, const std::string& session_id)
{
	QLOG_APP(L"StartChat mode={0}, uid={1}") << mode << uid;
	Json::FastWriter fs;
	Json::Value value;
	value[nim::kNIMVChatSessionId] = session_id;
	value[nim::kNIMVChatUids].append(uid);
	value[nim::kNIMVChatSound] = "video_chat_tip_receiver.aac";
	value[nim::kNIMVChatNeedBadge] = 0;
	value[nim::kNIMVChatNeedFromNick] = 0;
	if (1)
	{
		std::string video_quality = GetConfigValue("video_quality");
		std::string audio_record = GetConfigValue("audio_record");
		std::string video_record = GetConfigValue("video_record");
		value[nim::kNIMVChatVideoQuality] = atoi(video_quality.c_str());
		value[nim::kNIMVChatRecord] = atoi(audio_record.c_str());
		value[nim::kNIMVChatVideoRecord] = atoi(video_record.c_str());
	}
	std::string json_value = fs.write(value);
	return nim::VChat::Start(mode, nbase::UTF16ToUTF8(mode == nim::kNIMVideoChatModeAudio ? L"语音通话邀请test" : L"视频通话邀请test"), "test custom info", json_value);
}
//设置视频类型
bool VideoManager::SetVoipMode(nim::NIMVideoChatMode mode)
{
	QLOG_APP(L"SetVoipMode mode={0}") << mode;
	return nim::VChat::SetTalkingMode(mode, "");
}
//回应邀请
bool VideoManager::VChatCalleeAck(uint64_t channel_id, bool accept, const std::string& session_id)
{
	QLOG_APP(L"VChatCalleeAck channel_id={0}, accept={1}") << channel_id << accept;
	std::string json_value;
	if (accept)
	{
		std::string video_quality = GetConfigValue("video_quality");
		std::string audio_record = GetConfigValue("audio_record");
		std::string video_record = GetConfigValue("video_record");
		Json::FastWriter fs;
		Json::Value value;
		value[nim::kNIMVChatVideoQuality] = atoi(video_quality.c_str());
		value[nim::kNIMVChatRecord] = atoi(audio_record.c_str());
		value[nim::kNIMVChatVideoRecord] = atoi(video_record.c_str());
		value[nim::kNIMVChatSessionId] = session_id;
		json_value = fs.write(value);
	}
	return nim::VChat::CalleeAck(channel_id, accept, json_value);
}
//通话控制
bool VideoManager::VChatControl(uint64_t channel_id, nim::NIMVChatControlType type)
{
	QLOG_APP(L"VChatControl channel_id={0}, type={1}") << channel_id << type;
	return nim::VChat::Control(channel_id, type);
}
void VideoManager::EndChat(const std::string& session_id)
{
	QLOG_APP(L"EndChat {0}") << session_id;
	Json::FastWriter fs;
	Json::Value value;
	value[nim::kNIMVChatSessionId] = session_id;
	std::string json_value = fs.write(value);
	nim::VChat::End(json_value);
}

void VideoManager::SetViewerMode(bool viewer)
{
	nim::VChat::SetViewerMode(viewer);
}

bool VideoManager::GetViewerMode()
{
	return nim::VChat::GetViewerMode();
}

void VideoManager::SetAudioMuted(bool muted)
{
	nim::VChat::SetAudioMuted(muted);
}

bool VideoManager::GetAudioMuteEnabled()
{
	return nim::VChat::GetAudioMuteEnabled();
}

void VideoManager::SetMemberBlacklist(const std::string& uid, bool add, bool audio, nim::VChat::OptCallback cb)
{
	nim::VChat::SetMemberBlacklist(uid, add, audio, "", cb);
}

void VideoManager::CreateRoom(const std::string& room_name, const std::string& custom_info, nim::VChat::Opt2Callback cb)
{
	nim::VChat::CreateRoom(room_name, custom_info, "", cb);
}

bool VideoManager::JoinRoom(nim::NIMVideoChatMode mode, const std::string& room_name, const std::string& session_id, nim::VChat::Opt2Callback cb)
{
	Json::FastWriter fs;
	Json::Value value;
	value[nim::kNIMVChatSessionId] = session_id;
	std::string json_value = fs.write(value);
	return nim::VChat::JoinRoom(mode, room_name, json_value, cb);
}

void VideoManager::SetChatRoomCb(ConnectCallback connect_cb /*= nullptr*/, PeopleChangeCallback people_cb /*= nullptr*/)
{
	chatroom_connect_cb_ = connect_cb;
	chatroom_people_cb_ = people_cb;
}
}
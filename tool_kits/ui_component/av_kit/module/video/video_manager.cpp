
#include "video_manager.h"
#include "callback/vchat/vchat_callback.h"
#include "shared/ui/toast/toast.h"
//#include "gui/contact_select_form/contact_select_form.h"
//#include "module/session/session_manager.h"

namespace nim_comp
{

VideoManager::VideoManager()
{
	for (int i = nim::kNIMDeviceTypeAudioIn; i <= nim::kNIMDeviceTypeVideo; i++)
	{
		device_session_type_[i] = kDeviceSessionTypeNone;
	}
	webrtc_setting_ = false;
	setting_audio_in_ = true;
	setting_audio_out_ = true;
	setting_video_in_ = true;
	vchat_status_ = kMultiVChatEnd;
}

VideoManager::~VideoManager()
{
	if (video_form_ == NULL || !IsWindow(video_form_flag_.expired() ? NULL : video_form_->GetHWND()))
	{
		video_form_.release();
	}
	if (multi_video_form_ == NULL || !IsWindow(multi_video_form_flag_.expired() ? NULL : multi_video_form_->GetHWND()))
	{
		multi_video_form_.release();
	}
	if (setting_form_ == NULL || !IsWindow(setting_form_flag_.expired() ? NULL : setting_form_->GetHWND()))
	{
		setting_form_.release();
	}
	if (multi_vchat_invite_form_ == NULL || !IsWindow(multi_vchat_invite_form_flag_.expired() ? NULL : multi_vchat_invite_form_->GetHWND()))
	{
		multi_vchat_invite_form_.release();
	}
}

void VideoManager::OnDeviceStatus(nim::NIMDeviceType type, UINT status, std::string path)
{
	if (device_session_type_[type] == kDeviceSessionTypeNone)
	{
		nim::VChat::RemoveDeviceStatusCb(type);
	}
		
	if (video_form_ != NULL && !video_form_flag_.expired())
	{
		if (type == nim::kNIMDeviceTypeAudioIn)
		{
			video_form_->OnAudioInDeviceChange(status);
		}
		else if (type == nim::kNIMDeviceTypeVideo)
		{
			video_form_->OnVideoInDeviceChange(status);
		}
	}

	if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
	{
		if (type == nim::kNIMDeviceTypeAudioIn)
		{
			multi_video_form_->OnAudioInDeviceChange(status);
		}
		else if (type == nim::kNIMDeviceTypeVideo)
		{
			multi_video_form_->OnVideoInDeviceChange(status);
		}
	}

	if (setting_form_ != NULL && !setting_form_flag_.expired())
	{
		if (type == nim::kNIMDeviceTypeAudioIn)
		{
			setting_form_->OnAudioInDeviceChange(status);
		}
		else if (type == nim::kNIMDeviceTypeVideo)
		{
			setting_form_->OnVideoInDeviceChange(status);
		}
	}
}
void VideoManager::OnStartDeviceCb(nim::NIMDeviceType type, bool ret)
{
	switch (type)
	{
	case nim::kNIMDeviceTypeVideo:
		if (video_form_ != NULL && !video_form_flag_.expired())
		{
			video_form_->OnVideoDeviceStartCallback(ret);
		}
		if (setting_form_ != NULL && !setting_form_flag_.expired())
		{
			setting_form_->OnVideoDeviceStartCallback(ret);
		}
		if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
		{
			multi_video_form_->OnVideoDeviceStartCallback(ret);
		}
		break;
	case nim::kNIMDeviceTypeAudioIn:
		if (video_form_ != NULL && !video_form_flag_.expired())
		{
			video_form_->OnAudioDeviceStartCallback(ret);
		}
		if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
		{
			multi_video_form_->OnAudioDeviceStartCallback(ret);
		}
		break;
	}
}

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
		if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
		{
			video_form_->ChatStartCallback(code == 200, channel_id);
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
				VChatCalleeAck(channel_id, nim_comp::kVChatAskBusLine, "");
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
				if (video_form_ != NULL && !video_form_flag_.expired())
				{
					video_form_->OnLogin(0);
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
			if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
			{
				if (!accept)
				{
					video_form_->OnRejected(channel_id);
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
			if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
			{
				video_form_->OnControlModeChange(channel_id, (nim::NIMVChatControlType)type);
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeConnect:{
		if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
		{
			video_form_->OnLogin(code == 200);
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
		Json::Value valus;
		Json::Reader reader;
		std::string uid = "";
		if (reader.parse(json, valus))
		{
			uid = valus[nim::kNIMVChatUid].asString();
			int32_t leave_type = valus[nim::kNIMVChatStatus].asInt();
			QLOG_APP(L"vchat people leave uid: {0}, leave_type {1}") << uid << leave_type;
		}
		if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
		{
			if (code == nim::kNIMVideoChatSessionStatusJoined)
				multi_video_form_->OnPeopleJoin(uid);
			else
				multi_video_form_->OnPeopleLeave(uid);
		}

		if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
		{
			if (code == nim::kNIMVideoChatSessionStatusJoined)
			{
				video_form_->OnComeIn(0);
			}
			else if (code == nim::kNIMVideoChatSessionStatusLeaved)
			{
				Json::Value valus;
				Json::Reader reader;
				if (reader.parse(json, valus))
				{
					std::string uid = valus[nim::kNIMVChatUid].asString();
					int32_t leave_type = valus[nim::kNIMVChatStatus].asInt();
					QLOG_APP(L"vchat people leave uid: {0}, leave_type {1}") << uid << leave_type;
				}
				video_form_->OnComeOut(0);
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
		if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
		{
			video_form_->OnNetstatChange(code);
		}
	}break;
	case nim::kNIMVideoChatSessionTypeHangupRes:{
		//
	}break;
	case nim::kNIMVideoChatSessionTypeHangupNotify:{
		if (video_form_ != NULL && !video_form_flag_.expired())
		{
			video_form_->OnHangup(channel_id);
		}
	}break;
	case nim::kNIMVideoChatSessionTypeSyncAckNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			bool accept = valus[nim::kNIMVChatAccept].asInt() > 0;
			if (video_form_ != NULL && !video_form_flag_.expired())
			{
				video_form_->OnVideochatSyncNotify(accept, channel_id);
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeMp4Notify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
			{
				if (valus[nim::kNIMVChatMp4Start].isObject())
				{
					std::string path = valus[nim::kNIMVChatMp4Start][nim::kNIMVChatMp4File].asString();
					int64_t time = valus[nim::kNIMVChatMp4Start][nim::kNIMVChatTime].asInt64();
					video_form_->OnStartRecord(true, path, time);
				}
				else if (valus[nim::kNIMVChatMp4Close].isObject())
				{
					std::string path = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatMp4File].asString();
					int code = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatStatus].asInt();
					int64_t time = valus[nim::kNIMVChatMp4Close][nim::kNIMVChatTime].asInt64();
					video_form_->OnStopRecord(true, code, path, time);
				}
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeAuRecordNotify:{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			if (video_form_ != NULL && !video_form_flag_.expired() && video_form_->IsStart())
			{
				if (valus[nim::kNIMVChatAuRecordStart].isObject())
				{
					std::string path = valus[nim::kNIMVChatAuRecordStart][nim::kNIMVChatFile].asString();
					int64_t time = valus[nim::kNIMVChatAuRecordStart][nim::kNIMVChatTime].asInt64();
					video_form_->OnStartRecord(false, path, time);
				}
				else if (valus[nim::kNIMVChatAuRecordClose].isObject())
				{
					std::string path = valus[nim::kNIMVChatAuRecordClose][nim::kNIMVChatFile].asString();
					int code = valus[nim::kNIMVChatAuRecordClose][nim::kNIMVChatStatus].asInt();
					int64_t time = valus[nim::kNIMVChatAuRecordClose][nim::kNIMVChatTime].asInt64();
					video_form_->OnStopRecord(false, code, path, time);
				}
			}
		}
	}break;
	case nim::kNIMVideoChatSessionTypeVolumeNotify:
	{
		Json::Value valus;
		Json::Reader reader;
		if (reader.parse(json, valus))
		{
			if (valus.isObject() && valus[nim::kNIMVChatAudioVolume].isObject())
			{
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				std::map<std::string, int32_t> volume_infos;
				int32_t self_volume = valus[nim::kNIMVChatAudioVolume][nim::kNIMVChatSelf][nim::kNIMVChatStatus].asInt();
				volume_infos[current_user_id] = self_volume;
				if (valus[nim::kNIMVChatAudioVolume][nim::kNIMVChatReceiver].isArray())
				{
					int count = valus[nim::kNIMVChatAudioVolume][nim::kNIMVChatReceiver].size();
					for (int i = 0; i < count; ++i)
					{
						Json::Value item;
						item = valus[nim::kNIMVChatAudioVolume][nim::kNIMVChatReceiver].get(i, item);
						std::string uid = item[nim::kNIMVChatUid].asString();
						int32_t status = item[nim::kNIMVChatStatus].asInt();
						volume_infos[uid] = status;
					}
				}
				if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
				{
					multi_video_form_->OnVChatVolumeCallback(volume_infos);
				}
			}
		}
	}
	break;
	}
}

bool VideoManager::ShowVideoChatForm(std::string id, 
	bool video, 
	uint64_t channel_id, 
	const std::list<UTF8String>& friend_list,
	const MultiRoomCreatedCallback& cb)
{
	if (IsVideoChatFormExist(true) || IsMultiVideoChatFormExist(true))
	{
		return false;
	}
	else if (friend_list.size() > 0)
	{
		multi_vchat_session_id_ = id;
		DoMultiVChat(friend_list, cb);
	}
	else
	{
 		video_form_.release();
		video_form_.reset(new VideoForm(id));
		video_form_flag_ = video_form_->GetWeakFlag();
		video_form_->Create(NULL, L"", WS_OVERLAPPEDWINDOW, 0, false);
		video_form_->CenterWindow();
		video_form_->ShowWindow();
		if (channel_id == 0)
		{
			video_form_->StartCalling(video);
		}
		else
		{
			video_form_->StartReceiving(channel_id, video);
		}
	}
	return true;
}

void VideoManager::DestroyVideoChatForm()
{
    if (IsVideoChatFormExist(true) && !video_form_flag_.expired())
    {
        video_form_->DirectQuit();
    }

    if (IsMultiVideoChatFormExist(true) && !multi_video_form_flag_.expired())
    {
        multi_video_form_->DirectQuit();
    }
}

void VideoManager::DoMultiVChat(const std::list<UTF8String>& friend_list, const MultiRoomCreatedCallback& cb)
{
	//创建多人音视频房间
	//发送自定义系统消息给成员
	//显示视频界面
	UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
	std::string room_name = nim::Tool::GetUuid();
	if (atoi(GetConfigValue("kNIMMultiVideo").c_str()) < 1)
	{
		shared::Toast::ShowToast(L"功能未开放，请参考源码和群视频开发手册进行对接", 2000);
		return;
	}
	auto join_room_cb = [=](int code, int64_t channel_id, const std::string& json_extension)
	{
		StdClosure closure = [=]()
		{
			if (code == 200)
			{
				Json::FastWriter fw;
				Json::Value value;
				std::set<std::string> members;
				UserService* user_service = UserService::GetInstance();
				nim::UserNameCard user_info;
				nim::UserNameCard temp_user_info;
				user_service->GetUserInfo(LoginManager::GetInstance()->GetAccount(), user_info);
				std::wstring team_name = nbase::UTF8ToUTF16(user_info.GetName());
				for each (auto friend_info in friend_list)
				{
					if (!team_name.empty())
					{
						team_name.append(L"、");
					}
					members.insert(friend_info);
					value["members"].append(friend_info);
					user_service->GetUserInfo(friend_info, temp_user_info);
					team_name.append(nbase::UTF8ToUTF16(temp_user_info.GetName()));
				}
				
				value["teamId"] = multi_vchat_session_id_;
				value["channelId"] = channel_id;
				value["teamName"] = nbase::UTF16ToUTF8(team_name);
				value["roomName"] = room_name;
				value["id"] = kMsgMultiVchatVideo;
				value["creatorId"] = current_user_id;
				value["members"].append(LoginManager::GetInstance()->GetAccount());// = array_value;
				Json::Value cmd_value;
				cmd_value["id"] = 100;
				cmd_value["content"] = value;
				std::string msg_attach = fw.write(cmd_value);
				//发送自定义消息给用户

				auto msg_content = nbase::StringPrintf(ui::MutiLanSupport::GetInstance()->GetStringViaID(
					L"STRING_MULTIVIDEOCHATFORM_INVITE_PROMPT").c_str(),
					nbase::UTF8ToUTF16(LoginManager::GetInstance()->GetAccount()).c_str());

				nim::IMMessage invite_msg;
				invite_msg.session_type_ = nim::kNIMSessionTypeTeam;
				invite_msg.receiver_accid_ = multi_vchat_session_id_;
				invite_msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
				invite_msg.client_msg_id_ = QString::GetGUID();
				invite_msg.msg_setting_.resend_flag_ = BS_FALSE;
				invite_msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
				invite_msg.content_ = nbase::UTF16ToUTF8(msg_content);
				invite_msg.attach_ = msg_attach;
				invite_msg.status_ = nim::kNIMMsgLogStatusSending;
				invite_msg.type_ = nim::kNIMMessageTypeTips;
				invite_msg.msg_setting_.need_offline_ = BS_TRUE;	// 存离线
				invite_msg.msg_setting_.roaming_ = BS_FALSE;		// 不漫游

				for each(auto friend_info in friend_list)
				{
					invite_msg.msg_setting_.force_push_ids_list_.push_back(friend_info);
				}

				nim::Talk::SendMsg(invite_msg.ToJsonString(true));

#if 0
				nim::SysMessage msg;
				msg.sender_accid_ = current_user_id;
				msg.client_msg_id_ = QString::GetGUID();
				msg.attach_ = json_value;
				msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;
				msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();

				for each (auto friend_info in friend_list)
				{
					msg.receiver_accid_ = friend_info;
					QLOG_APP(L"send MsgMultiVchatVideo:{0}") << msg.ToJsonString();
					nim::SystemMsg::SendCustomNotificationMsg(msg.ToJsonString());
				}
#endif

				//弹出多人视频界面
				if (multi_video_form_ == NULL || !IsWindow(multi_video_form_flag_.expired() ? NULL : multi_video_form_->GetHWND()))
				{
					multi_video_form_.release();
					multi_video_form_.reset(new MultiVideoChatForm(room_name, multi_vchat_session_id_, channel_id, true, true, current_user_id));
					multi_video_form_flag_ = multi_video_form_->GetWeakFlag();
					multi_video_form_->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L,false);
					multi_video_form_->CenterWindow();

				}
				multi_video_form_->SetInvitingMember(members);
				multi_video_form_->ActiveWindow();
				//发送消息
			}
			else
			{
				QLOG_ERR(L"join room error code:{0}") << code;
				std::wstring toast = nbase::StringPrintf(L"创建房间失败");
				shared::Toast::ShowToast(toast, 2000);
			}
		};
		Post2UI(closure);

	};

	auto create_room_cb = [=](int code, int64_t channel_id, const std::string& json_extension)
	{
		if (code == 200)
		{
			VideoManager::GetInstance()->JoinRoom(nim::kNIMVideoChatModeVideo, room_name, multi_vchat_session_id_, join_room_cb);
		}
		else
		{
			QLOG_ERR(L"create room error roomname:{0} code:{1}") << room_name << code;
			StdClosure closure = []()
			{
				std::wstring toast = nbase::StringPrintf(L"加入房间失败");
				shared::Toast::ShowToast(toast, 5000);
			};
			Post2UI(closure);
		}
	};
	if (!VideoManager::GetInstance()->IsVideoChatFormExist(true))
	{
		VideoManager::GetInstance()->CreateRoom(room_name, "", create_room_cb);
	}
	else
	{
		std::wstring toast = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRING_MULTIVIDEOCHATFORM_START_CHATING_PROMPT");
		shared::Toast::ShowToast(toast, 2000);
		QLOG_ERR(L"video chat form is exist!");
	}
}

bool VideoManager::IsVideoChatFormExist(bool show)
{
	if (video_form_ != NULL && !video_form_flag_.expired())
	{
		if (show)
		{
			video_form_->ShowWindow(true, true);
		}
		return true;
	}
	return false;
}

bool VideoManager::IsMultiVideoChatFormExist(bool show)
{
	if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
	{
		if (show)
		{
			multi_video_form_->ShowWindow(true, true);
		}
		return true;
	}
	return false;
}


bool VideoManager::ShowVideoSetting(bool video)
{
	if (setting_form_ != NULL && !setting_form_flag_.expired())
	{
		setting_form_->ShowWindow(true, true);
		setting_form_->ShowPage(video);
		SetForegroundWindow(setting_form_->GetHWND());
		return false;
	}
	else
	{
		setting_form_.release();
		setting_form_.reset(new VideoSettingForm());
		setting_form_flag_ = setting_form_->GetWeakFlag();
		setting_form_->Create(NULL, L"", WS_OVERLAPPEDWINDOW, 0);
		setting_form_->CenterWindow();
		setting_form_->ShowWindow();
		setting_form_->ShowPage(video);
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
			for (int i = 0; i < num; ++i)
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
	if (device_info_list_[type].size() != 0)
	{
		if (!def_device_path_[type].empty())
		{
			int k = 0;
			for (auto i = device_info_list_[type].begin(); i != device_info_list_[type].end(); i++, k++)
			{
				if (i->device_path_ == def_device_path_[type])
				{
					no = k;
					device_path = def_device_path_[type];
					break;
				}
			}
		}
		if (device_path.empty())
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
	if (setting_form_ != NULL && !setting_form_flag_.expired())
	{
		setting_form_->AudioVolumnChange();
	}
	if (video_form_ != NULL && !video_form_flag_.expired())
	{
		video_form_->InitVolumnSetting();
	}
	if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
	{
		multi_video_form_->InitVolumnSetting();
	}
}
//自动调节麦克风音量
void VideoManager::SetAudioInputAutoVolumn(bool auto_volumn)
{
	nim::VChat::SetAudioInputAutoVolumn(auto_volumn);
	if (setting_form_ != NULL && !setting_form_flag_.expired())
	{
		setting_form_->AudioVolumnChange();
	}
}

void VideoManager::SetVideoQuality(int video_quality)
{
	video_quality_ = video_quality;
	nim::VChat::SetVideoQuality(video_quality_);
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
	auto sWidth = GetConfigValue("video_width");
	auto sHeight = GetConfigValue("video_height");
	if (!sWidth.empty() && !sHeight.empty())
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

	std::string video_quality = GetConfigValue("kNIMVideoQuality");
	std::string audio_record = GetConfigValue("kNIMAudioRecord");
	std::string video_record = GetConfigValue("kNIMVideoRecord");
	std::string record_type = GetConfigValue("kNIMRecordType");
	std::string record_host_speaker = GetConfigValue("kNIMRecordHostSpeaker");
	std::string keep_calling = GetConfigValue("kNIMKeepCalling");
	std::string stream_enc_type = GetConfigValue("NRTCStreamENCType");
	std::string stream_enc_key = GetConfigValue("NRTCStreamENCKey");
	value[nim::kNIMVChatVideoQuality] = atoi(video_quality.c_str());
	value[nim::kNIMVChatRecord] = atoi(audio_record.c_str());
	value[nim::kNIMVChatVideoRecord] = atoi(video_record.c_str());
	value[nim::kNIMVChatRecordType] = atoi(record_type.c_str());
	value[nim::kNIMVChatRHostSpeaker] = atoi(record_host_speaker.c_str());
	//value[nim::kNIMVChatWebrtc] = GetWebrtc() ? 1 : 0;
	value["stream_encrypt_type"] = atoi(stream_enc_type.c_str());
	value["stream_encrypt_token"] = stream_enc_key;
	if (!keep_calling.empty())
	{
		value[nim::kNIMVChatKeepCalling] = atoi(keep_calling.c_str());
	}
	std::string json_value = fs.write(value);
	std::wstring apns = ui::MutiLanSupport::GetInstance()->GetStringViaID(mode == nim::kNIMVideoChatModeAudio ? L"STRID_VIDEO_AUDIO_INVITING_TEST" : L"STRID_VIDEO_VIDEO_INVITING_TEST");
	return nim::VChat::Start(mode, nbase::UTF16ToUTF8(apns), custom_info, json_value);
}
//设置视频类型
bool VideoManager::SetVoipMode(nim::NIMVideoChatMode mode)
{
	QLOG_APP(L"SetVoipMode mode={0}") << mode;
	return nim::VChat::SetTalkingMode(mode, "");
}
//回应邀请
bool VideoManager::VChatCalleeAck(uint64_t channel_id, VChatAskType ask_type, const std::string& session_id)
{
	QLOG_APP(L"VChatCalleeAck channel_id={0}, ask_type={1}") << channel_id << ask_type;
	std::string json_value;
	if (ask_type == kVChatAskAccept)
	{
		std::string video_quality = GetConfigValue("kNIMVideoQuality");
		std::string audio_record = GetConfigValue("kNIMAudioRecord");
		std::string video_record = GetConfigValue("kNIMVideoRecord");
		std::string record_type = GetConfigValue("kNIMRecordType");
		std::string record_host_speaker = GetConfigValue("kNIMRecordHostSpeaker");
		std::string stream_enc_type = GetConfigValue("NRTCStreamENCType");
		std::string stream_enc_key = GetConfigValue("NRTCStreamENCKey");
		Json::FastWriter fs;
		Json::Value value;
		value[nim::kNIMVChatVideoQuality] = atoi(video_quality.c_str());
		value[nim::kNIMVChatRecord] = atoi(audio_record.c_str());
		value[nim::kNIMVChatVideoRecord] = atoi(video_record.c_str());
		value[nim::kNIMVChatRecordType] = atoi(record_type.c_str());
		value[nim::kNIMVChatRHostSpeaker] = atoi(record_host_speaker.c_str());
		value[nim::kNIMVChatSessionId] = session_id;
		value["stream_encrypt_type"] = atoi(stream_enc_type.c_str());
		value["stream_encrypt_token"] = stream_enc_key;
		json_value = fs.write(value);
	}

	// 忙线状态下不回复对方，让对方主动取消，这样对方会发送一个 miss 消息
	// 若开启了统计音视频未接通的消息计数开关，则可以正常统计到数据
	// 若这里直接拒绝，则对方不会发送 miss 消息
	if (ask_type == kVChatAskAccept || ask_type == kVChatAskRefuse)
		return nim::VChat::CalleeAck(channel_id, ask_type == kVChatAskAccept, json_value);
	else
		return true;
}
//通话控制
bool VideoManager::VChatControl(uint64_t channel_id, nim::NIMVChatControlType type)
{
	QLOG_APP(L"VChatControl channel_id={0}, type={1}") << channel_id << type;
	return nim::VChat::Control(channel_id, type);
}
void VideoManager::EndChat(const std::string& session_id, bool is_time_out/* = false*/)
{
	QLOG_APP(L"EndChat {0}") << session_id;
	Json::FastWriter fs;
	Json::Value value;
	value[nim::kNIMVChatSessionId] = session_id;
	value[nim::kNIMVChatTimeout] = is_time_out;
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

	std::string video_quality = GetConfigValue("kNIMVideoQuality");
	std::string audio_record = GetConfigValue("kNIMAudioRecord");
	std::string video_record = GetConfigValue("kNIMVideoRecord");
	std::string record_type = GetConfigValue("kNIMRecordType");
	std::string record_host_speaker = GetConfigValue("kNIMRecordHostSpeaker");
	std::string stream_enc_type = GetConfigValue("NRTCStreamENCType");
	std::string stream_enc_key = GetConfigValue("NRTCStreamENCKey");
	value[nim::kNIMVChatVideoQuality] = atoi(video_quality.c_str());
	value[nim::kNIMVChatRecord] = atoi(audio_record.c_str());
	value[nim::kNIMVChatVideoRecord] = atoi(video_record.c_str());
	value[nim::kNIMVChatRecordType] = atoi(record_type.c_str());
	value[nim::kNIMVChatRHostSpeaker] = atoi(record_host_speaker.c_str());
	value["stream_encrypt_type"] = atoi(stream_enc_type.c_str());
	value["stream_encrypt_token"] = stream_enc_key;
	std::string json_value = fs.write(value);
	return nim::VChat::JoinRoom(mode, room_name, json_value, cb);
}

void VideoManager::SetChatRoomCb(ConnectCallback connect_cb /*= nullptr*/, PeopleChangeCallback people_cb /*= nullptr*/)
{
	chatroom_connect_cb_ = connect_cb;
	chatroom_people_cb_ = people_cb;
}

void VideoManager::SetMultiVChatStatus(MultiVChatStatusType vchat_status)
{
	nbase::NAutoLock lock(&vchat_status_lock_);
	QLOG_APP(L"Set vchat status:{0}-cur_status:{1}") << vchat_status << vchat_status_;
	vchat_status_ = vchat_status;
}

MultiVChatStatusType VideoManager::GetMultiVChatStatus()
{
	nbase::NAutoLock lock(&vchat_status_lock_);
	QLOG_APP(L"Get vchat status:{0}") << vchat_status_;
	return vchat_status_;
}

void VideoManager::EndJoinTimer()
{
	join_vchat_timer_.Cancel();
}

void VideoManager::StartJoinTimer()
{
	StdClosure time_out_task = nbase::Bind(&VideoManager::JoinTimeOut, this);
	nbase::ThreadManager::PostDelayedTask(kThreadUI, join_vchat_timer_.ToWeakCallback(time_out_task), nbase::TimeDelta::FromSeconds(5));
}

void VideoManager::JoinTimeOut()
{
	QLOG_APP(L"wait join time out");
	SetMultiVChatStatus(kMultiVChatEnd);
	StdClosure closure = []()
	{
		std::wstring toast = nbase::StringPrintf(L"对方已挂断");
		shared::Toast::ShowToast(toast, 2000);
	};
	Post2UI(closure);
}

void VideoManager::InvokeReceiveCustomP2PMessage(const Json::Value &json, const std::string &sender)
{
	if (json.isMember("teamId"))
	{
		uint64_t channel_id = json["channelId"].asUInt64();
		std::string creator_id = json["creatorId"].asString();
		int id = json["id"].asInt();
		std::string team_name = json["teamName"].asString();
		std::string team_id = json["teamId"].asString();
		std::string room_name = json["roomName"].asString();
		MultiVchatMsgType msg_type = (MultiVchatMsgType)id;
		switch (msg_type)
		{
		case kMsgMultiVchatAudio:
			break;
		case kMsgMultiVchatVideo:
		{
			//收到群视频邀请，弹出群聊邀请界面
			MultiVChatStatusType vchat_status = VideoManager::GetInstance()->GetMultiVChatStatus();
			if (vchat_status != kMultiVChatEnd)
			{
				//已经存在一个通话邀请请求或者存在一个群视频
				QLOG_APP(L"exist the invite or vchat form!!!");
				Json::FastWriter fw;
				int index = 0;
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				Json::Value value;
				value["teamId"] = team_id;
				value["channelId"] = channel_id;
				value["opt"] = 2;//拒绝
				value["id"] = kMsgMultiVchatAck;
				value["roomName"] = room_name;
				Json::Value cmd_value;
				cmd_value["id"] = 100;
				cmd_value["content"] = value;

				std::string json_value = fw.write(cmd_value);

				//发送自定义消息给用户
				nim::SysMessage temp_msg;
				temp_msg.sender_accid_ = current_user_id;
				temp_msg.client_msg_id_ = QString::GetGUID();
				temp_msg.attach_ = json_value;
				temp_msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;
				temp_msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
				temp_msg.receiver_accid_ = sender;//
				QLOG_APP(L"Send MsgMultiVchatAck:{0}") << temp_msg.ToJsonString();
				nim::SystemMsg::SendCustomNotificationMsg(temp_msg.ToJsonString());
				return;

			}
			VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatInvite);
			MultiVchatInfo vchat_info;
			vchat_info.channel_id_ = channel_id;
			vchat_info.creator_id_ = sender;//creator_id
			vchat_info.team_id_ = team_id;
			vchat_info.team_name_ = team_name;
			vchat_info.room_name_ = room_name;
			std::set<std::string> invite_members;
			if (json["members"].isArray())
			{
				vchat_info.member_count_ = json["members"].size();
				std::wstring temp_name;
				bool need_skip_ = false;
				for (auto& it : json["members"])
				{
					std::string member_uid = it.asString();

					if (member_uid != LoginManager::GetInstance()->GetAccount())
					{
						invite_members.insert(member_uid);
						QLOG_APP(L"invite temp_name:{0}") << temp_name;
						//if ((i > 1)||(i==1&&!need_skip_))
						//	temp_name.append(L"、");
						//temp_name.append(UserService::GetInstance()->GetUserNameW(member_uid, false));
					}
					else
					{
						vchat_info.member_count_ -= 1;
						//	if (i == 0)
						//		need_skip_ = true;
					}
				}				
				//QLOG_APP(L"invite temp_name:{0}") << temp_name;
				//vchat_info.team_name_ = nbase::UTF16ToUTF8(temp_name);
			}

			multi_vchat_invite_form_.release();
			multi_vchat_invite_form_.reset(new MultiVideoInviteForm(vchat_info));
			multi_vchat_invite_form_flag_ = multi_vchat_invite_form_->GetWeakFlag();
			multi_vchat_invite_form_->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L);
			multi_vchat_invite_form_->CenterWindow();
			multi_vchat_invite_form_->AddInviteMembers(invite_members);
			multi_vchat_invite_form_->ActiveWindow();
			VideoManager::GetInstance()->SetMultiVChatCreator(creator_id);

		}
		break;
		case kMsgMultiVchatAck:
		{
			//收到被邀请方的同意，拒绝群视频邀请
			int opt = json["opt"].asInt(); //被邀请方接受群视频聊天
			if (opt == 1)
			{
				//检测是否有其他端进入音视频房间
				int need_opt = 0;
				bool video = false;
				if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
				{
					QLOG_APP(L"roomName:{0},vchat room name:{1}") << room_name << multi_video_form_->GetRoomName();
					if (room_name == multi_video_form_->GetRoomName())
					{
						std::map<std::string, bool> invited_members = multi_video_form_->GetInvitedMember();
						std::map<std::string, bool>::iterator it = invited_members.find(sender);

						if (it == invited_members.end())
						{
							int video_count = multi_video_form_->GetVideoCount();
							if (video_count < 4)
							{
								video = true;
							}
							need_opt = 1;//允许进入群视频房间
							multi_video_form_->SetInvitedMember(sender, video);

						}
						else
						{
							need_opt = 3;//已同意过该账号，被视为多端登录
						}
					}
					else
					{
						need_opt = 2;//邀请方已不存在
						return;
					}
				}
				else
				{
					need_opt = 2; //邀请方不存在,可能移动端发起，先过滤掉，不做处理
					return;
				}
				Json::FastWriter fw;
				int index = 0;
				UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
				Json::Value value;
				value["teamId"] = team_id;
				value["channelId"] = channel_id;
				value["opt"] = need_opt;
				value["id"] = kMsgMultiVchatJoin;
				value["roomName"] = room_name;
				value["video"] = video;
				Json::Value cmd_value;
				cmd_value["id"] = 100;
				cmd_value["content"] = value;

				std::string json_value = fw.write(cmd_value);
				//发送自定义消息给用户
				nim::SysMessage temp_msg;
				temp_msg.sender_accid_ = current_user_id;
				temp_msg.client_msg_id_ = QString::GetGUID();
				temp_msg.attach_ = json_value;
				temp_msg.type_ = nim::kNIMSysMsgTypeCustomP2PMsg;
				temp_msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
				temp_msg.receiver_accid_ = sender;
				QLOG_APP(L"Send MsgMultiVchatJoin:{0}") << temp_msg.ToJsonString();
				nim::SystemMsg::SendCustomNotificationMsg(temp_msg.ToJsonString());
			}
			else if (opt == 2) //被邀请方拒绝群视频聊天
			{
				if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
				{
					multi_video_form_->OnSetShowBitmap(sender, false, true);
				}
			}
		}
		break;
		case kMsgMultiVchatJoin:
		{
			//收到邀请方的确认结果
			int opt = json["opt"].asInt();
			MultiVChatStatusType vchat_status = VideoManager::GetInstance()->GetMultiVChatStatus();
			VideoManager::GetInstance()->EndJoinTimer();
			if (opt == 1)//允许进入房间
			{
				if (vchat_status == kMultiVChatJoin)//||multi_vchat_invite_form == nullptr||(multi_vchat_invite_form->IsInvited())
				{
					bool video = json["video"].asBool();
					UTF8String current_user_id = LoginManager::GetInstance()->GetAccount();
					auto join_room_cb = [=](int code, int64_t channel_id, const std::string& json_extension)
					{
						if (code == 200)
						{
							//弹出多人视频界面
							if (multi_video_form_ == NULL || !IsWindow(multi_video_form_flag_.expired() ? NULL : multi_video_form_->GetHWND()))
							{
								std::string creator_id = VideoManager::GetInstance()->GetMultiVChatCreator();
								multi_video_form_.release();
								multi_video_form_.reset(new MultiVideoChatForm(room_name, team_id, channel_id, video, false, creator_id));
								multi_video_form_flag_ = multi_video_form_->GetWeakFlag();
								multi_video_form_->Create(NULL, L"", UI_WNDSTYLE_FRAME& ~WS_MAXIMIZEBOX, 0L,false);
								multi_video_form_->CenterWindow();
							}
							multi_video_form_->ActiveWindow();
						}
						else
						{
							QLOG_ERR(L"join room error code:{0}") << code;
							std::wstring toast = nbase::StringPrintf(L"加入群视频房间失败");
							VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
							shared::Toast::ShowToast(toast, 2000);
						}
					};
					nim::NIMVideoChatMode mode = video ? nim::kNIMVideoChatModeVideo : nim::kNIMVideoChatModeAudio;
					VideoManager::GetInstance()->JoinRoom(mode, room_name, team_id, join_room_cb);	//以音频模式加入房间
				}
				else if (vchat_status == kMultiVChatInvite) //被视为其他端已登录
				{
					VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
					if (multi_vchat_invite_form_ != NULL && IsWindow(multi_vchat_invite_form_->GetHWND()))
						multi_vchat_invite_form_->Close();
					std::wstring toast = nbase::StringPrintf(L"移动端已接听");
					shared::Toast::ShowToast(toast, 2000);
					QLOG_ERR(toast);
				}

			}
			else if (vchat_status == kMultiVChatJoin&&opt == 2)
			{
				VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
				std::wstring toast = nbase::StringPrintf(L"加入群聊失败");
				QLOG_ERR(toast);
				shared::Toast::ShowToast(toast, 2000);
			}
			else if (vchat_status == kMultiVChatJoin&&opt == 3) //群视频其他端已进入
			{
				VideoManager::GetInstance()->SetMultiVChatStatus(kMultiVChatEnd);
				std::wstring toast = nbase::StringPrintf(L"移动端已接听");
				QLOG_ERR(toast);
				shared::Toast::ShowToast(toast, 2000);
			}
		}
		break;
		case kMsgMultiVchatP2PChangeMode:
		{
			//单点通知音视频模式切换
			if (multi_video_form_ != NULL && !multi_video_form_flag_.expired() && room_name == multi_video_form_->GetRoomName())
			{
				bool video = json["video"].asBool();
				multi_video_form_->OnSetShowBitmap(sender, video);
				multi_video_form_->SetCacheMemberVideo(sender, video);
			}
		}
		break;
		default:
			break;
		}
	}
}

void VideoManager::InvokeReceiveCustomTeamMessage(const Json::Value &json, const std::string &sender)
{
	if (json.isMember("teamId"))
	{
		uint64_t channel_id = json["channelId"].asUInt64();
		int id = json["id"].asInt();
		std::string team_name = json["teamName"].asString();
		std::string team_id = json["teamId"].asString();
		std::string room_name = json["roomName"].asString();
		bool video = json["video"].asBool();
		MultiVchatMsgType msg_type = (MultiVchatMsgType)id;
		switch (msg_type)
		{
		case kMsgMultiVchatTeamChangeMode:
		{
			//收到群视频成员的音视频模式切换通知
			if (multi_video_form_ != NULL && !multi_video_form_flag_.expired() && room_name == multi_video_form_->GetRoomName())
			{
				multi_video_form_->OnSetShowBitmap(sender, video);
			}
		}
		break;
		default:
			break;
		}
	}
}

std::set<std::string> VideoManager::GetMeetingMember()
{
	if (multi_video_form_ != NULL && !multi_video_form_flag_.expired())
		return multi_video_form_->GetTalkingMember();
	return std::set<std::string>();
}
}
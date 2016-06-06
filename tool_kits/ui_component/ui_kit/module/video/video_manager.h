#pragma once

#include "video_frame_mng.h"
#include "audio_frame_mng.h"

struct MEDIA_DEVICE_DRIVE_INFO
{
	std::string device_path_;
	std::string friendly_name_;
};
enum DeviceSessionType
{
	kDeviceSessionTypeNone		= 0x0,
	kDeviceSessionTypeChat		= 0x1,
	kDeviceSessionTypeSetting	= 0x2,
	kDeviceSessionTypeRts		= 0x4,
	kDeviceSessionTypeChatRoom	= 0x8,
};
typedef std::function<void(int code)>  ConnectCallback;
typedef std::function<void(std::string uid, bool join_type)>  PeopleChangeCallback;
namespace nim_comp
{
class VideoManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(VideoManager);

	VideoManager();
	~VideoManager();

	bool ShowVideoChatForm(std::string id, bool video, uint64_t channel_id = 0);
	bool ShowVideoSetting(bool video = false);
	bool IsVideoChatFormExist(bool show);

	//监听
	//void OnVideoDataCb(bool capture, const std::string& data, unsigned int width, unsigned int height, const std::string& json);
	void OnDeviceStatus(nim::NIMDeviceType type, UINT status, std::string path);
	void OnStartDeviceCb(nim::NIMDeviceType type, bool ret);
	void OnVChatEvent(nim::NIMVideoChatSessionType type, uint64_t channel_id, int code, const std::string& json);

	//设备信息
	bool GetDefaultDevicePath(int &no, std::string &device_path, nim::NIMDeviceType type);
	void SetDefaultDevicePath(std::string device_path, nim::NIMDeviceType type);
	std::list<MEDIA_DEVICE_DRIVE_INFO> GetDeviceInfo(nim::NIMDeviceType type);
	void GetDeviceByJson(bool ret, nim::NIMDeviceType type, const char* json);

	//音频设置	
	void SetAudioVolumn(unsigned char volumn, bool capture);
	//自动调节麦克风音量
	void SetAudioInputAutoVolumn(bool auto_volumn);

	//设备启动
	void StartDevice(nim::NIMDeviceType type, std::string device_path, DeviceSessionType session_type);
	void EndDevice(nim::NIMDeviceType type, DeviceSessionType session_type, bool forced = false);

	//通话
	bool StartChat(nim::NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& uid, const std::string& session_id);
	//bool JoinRoomChat(nim::NIMVideoChatMode mode, const std::string& room_id, const std::string& master, nim::NIMVChatVideoQuality quality_type, const std::string& apns_text, const std::string& custom_info);
	//设置视频类型
	bool SetVoipMode(nim::NIMVideoChatMode mode);
	//回应邀请
	bool VChatCalleeAck(uint64_t channel_id, bool accept, const std::string& session_id);
	//通话控制
	bool VChatControl(uint64_t channel_id, nim::NIMVChatControlType type);
	//结束通话
	void EndChat(const std::string& session_id);

	void SetViewerMode(bool viewer);

	bool GetViewerMode();

	void SetAudioMuted(bool muted);

	bool GetAudioMuteEnabled();

	void SetMemberBlacklist(const std::string& uid, bool add, bool audio, nim::VChat::OptCallback cb);

	void CreateRoom(const std::string& room_name, const std::string& custom_info, nim::VChat::Opt2Callback cb);

	bool JoinRoom(nim::NIMVideoChatMode mode, const std::string& room_name, const std::string& session_id, nim::VChat::Opt2Callback cb);
	//多人音视频
	void SetChatRoomCb(ConnectCallback connect_cb = nullptr, PeopleChangeCallback people_cb = nullptr)
	{
		chatroom_connect_cb_ = connect_cb;
		chatroom_people_cb_ = people_cb;
	}
public:
	VideoFrameMng video_frame_mng_;
	AudioFrameMng audio_frame_mng_;
private:
	std::list<MEDIA_DEVICE_DRIVE_INFO> device_info_list_[4];
	std::string def_device_path_[4];
	DWORD device_session_type_[4];
	ConnectCallback		chatroom_connect_cb_ = nullptr;
	PeopleChangeCallback		chatroom_people_cb_ = nullptr;

};
}
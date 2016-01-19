#pragma once

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
};
namespace nim_comp
{
class VideoManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(VideoManager);

	VideoManager();
	~VideoManager();

	bool ShowVideoChatForm(std::string id, bool video, uint64_t channel_id = 0);
	bool ShowVideoSetting();
	bool IsVideoChatFormExist(bool show);

	//监听
	void OnVideoDataCb(bool capture, std::string data, unsigned int width, unsigned int height);
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
	bool StartChat(nim::NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& uid);
	//设置视频类型
	bool SetVoipMode(nim::NIMVideoChatMode mode);
	//回应邀请
	bool VChatCalleeAck(uint64_t channel_id, bool accept);
	//通话控制
	bool VChatControl(uint64_t channel_id, nim::NIMVChatControlType type);
	//结束通话
	void EndChat();

private:
	std::list<MEDIA_DEVICE_DRIVE_INFO> device_info_list_[4];
	std::string def_device_path_[4];
	DWORD device_session_type_[4];

};
}
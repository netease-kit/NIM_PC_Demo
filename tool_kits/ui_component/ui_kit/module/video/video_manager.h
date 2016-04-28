#pragma once
#include "base/synchronization/lock.h"

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
typedef std::function<void(const std::string &data, ::ui::CSize size, const std::string &uid)>  VideoDataCallback;
typedef std::function<void(int code)>  ConnectCallback;
namespace nim_comp
{
struct PicRegion //一块颜色数据区的描述，便于参数传递
{
	PicRegion()
	{
		pdata_ = NULL;
		subtype_ = nim::kNIMVideoSubTypeARGB;
		size_max_ = 0;
		size_ = 0;
	}

	~PicRegion()
	{
		Clear();
	}
	void Clear()
	{
		if (pdata_)
		{
			delete[] pdata_;
			pdata_ = NULL;
		}
		size_max_ = 0;
		size_ = 0;
	}
	int ResetData(uint64_t time, const char* data, int size, unsigned int width, unsigned int height, nim::NIMVideoSubType subtype)
	{
		if (size > size_max_)
		{
			if (pdata_)
			{
				delete[] pdata_;
				pdata_ = NULL;
			}
			pdata_ = new char[size];
			size_max_ = size;
		}
		width_ = width;
		height_ = height;
		timestamp_ = time;
		subtype_ = subtype;
		size_ = size;
		memcpy(pdata_, data, size);
		return size;
	}
	nim::NIMVideoSubType subtype_;
	char*		pdata_;         //颜色数据首地址
	int			size_max_;
	int			size_;
	long        width_;         //像素宽度
	long        height_;        //像素高度
	uint64_t	timestamp_;     //时间戳（毫秒）
};
class VideoFrameMng
{
public:
	VideoFrameMng();
	~VideoFrameMng();

	static void AddVideoFrame(bool capture, int64_t time, const char* data, int size, int width, int height, const std::string& json);
	static bool GetVideoFrame(bool capture, int64_t& time, char* out_data, int& width, int& height, bool mirror = false);
private:
};
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
	bool StartChat(nim::NIMVideoChatMode mode, const std::string& apns_text, const std::string& custom_info, const std::string& uid);
	//bool JoinRoomChat(nim::NIMVideoChatMode mode, const std::string& room_id, const std::string& master, nim::NIMVChatVideoQuality quality_type, const std::string& apns_text, const std::string& custom_info);
	//设置视频类型
	bool SetVoipMode(nim::NIMVideoChatMode mode);
	//回应邀请
	bool VChatCalleeAck(uint64_t channel_id, bool accept);
	//通话控制
	bool VChatControl(uint64_t channel_id, nim::NIMVChatControlType type);
	//结束通话
	void EndChat();
	//多人音视频
	void SetChatRoomCb(ConnectCallback connect_cb = nullptr, VideoDataCallback videodata_cb = nullptr)
	{
		chatroom_connect_cb_ = connect_cb;
		chatroom_videorec_cb_ = videodata_cb;
	}
private:
	std::list<MEDIA_DEVICE_DRIVE_INFO> device_info_list_[4];
	std::string def_device_path_[4];
	DWORD device_session_type_[4];
	ConnectCallback		chatroom_connect_cb_ = nullptr;
	VideoDataCallback	chatroom_videorec_cb_ = nullptr;

};
}
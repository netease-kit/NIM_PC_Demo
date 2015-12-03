#pragma once

namespace nim_comp
{
class VChatCallback
{
public:
	//callback----------------------
	static void EnumDevCb(bool ret, nim::NIMDeviceType type, const char* json, const void*);
	static void DeviceStatusCb(nim::NIMDeviceType type, UINT status, const char* path, const char* json, const void *user_data);
	static void StartDeviceCb(nim::NIMDeviceType type, bool ret, const char* json, const void*);
	static void VideoCaptureData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char* json, const void *user_data);
	static void VideoRecData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char* json, const void *user_data);
	static void VChatCb(nim::NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json, const void*);
//private:
//	static void OnDeviceStatusCb(nim::NIMDeviceType type, UINT status, const std::string& path);
//	static void OnStartDeviceCb(nim::NIMDeviceType type, bool ret);
//	static void OnVideoCaptureData(uint64_t time, const std::string& data, unsigned int width, unsigned int height);
//	static void OnVideoRecData(uint64_t time, const std::string& data, unsigned int width, unsigned int height);
//	static void OnVChatCb(NIMVideoChatSessionType type, uint64_t channel_id, int code, const std::string& json);
};

}

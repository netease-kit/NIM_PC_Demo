#include "vchat_callback.h"
#include "module/video/video_manager.h"

namespace nim_comp
{
//callback----------------------
void VChatCallback::EnumDevCb(bool ret, nim::NIMDeviceType type, const char* json, const void*)
{
	VideoManager::GetInstance()->GetDeviceByJson(ret, type, json);
}
void OnDeviceStatusCb(nim::NIMDeviceType type, UINT status, const std::string& path)
{
	VideoManager::GetInstance()->OnDeviceStatus(type, status, path);
}
void VChatCallback::DeviceStatusCb(nim::NIMDeviceType type, UINT status, const char* path, const char *json, const void *user_data)
{
	std::string device(path);
	auto closure = nbase::Bind(&OnDeviceStatusCb, type, status, device);
	Post2UI(closure);
}
void OnStartDeviceCb(nim::NIMDeviceType type, bool ret)
{
	VideoManager::GetInstance()->OnStartDeviceCb(type, ret);
}
void VChatCallback::StartDeviceCb(nim::NIMDeviceType type, bool ret, const char *json, const void*)
{
	auto closure = nbase::Bind(&OnStartDeviceCb, type, ret);
	Post2UI(closure);
}
void VChatCallback::VideoCaptureData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char *json, const void *user_data)
{
	std::string json_temp(json);
	VideoFrameMng::AddVideoFrame(true, time, data, size, width, height, json_temp);
}
void VChatCallback::VideoRecData(unsigned __int64 time, const char* data, unsigned int size, unsigned int width, unsigned int height, const char *json, const void *user_data)
{
	std::string json_temp(json);
	VideoFrameMng::AddVideoFrame(false, time, data, size, width, height, json_temp);
}

void OnVChatCb(nim::NIMVideoChatSessionType type, uint64_t channel_id, int code, const std::string& json)
{
	VideoManager::GetInstance()->OnVChatEvent(type, channel_id, code, json);
}
void VChatCallback::VChatCb(nim::NIMVideoChatSessionType type, __int64 channel_id, int code, const char *json, const void*)
{
	std::string json_temp(json);
	auto closure = nbase::Bind(&OnVChatCb, type, channel_id, code, json_temp);
	Post2UI(closure);
}
}
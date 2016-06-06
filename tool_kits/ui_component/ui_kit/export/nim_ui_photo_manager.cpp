#include "nim_ui_photo_manager.h"

namespace nim_ui
{
std::wstring PhotoManager::GetUserPhoto(const std::string &accid)
{
	return nim_comp::PhotoService::GetInstance()->GetUserPhoto(accid);
}

std::wstring PhotoManager::GetTeamPhoto(const std::string &tid, bool full_path/* = true*/)
{
	return nim_comp::PhotoService::GetInstance()->GetTeamPhoto(tid, full_path);
}

UnregisterCallback PhotoManager::RegPhotoReady(const OnPhotoReadyCallback& callback)
{
	return nim_comp::PhotoService::GetInstance()->RegPhotoReady(callback);
}

}
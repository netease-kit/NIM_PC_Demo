#include "nim_ui_muteblack_manager.h"

namespace nim_ui
{

const std::set<std::string> & MuteBlackManager::GetMuteList()
{
	return nim_comp::MuteBlackService::GetInstance()->GetMuteList();
}

const std::set<std::string> & MuteBlackManager::GetBlackList()
{
	return nim_comp::MuteBlackService::GetInstance()->GetBlackList();
}

bool MuteBlackManager::IsInMuteList(const std::string& id)
{
	return nim_comp::MuteBlackService::GetInstance()->IsInMuteList(id);
}

bool MuteBlackManager::IsInBlackList(const std::string& id)
{
	return nim_comp::MuteBlackService::GetInstance()->IsInBlackList(id);
}

void MuteBlackManager::InvokeSetMute(const std::string& id, bool mute)
{
	nim_comp::MuteBlackService::GetInstance()->InvokeSetMute(id, mute);
}

void MuteBlackManager::InvokeSetBlack(const std::string& id, bool black)
{
	nim_comp::MuteBlackService::GetInstance()->InvokeSetBlack(id, black);
}

UnregisterCallback MuteBlackManager::RegSyncSetMuteCallback(const SetStateCallback& cb)
{
	return nim_comp::MuteBlackService::GetInstance()->RegSyncSetMuteCallback(cb);
}

UnregisterCallback MuteBlackManager::RegSyncSetBlackCallback(const SetStateCallback& cb)
{
	return nim_comp::MuteBlackService::GetInstance()->RegSyncSetBlackCallback(cb);
}

}


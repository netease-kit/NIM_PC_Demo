#include "nim_ui_session_manager.h"

namespace nim_ui
{
void SessionManager::SetEnableMerge(bool enable)
{
	nim_comp::SessionManager::GetInstance()->SetEnableMerge(enable);
}

bool SessionManager::IsEnableMerge() const
{
	return nim_comp::SessionManager::GetInstance()->IsEnableMerge();
}

void SessionManager::SetUseCustomDragImage(bool use)
{
	nim_comp::SessionManager::GetInstance()->SetUseCustomDragImage(use);
}

bool SessionManager::IsUseCustomDragImage() const
{
	return nim_comp::SessionManager::GetInstance()->IsUseCustomDragImage();
}

nim_comp::SessionBox* SessionManager::OpenSessionBox(std::string id, nim::NIMSessionType type, bool reopen /*= false*/)
{
	return nim_comp::SessionManager::GetInstance()->OpenSessionBox(id, type, reopen);
}

bool SessionManager::IsSessionBoxActive(const std::string& id)
{
	return nim_comp::SessionManager::GetInstance()->IsSessionBoxActive(id);
}

void SessionManager::AddNewMsg(const nim::IMMessage &msg)
{
	nim_comp::SessionManager::GetInstance()->AddNewMsg(msg);
}

void SessionManager::ResetUnread(const std::string &id)
{
	nim_comp::SessionManager::GetInstance()->ResetUnread(id);
}

nim_comp::SessionBox* SessionManager::FindSessionBox(const std::string &id)
{
	return nim_comp::SessionManager::GetInstance()->FindSessionBox(id);
}

void SessionManager::RemoveSessionBox(std::string id)
{
	nim_comp::SessionManager::GetInstance()->RemoveSessionBox(id);
}

}


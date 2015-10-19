#include "nim_ui_session_manager.h"

namespace nim_ui
{

SessionManager::SessionManager()
{

}

SessionManager::~SessionManager()
{

}

nim_comp::SessionForm* SessionManager::OpenSessionForm(std::string id, nim::NIMSessionType type, bool reopen /*= false*/)
{
	return nim_comp::SessionManager::GetInstance()->OpenSessionForm(id, type, reopen);
}

bool SessionManager::IsSessionWndActive(const std::string& id)
{
	return nim_comp::SessionManager::GetInstance()->IsSessionWndActive(id);
}

void SessionManager::AddNewMsg(const MsgData &msg)
{
	nim_comp::SessionManager::GetInstance()->AddNewMsg(msg);
}

void SessionManager::ResetUnread(const std::string &id)
{
	nim_comp::SessionManager::GetInstance()->ResetUnread(id);
}

nim_comp::SessionForm* SessionManager::Find(const std::string &id)
{
	return nim_comp::SessionManager::GetInstance()->Find(id);
}

void SessionManager::RemoveForm(std::string id)
{
	nim_comp::SessionManager::GetInstance()->RemoveForm(id);
}

void SessionManager::AddFileUpProgressCb(std::string msg_id, nim::Talk::FileUpPrgCallback* cb)
{
	nim_comp::SessionManager::GetInstance()->AddFileUpProgressCb(msg_id, cb);
}

void SessionManager::RemoveFileUpProgressCb(std::string msg_id)
{
	nim_comp::SessionManager::GetInstance()->RemoveFileUpProgressCb(msg_id);
}

void SessionManager::QueryMyTList(const std::string& tid)
{
	nim_comp::SessionManager::GetInstance()->QueryMyTList(tid);
}

}


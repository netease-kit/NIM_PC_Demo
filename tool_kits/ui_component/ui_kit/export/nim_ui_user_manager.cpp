#include "nim_ui_user_manager.h"

namespace nim_ui
{
void UserManager::InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb)
{
	nim_comp::UserService::GetInstance()->InvokeRegisterAccount(username, password, nickname, cb);
}

nim::NIMFriendFlag UserManager::GetUserType(const std::string &id)
{
	return nim_comp::UserService::GetInstance()->GetUserType(id);
}

bool UserManager::GetUserInfo(const std::string &account, nim::UserNameCard &info)
{
	return nim_comp::UserService::GetInstance()->GetUserInfo(account, info);
}

void UserManager::GetUserInfos(const std::list<std::string>& ids, std::list<nim::UserNameCard>& uinfos)
{
	nim_comp::UserService::GetInstance()->GetUserInfos(ids, uinfos);
}

const std::map<std::string, nim::UserNameCard>& UserManager::GetAllUserInfos()
{
	return nim_comp::UserService::GetInstance()->GetAllUserInfos();
}

std::wstring UserManager::GetUserName(const std::string &id, bool alias_prior/* = true */)
{
	return nim_comp::UserService::GetInstance()->GetUserName(id, alias_prior);
}

std::wstring UserManager::GetFriendAlias(const std::string & id)
{
	return nim_comp::UserService::GetInstance()->GetFriendAlias(id);
}

UnregisterCallback UserManager::RegFriendListChange(const OnFriendListChangeCallback & callback)
{
	return nim_comp::UserService::GetInstance()->RegFriendListChange(callback);
}

UnregisterCallback UserManager::RegUserInfoChange(const OnUserInfoChangeCallback& callback)
{
	return nim_comp::UserService::GetInstance()->RegUserInfoChange(callback);
}

UnregisterCallback UserManager::RegMiscUInfoChange(const OnUserInfoChangeCallback & callback)
{
	return nim_comp::UserService::GetInstance()->RegMiscUInfoChange(callback);
}

}


#pragma once

#include "nim_ui_dll.h"
#include "module/service/user_service.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class UserManager
  * @brief 提供用户数据获取接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API UserManager
{
public:
	SINGLETON_DEFINE(UserManager);
public:
	UserManager();
	~UserManager();
public:
	void InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb);
	void InvokeGetAllUserInfo(const OnGetUserInfoCallback& cb);
	void GetUserInfoWithEffort(const std::list<std::string>& account_list, const OnGetUserInfoCallback& cb);
	void InvokeUpdateUserInfo(const UserInfo &new_info, const OnUpdateUserInfoCallback& cb);
	void InvokeChangeUserPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb);
	const std::map<std::string, UserInfo>& GetAllUserInfos();
	bool GetUserInfo(const std::string &account, UserInfo &info);
	UserType GetUserType(const std::string &id);
	std::wstring GetUserName(const std::string &id);
	std::wstring GetUserPhoto(const std::string &icon);
	std::wstring GetUserPhotoDir();
	UnregisterCallback RegFriendListChange(const OnFriendListChangeCallback& callback);
	UnregisterCallback RegUserInfoChange(const OnUserInfoChangeCallback& callback);
	UnregisterCallback RegUserPhotoReady(const OnUserPhotoReadyCallback& callback);
};

}
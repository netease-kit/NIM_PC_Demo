#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

const std::string kAppServerAddress = "http://app.netease.im";

enum UserType
{
	UT_UNKNOWN,		//非好友
	UT_FRIEND,		//好友
};

enum UserChangeType
{
	kChangeTypeAdd,
	kChangeTypeDelete
};

enum UserGender
{
	UG_UNKNOWN,
	UG_MALE,
	UG_FEMALE
};

enum UInfoFieldFlag
{
	kUInfoFlagNone			= 0,
	kUInfoFlagName			= 1,
	kUInfoFlagHeadImage		= 1 << 1,
	kUInfoFlagGender		= 1 << 2,
	kUInfoFlagBirthday		= 1 << 3,
	kUInfoFlagPhone			= 1 << 4,
	kUInfoFlagEmail			= 1 << 5,
	kUInfoFlagSignature		= 1 << 6,
	kUInfoFlagAll			= (1 << 7) - 1
};

struct UserInfo
{
	std::string	account;
	std::string	name;
	std::string	head_image; //url
	UserGender	gender = UG_UNKNOWN;
	std::string	birthday;
	std::string	phone;
	std::string email;
	std::string	signature;
	unsigned char field_avail_flag = 0;

	UserInfo() = default;
	UserInfo(Json::Value json_info);
	Json::Value ToJson() const;
	void Update(const UserInfo& info);
};

typedef std::function<void(int res, const std::string& err_msg)> OnRegisterAccountCallback;
typedef std::function<void(UserChangeType change_type, const UserInfo&)> OnFriendListChangeCallback;
typedef std::function<void(const std::list<UserInfo>&)> OnUserInfoChangeCallback;
typedef std::function<void(bool, const std::list<UserInfo>&)> OnGetUserInfoCallback;
typedef std::function<void(int res)> OnUpdateUserInfoCallback;
typedef std::function<void(const std::string& accid, const std::wstring &photo_path)> OnUserPhotoReadyCallback;

const static char* g_AppServerAddress = "kAppServerAddress";
const static char* g_AppKey = "kAppKey";
std::string GetConfigValue(const std::string& key);

namespace nim_comp
{
class UserService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(UserService);
	UserService();

public:
	void InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb);
	void InvokeGetAllUserInfo(const OnGetUserInfoCallback& cb); // 向数据库和服务器获取全部好友以及近期获取过的用户信息
	void InvokeUpdateUserInfo(const UserInfo &new_info, const OnUpdateUserInfoCallback& cb); // 修改自己的个人信息
	void InvokeChangeUserPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb); // 修改自己的头像
	const std::map<std::string,UserInfo>& GetAllUserInfos(); // 获取UserService::all_user_保存的所有用户信息
	void GetUserInfoWithEffort(const std::list<std::string>& account_list, const OnGetUserInfoCallback& cb); // 尽最大努力获取用户信息，只能异步获取
	bool GetUserInfo(const std::string &id, UserInfo &info); // 查询UserService::all_user_中某个用户信息（可能查不到）
	UserType GetUserType(const std::string &id); // 好友or陌生人？
	std::wstring GetUserName(const std::string &id); // 从UserService::all_user_中查询用户昵称
	std::wstring GetUserPhoto(const std::string &accid); // 从UserService::all_user_中查询用户头像
	std::wstring GetUserPhotoDir(); // 获取存放用户头像的文件夹路径
	UnregisterCallback RegFriendListChange(const OnFriendListChangeCallback& callback); // 注册好友列表改变的回调
	UnregisterCallback RegUserInfoChange(const OnUserInfoChangeCallback& callback); // 注册用户信息改变的回调
	UnregisterCallback RegUserPhotoReady(const OnUserPhotoReadyCallback& callback); // 注册用户头像下载完成的回调

	void OnFriendListChange(nim::NIMFriendChangeType type, const std::list<nim::UserProfile>& user_profile_list);
	void OnUserInfoChange(const std::string &json_result);

private:
	void InvokeGetUserInfo(const std::list<std::string>& account_list, const OnGetUserInfoCallback& cb);// 向数据库和服务器获取指定id的用户信息
	void InvokeFriendListChangeCallback(UserChangeType change_type, const UserInfo& uinfo);
	void DownloadUserPhoto(const UserInfo &info); //获取用户信息后，或者用户信息修改后，下载用户头像
	bool CheckPhotoOK(std::wstring photo_path); // 检查某个头像图片是否完好

private:
	std::map<std::string,UserInfo> all_user_; //好友+陌生人
	std::set<std::string> post_ids_; //已经发出请求的id们
	std::set<std::string> friend_list_; //好友列表
	std::map<int, std::unique_ptr<OnFriendListChangeCallback>> friend_list_change_cb_list_; //有好友增减回调列表
	std::map<int, std::unique_ptr<OnUserInfoChangeCallback>> uinfo_change_cb_list_; //用户信息变化回调列表
	std::map<int, std::unique_ptr<OnUserPhotoReadyCallback>> photo_ready_cb_list_; //用户头像下载完成回调列表
};

}

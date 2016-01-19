#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

const std::string kAppServerAddress = "http://app.netease.im";

//数据变化类型
enum FriendChangeType
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

typedef std::function<void(int res, const std::string& err_msg)> OnRegisterAccountCallback;
typedef std::function<void(FriendChangeType change_type, const std::string& accid)> OnFriendListChangeCallback;
typedef std::function<void(const std::list<nim::UserNameCard>&)> OnUserInfoChangeCallback;
typedef std::function<void(const std::list<nim::UserNameCard>&)> OnGetUserInfoCallback;
typedef std::function<void(nim::NIMResCode res)> OnUpdateUserInfoCallback;
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
	void InvokeUpdateUserInfo(const nim::UserNameCard &new_info, const OnUpdateUserInfoCallback& cb); // 修改自己的个人信息
	void InvokeChangeUserPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb); // 修改自己的头像
	const std::map<std::string, nim::UserNameCard>& GetAllUserInfos(); // 获取UserService::all_user_保存的所有用户信息
	bool GetUserInfo(const std::string &id, nim::UserNameCard &info); // 查询UserService::all_user_中某个用户信息（可能查不到）
	void GetUserInfos(const std::list<std::string> &ids, std::list<nim::UserNameCard>&uinfos);
	nim::NIMFriendFlag GetUserType(const std::string &id); // 好友or陌生人？
	std::wstring GetUserName(const std::string &id, bool alias_prior = true); // 查询用户昵称或备注名
	std::wstring GetFriendAlias(const std::string &id); //查询好友备注名
	std::wstring GetUserPhoto(const std::string &accid); // 从UserService::all_user_中查询用户头像
	std::wstring GetUserPhotoDir(); // 获取存放用户头像的文件夹路径
	UnregisterCallback RegFriendListChange(const OnFriendListChangeCallback& callback); // 注册好友列表改变的回调
	UnregisterCallback RegUserInfoChange(const OnUserInfoChangeCallback& callback); // 注册用户名、头像改变的回调
	UnregisterCallback RegMiscUInfoChange(const OnUserInfoChangeCallback& callback); // 注册用户其他信息改变的回调
	UnregisterCallback RegUserPhotoReady(const OnUserPhotoReadyCallback& callback); // 注册用户头像下载完成的回调

	void OnFriendListChange(const nim::FriendChangeEvent& change_event);
	void OnUserInfoChange(const std::list<nim::UserNameCard> &json_result);

private:
	void InvokeGetUserInfo(const std::list<std::string>& account_list);// 向数据库和服务器获取指定id的用户信息
	void InvokeFriendListChangeCallback(FriendChangeType change_type, const std::string& accid);
	void UIFriendListChangeCallback(FriendChangeType change_type, const std::string& accid);
	void DownloadUserPhoto(const nim::UserNameCard &info); //获取用户信息后，或者用户信息修改后，下载用户头像
	bool CheckPhotoOK(std::wstring photo_path); // 检查某个头像图片是否完好

private:
	std::map<std::string, nim::UserNameCard> all_user_; //好友+陌生人
	std::set<std::string> post_ids_; //已经发出请求的id们
	std::map<std::string, nim::FriendProfile> friend_list_; //好友列表
	std::map<int, std::unique_ptr<OnFriendListChangeCallback>> friend_list_change_cb_list_; //有好友增减回调列表
	std::map<int, std::unique_ptr<OnUserInfoChangeCallback>> uinfo_change_cb_list_; //用户名、头像变化回调列表
	std::map<int, std::unique_ptr<OnUserInfoChangeCallback>> misc_uinfo_change_cb_list_; //用户其他信息变化回调列表
	std::map<int, std::unique_ptr<OnUserPhotoReadyCallback>> photo_ready_cb_list_; //用户头像下载完成回调列表

	std::set<std::string> on_query_list_; //已经要求查询，但还未返回结果的
};

}

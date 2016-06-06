#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

enum PhotoType
{
	kUser,
	kTeam,
};

typedef std::function<void(PhotoType type, const std::string& id, const std::wstring &photo_path)> OnPhotoReadyCallback;

namespace nim_comp
{

class PhotoService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(PhotoService);

private:
	PhotoService() {}
	~PhotoService() {}

public:
	UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback); // 注册头像下载完成的回调
	std::wstring GetUserPhoto(const std::string &accid); // 从UserService::all_user_中查询用户头像
	std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

public:
	//不开放
	void DownloadUserPhoto(const nim::UserNameCard &info); //获取用户信息后，或者用户信息修改后，下载用户头像
	void DownloadTeamIcon(const nim::TeamInfo &info);
	std::wstring GetPhotoDir(PhotoType type);

private:
	bool CheckPhotoOK(std::wstring photo_path); // 检查某个头像图片是否完好
	int CheckForDownload(PhotoType type, const std::string& url);

private:
	std::map<int, std::unique_ptr<OnPhotoReadyCallback>> photo_ready_cb_list_; //用户头像下载完成回调列表
};


}
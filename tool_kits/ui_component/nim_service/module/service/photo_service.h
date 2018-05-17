#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

enum PhotoType
{
	kUser,
	kRobot,
	kTeam,
	kOther,
};

typedef std::function<void(PhotoType type, const std::string& id, const std::wstring &photo_path)> OnPhotoReadyCallback;

namespace nim_comp
{
/** @class PhotoService
  * @brief 头像下载与获取服务
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/13
  */
class PhotoService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(PhotoService);
	PhotoService() {}
	~PhotoService() {}

public:

	/**
	* 注册头像下载完毕后的回调函数
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback);

	/**
	* 获取用户头像
	* @param[in] accid 用户id
	* @param[in] is_robot 该id是不是机器人
	* @return wstring 头像的路径
	*/
	std::wstring GetUserPhoto(const std::string &accid, bool is_robot = false);

	/**
	* 获取群组头像
	* @param[in] tid 群id
	* @param[in] full_path 是否需要绝对路径
	* @return wstring 头像的路径
	*/
	std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

public:
	/**
	* 获取用户信息后，或者用户信息修改后，下载用户头像(不对外开放，提供给其他service调用)
	* @param[in] info 要下载头像的用户名片
	* @return void	无返回值
	*/
	void DownloadUserPhoto(const nim::UserNameCard &info);

	/**
	* 获取机器人信息后，或者机器人信息修改后，下载机器人头像(不对外开放，提供给其他service调用)
	* @param[in] info 要下载头像的机器人信息
	* @return void	无返回值
	*/
	void DownloadRobotPhoto(const nim::RobotInfo &info);

	/**
	* 获取群信息后，或者群信息修改后，下载群头像(不对外开放，提供给其他service调用)
	* @param[in] info 要下载头像的群信息
	* @return void	无返回值
	*/
	void DownloadTeamIcon(const nim::TeamInfo &info);

	/**
	* 获取用户或群头像所在目录
	* @param[in] type 获取头像的类型
	* @return wstring 头像目录所在路径
	*/
	std::wstring GetPhotoDir(PhotoType type);

	/**
	* 检查某个头像图片是否完好
	* @param[in] photo_path 头像文件路径
	* @return bool true 完好，false 不完好
	*/
	bool CheckPhotoOK(std::wstring photo_path);

private:


	/**
	* 检查某个url对应的头像图片在本地是否完好
	* @param[in] type 头像的类型
	* @param[in] url 头像的下载地址
	* @return int -1 url不正确，0 不玩好，1 完好
	*/
	int CheckForDownload(PhotoType type, const std::string& url);

private:
	std::map<int, std::unique_ptr<OnPhotoReadyCallback>> photo_ready_cb_list_; //用户头像下载完成回调列表
	std::map<PhotoType, std::map<std::string, std::string>> download_tasks_;
};


}
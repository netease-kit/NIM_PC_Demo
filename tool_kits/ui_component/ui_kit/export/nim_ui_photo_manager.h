#pragma once

#include "nim_ui_dll.h"
#include "module/service/photo_service.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class PhotoManager
  * @brief 提供头像资源有关的接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2016/5/21
  */
class NIM_UI_DLL_API PhotoManager
{
public:
	SINGLETON_DEFINE(PhotoManager);

public:

	/**
	* 获取用户头像
	* @param[in] accid 用户id
	* @return wstring 头像的路径
	*/
	std::wstring GetUserPhoto(const std::string &accid);

	/**
	* 获取群组头像
	* @param[in] tid 群id
	* @param[in] full_path 是否需要绝对路径
	* @return wstring 头像的路径
	*/
	std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

	/**
	* 注册头像下载完毕后的回调函数
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback);

private:
	PhotoManager() {}
	~PhotoManager() {}
};

}
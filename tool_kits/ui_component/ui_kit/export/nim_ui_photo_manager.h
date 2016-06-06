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
	PhotoManager() {}
	~PhotoManager() {}

public:
	std::wstring GetUserPhoto(const std::string &accid);
	std::wstring GetTeamPhoto(const std::string &tid, bool full_path = true);

	UnregisterCallback RegPhotoReady(const OnPhotoReadyCallback& callback);

};

}
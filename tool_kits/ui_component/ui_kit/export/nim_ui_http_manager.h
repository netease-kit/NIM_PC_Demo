#pragma once

#include "nim_ui_dll.h"
#include "module/service/http_service.h"
#include "tool_kits/base/memory/singleton.h"

namespace nim_ui
{

/** @class HttpManager
  * @brief 提供Http下载资源有关的接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2016/5/12
  */
class NIM_UI_DLL_API HttpManager
{
public:
	SINGLETON_DEFINE(HttpManager);
public:
	HttpManager() {}
	~HttpManager() {}
public:
	std::wstring GetCustomImage(HttpResourceType type, const std::string &id, const std::string &url);
	UnregisterCallback RegDownloadComplete(const OnDownloadCompleteCallback& callback);
};

}
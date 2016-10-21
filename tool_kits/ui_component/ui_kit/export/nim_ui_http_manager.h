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
	HttpManager() {}
	~HttpManager() {}

public:
	/**
	* 下载图片资源
	* @param[in] type 资源类型
	* @param[in] id 资源ID，作为识别标识
	* @param[in] url 下载地址
	* @return wstring 图片路径
	*/
	std::wstring GetCustomImage(HttpResourceType type, const std::string &id, const std::string &url);

	/**
	* 注册资源下载完成的回调
	* @param[in] callback 下载完成的回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegDownloadComplete(const OnDownloadCompleteCallback& callback);
};

}
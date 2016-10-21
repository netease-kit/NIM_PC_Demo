#pragma once
#include "shared/threads.h"
#include "shared/auto_unregister.h"

enum HttpResourceType
{
	kUndefined	= 0,
	kImage		= 1,
	kChatroomMemberIcon = 2,
};

typedef std::function<void(HttpResourceType type, const std::string& id, const std::wstring &res_path)> OnDownloadCompleteCallback;
typedef std::function<void(HttpResourceType type, const std::string& id, const std::wstring &local_path, const std::string& url)> OnUploadCompleteCallback;

namespace nim_comp
{
/** @class HttpService
  * @brief 资源下载服务
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/18
  */
class HttpService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(HttpService);

private:
	HttpService() {}
	~HttpService() {}

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

private:
	/**
	* 下载资源文件
	* @param[in] type 资源类型
	* @param[in] id 资源ID，作为识别标识
	* @param[in] url 下载地址
	* @return wstring 图片路径
	* @return void	无返回值
	*/
	void DownloadResource(HttpResourceType type, const std::string &id, const std::string &url);

	/**
	* 检查图片是否损坏
	* @param[in] image_path 图片路径
	* @return bool true 成功，false 失败
	*/
	bool CheckImageValid(std::wstring image_path);

	/**
	* 获取http文件缓存路径
	* @return wstring 缓存路径
	*/
	std::wstring GetHttpCachedFileDir(); 

private:
	std::map<int, std::unique_ptr<OnDownloadCompleteCallback>> download_ready_cb_list_;
};


}
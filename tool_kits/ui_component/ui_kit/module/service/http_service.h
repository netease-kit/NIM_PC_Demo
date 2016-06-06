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
class HttpService : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(HttpService);

private:
	HttpService() {}
	~HttpService() {}

public:
	std::wstring GetCustomImage(HttpResourceType type, const std::string &id, const std::string &url);
	UnregisterCallback RegDownloadComplete(const OnDownloadCompleteCallback& callback);

private:
	void DownloadResource(HttpResourceType type, const std::string &id, const std::string &url);
	bool CheckImageValid(std::wstring image_path);
	std::wstring GetHttpCachedFileDir(); 

private:
	std::map<int, std::unique_ptr<OnDownloadCompleteCallback>> download_ready_cb_list_;
};


}
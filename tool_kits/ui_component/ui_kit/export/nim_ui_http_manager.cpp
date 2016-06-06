#include "nim_ui_http_manager.h"

namespace nim_ui
{
std::wstring HttpManager::GetCustomImage(HttpResourceType type, const std::string &id, const std::string &url)
{
	return nim_comp::HttpService::GetInstance()->GetCustomImage(type, id, url);
}

UnregisterCallback HttpManager::RegDownloadComplete(const OnDownloadCompleteCallback& callback)
{
	return nim_comp::HttpService::GetInstance()->RegDownloadComplete(callback);
}

}
#include "http_service.h"
#include "module/login/login_manager.h"

namespace nim_comp
{
std::wstring HttpService::GetCustomImage(HttpResourceType type, const std::string &id, const std::string &url)
{
	std::wstring default_img = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
	if (!nbase::FilePathIsExist(default_img, false))
		default_img = L"";

	if (id.empty() || url.empty())
		return default_img;

	// 检查图片是否存在
	std::wstring file_path = GetHttpCachedFileDir() + nbase::UTF8ToUTF16(QString::GetMd5(url));
	if (!CheckImageValid(file_path))
	{
		DownloadResource(type, id, url);
		return default_img;
	}

	return GetHttpCachedFileDir() + nbase::UTF8ToUTF16(QString::GetMd5(url));
}

UnregisterCallback HttpService::RegDownloadComplete(const OnDownloadCompleteCallback& callback)
{
	OnDownloadCompleteCallback* new_callback = new OnDownloadCompleteCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	download_ready_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		download_ready_cb_list_.erase(cb_id);
	});
	return cb;
}

void HttpService::DownloadResource(HttpResourceType type, const std::string &id, const std::string &url)
{
	if (id.empty() || url.find_first_of("http") != 0)
	{
		assert(0);
		return;
	}

	if (type == kChatroomMemberIcon)
	{
		std::wstring photo_path = GetHttpCachedFileDir() + nbase::UTF8ToUTF16(QString::GetMd5(url));
		if (CheckImageValid(photo_path)) // 如果已经，就不下载
			return;

		nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, type, id, photo_path](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
			if (res_code == nim::kNIMResSuccess)
			{
				std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
				if (nbase::FilePathIsExist(ws_file_path, false))
				{
					nbase::CopyFileW(ws_file_path, photo_path);
					nbase::DeleteFile(ws_file_path);

					for (auto &it : download_ready_cb_list_) 
						(*it.second)(type, id, photo_path);
				}
			}
		});
		nim::NOS::DownloadResource(url, cb);
	}
}

bool HttpService::CheckImageValid(std::wstring image_path)
{
	if (!nbase::FilePathIsExist(image_path, false))
		return false;

	// 检查图片是否损坏
	return (Gdiplus::Image(image_path.c_str()).GetLastStatus() == Gdiplus::Status::Ok);
}

std::wstring HttpService::GetHttpCachedFileDir()
{
	std::wstring dir = QPath::GetUserAppDataDir(LoginManager::GetInstance()->GetAccount()).append(L"demo_res\\");
	if (!nbase::FilePathIsExist(dir, true))
		nbase::win32::CreateDirectoryRecursively(dir.c_str());
	return dir;
}
}
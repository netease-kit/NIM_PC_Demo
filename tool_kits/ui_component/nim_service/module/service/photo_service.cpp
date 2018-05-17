#include "photo_service.h"
#include "user_service.h"
#include "module/login/login_manager.h"

namespace nim_comp
{

std::wstring PhotoService::GetUserPhoto(const std::string &accid, bool is_robot)
{
	std::wstring default_photo = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
	if (!nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";

	std::wstring photo_path;
	if (!is_robot)
	{
		nim::UserNameCard info;
		UserService::GetInstance()->GetUserInfo(accid, info);
		if (!info.ExistValue(nim::kUserNameCardKeyIconUrl) || info.GetIconUrl().empty())
			return default_photo;
		photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(QString::GetMd5(info.GetIconUrl()));
	}
	else
	{
		nim::RobotInfo rinfo;
		UserService::GetInstance()->GetRobotInfo(accid, rinfo);
		if (rinfo.GetIcon().empty())
			return default_photo;
		photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(QString::GetMd5(rinfo.GetIcon()));
	}

	// 检查图片是否存在
	if (!CheckPhotoOK(photo_path))
		return default_photo;

	return photo_path;
}

bool PhotoService::CheckPhotoOK(std::wstring photo_path)
{
	if (!nbase::FilePathIsExist(photo_path, false))
		return false;

	// 检查图片是否损坏
	return (Gdiplus::Image(photo_path.c_str()).GetLastStatus() == Gdiplus::Status::Ok);
}

std::wstring PhotoService::GetPhotoDir(PhotoType type)
{
	std::wstring photo_dir = QPath::GetUserAppDataDir(LoginManager::GetInstance()->GetAccount());
	if (type == kUser || type == kRobot)
		photo_dir = photo_dir.append(L"photo\\");
	else if (type == kTeam)
		photo_dir = photo_dir.append(L"team_photo\\");
	else if (type == kOther)
		photo_dir = photo_dir.append(L"other\\");

	if (!nbase::FilePathIsExist(photo_dir, true))
		nbase::win32::CreateDirectoryRecursively(photo_dir.c_str());
	return photo_dir;
}

UnregisterCallback PhotoService::RegPhotoReady(const OnPhotoReadyCallback & callback)
{
	OnPhotoReadyCallback* new_callback = new OnPhotoReadyCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	photo_ready_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		photo_ready_cb_list_.erase(cb_id);
	});
	return cb;
}

int PhotoService::CheckForDownload(PhotoType type, const std::string& url)
{
	if (url.find_first_of("http") != 0) //info.head_image不是正确的url
		return -1;

	std::wstring photo_path = GetPhotoDir(type) + nbase::UTF8ToUTF16(QString::GetMd5(url));
	if (CheckPhotoOK(photo_path)) // 如果头像已经存在且完好，就不下载
		return 1;

	std::string url_md5 = nim::Tool::GetMd5(url);
	if (download_tasks_[type].find(url_md5) != download_tasks_[type].end())
		return 2;

	download_tasks_[type][url_md5] = "";
	return 0;
}

void PhotoService::DownloadUserPhoto(const nim::UserNameCard &info)
{
	std::string url = info.GetIconUrl();
	std::wstring photo_path = GetPhotoDir(kUser) + nbase::UTF8ToUTF16(QString::GetMd5(url));
	int valid = CheckForDownload(kUser, url);
	if (valid != 0)
	{
		if (valid == 1)
		{
			for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
				(*it.second)(kUser, info.GetAccId(), photo_path);
		}
		return;
	}

	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, info, photo_path](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		download_tasks_[kUser].erase(nim::Tool::GetMd5(info.GetIconUrl()));
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);

				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
					(*it.second)(kUser, info.GetAccId(), photo_path);
			}
		}
	});
	nim::NOS::DownloadResource(url, cb);
}

void PhotoService::DownloadRobotPhoto(const nim::RobotInfo &info)
{
	std::string url = info.GetIcon();
	std::wstring photo_path = GetPhotoDir(kRobot) + nbase::UTF8ToUTF16(QString::GetMd5(url));
	int valid = CheckForDownload(kRobot, url);
	if (valid != 0)
	{
		if (valid == 1)
		{
			for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
				(*it.second)(kRobot, info.GetAccid(), photo_path);
		}
		return;
	}

	nim::NOS::DownloadMediaExCallback cb = ToWeakCallback([this, info, photo_path](nim::NIMResCode res_code, const nim::DownloadMediaResult& result) {
		download_tasks_[kRobot].erase(nim::Tool::GetMd5(info.GetIcon()));
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(result.file_path_);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);

				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
					(*it.second)(kRobot, info.GetAccid(), photo_path);
			}
		}
	});
	Json::Value values;
	std::wstring temp = GetPhotoDir(kRobot) + nbase::UTF8ToUTF16(info.GetAccid());
	values[nim::kNIMNosSaveAsFilePath] = nbase::UTF16ToUTF8(temp);
	nim::NOS::DownloadResourceEx(url, values.toStyledString(), cb);
}

void PhotoService::DownloadTeamIcon(const nim::TeamInfo &info)
{
	std::string url = info.GetIcon();
	std::wstring photo_path = GetPhotoDir(kTeam) + nbase::UTF8ToUTF16(QString::GetMd5(url));
	int valid = CheckForDownload(kTeam, url);
	if (valid != 0)
	{
		if (valid == 1)
		{
			for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
			(*it.second)(kTeam, info.GetTeamID(), photo_path);
		}
		return;
	}

	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, info, photo_path](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		download_tasks_[kTeam].erase(nim::Tool::GetMd5(info.GetIcon()));
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);

				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
					(*it.second)(kTeam, info.GetTeamID(), photo_path);
			}
		}
	});
	nim::NOS::DownloadResource(url, cb);
}

std::wstring PhotoService::GetTeamPhoto(const std::string &tid, bool full_path/* = true*/)
{
	std::wstring default_photo = QPath::GetAppPath();
	if (full_path)
		default_photo.append(L"themes\\default\\public\\header\\head_team.png");
	else
		default_photo = L"../public/header/head_team.png";

	if (full_path && !nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";

	std::string icon;
	if (!TeamService::GetInstance()->GetTeamIcon(tid, icon) || icon.empty())
		return default_photo;

	std::wstring photo_path = GetPhotoDir(kTeam) + nbase::UTF8ToUTF16(QString::GetMd5(icon));
	if (!CheckPhotoOK(photo_path))
		return default_photo;

	return photo_path;
}


}
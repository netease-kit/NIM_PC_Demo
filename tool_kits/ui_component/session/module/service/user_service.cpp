#include "user_service.h"
#include "module/local/local_helper.h"
#include "shared/xml_util.h"
#include "module/login/login_manager.h"

UserInfo::UserInfo(Json::Value json_info) : 
	account(json_info[nim::kUInfoKeyAccid].asString())
{
	if (json_info.isMember(nim::kUInfoKeyName))
	{
		field_avail_flag |= kUInfoFlagName;
		name = json_info[nim::kUInfoKeyName].asString();
	}
	if (json_info.isMember(nim::kUInfoKeyIcon))
	{
		field_avail_flag |= kUInfoFlagHeadImage;
		head_image = json_info[nim::kUInfoKeyIcon].asString();
	}
	if (json_info.isMember(nim::kUInfoKeyGender))
	{
		field_avail_flag |= kUInfoFlagGender;
		switch (json_info[nim::kUInfoKeyGender].asInt())
		{
		case 1:
			gender = UG_MALE;
			break;
		case 2:
			gender = UG_FEMALE;
			break;
		default:
			gender = UG_UNKNOWN;
			break;
		}
	}
	if (json_info.isMember(nim::kUInfoKeyBirth))
	{
		field_avail_flag |= kUInfoFlagBirthday;
		birthday = json_info[nim::kUInfoKeyBirth].asString();
	}
	if (json_info.isMember(nim::kUInfoKeyMobile))
	{
		field_avail_flag |= kUInfoFlagPhone;
		phone = json_info[nim::kUInfoKeyMobile].asString();
	}
	if (json_info.isMember(nim::kUInfoKeyEmail))
	{
		field_avail_flag |= kUInfoFlagEmail;
		email = json_info[nim::kUInfoKeyEmail].asString();
	}
	if (json_info.isMember(nim::kUInfoKeySign))
	{
		field_avail_flag |= kUInfoFlagSignature;
		signature = json_info[nim::kUInfoKeySign].asString();
	}
}

Json::Value UserInfo::ToJson() const
{
	Json::Value value;

	value[nim::kUInfoKeyAccid] = account;
	if ((field_avail_flag & kUInfoFlagName) != 0)
		value[nim::kUInfoKeyName] = name;
	if ((field_avail_flag & kUInfoFlagHeadImage) != 0)
		value[nim::kUInfoKeyIcon] = head_image;
	if ((field_avail_flag & kUInfoFlagGender) != 0)
		value[nim::kUInfoKeyGender] = (int)gender;
	if ((field_avail_flag & kUInfoFlagBirthday) != 0)
		value[nim::kUInfoKeyBirth] = birthday;
	if ((field_avail_flag & kUInfoFlagPhone) != 0)
		value[nim::kUInfoKeyMobile] = phone;
	if ((field_avail_flag & kUInfoFlagEmail) != 0)
		value[nim::kUInfoKeyEmail] = email;
	if ((field_avail_flag & kUInfoFlagSignature) != 0)
		value[nim::kUInfoKeySign] = signature;

	return value;
}

void UserInfo::Update(const UserInfo & info)
{
	if (account != info.account)
		return;

	if ((info.field_avail_flag & kUInfoFlagName) != 0)
		name = info.name;
	if ((info.field_avail_flag & kUInfoFlagHeadImage) != 0)
		head_image = info.head_image;
	if ((info.field_avail_flag & kUInfoFlagGender) != 0)
		gender = info.gender;
	if ((info.field_avail_flag & kUInfoFlagBirthday) != 0)
		birthday = info.birthday;
	if ((info.field_avail_flag & kUInfoFlagPhone) != 0)
		phone = info.phone;
	if ((info.field_avail_flag & kUInfoFlagEmail) != 0)
		email = info.email;
	if ((info.field_avail_flag & kUInfoFlagSignature) != 0)
		signature = info.signature;

	field_avail_flag |= info.field_avail_flag;
}

std::string GetConfigValue(const std::string& key)
{
	std::string value;
	std::wstring server_conf_path = QPath::GetAppPath();
	server_conf_path.append(L"server_conf.txt");
	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, server_conf_path))
	{
		TiXmlElement* root = document.RootElement();
		if (root)
		{
			Json::Value srv_config;
			if (auto pchar = root->Attribute(key.c_str()))
			{
				value = pchar;
			}
		}
	}

	return value;
}

namespace nim_comp
{

UserService::UserService()
{
	nim::Friend::RegChangeCb(nbase::Bind(&UserService::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2));
	nim::User::RegUserInfoChangedCb(nbase::Bind(&UserService::OnUserInfoChange, this, std::placeholders::_1));
}

void UserService::OnFriendListChange(nim::NIMFriendChangeType type, const std::list<nim::UserProfile>& user_profile_list)
{
	std::list<std::string> add_list;
	std::list<std::string> delete_list;
	for (auto& it : user_profile_list)
	{
		if (it.flag == nim::kNIMFriendFlagNotFriend)
		{
			friend_list_.erase(it.accid); // 从friend_list_删除
			delete_list.push_back(it.accid);
		}
		else
		{
			friend_list_.insert(it.accid);
			add_list.push_back(it.accid);
		}
	}
	if (!add_list.empty())
	{
		OnGetUserInfoCallback cb = ToWeakCallback([this](bool ret, std::list<UserInfo> uinfos) {
			for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
				InvokeFriendListChangeCallback(kChangeTypeAdd, *iter);
		});
		GetUserInfoWithEffort(add_list, cb);
	}
	if (!delete_list.empty())
	{
		OnGetUserInfoCallback cb = ToWeakCallback([this](bool ret, std::list<UserInfo> uinfos) {
			for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
				InvokeFriendListChangeCallback(kChangeTypeDelete, *iter);
		});
		GetUserInfoWithEffort(delete_list, cb);
	}
}

void UserService::OnUserInfoChange(const std::string &json_result)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	Json::Value result;
	Json::Reader reader;
	if (reader.parse(json_result, result))
	{
		std::list<UserInfo> uinfos;
		size_t count = result.size();
		for (size_t i = 0; i < count; i++)
		{
			UserInfo info(result[i]);

			auto iter = all_user_.find(info.account);
			if (iter != all_user_.end())
				iter->second.Update(info);
			else
				InvokeGetUserInfo(std::list<std::string>(1, info.account), nullptr);

			if((info.field_avail_flag & kUInfoFlagHeadImage) != 0)
				DownloadUserPhoto(info);
			uinfos.push_back(info);
		}

		for (auto& it : uinfo_change_cb_list_) // 执行回调列表中所有回调
			(*(it.second))(uinfos);
	}
}

UnregisterCallback UserService::RegFriendListChange(const OnFriendListChangeCallback& callback)
{
	OnFriendListChangeCallback* new_callback = new OnFriendListChangeCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	friend_list_change_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		friend_list_change_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback UserService::RegUserInfoChange(const OnUserInfoChangeCallback& callback)
{
	OnUserInfoChangeCallback* new_callback = new OnUserInfoChangeCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	uinfo_change_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		uinfo_change_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback UserService::RegUserPhotoReady(const OnUserPhotoReadyCallback & callback)
{
	OnUserPhotoReadyCallback* new_callback = new OnUserPhotoReadyCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	photo_ready_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		photo_ready_cb_list_.erase(cb_id);
	});
	return cb;
}

void UserService::InvokeFriendListChangeCallback(UserChangeType change_type, const UserInfo& user_infos)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : friend_list_change_cb_list_)
	{
		(*(it.second))(change_type, user_infos);
	}
}

void UserService::DownloadUserPhoto(const UserInfo &info)
{
	if (info.head_image.find_first_of("http") != 0) //info.head_image不是正确的url
		return;

	std::wstring photo_path = GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.head_image));
	if (info.head_image.empty() || CheckPhotoOK(photo_path)) // 如果头像已经存在且完好，就不下载
		return;

	nim::Http::DownloadResourceCallback cb = ToWeakCallback([this, info, photo_path](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);

				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
					(*it.second)(info.account, photo_path);
			}
		}
	});
	nim::Http::DownloadResource(info.head_image, cb);
}

void UserService::InvokeRegisterAccount(const std::string &username, const std::string &password, const std::string &nickname, const OnRegisterAccountCallback& cb)
{
	std::string addressbook_address = kAppServerAddress;
	std::string new_addressbook_address = GetConfigValue(g_AppServerAddress);
	if (!new_addressbook_address.empty())
	{
		addressbook_address = new_addressbook_address;
	}
	addressbook_address += "/api/createDemoUser";
	auto reg_cb = [cb](bool ret, int response_code, const std::string& reply)
	{
		if (ret && response_code == 200) {
			Json::Value json;
			Json::Reader reader;
			bool res = reader.parse(reply, json);
			if (!res)
			{
				nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(cb, 0, "未知错误"));
				return;
			}

			int json_res = json["res"].asInt();
			std::string err_msg = json["errmsg"].asString();
			nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(cb, json_res, err_msg));
		}
		else {
			nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(cb, 0, nbase::UTF16ToUTF8(L"网络出现问题，请确认网络连接")));
		}
	};

	std::string body;
	body += "username=" + username;
	body += "&password=" + password;
	body += "&nickname=" + nickname;

	std::string app_key = "45c6af3c98409b18a84451215d0bdd6e";
	std::string new_app_key = GetConfigValue(g_AppKey);
	if (!new_app_key.empty())
	{
		app_key = new_app_key;
	}
	nim_http::HttpRequest request(addressbook_address, body.c_str(), body.size(), reg_cb);
	request.AddHeader("Content-Type", "application/x-www-form-urlencoded");
	request.AddHeader("charset", "utf-8");
	request.AddHeader("appkey", app_key);
	request.SetMethodAsPost();
	nim_http::PostRequest(request);
}

void UserService::InvokeGetAllUserInfo(const OnGetUserInfoCallback& cb)
{
	nim::Friend::GetList(ToWeakCallback([this, cb](int res_code, const std::list<nim::UserProfile>& user_profile_list)
	{
		std::list<std::string> account_list;
		for (auto& it : user_profile_list)
		{
			if (it.flag == nim::kNIMFriendFlagNormal)
				friend_list_.insert(it.accid); //插入friend_list_（类的成员变量）好友列表
			account_list.push_back(it.accid);
		}
		if(!account_list.empty())
			InvokeGetUserInfo(account_list, cb); // 从db和服务器查询用户信息
	}));
}

void UserService::InvokeGetUserInfo(const std::list<std::string>& account_list, const OnGetUserInfoCallback & cb)
{
	// 先在本地db中找
	nim::User::GetUserInfoCallback cb1 = ToWeakCallback([this, account_list, cb](const std::string &json_result)
	{
		Json::Reader reader;
		Json::Value result1;
		if (reader.parse(json_result, result1))
		{
			std::list<UserInfo> already_get;
			std::set<std::string> not_get_set(account_list.cbegin(), account_list.cend());
			size_t count1 = result1.size();
			for (size_t i = 0; i < count1; i++)
			{
				std::string accid = result1[i][nim::kUInfoKeyAccid].asString();
				UserInfo uinfo(result1[i]);
				already_get.push_back(uinfo);
				not_get_set.erase(accid);
				all_user_[accid] = uinfo; // 插入all_user
				//能在数据库中查到用户信息，则用户头像应该以前下载过，因此此处不下载。
			}
			if (cb && !already_get.empty())
			{
				assert(nbase::MessageLoop::current()->ToUIMessageLoop());
				cb(true, already_get); // 执行参数传入的回调
			}
			if (not_get_set.empty()) // 全部从本地db找到，直接返回
				return;

			// 有些信息本地db没有，再从服务器获取
			std::list<std::string> not_get_list(not_get_set.cbegin(), not_get_set.cend());
			nim::User::GetUserInfoCallback cb2 = ToWeakCallback([this, not_get_list, cb](const std::string &json_result)
			{
				Json::Reader reader;
				Json::Value result2;
				if (reader.parse(json_result, result2))
				{
					std::list<UserInfo> last_get;
					size_t count2 = result2.size();
					for (size_t i = 0; i < count2; i++)
					{
						std::string accid = result2[i][nim::kUInfoKeyAccid].asString();
						UserInfo uinfo(result2[i]);
						last_get.push_back(uinfo);
						all_user_[uinfo.account] = uinfo; // 插入all_user
						if((uinfo.field_avail_flag & kUInfoFlagHeadImage) != 0)
							DownloadUserPhoto(uinfo); // 下载头像
					}
					if (cb)
					{
						assert(nbase::MessageLoop::current()->ToUIMessageLoop());
						cb(true, last_get); // 执行参数传入的回调
					}
				}
			});
			nim::User::GetUserInfoOnline(not_get_list, cb2);
		}
	});
	nim::User::GetUserInfo(account_list, cb1);
}

void UserService::GetUserInfoWithEffort(const std::list<std::string>& account_list, const OnGetUserInfoCallback& cb)
{
	std::list<UserInfo> already_get;
	std::list<std::string> not_get_list;
	for (auto accid : account_list)
	{
		auto iter = all_user_.find(accid); // 先从all_user_里面找
		if (iter != all_user_.cend())
			already_get.push_back(iter->second);
		else
			not_get_list.push_back(accid);
	}
	if (cb && !already_get.empty())
	{
		assert(nbase::MessageLoop::current()->ToUIMessageLoop());
		cb(true, already_get); // 执行参数传入的回调
	}
	if (not_get_list.empty()) // 全部从all_user_里面找到，直接返回
		return;

	// 有些信息不在all_user_里面，再到本地db和服务器中找
	InvokeGetUserInfo(not_get_list, cb);
}

void UserService::InvokeUpdateUserInfo(const UserInfo &new_info, const OnUpdateUserInfoCallback& cb)
{
	auto update_uinfo_cb = ToWeakCallback([this, new_info, cb](int res) {
		if (res == nim::kNIMResSuccess)
		{
			assert(nbase::MessageLoop::current()->ToUIMessageLoop());
			Json::Value json_infos;
			json_infos.append(new_info.ToJson());
			OnUserInfoChange(json_infos.toStyledString());
		}
		if (cb != nullptr)
			cb(res);
	});
	nim::User::UpdateUserInfo(new_info.ToJson().toStyledString(), update_uinfo_cb);
}

void UserService::InvokeChangeUserPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb)
{
	UserInfo my_info;
	my_info.account = LoginManager::GetInstance()->GetAccount();
	my_info.field_avail_flag = kUInfoFlagHeadImage;
	my_info.head_image = url;
	InvokeUpdateUserInfo(my_info, cb);
}

const std::map<std::string, UserInfo>& UserService::GetAllUserInfos()
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	return all_user_;
}

bool UserService::GetUserInfo(const std::string &id, UserInfo &info)
{
	auto iter = all_user_.find(id);
	if (iter != all_user_.cend())
	{
		info = iter->second;
		return true;
	}
	else
	{
		info.name = info.account = id;
		InvokeGetUserInfo(std::list<std::string>(1, id), nullptr);
		return false;
	}
}

UserType UserService::GetUserType(const std::string &id)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	return (friend_list_.find(id) != friend_list_.end() ? UT_FRIEND : UT_UNKNOWN);
}

std::wstring UserService::GetUserName(const std::string &id)
{
	UserInfo info;
	GetUserInfo(id, info);
	return nbase::UTF8ToUTF16(info.name);
}

std::wstring UserService::GetUserPhoto(const std::string &accid)
{
	std::wstring default_photo = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
	if (!nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";

	UserInfo info;
	GetUserInfo(accid, info);
	if ((info.field_avail_flag & kUInfoFlagHeadImage) == 0 || info.head_image.empty())
		return default_photo;

	// 检查图片是否存在
	std::wstring photo_path = GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.head_image));
	if (!nbase::FilePathIsExist(photo_path, false))
		return default_photo;

	if (!CheckPhotoOK(photo_path))
		return default_photo;

	return GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.head_image));
}

bool UserService::CheckPhotoOK(std::wstring photo_path)
{
	if (!nbase::FilePathIsExist(photo_path, false))
		return false;

	// 检查图片是否损坏
	return (Gdiplus::Image(photo_path.c_str()).GetLastStatus() == Gdiplus::Status::Ok);
}

std::wstring UserService::GetUserPhotoDir()
{
	std::wstring photo_dir = QPath::GetUserAppDataDir(LoginManager::GetInstance()->GetAccount()).append(L"photo\\");
	if (!nbase::FilePathIsExist(photo_dir, true))
		nbase::win32::CreateDirectoryRecursively(photo_dir.c_str());
	return photo_dir;
}

}

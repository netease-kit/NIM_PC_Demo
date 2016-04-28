#include "user_service.h"
#include "module/local/local_helper.h"
#include "shared/xml_util.h"
#include "module/login/login_manager.h"


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
	//向SDK注册监听好友列表变化
	nim::Friend::RegChangeCb(nbase::Bind(&UserService::OnFriendListChange, this, std::placeholders::_1));

	//向SDK注册监听用户名片变化
	nim::User::RegUserNameCardChangedCb(nbase::Bind(&UserService::OnUserInfoChange, this, std::placeholders::_1));
}

void UserService::OnFriendListChange(const nim::FriendChangeEvent& change_event)
{
	std::list<std::string> add_list;
	std::list<std::string> delete_list;
	std::list<std::string> update_list; // 需要更换备注名的用户列表

	switch (change_event.type_)
	{
	case nim::kNIMFriendChangeTypeDel:
	{
		nim::FriendDelEvent del_event;
		nim::Friend::ParseFriendDelEvent(change_event, del_event);
		delete_list.push_back(del_event.accid_);
		update_list.push_back(del_event.accid_); // 删除好友之后，其原来的备注名改为其昵称
		friend_list_.erase(del_event.accid_); // 从friend_list_删除
		break;
	}
	case nim::kNIMFriendChangeTypeRequest:
	{
		nim::FriendAddEvent add_event;
		nim::Friend::ParseFriendAddEvent(change_event, add_event);
		if (add_event.add_type_ == nim::kNIMVerifyTypeAdd || add_event.add_type_ == nim::kNIMVerifyTypeAgree)
		{
			// 此处根据accid获取该好友的FriendProfile，添加到friend_list_中。
			nim::Friend::GetFriendProfileCallback cb = ToWeakCallback([this](const std::string& accid, const nim::FriendProfile& user_profile) 
			{
				friend_list_[user_profile.GetAccId()] = user_profile;
				InvokeFriendListChangeCallback(kChangeTypeAdd, user_profile.GetAccId());
			});
			nim::Friend::GetFriendProfile(add_event.accid_, cb);
		}
		break;
	}
	case nim::kNIMFriendChangeTypeSyncList:
	{
		nim::FriendProfileSyncEvent sync_event;
		nim::Friend::ParseFriendProfileSyncEvent(change_event, sync_event);
		for (auto& info : sync_event.profiles_)
		{
			std::string accid = info.GetAccId();
			if (info.GetRelationship() == nim::kNIMFriendFlagNormal)
			{
				if (GetUserType(accid) == nim::kNIMFriendFlagNotFriend) //不在friend_list_里面，就添加进去
				{
					add_list.push_back(accid);
					friend_list_.insert(decltype(friend_list_)::value_type(accid, info));
				}
				else //在friend_list_里面，则更新之
				{
					update_list.push_back(accid);
					friend_list_.at(accid).Update(info);
				}
			}
			else
			{
				delete_list.push_back(accid);
				update_list.push_back(accid); // 删除好友之后，其原来的备注名改为其昵称
				friend_list_.erase(accid); // 从friend_list_删除
			}
		}
		break;
	}
	case nim::kNIMFriendChangeTypeUpdate:
	{
		nim::FriendProfileUpdateEvent update_event;
		nim::Friend::ParseFriendProfileUpdateEvent(change_event, update_event);

		std::string accid = update_event.profile_.GetAccId();
		update_list.push_back(accid);
		friend_list_.at(accid).Update(update_event.profile_);

		//QLOG_APP(L"OnFriendListChange id : {0}, ext: {1}") << update_event.profile_.GetAccId() << update_event.profile_.GetEx().toStyledString();

		break;
	}
	default:
		break;
	}

	for each (const auto& id in add_list)
		InvokeFriendListChangeCallback(kChangeTypeAdd, id);

	for each (const auto& id in delete_list)
		InvokeFriendListChangeCallback(kChangeTypeDelete, id);

	if (!update_list.empty())
	{
		std::list<nim::UserNameCard> uinfos;
		GetUserInfos(update_list, uinfos);
		for (auto& it : uinfo_change_cb_list_) //通知上层修改用户的备注名
			(*(it.second))(uinfos);
	}
}

void UserService::OnUserInfoChange(const std::list<nim::UserNameCard> &uinfo_list)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	std::list<nim::UserNameCard> name_photo_list;
	std::list<nim::UserNameCard> misc_uinfo_list;

	for (auto& info : uinfo_list)
	{
		auto iter = all_user_.find(info.GetAccId());
		if (iter != all_user_.end()) //all_user_中存在，就更新
			iter->second.Update(info);
		else if(on_query_list_.find(info.GetAccId()) == on_query_list_.cend())//all_user_中不存在，就获取该用户信息并插入all_user_
			InvokeGetUserInfo(std::list<std::string>(1, info.GetAccId()));

		if (!info.GetIconUrl().empty())
			DownloadUserPhoto(info);

		if (info.ExistValue(nim::kUserNameCardKeyName) || info.ExistValue(nim::kUserNameCardKeyIconUrl)) //用户名或头像变化了
			name_photo_list.push_back(info);
		if (info.ExistValue((nim::UserNameCardValueKey)(nim::kUserNameCardKeyAll - nim::kUserNameCardKeyName - nim::kUserNameCardKeyIconUrl))) //用户其他信息变化了
			misc_uinfo_list.push_back(info);

		//QLOG_APP(L"OnUserInfoChange id : {0}, ext: {1}") << info.GetAccId() << info.GetExpand().toStyledString();
	}

	// 执行回调列表中所有回调
	for (auto& it : uinfo_change_cb_list_) 
		(*(it.second))(name_photo_list);
	for (auto& it : misc_uinfo_change_cb_list_)
		(*(it.second))(misc_uinfo_list);
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

UnregisterCallback nim_comp::UserService::RegMiscUInfoChange(const OnUserInfoChangeCallback & callback)
{
	OnUserInfoChangeCallback* new_callback = new OnUserInfoChangeCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	misc_uinfo_change_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		misc_uinfo_change_cb_list_.erase(cb_id);
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

void UserService::UIFriendListChangeCallback(FriendChangeType change_type, const std::string& accid)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	for (auto& it : friend_list_change_cb_list_)
	{
		(*(it.second))(change_type, accid);
	}
}

void UserService::InvokeFriendListChangeCallback(FriendChangeType change_type, const std::string& accid)
{
	auto task = nbase::Bind(&UserService::UIFriendListChangeCallback, this, change_type, accid);
	nbase::ThreadManager::PostTask(kThreadUI, task);
}

void UserService::DownloadUserPhoto(const nim::UserNameCard &info)
{
	if (info.GetIconUrl().find_first_of("http") != 0) //info.head_image不是正确的url
		return;

	std::wstring photo_path = GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.GetIconUrl()));
	if (info.GetIconUrl().empty() || CheckPhotoOK(photo_path)) // 如果头像已经存在且完好，就不下载
		return;

	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, info, photo_path](nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);

				for (auto &it : photo_ready_cb_list_) // 执行监听头像下载的回调
					(*it.second)(info.GetAccId(), photo_path);
			}
		}
	});
	nim::NOS::DownloadResource(info.GetIconUrl(), cb);
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
	request.AddHeader("User-Agent", "nim_demo_pc");
	request.SetMethodAsPost();
	nim_http::PostRequest(request);
}

void UserService::InvokeGetAllUserInfo(const OnGetUserInfoCallback& cb)
{
	nim::Friend::GetList(ToWeakCallback([this, cb](nim::NIMResCode res_code, const std::list<nim::FriendProfile>& user_profile_list)
	{
		std::list<std::string> account_list;
		for (auto& it : user_profile_list)
		{
			if (it.GetRelationship() == nim::kNIMFriendFlagNormal)
				friend_list_[it.GetAccId()] = it; //插入friend_list_（类的成员变量）好友列表
			
			account_list.push_back(it.GetAccId());
		}
		
		std::list<nim::UserNameCard> uinfos;
		GetUserInfos(account_list, uinfos); // 从db和服务器查询用户信息
		if(cb)
			cb(uinfos);
	}));
}

void UserService::InvokeGetUserInfo(const std::list<std::string>& account_list)
{
	// 先在本地db中找
	nim::User::GetUserNameCardCallback cb1 = ToWeakCallback([this, account_list](const std::list<nim::UserNameCard> &json_result)
	{
		std::set<std::string> not_get_set(account_list.cbegin(), account_list.cend());
		for (auto& card : json_result)
		{
			all_user_[card.GetAccId()] = card; // 插入all_user
			on_query_list_.erase(card.GetAccId()); //已经查到，就从on_query_list_删除
			not_get_set.erase(card.GetAccId());
		}

		OnUserInfoChange(json_result); //触发监听

		if (not_get_set.empty()) // 全部从本地db找到，直接返回
			return;

		// 有些信息本地db没有，再从服务器获取
		nim::User::GetUserNameCardCallback cb2 = ToWeakCallback([this, not_get_set](const std::list<nim::UserNameCard> &json_result)
		{
			auto tmp_set = not_get_set;
			for (auto& card : json_result)
			{
				all_user_[card.GetAccId()] = card; // 插入all_user

				if (card.ExistValue(nim::kUserNameCardKeyIconUrl))
					DownloadUserPhoto(card); // 下载头像

				on_query_list_.erase(card.GetAccId()); //已经查到，就从on_query_list_删除
				tmp_set.erase(card.GetAccId());
			}

			//OnUserInfoChange(json_result); //sdk会自动触发此回调

			for (const auto& id : tmp_set) //从服务器也查不到的用户
			{
				QLOG_APP(L"Can't get user's name card from server. Account id: {0}.") << id;
				on_query_list_.erase(id); //从on_query_list_删除，以免积压
			}
		});
		nim::User::GetUserNameCardOnline(std::list<std::string>(not_get_set.cbegin(), not_get_set.cend()), cb2);
	});
	nim::User::GetUserNameCard(account_list, cb1);

	for (const auto& id : account_list)
		on_query_list_.insert(id);
}

void UserService::InvokeUpdateUserInfo(const nim::UserNameCard &new_info, const OnUpdateUserInfoCallback& cb)
{
	nim::UserNameCard info = new_info;

	auto update_uinfo_cb = ToWeakCallback([this, info, cb](nim::NIMResCode res) {
		if (res == nim::kNIMResSuccess)
		{
			assert(nbase::MessageLoop::current()->ToUIMessageLoop());
			std::list<nim::UserNameCard> lst;
			lst.push_back(info);
			OnUserInfoChange(lst);
		}
		if (cb != nullptr)
			cb(res);
	});
	//Json::Value values;
	//Json::Reader reader;
	//std::string test_string = "{\"remote\":{\"mapmap\":{\"int\":1,\"boolean\":false,\"list\":[1,2,3],\"string\":\"string, lalala\"}}}";
	//if (reader.parse(test_string, values))
	//	info.SetExpand(values);
	nim::User::UpdateMyUserNameCard(info, update_uinfo_cb);
}

void UserService::InvokeChangeUserPhoto(const std::string &url, const OnUpdateUserInfoCallback& cb)
{
	nim::UserNameCard my_info;
	my_info.SetAccId(LoginManager::GetInstance()->GetAccount());
	my_info.SetIconUrl(url);
	InvokeUpdateUserInfo(my_info, cb);
}

const std::map<std::string, nim::UserNameCard>& UserService::GetAllUserInfos()
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	return all_user_;
}

bool UserService::GetUserInfo(const std::string &id, nim::UserNameCard &info)
{
	auto iter = all_user_.find(id);
	if (iter != all_user_.cend())
	{
		info = iter->second;
		return true;
	}
	else
	{
		info.SetName(id);
		info.SetAccId(id);
		if(on_query_list_.find(id) == on_query_list_.cend())
			InvokeGetUserInfo(std::list<std::string>(1, id));
		return false;
	}
}

void UserService::GetUserInfos(const std::list<std::string>& ids, std::list<nim::UserNameCard>& uinfos)
{
	uinfos.clear();
	std::list<std::string> not_get_list;

	for (const auto &id : ids)
	{
		auto iter = all_user_.find(id);
		if (iter != all_user_.cend())
			uinfos.push_back(iter->second);
		else
		{
			nim::UserNameCard info(id);
			info.SetName(id);
			uinfos.push_back(info);

			if(on_query_list_.find(id) == on_query_list_.end()) //不在all_user_里面，也不在查询途中
				not_get_list.push_back(id);
		}
	}

	if (!not_get_list.empty())
		InvokeGetUserInfo(not_get_list);
}

nim::NIMFriendFlag UserService::GetUserType(const std::string &id)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	return (friend_list_.find(id) != friend_list_.end() ? nim::kNIMFriendFlagNormal : nim::kNIMFriendFlagNotFriend);
}

std::wstring UserService::GetUserName(const std::string &id, bool alias_prior/* = true */)
{
	if(alias_prior && GetUserType(id) == nim::kNIMFriendFlagNormal && !friend_list_.at(id).GetAlias().empty()) //优先使用备注名
		return nbase::UTF8ToUTF16(friend_list_.at(id).GetAlias());

	nim::UserNameCard info;
	GetUserInfo(id, info);
	return nbase::UTF8ToUTF16(info.GetName());
}

std::wstring UserService::GetFriendAlias(const std::string & id)
{
	auto iter = friend_list_.find(id);
	if (iter == friend_list_.cend())
		return L"";
	return nbase::UTF8ToUTF16(iter->second.GetAlias());
}

std::wstring UserService::GetUserPhoto(const std::string &accid)
{
	std::wstring default_photo = QPath::GetAppPath() + L"res\\faces\\default\\default.png";
	if (!nbase::FilePathIsExist(default_photo, false))
		default_photo = L"";

	nim::UserNameCard info;
	GetUserInfo(accid, info);
	if (!info.ExistValue(nim::kUserNameCardKeyIconUrl) || info.GetIconUrl().empty())
		return default_photo;

	// 检查图片是否存在
	std::wstring photo_path = GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.GetIconUrl()));
	if (!nbase::FilePathIsExist(photo_path, false))
		return default_photo;

	if (!CheckPhotoOK(photo_path))
		return default_photo;

	return GetUserPhotoDir() + nbase::UTF8ToUTF16(QString::GetMd5(info.GetIconUrl()));
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

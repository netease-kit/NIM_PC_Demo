#include "chatroom_form.h"

using namespace ui;
using namespace nim_comp;

namespace nim_chatroom
{
void ChatroomForm::OnLoadEnd(int httpStatusCode)
{

}

void ChatroomForm::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
#if 0
	nbase::ThreadManager::PostRepeatedTask(kThreadGlobalMisc, ToWeakCallback([this]() {
		std::vector<std::wstring> bq = { L"[心碎]", L"[大笑]", L"[可爱]", L"[鬼脸]", L"[口水]", L"[害羞]" };
		std::wstring text(L"Hello");
		text.append(bq[rand() % bq.size()]).append(L"Hello");
		AddText(text, L"jiajia01", "jiajia01", kMember, false);
		//AddJsb(1, L"jiajia01", "jiajia01", false);
		//AddNotify(L"Hello", false);
	}), nbase::TimeDelta::FromMilliseconds(10));
#endif

	msg_list_->RegisterCppFunc(L"getHistory", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) { GetHistorys(); }));
	msg_list_->RegisterCppFunc(L"getMemberInfo", nbase::Bind(&ChatroomForm::GetMemberInfo, this, std::placeholders::_1, std::placeholders::_2));
	msg_list_->RegisterCppFunc(L"kickMember", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) { 
		Json::Value notify_ext;
		notify_ext["operator_id"] = my_info_.account_id_;
		notify_ext["operator_nick"] = my_info_.nick_;
		ChatRoom::KickMemberAsync(room_id_, clicked_user_account_, Json::FastWriter().write(notify_ext), nbase::Bind(&ChatroomForm::OnKickMemberCallback, this, std::placeholders::_1, std::placeholders::_2));
		kicked_user_account_ = clicked_user_account_;

		clicked_user_account_.clear();
	}));

	msg_list_->RegisterCppFunc(L"setMemberAdmin", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) {
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(params, values) && values.isMember("setAdmin"))
		{
			ChatRoomSetMemberAttributeParameters param;
			param.account_id_ = clicked_user_account_;
			param.opt_ = values["setAdmin"].asBool();
			param.attribute_ = kNIMChatRoomMemberAttributeAdminister;
			ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			clicked_user_account_.clear();
		}
	}));

	msg_list_->RegisterCppFunc(L"setMemberBlack", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) {
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(params, values) && values.isMember("setBlack"))
		{
			ChatRoomSetMemberAttributeParameters param;
			param.account_id_ = clicked_user_account_;
			param.opt_ = values["setBlack"].asBool();
			param.attribute_ = kNIMChatRoomMemberAttributeBlackList;
			ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			clicked_user_account_.clear();
		}
	}));

	msg_list_->RegisterCppFunc(L"setMemberMute", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) {
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(params, values) && values.isMember("setMute"))
		{
			ChatRoomSetMemberAttributeParameters param;
			param.account_id_ = clicked_user_account_;
			param.attribute_ = kNIMChatRoomMemberAttributeMuteList;
			param.opt_ = values["setMute"].asBool();

			ChatRoom::SetMemberAttributeOnlineAsync(room_id_, param, nbase::Bind(&ChatroomForm::OnSetMemberAttributeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			clicked_user_account_.clear();
		}
	}));

	msg_list_->RegisterCppFunc(L"setMemberTempMute", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) {
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(params, values) && values.isMember("setTempMute"))
		{
			bool mute = values["setTempMute"].asBool();
			ChatRoom::TempMuteMemberAsync(room_id_, clicked_user_account_, values["setTempMute"].asBool() ? 60 : 0, true, "", nbase::Bind(&ChatroomForm::OnTempMuteCallback, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		}
	}));

	msg_list_->RegisterCppFunc(L"replyRobot", ToWeakCallback([this](const std::string& params, nim_cef::ReportResultFunction callback) {
		Json::Value values;
		Json::Reader reader;

		if (reader.parse(params, values))
		{
			std::string type = values["type"].asString();
			if (type == "url")
			{
				std::string url = values["target"].asString();
				Post2GlobalMisc(nbase::Bind(&shared::tools::SafeOpenUrl, nbase::UTF8ToUTF16(url), SW_SHOW));
			}
			else if (type == "block")
			{
				std::string robot_id = values["account"].asString();
				std::string content = values["linkText"].asString();
				nim::IMBotRobot bot;
				bot.robot_accid_ = robot_id;
				bot.sent_param_["target"] = values["target"].asString();
				bot.sent_param_["type"] = "03";
				bot.sent_param_["params"] = values["params"].asString();
				std::string json_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeRobot, QString::GetGUID(), bot.ToJsonString(), content, ChatRoomMessageSetting());
				ChatRoom::SendMsg(room_id_, json_msg);
				std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
				std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
				AddText(nbase::UTF8ToUTF16(content), my_name, my_id, kMember, false);
			}
		}
	}));
}

void ChatroomForm::OnBeforeContextMenu(CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear();
}

void ChatroomForm::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{

}

void ChatroomForm::AddText(const std::wstring &text, const std::wstring &sender_name, const std::string &sender_id, SenderType sender_type, bool is_history, bool first_msg_each_batch/* = false*/)
{
	if (!text.empty())
	{
		auto account = nim_comp::LoginManager::GetInstance()->GetAccount();

		Json::Value values;
		values["content"] = nbase::UTF16ToUTF8(text);
		values["nickname"] = nbase::UTF16ToUTF8(sender_name);
		values["account"] = sender_id;
		values["type"] = sender_type;
		values["isHistory"] = is_history;
		values["self"] = account == sender_id;

		msg_list_->CallJSFunction(L"addMsg", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
		input_edit_->SetFocus();
	}
}

void ChatroomForm::AddNotify(const std::wstring &notify, bool is_history, bool first_msg_each_batch/* = false*/)
{
	if (!notify.empty())
	{
		Json::Value values;
		values["content"] = nbase::UTF16ToUTF8(notify);
		values["isHistory"] = is_history;
		msg_list_->CallJSFunction(L"addNotify", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
	}
}

void ChatroomForm::AddJsb(const int value, const std::wstring &sender_name, const std::string& sender_id, bool is_history, bool first_msg_each_batch /*= false*/)
{
	std::wstring file_name;
	if (value == 1)
		file_name = L"jsb_s.png";
	else if (value == 2)
		file_name = L"jsb_j.png";
	else if (value == 3)
		file_name = L"jsb_b.png";

	if (file_name.empty()) return;

	auto account = nim_comp::LoginManager::GetInstance()->GetAccount();

	Json::Value values;
	values["content"] = nbase::UTF16ToUTF8(file_name);
	values["nickname"] = nbase::UTF16ToUTF8(sender_name);
	values["account"] = sender_id;
	values["type"] = kJsb;
	values["isHistory"] = is_history;
	values["self"] = account == sender_id;

	msg_list_->CallJSFunction(L"addMsg", nbase::UTF8ToUTF16(values.toStyledString()), nullptr);
}

void ChatroomForm::GetMemberInfo(const std::string& params, nim_cef::ReportResultFunction callback)
{
	Json::Value values;
	Json::Reader reader;
	if (!reader.parse(params, values))
	{
		callback(true, R"({ "message": "Failed to parse params."})");
		return;
	}

	std::string my_id = nim_comp::LoginManager::GetInstance()->GetAccount();
	std::string account = values["account"].asString();
	bool my_room = my_id == creater_id_;
	Json::Value reponse;

	ChatRoomMemberInfo member_info;
	auto it_member = members_map_.find(account);
	if (it_member != members_map_.end())
		member_info = it_member->second;

	// 是否是自己
	reponse["self"] = account == my_id;

	// 只有创建者和管理员可以操作
	reponse["hasPermission"] = my_room || std::find(managers_list_.begin(), managers_list_.end(), my_id) != managers_list_.end();

	// 如果目标用户是创建者，不能操作，直接返回
	reponse["isCreator"] = account == creater_id_;

	// 如果目标用户和自己都是管理员，不能操作，直接返回
	reponse["isAdmin"] = (!my_room) && member_info.type_ == 2;

	// 用户信息
	reponse["isMuted"] = member_info.is_muted_;
	reponse["isTempMuted"] = member_info.temp_muted_;
	reponse["inBlackList"] = member_info.is_blacklist_;
	reponse["memberType"] = member_info.type_;

	QLOG_APP(L"member info: {0}") << nbase::UTF8ToUTF16(reponse.toStyledString());

	clicked_user_account_ = account;

	callback(false, reponse.toStyledString());
}

void ChatroomForm::SetMemberAdmin(const std::string &id, bool is_admin)
{
	if (id == creater_id_)
		return;

	if (id == nim_ui::LoginManager::GetInstance()->GetAccount())
		bulletin_->SetReadOnly(!is_admin);

	auto info = members_map_.find(id);
	if (info == members_map_.end())
		return;

	auto iter = std::find(managers_list_.begin(), managers_list_.end(), id);
	if (is_admin)
	{
		if (iter != managers_list_.end())
			return;

		managers_list_.push_back(id);

		auto iter_member = std::find(members_list_.begin(), members_list_.end(), id);
		if (iter_member != members_list_.end())
			members_list_.erase(iter_member);

		info->second.type_ = 2;
		members_map_[id] = info->second;
	}
	else
	{
		if (iter == managers_list_.end())
			return;

		members_list_.push_back(id);
		managers_list_.erase(iter);

		info->second.type_ = 0;
		members_map_[id] = info->second;
	}

	// 单击了在线成员列表后会重新刷新成员，所以只有切换到在线成员列表页时，才操作UI
	if (option_online_members_->IsSelected())
	{
		if (NULL != online_members_virtual_list_->FindSubControl(nbase::UTF8ToUTF16(id)))
			online_members_virtual_list_->Refresh();
	}
}

void ChatroomForm::SetMemberBlacklist(const std::string &id, bool is_black)
{
	if (id == creater_id_)
		return;

	auto info = members_map_.find(id);
	if (info != members_map_.end())
	{
		info->second.is_blacklist_ = is_black;
		members_map_[id] = info->second;
	}
}

void ChatroomForm::SetMemberMute(const std::string &id, bool is_mute)
{
	if (id == creater_id_)
		return;

	auto info = members_map_.find(id);
	if (info != members_map_.end())
	{
		info->second.is_muted_ = is_mute;
		members_map_[id] = info->second;
	}
}

void ChatroomForm::SetMemberFixed(const std::string &id, bool is_fixed)
{
	if (id == creater_id_)
		return;

	auto info = members_map_.find(id);
	if (info != members_map_.end())
	{
		info->second.guest_flag_ = is_fixed ? kNIMChatRoomGuestFlagNoGuest : kNIMChatRoomGuestFlagGuest;
		members_map_[id] = info->second;
	}
}

void ChatroomForm::SetMemberTempMute(const std::string &id, bool temp_mute, __int64 duration)
{
	auto info = members_map_.find(id);
	if (info != members_map_.end())
	{
		info->second.temp_muted_ = temp_mute;
		info->second.temp_muted_duration_ = temp_mute ? duration : 0;
		members_map_[id] = info->second;
		auto iter = temp_unmute_id_task_map_.find(id);
		if (iter != temp_unmute_id_task_map_.end())
		{
			(*iter).second.Cancel();
			temp_unmute_id_task_map_.erase(iter);
		}
		if (temp_mute)
		{
			StdClosure task = nbase::Bind(&ChatroomForm::SetMemberTempMute, this, id, false, 0);
			nbase::WeakCallbackFlag weak_flag;
			task = weak_flag.ToWeakCallback(task);
			temp_unmute_id_task_map_[id] = weak_flag;
			nbase::ThreadManager::PostDelayedTask(task, nbase::TimeDelta::FromSeconds(duration));
		}
	}
}

void ChatroomForm::SetRoomMemberMute(bool mute)
{
	if (room_mute_ == mute)
		return;

	for (auto& it : members_map_)
	{
		if (it.second.type_ < 1)
			SetMemberMute(it.first, mute);
	}
}

void ChatroomForm::RemoveMember(const std::string &uid)
{
	// 单击了在线成员列表后会重新刷新成员，所以只有切换到在线成员列表页时，才操作UI
	auto exit_member = members_map_.find(uid);
	if (exit_member != members_map_.end())
	{
		if (!exit_member->second.is_blacklist_ &&
			(exit_member->second.type_ == 0 || exit_member->second.type_ == 4))
		{
			members_map_.erase(exit_member);

			if (option_online_members_->IsSelected())
			{
				auto iter_member = std::find(members_list_.begin(), members_list_.end(), uid);
				if (iter_member != members_list_.end())
					members_list_.erase(iter_member);

				online_members_virtual_list_->Refresh();
			}
		}
	}
}

} // namespace nim_comp

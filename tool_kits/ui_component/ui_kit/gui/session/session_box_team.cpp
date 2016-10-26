#include "session_form.h"
#include "session_form.h"
#include "module/session/session_manager.h"

using namespace ui;

namespace nim_comp
{

void SessionBox::InvokeGetTeamInfo(bool sync_block/* = false*/)
{
	if (sync_block)
	{
		//TODO(oleg) 测试用，不建议使用同步堵塞接口
		nim::TeamInfo info = nim::Team::QueryTeamInfoBlock(session_id_);
		OnGetTeamInfoCallback(session_id_, info);
	}
	else
		nim::Team::QueryTeamInfoAsync(session_id_, nbase::Bind(&SessionBox::OnGetTeamInfoCallback, this, std::placeholders::_1, std::placeholders::_2));
}

void SessionBox::OnGetTeamInfoCallback(const std::string& tid, const nim::TeamInfo& result)
{
	team_info_ = result;

	std::wstring wname = nbase::UTF8ToUTF16(team_info_.GetName());
	if (!wname.empty())
	{
		SetTitleName(wname);
	}

	if (!team_info_.IsMemberValid())
	{
		HandleLeaveTeamEvent();
	}
	else if (!team_info_.IsValid())
	{
		HandleDismissTeamEvent();
	}
	else
	{
		HandleEnterTeamEvent();
	}

	if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		session_form_->AdjustFormSize();
		Json::Value json;
		if (StringToJson(team_info_.GetAnnouncement(), json))
		{
			UpdateBroad(json);
		}
		else
		{
			edit_broad_->SetUTF8Text(team_info_.GetAnnouncement());
		}
		CheckTeamType(nim::kNIMTeamTypeAdvanced);

		InvokeGetTeamMember();
	}

	if (team_info_.ExistValue(nim::kNIMTeamInfoKeyMuteAll))
	{
		bool new_mute_all_status = team_info_.IsAllMemberMute();
		mute_all_ = new_mute_all_status;
	}

	is_header_enable_ = true;
}

void SessionBox::InvokeGetTeamMember()
{
	btn_refresh_member_->SetEnabled(false);
	nim::Team::QueryTeamMembersAsync(session_id_, nbase::Bind(&SessionBox::OnGetTeamMemberCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void SessionBox::OnGetTeamMemberCallback(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list)
{
	team_member_info_list_.clear();
	std::list<std::string> acc_list;
	for (auto& it : team_member_info_list)
	{
		if (IsTeamMemberType(it.GetUserType()))
		{
			team_member_info_list_[it.GetAccountID()] = it;
			acc_list.push_back(it.GetAccountID());
		}
	}

	btn_refresh_member_->SetEnabled(true);
	member_list_->RemoveAll();
	label_member_->SetText(nbase::StringPrintf(L"群成员（共%d人）", team_member_info_list_.size()));

	std::list<nim::UserNameCard> uinfos;
	UserService::GetInstance()->GetUserInfos(acc_list, uinfos); //查询群成员的用户信息

	bool set_new_broad_visible = team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateCustomModeEveryone;
	btn_new_broad_->SetVisible(set_new_broad_visible);
	for (const auto &tm_info : team_member_info_list_)
	{
		std::string uid = tm_info.second.GetAccountID();
		if (member_list_->FindSubControl(nbase::UTF8ToUTF16(uid)) == NULL)
		{
			TeamItem* item = new TeamItem;
			GlobalManager::FillBoxWithCache(item, L"session/team_item.xml");
			auto user_type = tm_info.second.GetUserType();
			if (user_type == nim::kNIMTeamUserTypeCreator)
				member_list_->AddAt(item, 0);
			else
				member_list_->Add(item);

			item->InitControl();
			item->InitInfo(tm_info.second);
			if (mute_all_ && user_type != nim::kNIMTeamUserTypeCreator && user_type != nim::kNIMTeamUserTypeManager)
				SetTeamMemberMute(tid, uid, tm_info.second.IsMute(), mute_all_);
			else
				SetTeamMemberMute(tid, uid, tm_info.second.IsMute(), false);
		}
		else
		{
			QLOG_WAR(L"OnGetTeamMemberCb found the duplicate id, id={0}") << nbase::UTF8ToUTF16(uid).c_str();
		}

		RefreshMsglistShowname(uid); //刷新消息列表中显示的名字

		if (!set_new_broad_visible)
		{
			if (LoginManager::GetInstance()->IsEqual(tm_info.second.GetAccountID()))
			{
				if (tm_info.second.GetUserType() == nim::kNIMTeamUserTypeCreator || tm_info.second.GetUserType() == nim::kNIMTeamUserTypeManager)
					btn_new_broad_->SetVisible(true);
				set_new_broad_visible = true;
			}
		}

		if (LoginManager::GetInstance()->IsEqual(tm_info.second.GetAccountID()))
		{
			auto user_type = tm_info.second.GetUserType();
			if (mute_all_ && user_type != nim::kNIMTeamUserTypeCreator && user_type != nim::kNIMTeamUserTypeManager)
				SetTeamMuteUI(true);
			else
				SetTeamMuteUI(tm_info.second.IsMute());
		}
	}
}

nim::TeamMemberProperty SessionBox::GetTeamMemberInfo(const std::string& uid)
{
	auto iter = team_member_info_list_.find(uid);
	if (iter != team_member_info_list_.cend())
		return iter->second;
	else
		return nim::TeamMemberProperty();
}

void SessionBox::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
{
	if (!IsTeamMemberType(team_member_info.GetUserType()))
		return;

	if(tid == session_id_)
	{
		team_member_info_list_[team_member_info.GetAccountID()] = team_member_info;
		std::wstring wid = nbase::UTF8ToUTF16(team_member_info.GetAccountID());

		Control* ctrl = member_list_->FindSubControl(wid);
		if (ctrl)
		{
			QLOG_WAR(L"OnTeamMemberAdd found the duplicate id, id={0}") << wid.c_str();
			member_list_->Remove(ctrl);
		}

		TeamItem* item = new TeamItem;
		GlobalManager::FillBoxWithCache(item, L"session/team_item.xml");
		auto user_type = team_member_info.GetUserType();
		if (user_type == nim::kNIMTeamUserTypeCreator)
			member_list_->AddAt(item, 0);
		else
			member_list_->Add(item);

		item->InitControl();
		item->InitInfo(team_member_info);
		std::string uid = team_member_info.GetAccountID();
		if (mute_all_ && user_type != nim::kNIMTeamUserTypeCreator && user_type != nim::kNIMTeamUserTypeManager)
			SetTeamMemberMute(tid, uid, team_member_info.IsMute(), mute_all_);
		else
			SetTeamMemberMute(tid, uid, team_member_info.IsMute(), false);

		std::wstring str = nbase::StringPrintf(L"群成员（共%d人）", member_list_->GetCount());
		label_member_->SetText(str);

		if (LoginManager::GetInstance()->IsEqual(team_member_info.GetAccountID()))
		{
			btn_header_->SetEnabled(true);
		}
	}
}

void SessionBox::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
{
	if(tid == session_id_)
	{
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		Control* ctrl = member_list_->FindSubControl(wid);
		if(ctrl)
		{
			member_list_->Remove(ctrl);
		}

		std::wstring str = nbase::StringPrintf(L"群成员（共%d人）", member_list_->GetCount());
		label_member_->SetText(str);

		team_member_info_list_.erase(uid);

		RefreshMsglistShowname(uid);
	}
}

void SessionBox::OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card)
{
	size_t splitter = tid_uid.find_first_of('#');
	std::string tid = tid_uid.substr(0, splitter), uid = tid_uid.substr(splitter + 1);
	if(tid == session_id_)
	{
		auto it = team_member_info_list_.find(uid);
		if (it != team_member_info_list_.end())
		{
			it->second.SetNick(team_card);
		}
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		TeamItem* ctrl = (TeamItem*)(member_list_->FindSubControl(wid));
		if(ctrl)
		{
			ctrl->SetMemberName(team_card);
		}
	}
}

void SessionBox::OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin)
{
	if(tid == session_id_)
	{
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
		if(item)
		{
			item->SetAdmin(admin);
		}

		if (team_info_.GetUpdateInfoMode() != nim::kNIMTeamUpdateCustomModeEveryone && LoginManager::GetInstance()->IsEqual(uid))
		{
			btn_new_broad_->SetVisible(admin);
		}
	}
}

void SessionBox::OnTeamMuteMember(const std::string& tid, const std::string& uid, bool set_mute)
{
	if (tid == session_id_)
	{
		auto member_info = GetTeamMemberInfo(uid);
		auto user_type = member_info.GetUserType();
		if (mute_all_ && user_type != nim::kNIMTeamUserTypeCreator && user_type != nim::kNIMTeamUserTypeManager)
			SetTeamMemberMute(tid, uid, set_mute, mute_all_);
		else
			SetTeamMemberMute(tid, uid, set_mute, false);
	}
}

void SessionBox::OnTeamOwnerChange(const std::string& tid, const std::string& uid)
{
	if (tid == session_id_)
	{
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		for (int i = 0; i < member_list_->GetCount(); i++)
		{
			TeamItem* item = dynamic_cast<TeamItem*>(member_list_->GetItemAt(i));
			if (item->GetTeamUserType() == nim::kNIMTeamUserTypeCreator)
			{
				item->SetOwner(false);

				if (team_info_.GetUpdateInfoMode() == nim::kNIMTeamUpdateCustomModeEveryone)
					break;

				if(LoginManager::GetInstance()->IsEqual(item->GetUTF8Name()))
					btn_new_broad_->SetVisible(false);

				break;
			}
		}

		TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
		if (item)
			item->SetOwner(true);
		if (LoginManager::GetInstance()->IsEqual(nbase::UTF16ToUTF8(wid)))
			btn_new_broad_->SetVisible(true);
	}
}

void SessionBox::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (session_id_ == team_info.GetTeamID())
	{
		std::wstring name = nbase::UTF8ToUTF16(team_info.GetName());

		SetTitleName(name);
	}
}

void SessionBox::OnTeamRemove(const std::string& tid)
{
	if (tid == session_id_)
	{
		HandleLeaveTeamEvent();
	}
}

bool SessionBox::IsTeamMemberType(const nim::NIMTeamUserType user_type)
{
	if (user_type == nim::kNIMTeamUserTypeNomal || 
		user_type == nim::kNIMTeamUserTypeManager || 
		user_type == nim::kNIMTeamUserTypeCreator)
		return true;

	return false;
}

void SessionBox::SendReceiptIfNeeded(bool auto_detect/* = false*/)
{
	//发送已读回执目前只支持P2P会话
	if (session_type_ != nim::kNIMSessionTypeP2P)
		return;

	if (auto_detect)
	{
		if (!session_form_->IsActiveSessionBox(this) || msg_list_ == nullptr || !msg_list_->IsAtEnd())
		{
			receipt_need_send_ = true;
			return;
		}
	}

	nim::IMMessage msg;
	if (GetLastNeedSendReceiptMsg(msg))
	{
		receipt_need_send_ = false;
		nim::MsgLog::SendReceiptAsync(msg.ToJsonString(false), [](const nim::MessageStatusChangedResult& res) {
			auto iter = res.results_.begin();
			QLOG_APP(L"mark receipt id:{0} time:{1} rescode:{2}") << iter->talk_id_ << iter->msg_timetag_ << res.rescode_;
		});
	}
}

void SessionBox::RefreshMsglistShowname(const std::string& uid)
{
	if (session_type_ != nim::kNIMSessionTypeTeam)
		return;

	nim::TeamMemberProperty tm_info = GetTeamMemberInfo(uid);
	std::string show_name = tm_info.GetNick();
	if (show_name.empty())
		show_name = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(uid));

	int msg_count = msg_list_->GetCount();
	for (int i = 0; i < msg_count; i++)
	{
		MsgBubbleItem* bubble_item = dynamic_cast<MsgBubbleItem*>(msg_list_->GetItemAt(i));
		if (bubble_item && bubble_item->sender_name_->IsVisible() && bubble_item->msg_.sender_accid_ == uid)
			bubble_item->SetShowName(true, show_name);

		if (!bubble_item)
		{
			MsgBubbleNotice* notice_item = dynamic_cast<MsgBubbleNotice*>(msg_list_->GetItemAt(i));
			if (notice_item)
				notice_item->RefreshNotice();
		}
	}
}

void SessionBox::SetTeamMemberMute(const std::string& tid, const std::string& uid, bool set_mute, bool team_mute)
{
	std::wstring wid = nbase::UTF8ToUTF16(uid);

	TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
	if (item)
		item->SetMute(set_mute, team_mute);

//	if (LoginManager::GetInstance()->IsEqual(uid))
//		SetTeamMuteUI(set_mute);

}

void SessionBox::SetTeamMuteUI(bool mute)
{
	ControlStateType type = mute ? kControlStateDisabled : kControlStateNormal;
	input_edit_->SetEnabled(!mute);
	input_edit_->SetReadOnly(mute);
	btn_capture_audio_->SetEnabled(!mute);
	//btn_send_->SetEnabled(!mute);
	FindSubControl(L"btn_face")->SetEnabled(!mute);
	FindSubControl(L"btn_image")->SetEnabled(!mute);
	FindSubControl(L"btn_file")->SetEnabled(!mute);
	FindSubControl(L"btn_jsb")->SetEnabled(!mute);
	FindSubControl(L"btn_tip")->SetEnabled(!mute);
	FindSubControl(L"btn_clip")->SetEnabled(!mute);
	FindSubControl(L"btn_custom_msg")->SetEnabled(!mute);
	FindSubControl(L"input_edit_mute_tip")->SetVisible(mute);
	FindSubControl(L"hbox_send")->SetEnabled(!mute);
}

void SessionBox::UpdateBroad(const Json::Value &broad)
{
	std::string output;
	for (size_t i = 0; i < broad.size(); i++)
	{
		std::string title = broad[i]["title"].asString();
		std::string content = broad[i]["content"].asString();
		if (!output.empty())
			output.append("\r\n");
		output.append(title + " : " + content);
	}
	edit_broad_->SetUTF8Text(output);
}

void SessionBox::CheckTeamType(nim::NIMTeamType type)
{
	bool show = (type == nim::kNIMTeamTypeAdvanced);

	Control* split = FindSubControl(L"frame_mid_split");
	split->SetVisible(show);
	Control* frame_right = FindSubControl(L"frame_right");
	frame_right->SetVisible(show);
}

bool SessionBox::IsAdvancedTeam()
{
	return (session_type_ == nim::kNIMSessionTypeTeam && team_info_.GetType() == nim::kNIMTeamTypeAdvanced);
}


void SessionBox::HandleEnterTeamEvent()
{
	RemoveTip(STT_LEAVE);
	is_team_valid_ = true;

	is_header_enable_ = true;
	input_edit_->SetEnabled(true);
	btn_send_->SetEnabled(true);
	FindSubControl(L"btn_custom_msg")->SetEnabled(true);
	FindSubControl(L"btn_msg_record")->SetEnabled(true);
}

void SessionBox::HandleLeaveTeamEvent()
{
	AddTip(STT_LEAVE);
	is_team_valid_ = false;

	is_header_enable_ = false;
	btn_invite_->SetEnabled(false);
	input_edit_->SetReadOnly(true);
	FindSubControl(L"bottom_panel")->SetEnabled(false);
	btn_new_broad_->SetEnabled(false);
	btn_refresh_member_->SetEnabled(false);
}

void SessionBox::HandleDismissTeamEvent()
{
	AddTip(STT_DISMISS);
	is_team_valid_ = false;

	is_header_enable_ = false;
	btn_invite_->SetEnabled(false);
	input_edit_->SetReadOnly(true);
	FindSubControl(L"bottom_panel")->SetEnabled(false);
	btn_new_broad_->SetEnabled(false);
	btn_refresh_member_->SetEnabled(false);
}

}
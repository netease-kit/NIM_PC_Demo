#include "session_form.h"
#include "module/session/session_manager.h"

using namespace ui;

namespace nim_comp
{
void SessionForm::UpdateBroad(const Json::Value &broad)
{
	std::string output;
	for(size_t i = 0; i < broad.size(); i++)
	{
		std::string title = broad[i]["title"].asString();
		std::string content = broad[i]["content"].asString();
		if(!output.empty())
			output.append("\r\n");
		output.append(title + " : " + content);
	}
	re_broad_->SetUTF8Text(output);
}

void SessionForm::CheckTeamType(nim::NIMTeamType type)
{
	bool show = (type == nim::kNIMTeamTypeAdvanced);

	Control* split = FindControl(L"frame_mid_split");
	split->SetVisible(show);
	Control* frame_right = FindControl(L"frame_right");
	frame_right->SetVisible(show);
}

void SessionForm::InvokeGetTeamInfo()
{
	nim::Team::QueryTeamInfoAsync(session_id_, nbase::Bind(&SessionForm::OnGetTeamInfoCb, this, std::placeholders::_1, std::placeholders::_2));
}

void SessionForm::OnGetTeamInfoCb(const std::string& tid, const nim::TeamInfo& result)
{
	team_info_ = result;

	std::wstring wname = nbase::UTF8ToUTF16(team_info_.GetName());
	if (!wname.empty())
	{
		SetTaskbarTitle(wname);
		label_title_->SetText(wname);
	}

	bool valid = team_info_.IsValid();
	if (!valid)
	{
		LeaveTeamHandle();
		return;
	}
	else
	{
		EnterTeamHandle();
	}

	if (team_info_.GetType() == nim::kNIMTeamTypeAdvanced)
	{
		AdjustSizeForAdvancedTeam();
		Json::Value json;
		if (StringToJson(team_info_.GetAnnouncement(), json))
		{
			UpdateBroad(json);
		}
		else
		{
			re_broad_->SetUTF8Text(team_info_.GetAnnouncement());
		}
		CheckTeamType(nim::kNIMTeamTypeAdvanced);
		//member
		InvokeGetTeamMember();
	}

	btn_header_->SetEnabled(true);
}


void SessionForm::InvokeGetTeamMember()
{
	btn_refresh_member_->SetEnabled(false);
	nim::Team::QueryTeamMembersAsync(session_id_, nbase::Bind(&SessionForm::OnGetTeamMemberCb, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void SessionForm::OnGetTeamMemberCb(const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list)
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

	for (const auto &tm_info : team_member_info_list_)
	{
		std::string uid = tm_info.second.GetAccountID();
		if (member_list_->FindSubControl(nbase::UTF8ToUTF16(uid)) == NULL)
		{
			TeamItem* item = new TeamItem;
			GlobalManager::FillBoxWithCache(item, L"session/team_item.xml");
			if (tm_info.second.GetUserType() == nim::kNIMTeamUserTypeCreator)
				member_list_->AddAt(item, 0);
			else
				member_list_->Add(item);

			item->InitControl();
			item->InitInfo(tm_info.second);
		}
		else
		{
			QLOG_WAR(L"OnGetTeamMemberCb found the duplicate id, id={0}") << nbase::UTF8ToUTF16(uid).c_str();
		}

		RefreshMsglistShowname(uid); //刷新消息列表中显示的名字

		if (tm_info.second.GetUserType() == nim::kNIMTeamUserTypeCreator || tm_info.second.GetUserType() == nim::kNIMTeamUserTypeManager)
		{
			if (LoginManager::GetInstance()->IsEqual(tm_info.second.GetAccountID()))
			{
				btn_new_broad_->SetVisible(true);
			}
		}
	}
}

void SessionForm::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberProperty& team_member_info)
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
		member_list_->Add(item);

		item->InitControl();
		item->InitInfo(team_member_info);

		std::wstring str = nbase::StringPrintf(L"群成员（共%d人）", member_list_->GetCount());
		label_member_->SetText(str);

		if (LoginManager::GetInstance()->IsEqual(team_member_info.GetAccountID()))
		{
			btn_header_->SetEnabled(true);
		}
	}
}

void SessionForm::OnTeamMemberRemove(const std::string& tid, const std::string& uid)
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

void SessionForm::OnTeamMemberChange(const std::string& tid_uid, const std::string& team_card)
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

void SessionForm::OnTeamAdminSet(const std::string& tid, const std::string& uid, bool admin)
{
	if(tid == session_id_)
	{
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
		if(item)
		{
			item->SetAdmin(admin);
		}

		if (LoginManager::GetInstance()->IsEqual(uid))
		{
			btn_new_broad_->SetVisible(admin);
		}
	}
}

void SessionForm::OnTeamOwnerChange(const std::string& tid, const std::string& uid)
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
				if(LoginManager::GetInstance()->IsEqual(item->GetUTF8Name()))
					btn_new_broad_->SetVisible(false);

				break;
			}
		}

		TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
		item->SetOwner(true);
		if (LoginManager::GetInstance()->IsEqual(nbase::UTF16ToUTF8(wid)))
			btn_new_broad_->SetVisible(true);
	}
}

void SessionForm::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (session_id_ == team_info.GetTeamID())
	{
		std::wstring name = nbase::UTF8ToUTF16(team_info.GetName());

		SetTaskbarTitle(name);
		label_title_->SetText(name);
	}
}

void SessionForm::OnTeamRemove(const std::string& tid)
{
	if (tid == session_id_)
	{
		LeaveTeamHandle();
	}
}

bool SessionForm::IsTeamMemberType(const nim::NIMTeamUserType user_type)
{
	if (user_type == nim::kNIMTeamUserTypeNomal || 
		user_type == nim::kNIMTeamUserTypeManager || 
		user_type == nim::kNIMTeamUserTypeCreator)
		return true;

	return false;
}

void SessionForm::RefreshMsglistShowname(const std::string& uid)
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

}
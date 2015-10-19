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

	std::wstring wname = nbase::UTF8ToUTF16(team_info_.name);
	if (!wname.empty())
	{
		SetTaskbarTitle(wname);
		label_title_->SetText(wname);
	}

	int valid = team_info_.valid;
	if (valid == 0)
	{
		LeaveTeamHandle();
		btn_header_->SetEnabled(false);
		input_edit_->SetEnabled(false);
		btn_send_->SetEnabled(false);
		FindControl(L"btn_custom_msg")->SetEnabled(false);
		FindControl(L"btn_msg_record")->SetEnabled(false);
		btn_new_broad_->SetVisible(false);
		return;
	}
	else
	{
		EnterTeamHandle();
		btn_header_->SetEnabled(true);
		input_edit_->SetEnabled(true);
		btn_send_->SetEnabled(true);
		FindControl(L"btn_custom_msg")->SetEnabled(true);
		FindControl(L"btn_msg_record")->SetEnabled(true);
	}

	if (team_info_.type == nim::kNIMTeamTypeAdvanced)
	{
		AdjustSizeForAdvancedTeam();
		Json::Value json;
		if (StringToJson(team_info_.announcement, json))
		{
			UpdateBroad(json);
		}
		else
		{
			re_broad_->SetUTF8Text(team_info_.announcement);
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

void SessionForm::OnGetTeamMemberCb(const std::string& tid, int count, const std::list<nim::TeamMemberInfo>& team_member_info_list)
{
	std::list<std::string> acc_list;
	for (auto& it : team_member_info_list)
	{
		team_member_info_list_[it.account] = it;
		acc_list.push_back(it.account);
	}

	btn_refresh_member_->SetEnabled(true);
	member_list_->RemoveAll();
	label_member_->SetText(nbase::StringPrintf(L"群成员（共%d人）", team_member_info_list.size()));

	OnGetUserInfoCallback cb = ToWeakCallback([this](bool ret, const std::list<UserInfo>& uinfos) {
		if (!ret) return;
		for (const auto &uinfo : uinfos)
		{
			nim::TeamMemberInfo tm_info = team_member_info_list_.at(uinfo.account);
			TeamItem* item = new TeamItem;
			GlobalManager::FillBoxWithCache(item, L"session/team_item.xml");
			if (tm_info.type == nim::kNIMTeamUserTypeCreator)
				member_list_->AddAt(item, 0);
			else
				member_list_->Add(item);

			item->InitControl();
			item->InitInfo(tm_info);

			if (tm_info.type == nim::kNIMTeamUserTypeCreator || tm_info.type == nim::kNIMTeamUserTypeManager)
			{
				if (LoginManager::GetInstance()->IsEqual(tm_info.account))
				{
					btn_new_broad_->SetVisible(true);
				}
			}
		}
	});
	UserService::GetInstance()->GetUserInfoWithEffort(acc_list, cb);
}

void SessionForm::OnTeamMemberAdd(const std::string& tid, const nim::TeamMemberInfo& team_member_info)
{
	if(tid == session_id_)
	{
		team_member_info_list_[team_member_info.account] = team_member_info;
		std::wstring wid = nbase::UTF8ToUTF16(team_member_info.account);

		Control* ctrl = member_list_->FindSubControl(wid);
		if(ctrl == NULL)
		{
			TeamItem* item = new TeamItem;
			GlobalManager::FillBoxWithCache(item, L"session/team_item.xml");
			member_list_->Add(item);

			item->InitControl();
			item->SetTeamMember(team_member_info);
		}

		std::wstring str = nbase::StringPrintf(L"群成员（共%d人）", member_list_->GetCount());
		label_member_->SetText(str);

		if (LoginManager::GetInstance()->IsEqual(team_member_info.account))
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
		
		if (LoginManager::GetInstance()->IsEqual(uid))
		{
			LeaveTeamHandle();
			btn_header_->SetEnabled(false);
			input_edit_->SetEnabled(false);
			btn_send_->SetEnabled(false);
			FindControl(L"btn_custom_msg")->SetEnabled(false);
			FindControl(L"btn_msg_record")->SetEnabled(false);
			btn_invite_->SetEnabled(false);
			btn_new_broad_->SetEnabled(false);
		}
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
			it->second.nick = team_card;
		}
		std::wstring wid = nbase::UTF8ToUTF16(uid);

		TeamItem* ctrl = (TeamItem*)(member_list_->FindSubControl(wid));
		if(ctrl)
		{
			ctrl->UpdateInfo(team_card);
		}
	}
}

void SessionForm::OnTeamMemberAdmin(const std::string& tid, const std::string& uid, bool admin)
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

void SessionForm::OnSetTeamOwner(const std::string& tid, const std::string& uid)
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
				break;
			}
		}

		TeamItem* item = dynamic_cast<TeamItem*>(member_list_->FindSubControl(wid));
		item->SetOwner(true);
	}
}

void SessionForm::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	if (session_id_ == team_info.id)
	{
		std::wstring name = nbase::UTF8ToUTF16(team_info.name);

		SetTaskbarTitle(name);
		label_title_->SetText(name);
	}
}

}
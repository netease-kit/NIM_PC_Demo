#include "team_service.h"
#include "module/login/login_manager.h"

namespace nim_comp
{
UnregisterCallback TeamService::RegAddTeam(OnTeamAdd add)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamAdd* new_add = new OnTeamAdd(add);
	int cb_id = (int)new_add;
	add_team_cb_[cb_id].reset(new_add);
	auto unregister = ToWeakCallback([this, cb_id]() {
		add_team_cb_.erase(cb_id);
	});
	return unregister;
}

void TeamService::InvokeAddTeam(const std::string& tid, const std::string& tname, nim::NIMTeamType type)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"InvokeAddTeam: tid={0} ") <<tid;
	for(auto& it : add_team_cb_)
	{
		(*it.second)(tid, tname, type);
	}
}

UnregisterCallback TeamService::RegRemoveTeam(OnTeamRemove remove)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamRemove* new_cb = new OnTeamRemove(remove);
	int cb_id = (int)new_cb;
	remove_team_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		remove_team_cb_.erase(cb_id);
	});
	return unregister;
}

UnregisterCallback TeamService::RegChangeTeamName(OnTeamNameChange change)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamNameChange* new_cb = new OnTeamNameChange(change);
	int cb_id = (int)new_cb;
	change_team_name_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		change_team_name_cb_.erase(cb_id);
	});
	return unregister;
}

void TeamService::InvokeRemoveTeam(const std::string& tid)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"InvokeRemoveTeam: tid={0} ") <<tid;

	tid_tname_pair_.erase(tid);

	for(auto& it : remove_team_cb_)
	{
		(*it.second)(tid);
	}
}

UnregisterCallback TeamService::RegAddTeamMember( OnTeamMemberAdd add )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamMemberAdd* new_cb = new OnTeamMemberAdd(add);
	int cb_id = (int)new_cb;
	add_team_member_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		add_team_member_cb_.erase(cb_id);
	});
	return unregister;
}

void TeamService::InvokeAddTeamMember(const std::string& tid, const nim::TeamMemberProperty& team_member)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"invoke add: tid={0} uid={1}") << tid << team_member.GetAccountID();
	for(auto& it : add_team_member_cb_)
	{
		(*it.second)(tid, team_member);
	}
}

UnregisterCallback TeamService::RegRemoveTeamMember( OnTeamMemberRemove remove )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamMemberRemove* new_cb = new OnTeamMemberRemove(remove);
	int cb_id = (int)new_cb;
	remove_team_member_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		remove_team_member_cb_.erase(cb_id);
	});
	return unregister;
}

UnregisterCallback TeamService::RegChangeTeamMember( OnTeamMemberChange change )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamMemberChange* new_cb = new OnTeamMemberChange(change);
	int cb_id = (int)new_cb;
	change_team_member_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		change_team_member_cb_.erase(cb_id);
	});
	return unregister;
}

void TeamService::InvokeRemoveTeamMember( const std::string& tid, const std::string& uid )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"invoke remove: tid={0} uid={1}") <<tid <<uid;

	if (LoginManager::GetInstance()->IsEqual(uid))
	{
		tid_tname_pair_.erase(tid);
	}

	for(auto& it : remove_team_member_cb_)
	{
		(*it.second)(tid, uid);
	}
}

void TeamService::InvokeChangeTeamMember(const std::string& tid, const std::string& uid, const std::string& team_card)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"invoke change: tid={0} uid={1} team_card={2}") << tid << uid << team_card;

	for(auto& it : change_team_member_cb_)
	{
		std::string new_team_card = team_card;
		if (new_team_card.empty())
			new_team_card = nbase::UTF16ToUTF8(UserService::GetInstance()->GetUserName(uid));

		(*it.second)(tid + "#" + uid, new_team_card);
	}
}

void TeamService::InvokeChangeTeamName(const nim::TeamInfo& team_info)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	for (auto& it : change_team_name_cb_)
	{
		(*it.second)(team_info);
	}
}

UnregisterCallback TeamService::RegSetTeamAdmin( OnTeamAdminSet admin )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamAdminSet* new_cb = new OnTeamAdminSet(admin);
	int cb_id = (int)new_cb;
	change_team_admin_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		change_team_admin_cb_.erase(cb_id);
	});
	return unregister;
}

UnregisterCallback TeamService::RegChangeTeamOwner(OnTeamOwnerChange set_team_owner)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	OnTeamOwnerChange* new_cb = new OnTeamOwnerChange(set_team_owner);
	int cb_id = (int)new_cb;
	set_team_owner_cb_[cb_id].reset(new_cb);
	auto unregister = ToWeakCallback([this, cb_id]() {
		set_team_owner_cb_.erase(cb_id);
	});
	return unregister;
}

void TeamService::InvokeChangeTeamAdmin( const std::string& tid, const std::string& uid, bool admin )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"invoke admin: tid={0} uid={1} admin={2}") <<tid <<uid <<admin;
	for(auto& it : change_team_admin_cb_)
	{
		(*it.second)(tid, uid, admin);
	}
}

void TeamService::InvokeSetTeamOwner(const std::string& tid, const std::string& uid)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	QLOG_APP(L"invoke admin: tid={0} uid={1} admin={2}") << tid << uid;
	for (auto& it : set_team_owner_cb_)
	{
		(*it.second)(tid, uid);
	}
}

std::wstring TeamService::GetTeamName( const std::string& tid )
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	std::wstring tname;

	std::map<std::string,std::string>::iterator it = tid_tname_pair_.find(tid);
	if(it == tid_tname_pair_.end())
	{
		tname = nbase::UTF8ToUTF16(tid);

		GetTeamInfo(tid);
	}
	else
	{
		tname = nbase::UTF8ToUTF16(it->second);
	}

	return tname;
}

std::wstring TeamService::GetTeamPhoto(bool full_path)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	if(full_path)
	{
		std::wstring dir = QPath::GetAppPath();
		dir.append(L"themes\\default\\public\\header\\head_team.png");
		return dir;
	}
	else
		return L"../public/header/head_team.png";
}

int TeamService::GetTeamType(const std::string& tid)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	int type = -1;
	std::map<std::string, int>::iterator it = tid_type_pair_.find(tid);
	if (it != tid_type_pair_.end())
		type = it->second;

	return type;
}

void TeamService::QueryAllTeamInfo()
{
	nim::Team::QueryAllMyTeamsInfoAsync(nbase::Bind(&TeamService::QueryAllTeamInfoCb, this, std::placeholders::_1, std::placeholders::_2));
}

void TeamService::UIQueryAllTeamInfoCb(int team_count, const std::list<nim::TeamInfo>& team_info_list)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	for (auto& team_info : team_info_list)
	{
		if (!team_info.GetName().empty())
		{
			tid_tname_pair_[team_info.GetTeamID()] = team_info.GetName();

			InvokeChangeTeamName(team_info);
		}

		tid_type_pair_[team_info.GetTeamID()] = team_info.GetType();
	}
}

void TeamService::QueryTeamInfo(const std::string & tid, OnGetTeamInfo cb)
{
	nim::Team::QueryTeamInfoAsync(tid, ToWeakCallback([cb](const std::string& tid, const nim::TeamInfo& result) 
	{
		cb(result);
	}));
}

void TeamService::QueryAllTeamInfoCb(int team_count, const std::list<nim::TeamInfo>& team_info_list)
{
	auto task = nbase::Bind(&TeamService::UIQueryAllTeamInfoCb, this, team_count, team_info_list);
	Post2UI(task);
}

void TeamService::GetTeamInfo(const std::string& tid)
{
	if (on_query_tids_.find(tid) != on_query_tids_.cend())
		return;
	on_query_tids_.insert(tid);

	nim::Team::QueryTeamInfoOnlineAsync(tid, nbase::Bind(&TeamService::GetTeamInfoCb, this, std::placeholders::_1));
}

void TeamService::UIGetLocalTeamInfoCb(const std::string& tid, const nim::TeamInfo& result)
{
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());

	if (!result.GetName().empty())
	{
		tid_tname_pair_[tid] = result.GetName();

		InvokeChangeTeamName(result);
	}

	tid_type_pair_[tid] = result.GetType();

	on_query_tids_.erase(tid); //已经查到，从post_tids_中删除
}

void TeamService::GetLocalTeamInfoCb(const std::string& tid, const nim::TeamInfo& result)
{
	auto task = nbase::Bind(&TeamService::UIGetLocalTeamInfoCb, this, tid, result);
	Post2UI(task);
}

void TeamService::GetTeamInfoCb(const nim::TeamEvent& team_event)
{
	if (team_event.res_code_ == nim::kNIMResSuccess)
	{
		nim::Team::QueryTeamInfoAsync(team_event.team_id_, nbase::Bind(&TeamService::GetLocalTeamInfoCb, this, std::placeholders::_1, std::placeholders::_2));
	}
}
}
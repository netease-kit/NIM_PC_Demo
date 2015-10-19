#pragma once


//tid, tname
typedef std::function<void(const std::string&, const std::string&, nim::NIMTeamType)> OnTeamAdd;
typedef std::function<void(const std::string&)> OnTeamRemove;
typedef std::function<void(const nim::TeamInfo& team_info)> OnTeamNameChange;

//tid,uid
typedef std::function<void(const std::string&, const nim::TeamMemberInfo&)> OnTeamMemberAdd;
typedef std::function<void(const std::string&, const std::string&)> OnTeamMemberRemove;
typedef std::function<void(const std::string&, const std::string&)> OnTeamMemberChange;

//tid,uid,admin
typedef std::function<void(const std::string&, const std::string&, bool)> OnTeamMemberAdmin;
typedef std::function<void(const std::string&, const std::string&)> OnSetTeamOwner;

namespace nim_comp
{
class TeamService : public nbase::SupportWeakCallback
{
public:
	friend void UITeamEventCallback(nim::NIMResCode code, nim::NIMNotificationId notify_id, const std::string &info, const std::string& tid, const nim::TeamInfo& team_info);
	friend class TeamCallback;
	SINGLETON_DEFINE(TeamService);
	TeamService() { }
public:
	UnregisterCallback RegAddTeam(OnTeamAdd add);
	UnregisterCallback RegRemoveTeam(OnTeamRemove remove);
	UnregisterCallback RegChangeTeamName(OnTeamNameChange change);
	UnregisterCallback RegAddTeamMember(OnTeamMemberAdd add);
	UnregisterCallback RegRemoveTeamMember(OnTeamMemberRemove remove);
	UnregisterCallback RegChangeTeamMember(OnTeamMemberChange change);
	UnregisterCallback RegChangeTeamAdmin(OnTeamMemberAdmin admin);
	UnregisterCallback RegSetTeamOwner(OnSetTeamOwner set_team_owner);

	std::wstring GetTeamName(const std::string& tid);
	std::wstring GetTeamPhoto(bool full_path);

	//从SDK获取
	void QueryAllTeamInfo();
	void InvokeChangeTeamMember(const std::string& tid, const std::string& uid, const std::string& team_card);

private:
	void InvokeAddTeam(const std::string& tid, const std::string& tname, nim::NIMTeamType type);
	void InvokeRemoveTeam(const std::string& tid);
	void InvokeAddTeamMember(const std::string& tid, const nim::TeamMemberInfo& team_member);
	void InvokeRemoveTeamMember(const std::string& tid, const std::string& uid);
	void InvokeChangeTeamName(const nim::TeamInfo& team_info);
	void InvokeChangeTeamAdmin(const std::string& tid, const std::string& uid, bool admin);
	void InvokeSetTeamOwner(const std::string& tid, const std::string& uid);

	//从SDK获取
	void QueryAllTeamInfoCb(int team_count, const std::list<nim::TeamInfo>& team_info_list);

	//从服务器获取
	void GetTeamInfo(const std::string& tid);
	void GetTeamInfoCb(nim::NIMResCode res_code, int notification_id, const std::string& tid, const std::string& result);

private:
	std::map<int, std::unique_ptr<OnTeamAdd>>			add_team_cb_;
	std::map<int, std::unique_ptr<OnTeamRemove>>		remove_team_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberAdd>>		add_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberRemove>>	remove_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberChange>>	change_team_member_cb_;
	std::map<int, std::unique_ptr<OnTeamNameChange>>	change_team_name_cb_;
	std::map<int, std::unique_ptr<OnTeamMemberAdmin>>	change_team_admin_cb_;
	std::map<int, std::unique_ptr<OnSetTeamOwner>>		set_team_owner_cb_;

	std::map<std::string,std::string> tid_tname_pair_;
};
}
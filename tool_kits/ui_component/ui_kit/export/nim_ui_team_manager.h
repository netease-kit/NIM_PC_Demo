#pragma once

#include "nim_ui_dll.h"


namespace nim_ui
{

/** @class TeamManager
  * @brief 提供群信息获取接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API TeamManager
{
public:
	SINGLETON_DEFINE(TeamManager);
public:
	TeamManager();
	~TeamManager();
public:
	UnregisterCallback RegAddTeam(OnTeamAdd add);
	UnregisterCallback RegRemoveTeam(OnTeamRemove remove);
	UnregisterCallback RegChangeTeamName(OnTeamNameChange change);
	UnregisterCallback RegAddTeamMember(OnTeamMemberAdd add);
	UnregisterCallback RegRemoveTeamMember(OnTeamMemberRemove remove);
	UnregisterCallback RegChangeTeamMember(OnTeamMemberChange change);
	UnregisterCallback RegSetTeamAdmin(OnTeamAdminSet admin);
	UnregisterCallback RegChangeTeamOwner(OnTeamOwnerChange set_team_owner);
	UnregisterCallback RegMuteMember(OnMuteMember mute);

	std::wstring GetTeamName(const std::string& tid);

	//从SDK获取
	void QueryAllTeamInfo();
	void InvokeChangeTeamMember(const std::string& tid, const std::string& uid, const std::string& team_card);
	void InvokeTeamDataSyncCallback(nim::NIMDataSyncType sync_type, nim::NIMDataSyncStatus status, const std::string &data_sync_info);
};

}
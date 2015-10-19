#include "nim_team_cpp_wrapper.h"
#include "nim_sdk_function.h"


namespace nim
{

typedef void(*nim_team_reg_team_event_cb)(const char *json_extension, nim_team_event_cb_func cb, const void *user_data);
typedef void(*nim_team_create_team_async)(const char *team_info, const char *jsonlist_uids, const char *invitation_postscript, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_query_team_info_online_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_invite_async)(const char *tid, const char *jsonlist_uids, const char *invitation_postscript, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_kick_async)(const char *tid, const char *jsonlist_uids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_leave_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_dismiss_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_team_info_async)(const char *tid, const char *json_info, const char *json_extension, nim_team_event_cb_func cb_func, const void* user_data);
typedef void(*nim_team_apply_join_async)(const char *tid, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_pass_join_apply_async)(const char *tid, const char *applicant_id, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_reject_join_apply_async)(const char *tid, const char *applicant_id, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_add_managers_async)(const char *tid, const char *jsonlist_admin_ids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_remove_managers_async)(const char *tid, const char *jsonlist_admin_ids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_transfer_team_async)(const char *tid, const char *new_owner, bool is_leave, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_my_property_async)(const char *info, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_other_nick_async)(const char *info, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_accept_invitation_async)(const char *tid, const char *invitor, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_reject_invitation_async)(const char *tid, const char *invitor, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);

typedef void(*nim_team_query_all_my_teams_async)(const char *json_extension, nim_team_query_all_my_teams_cb_func cb, const void* user_data);
typedef void(*nim_team_query_all_my_teams_info_async)(const char *json_extension, nim_team_query_all_my_teams_info_cb_func cb, const void* user_data);
typedef void(*nim_team_query_team_members_async)(const char *tid, bool include_user_info, const char *json_extension, nim_team_query_team_members_cb_func cb, const void* user_data);
typedef void(*nim_team_query_team_member_async)(const char *tid, const char *user_id, const char *json_extension, nim_team_query_team_member_cb_func cb, const void *user_data);
typedef void(*nim_team_query_team_info_async)(const char *tid, const char *json_extension, nim_team_query_team_info_cb_func cb, const void* user_data);


static void CallbackWrapperNoDelete(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::TeamEventCallback* cb_pointer = (Team::TeamEventCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, (nim::NIMNotificationId)notification_id, PCharToString(tid), PCharToString(result)));
		}
	}
}

static void CallbackWrapper1(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::TeamEventCallback* cb_pointer = (Team::TeamEventCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), (nim::NIMResCode)res_code, (nim::NIMNotificationId)notification_id, PCharToString(tid), PCharToString(result)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapper2(int team_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryAllMyTeamsCallback* cb_pointer = (Team::QueryAllMyTeamsCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), team_count, PCharToString(result)));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperQueryTeamMembers(const char *tid, int member_count, bool include_user_info, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamMembersCallback* cb_pointer = (Team::QueryTeamMembersCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value json;
			Json::Reader reader;
			std::list<nim::TeamMemberInfo> team_member_info_list;
			if (reader.parse(PCharToString(result), json))
			{
				int n = json.size();
				for (int i = 0; i < n; i++)
				{
					nim::TeamMemberInfo info;
					info.type = (nim::NIMTeamUserType)json[i][nim::kNIMTeamUserKeyType].asInt();
					if (info.type == nim::kNIMTeamUserTypeApply || info.type == nim::kNIMTeamUserTypeLocalWaitAccept)
						continue;

					info.account = json[i][nim::kNIMTeamUserKeyAccID].asString();
					info.nick = json[i][nim::kNIMTeamUserKeyNick].asString();
					info.bits = json[i][nim::kNIMTeamUserKeyBits].asUInt64();
					team_member_info_list.push_back(info);
				}
			}
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(tid), member_count, team_member_info_list));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperQueryTeamMember(const char *tid, const char *uid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamMemberCallback* cb_pointer = (Team::QueryTeamMemberCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value json;
			Json::Reader reader;
			nim::TeamMemberInfo info;
			info.account = uid;
			if (reader.parse(PCharToString(result), json))
			{
				info.type = (nim::NIMTeamUserType)json[nim::kNIMTeamUserKeyType].asInt();
				if (info.type != nim::kNIMTeamUserTypeApply && info.type != nim::kNIMTeamUserTypeLocalWaitAccept)
				{
					info.account = json[nim::kNIMTeamUserKeyAccID].asString();
					info.nick = json[nim::kNIMTeamUserKeyNick].asString();
					info.bits = json[nim::kNIMTeamUserKeyBits].asUInt64();
				}
			}
			PostTaskToUIThread(std::bind((*cb_pointer), info));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperQueryAllTeamInfo(int team_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryAllMyTeamsInfoCallback* cb_pointer = (Team::QueryAllMyTeamsInfoCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<nim::TeamInfo> team_info_list;
			Json::Value json;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), json))
			{
				int len = json.size();
				for (int i = 0; i < len; i++)
				{
					nim::TeamInfo team_info;
					team_info.type = (nim::NIMTeamType)(json[i][kNIMTeamInfoKeyType].asInt());
					team_info.id = json[i][nim::kNIMTeamInfoKeyID].asString();
					team_info.owner_id = json[i][nim::kNIMTeamInfoKeyCreator].asString();
					team_info.name = json[i][nim::kNIMTeamInfoKeyName].asString();
					team_info.intro = json[i][nim::kNIMTeamInfoKeyIntro].asString();
					team_info.announcement = json[i][nim::kNIMTeamInfoKeyAnnouncement].asString();
					team_info.join_mode = (NIMTeamJoinMode)(json[i][nim::kNIMTeamInfoKeyJoinMode].asInt());
					if (!json[i][nim::kNIMTeamInfoKeyServerCustom].asString().empty()) {
						team_info.readonly = true;
					}
					else {
						team_info.readonly = false;
					}
					int is_valid = json[i][nim::kNIMTeamInfoKeyValidFlag].asInt();
					team_info.team_config_bits = json[i][nim::kNIMTeamInfoKeyBits].asUInt64();
					if (is_valid == 1)
					{
						team_info_list.push_back(team_info);
					}
				}
			}

			PostTaskToUIThread(std::bind((*cb_pointer), team_count, team_info_list));
		}
		delete cb_pointer;
	}
}

static void CallbackWrapperQueryTeamInfo(const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamInfoCallback* cb_pointer = (Team::QueryTeamInfoCallback*)user_data;
		if (*cb_pointer)
		{
			nim::TeamInfo team_info;
			Json::Value json;
			Json::Reader reader;
			if (reader.parse(PCharToString(result), json))
			{
				team_info.type = (nim::NIMTeamType)(json[nim::kNIMTeamInfoKeyType].asInt());
				team_info.id = json[nim::kNIMTeamInfoKeyID].asString();
				team_info.owner_id = json[nim::kNIMTeamInfoKeyCreator].asString();
				team_info.name = json[nim::kNIMTeamInfoKeyName].asString();
				team_info.intro = json[nim::kNIMTeamInfoKeyIntro].asString();
				team_info.announcement = json[nim::kNIMTeamInfoKeyAnnouncement].asString();
				team_info.join_mode = (NIMTeamJoinMode)(json[nim::kNIMTeamInfoKeyJoinMode].asInt());
				if (!json[nim::kNIMTeamInfoKeyServerCustom].asString().empty()) {
					team_info.readonly = true;
				}
				else {
					team_info.readonly = false;
				}
				team_info.valid = json[nim::kNIMTeamInfoKeyValidFlag].asInt();
				team_info.team_config_bits = json[nim::kNIMTeamInfoKeyBits].asUInt64();
			}

			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(tid), team_info));
		}
		delete cb_pointer;
	}
}

static Team::TeamEventCallback* g_cb_pointer = nullptr;
void Team::RegTeamEventCb(const TeamEventCallback& cb, const std::string& json_extension)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_reg_team_event_cb)(json_extension.c_str(), &CallbackWrapperNoDelete, g_cb_pointer);

}

void Team::CreateTeamAsync(const std::string& team_info, const std::list<std::string>& jsonlist_uids, const std::string& invitation_postscript, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_create_team_async)(team_info.c_str(), ListToJson(jsonlist_uids).c_str(), invitation_postscript.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::QueryTeamInfoOnlineAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_team_info_online_async)(tid.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::InviteAsync(const std::string& tid, const std::list<std::string>& jsonlist_uids, const std::string& invitation_postscript, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_invite_async)(tid.c_str(), ListToJson(jsonlist_uids).c_str(), invitation_postscript.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::KickAsync(const std::string& tid, const std::list<std::string>& jsonlist_uids, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_kick_async)(tid.c_str(), ListToJson(jsonlist_uids).c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::LeaveAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_leave_async)(tid.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::DismissAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_dismiss_async)(tid.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::UpdateTeamInfoAsync(const std::string& tid, const std::string& json_info, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_update_team_info_async)(tid.c_str(), json_info.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::ApplyJoinAsync(const std::string& tid, const std::string& reason, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_apply_join_async)(tid.c_str(), reason.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::PassJoinApplyAsync(const std::string& tid, const std::string& applicant_id, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_pass_join_apply_async)(tid.c_str(), applicant_id.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::RejectJoinApplyAsync(const std::string& tid, const std::string& applicant_id, const std::string& reason, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_reject_join_apply_async)(tid.c_str(), applicant_id.c_str(), reason.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::AddManagersAsync(const std::string& tid, const std::list<std::string>& jsonlist_admin_ids, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_add_managers_async)(tid.c_str(), ListToJson(jsonlist_admin_ids).c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::RemoveManagersAsync(const std::string& tid, const std::list<std::string>& jsonlist_admin_ids, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_remove_managers_async)(tid.c_str(), ListToJson(jsonlist_admin_ids).c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::TransferTeamAsync(const std::string& tid, const std::string& new_owner, bool is_leave, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_transfer_team_async)(tid.c_str(), new_owner.c_str(), is_leave, json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::UpdateMyPropertyAsync(const std::string& info, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_update_my_property_async)(info.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::UpdateOtherNickAsync(const std::string& info, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_update_other_nick_async)(info.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::AcceptInvitationAsync(const std::string& tid, const std::string& invitor, const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_accept_invitation_async)(tid.c_str(), invitor.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::RejectInvitationAsync(const std::string& tid, const std::string& invitor, const std::string& reason, 
	const TeamEventCallback& cb, const std::string& json_extension)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_reject_invitation_async)(tid.c_str(), invitor.c_str(), reason.c_str(), json_extension.c_str(), &CallbackWrapper1, cb_pointer);
}

void Team::QueryAllMyTeamsAsync(const QueryAllMyTeamsCallback& cb, const std::string& json_extension)
{
	QueryAllMyTeamsCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryAllMyTeamsCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_all_my_teams_async)(json_extension.c_str(), &CallbackWrapper2, cb_pointer);
}

void Team::QueryAllMyTeamsInfoAsync(const QueryAllMyTeamsInfoCallback& cb, const std::string& json_extension)
{
	QueryAllMyTeamsInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryAllMyTeamsInfoCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_all_my_teams_info_async)(json_extension.c_str(), &CallbackWrapperQueryAllTeamInfo, cb_pointer);
}

void Team::QueryTeamMembersAsync(const std::string& tid, const QueryTeamMembersCallback& cb, const std::string& json_extension)
{
	QueryTeamMembersCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamMembersCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_team_members_async)(tid.c_str(), true, json_extension.c_str(), &CallbackWrapperQueryTeamMembers, cb_pointer);
}

void Team::QueryTeamMemberAsync(const std::string& tid, const std::string& uid, const QueryTeamMemberCallback& cb, const std::string& json_extension)
{
	QueryTeamMemberCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamMemberCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_team_member_async)(tid.c_str(), uid.c_str(), json_extension.c_str(), &CallbackWrapperQueryTeamMember, cb_pointer);
}

void Team::QueryTeamInfoAsync(const std::string& tid, const QueryTeamInfoCallback& cb, const std::string& json_extension)
{
	QueryTeamInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamInfoCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_team_query_team_info_async)(tid.c_str(), json_extension.c_str(), &CallbackWrapperQueryTeamInfo, cb_pointer);
}

}
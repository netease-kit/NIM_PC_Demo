#include "team_callback.h"
#include "module/session/session_manager.h"
#include "module/msglog/msg_extend_db.h"
#include "gui/team_info/team_notify.h"
#include "gui/main/team_event_form.h"

namespace nim_comp
{
void UITeamEventCallback(nim::NIMResCode code, nim::NIMNotificationId notify_id, const std::string &info, const std::string& tid, const nim::TeamInfo& team_info)
{
	QLOG_APP(L"TeamEvent: notify_id={0} tid={1} code={2}") << notify_id << tid << code;
	nim::NIMNotificationId event_id = nim::NIMNotificationId(notify_id);

	if (event_id == nim::kNIMNotificationIdLocalCreateTeam || event_id == nim::kNIMNotificationIdLocalApplyTeam)
	{
		if (code == nim::kNIMResSuccess || code == nim::kNIMResTeamInviteSuccess) {
			TeamService::GetInstance()->InvokeAddTeam(tid, "", team_info.type);
			SessionManager::GetInstance()->OpenSessionForm(tid, nim::kNIMSessionTypeTeam);
		}
	}
	else if (event_id == nim::kNIMNotificationIdLocalUpdateTlist || event_id == nim::kNIMNotificationIdTeamApplyPass || event_id == nim::kNIMNotificationIdTeamSyncUpdateTlist)
	{
		if (code == nim::kNIMResSuccess)
		{
			if (event_id == nim::kNIMNotificationIdTeamApplyPass) {
				TeamService::GetInstance()->InvokeAddTeam(tid, "", team_info.type);
			}
			if (event_id == nim::kNIMNotificationIdLocalUpdateTlist || event_id == nim::kNIMNotificationIdTeamSyncUpdateTlist)
			{
				SessionManager::GetInstance()->QueryMyTList(tid);
				std::wstring session_id = nbase::UTF8ToUTF16(tid);
				TeamInfoForm* team_info_form = (TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
					(TeamInfoForm::kClassName, session_id);
				if (team_info_form)
				{
					team_info_form->UpdateTeamMember();
				}
			}
			SessionForm* session = SessionManager::GetInstance()->Find(tid);
			if(session)
				session->InvokeGetTeamMember();
		}
	}
	else if (event_id == nim::kNIMNotificationIdTeamUpdate)
	{
		if (code == nim::kNIMResSuccess)
		{
			SessionForm* session = SessionManager::GetInstance()->Find(tid);
			if(session)
				session->InvokeGetTeamInfo();

			TeamService::GetInstance()->GetTeamInfo(tid);
		}
	}
	else if (event_id == nim::kNIMNotificationIdTeamDismiss)
	{
		if (code == nim::kNIMResSuccess)
		{
			TeamService::GetInstance()->InvokeRemoveTeam(tid);
		}
	}
	else if (event_id == nim::kNIMNotificationIdTeamInviteAccept && info.empty())	//自己接受群主的邀请	
	{
		if (code == nim::kNIMResSuccess)
		{
			TeamService::GetInstance()->InvokeAddTeam(tid, "", team_info.type);
		}
		SessionManager::GetInstance()->OpenSessionForm(tid, nim::kNIMSessionTypeTeam, true);
	}
	else if (event_id == nim::kNIMNotificationIdLocalGetTeamList)
	{
		if (code == nim::kNIMResSuccess)
		{
			auto cb = [] (const std::string& tid, int count, const std::list<nim::TeamMemberInfo>& team_member_info_list)
			{
				for (auto& it : team_member_info_list)
				{
					TeamService::GetInstance()->InvokeAddTeamMember(tid, it);
				}
			};
			nim::Team::QueryTeamMembersAsync(tid, cb);
			SessionManager::GetInstance()->QueryMyTList(tid);
		}
	}
	else
	{
		Json::Value json;
		if (code == nim::kNIMResSuccess && StringToJson(info, json))
		{
			bool add = true;
			Json::Value array;

			if (event_id == nim::kNIMNotificationIdTeamInviteAccept)
			{
				std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationLeaveKeyUid].asString();
				assert(LoginManager::GetInstance()->GetAccount() != uid);
				//群成员收到别人入群消息
				nim::Team::QueryTeamMemberAsync(tid, uid, [tid](const nim::TeamMemberInfo& team_member_info) {
					TeamService::GetInstance()->InvokeAddTeamMember(tid, team_member_info);
				});
			}
			else if (event_id == nim::kNIMNotificationIdTeamInvite)
			{
				TeamService::GetInstance()->InvokeAddTeam(tid, "", team_info.type);
				array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationInviteKeyUids];
			}
			else if (event_id == nim::kNIMNotificationIdTeamKick)
			{
				add = false;
				array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKickKeyUids];
			}
			else if (event_id == nim::kNIMNotificationIdTeamLeave)
			{
				std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationLeaveKeyUid].asString();
				TeamService::GetInstance()->InvokeRemoveTeamMember(tid, uid);
				if (LoginManager::GetInstance()->IsEqual(uid)) {
					TeamService::GetInstance()->InvokeRemoveTeam(tid);
				}
			}
			else if (event_id == nim::kNIMNotificationIdTeamOwnerTransfer)
			{
				std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationOwnerTransferUid].asString();
				TeamService::GetInstance()->InvokeSetTeamOwner(tid, uid);
			}
			else if (event_id == nim::kNIMNotificationIdTeamAddManager)
			{
				Json::Value arr = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationAddManagerUids];
				for(UINT i = 0; i < arr.size(); i++)
				{
					std::string uid = arr[i].asString();
					TeamService::GetInstance()->InvokeChangeTeamAdmin(tid, uid, true);
				}
			}
			else if (event_id == nim::kNIMNotificationIdTeamRemoveManager)
			{
				Json::Value arr = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationRemoveManagerUids];
				for(UINT i = 0; i < arr.size(); i++)
				{
					std::string uid = arr[i].asString();
					TeamService::GetInstance()->InvokeChangeTeamAdmin(tid, uid, false);
				}
			}
			else if (event_id == nim::kNIMNotificationIdTeamMemberChanged)
			{
				std::string new_tid = json[nim::kNIMNotificationIdTeamMemberChangedKey][nim::kNIMTeamUserKeyID].asString();
				std::string uid = json[nim::kNIMNotificationIdTeamMemberChangedKey][nim::kNIMTeamUserKeyAccID].asString();
				std::string nick = json[nim::kNIMNotificationIdTeamMemberChangedKey][nim::kNIMTeamUserKeyNick].asString();
				TeamService::GetInstance()->InvokeChangeTeamMember(new_tid, uid, nick);
			}
			
			if( !array.empty() && array.isArray() )
			{
				int len = array.size();
				for(int i = 0; i < len; i++)
				{
					std::string uid = array[i].asString();
					if (add) {
						nim::Team::QueryTeamMemberAsync(tid, uid, [tid](const nim::TeamMemberInfo& team_member_info) {
							TeamService::GetInstance()->InvokeAddTeamMember(tid, team_member_info);
						});

					}
					else {
						TeamService::GetInstance()->InvokeRemoveTeamMember(tid, uid);
						if (LoginManager::GetInstance()->IsEqual(uid)) {
							TeamService::GetInstance()->InvokeRemoveTeam(tid);
						}
					}
				}
			}
		}
	}
}

void TeamCallback::OnTeamEventCallback(nim::NIMResCode rescode, nim::NIMNotificationId notification_id, const std::string& tid, const std::string& result)
{
	nim::Team::QueryTeamInfoAsync(tid, nbase::Bind(&UITeamEventCallback, rescode, notification_id, result, std::placeholders::_1, std::placeholders::_2));
	QLOG_APP(L"OnTeamEventCallback: notify_id={0} code={1} tid={2} param={3}") <<notification_id <<rescode <<tid <<result;
}


//sysmsg

TeamNotifyForm* GetTeamNotifyForm(const std::string &tid)
{
	std::wstring wid = nbase::UTF8ToUTF16(tid);
	TeamNotifyForm* notify = (TeamNotifyForm*)( WindowsManager::GetInstance()->GetWindow(
		TeamNotifyForm::kClassName, wid) );
	if(notify)
	{
		return notify;
	}
	else
	{
		notify = new TeamNotifyForm;
		notify->SetTid(tid);
		notify->Create(NULL, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZE & ~WS_MINIMIZE, 0);
		notify->ShowWindow();

		POINT pt = GetPopupWindowPos(notify);
		ui::UiRect rc(pt.x, pt.y, 0, 0);
		notify->SetPos(rc, SWP_NOSIZE, NULL, true);
		return notify;
	}
}

void TeamCallback::OnReceiveSysmsgCallback( const std::string& param)
{
	QLOG_PRO(L"OnReceiveSysmsgCallback: {0}") <<param;

	Json::Value value;
	if (StringToJson(param, value))
	{
		const Json::Value &content = value[nim::kNIMSysMsgKeyLocalContent];
		nim::NIMSysMsgType type = (nim::NIMSysMsgType)content[nim::kNIMSysMsgKeyType].asInt();
		if (type == nim::kNIMSysMsgTypeCustomP2PMsg || type == nim::kNIMSysMsgTypeCustomTeamMsg)
		{
			int code = value[nim::kNIMMsgKeyLocalRescode].asInt();
			int feature = value[nim::kNIMMsgKeyLocalMsgFeature].asInt();
			Json::Value json = value[nim::kNIMMsgKeyLocalReceiveMsgContent];
			json[nim::kNIMMsgKeyLocalRescode] = code;
			json[nim::kNIMMsgKeyLocalMsgFeature] = feature;
			MsgData msg;
			CustomSysJsonToMsg(json, msg);
			if (msg.custom_save_flag == 0)//只在线
			{
				if (type == nim::kNIMSysMsgTypeCustomP2PMsg)
				{
					Json::Value json;
					if (StringToJson(msg.msg_attach, json))
					{
						std::string id = json["id"].asString();
						if (id == "1")
						{
							std::string id = msg.from_account;

							SessionForm* session = SessionManager::GetInstance()->Find(id);
							if (session)
							{
								session->AddWritingMsg(msg);
							}
							return;
						}
					}
				}
			}
			if (feature == nim::kNIMMessageFeatureRoamMsg || feature == nim::kNIMMessageFeatureSyncMsg)
			{
				msg.msg_status = nim::kNIMMsgLogStatusRead;
			}
			else
			{
				msg.msg_status = nim::kNIMMsgLogStatusUnread;
			}
			bool add = true;
			MsgExDB::GetInstance()->InsertMsgData(msg);
			TeamEventForm* f = dynamic_cast<TeamEventForm*>(WindowsManager::GetInstance()->GetWindow(TeamEventForm::kClassName, TeamEventForm::kClassName));
			if (f)
			{
				f->OnOneCustomMsg(msg);
				add = !f->IsCustomList();
			}
			if (add)
			{
				UpdateCustomSysmsgUnread(true);
			}

			return;
		}
		else if (type == nim::kNIMSysMsgTypeFriendDel)
		{
			// 自己被好友删除，不显示提醒，直接从数据库删除该条消息。
			__int64 msg_id = content[nim::kNIMSysMsgKeyMsgId].asInt64();
			nim::SystemMsg::DeleteAsync(msg_id, nim::SystemMsg::DeleteCallback());
			return;
		}

		int unread = value[nim::kNIMSysMsglogQueryKeyUnreadCount].asInt();
		UpdateSysmsgUnread(unread);

		if (content.isObject())
		{
			std::string from_account = content[nim::kNIMSysMsgKeyFromAccount].asString();
			std::string to_account = content[nim::kNIMSysMsgKeyToAccount].asString();

			std::string tid = to_account;
			if (type == nim::kNIMSysMsgTypeTeamApply)
			{
				TeamNotifyForm* tf = GetTeamNotifyForm(tid);
				if (tf)
					tf->OnAskJoin(content);
			}
			else if (type == nim::kNIMSysMsgTypeTeamInvite)
			{
				TeamNotifyForm* tf = GetTeamNotifyForm(tid);
				if (tf)
					tf->OnInviteYou(content);
			}
			else if (type == nim::kNIMSysMsgTypeTeamInviteReject)
			{
				TeamNotifyForm* tf = GetTeamNotifyForm(tid);
				if (tf)
				{
					tf->OnRejectInvite(content);
				}
				TeamService::GetInstance()->InvokeRemoveTeamMember(tid, from_account);
			}
			else if (type == nim::kNIMSysMsgTypeTeamReject)
			{
				TeamNotifyForm* tf = GetTeamNotifyForm(tid);
				if (tf)
					tf->OnRejectJoin(content);
			}
			else
			{
				return;
			}

			TeamEventForm* f = dynamic_cast<TeamEventForm*>(WindowsManager::GetInstance()->GetWindow(TeamEventForm::kClassName, TeamEventForm::kClassName));
			if (f)
				f->OnOneTeamEvent(content);
		}
	}
}

}
#include "team_callback.h"
#include "module/session/session_manager.h"
#include "module/msglog/msg_extend_db.h"
#include "gui/team_info/team_notify.h"
#include "gui/main/team_event_form.h"
#include "export/nim_ui_window_manager.h"

namespace nim_comp
{

void TeamCallback::UITeamEventCallback(const nim::TeamEvent& info, const std::string& tid, const nim::TeamInfo& team_info)
{
	QLOG_APP(L"TeamEvent: notify_id={0} tid={1} code={2}") << info.notification_id_ << tid << info.res_code_;

	if (info.notification_id_ == nim::kNIMNotificationIdTeamSyncCreate)
	{
		if (info.res_code_ == nim::kNIMResSuccess || info.res_code_ == nim::kNIMResTeamInviteSuccess)
		{
			TeamService::GetInstance()->InvokeAddTeam(tid, team_info);
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdLocalCreateTeam 
		|| info.notification_id_ == nim::kNIMNotificationIdLocalApplyTeam)
	{
		if (info.res_code_ == nim::kNIMResSuccess || info.res_code_ == nim::kNIMResTeamInviteSuccess) {
			TeamService::GetInstance()->InvokeAddTeam(tid, team_info);
			SessionManager::GetInstance()->OpenSessionBox(tid, nim::kNIMSessionTypeTeam);
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdLocalUpdateMemberProperty || info.notification_id_ == nim::kNIMNotificationIdTeamApplyPass || info.notification_id_ == nim::kNIMNotificationIdTeamSyncUpdateMemberProperty)
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			if (info.notification_id_ == nim::kNIMNotificationIdTeamApplyPass) {
				TeamService::GetInstance()->InvokeAddTeam(tid, team_info);
			}
			if (info.notification_id_ == nim::kNIMNotificationIdLocalUpdateMemberProperty || info.notification_id_ == nim::kNIMNotificationIdTeamSyncUpdateMemberProperty)
			{
				SessionManager::GetInstance()->QueryMyTeamInfo(tid);
				std::wstring session_id = nbase::UTF8ToUTF16(tid);
				TeamInfoForm* team_info_form = (TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
					(TeamInfoForm::kClassName, session_id);
				if (team_info_form)
				{
					team_info_form->UpdateTeamMember();
				}
			}
			SessionBox* session = SessionManager::GetInstance()->FindSessionBox(tid);
			if (session)
				session->InvokeGetTeamMember();
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdTeamUpdate)
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			SessionBox* session = SessionManager::GetInstance()->FindSessionBox(tid);
			if (session)
				session->InvokeGetTeamInfo();

			TeamService::GetInstance()->GetTeamInfo(tid);
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdTeamDismiss)
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			TeamService::GetInstance()->InvokeRemoveTeam(tid);
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdTeamInviteAccept)	//接受入群邀请（包括自己和他人）
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			TeamService::GetInstance()->InvokeAddTeam(tid, team_info);
			if (info.ids_.front() == team_info.GetOwnerID())
			{	//ids中的用户是群主，说明是自己接受邀请入群，就添加到群列表并打开群聊窗口
				SessionManager::GetInstance()->OpenSessionBox(tid, nim::kNIMSessionTypeTeam, true);
			}
			else
			{
				std::string uid = *info.ids_.begin();
				//assert(LoginManager::GetInstance()->GetAccount() != uid);
				//群成员收到别人入群消息
				//TODO(litianyi) 同步堵塞接口测试
				//nim::TeamMemberProperty prop = nim::Team::QueryTeamMemberBlock(tid, uid);
				//TeamService::GetInstance()->InvokeAddTeamMember(tid, prop);
				nim::Team::QueryTeamMemberAsync(tid, uid, [tid](const nim::TeamMemberProperty& team_member_info) {
					TeamService::GetInstance()->InvokeAddTeamMember(tid, team_member_info);
				});
			}
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdLocalGetTeamList)
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			auto cb = [](const std::string& tid, int count, const std::list<nim::TeamMemberProperty>& team_member_info_list)
			{
				for (auto& it : team_member_info_list)
				{
					TeamService::GetInstance()->InvokeAddTeamMember(tid, it);
				}
			};
			nim::Team::QueryTeamMembersAsync(tid, cb);
			SessionManager::GetInstance()->QueryMyTeamInfo(tid);
		}
	}
	else if (info.notification_id_ == nim::kNIMNotificationIdLocalMuteMember || info.notification_id_ == nim::kNIMNotificationIdTeamMuteMember)
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			for (auto& id : info.ids_)
			{
				TeamService::GetInstance()->InvokeMuteMember(tid, id, info.opt_);
			}
		}
	}
	else
	{
		if (info.res_code_ == nim::kNIMResSuccess)
		{
			if (info.notification_id_ == nim::kNIMNotificationIdTeamInvite)
			{
				QLOG_APP(L"UITeamEventCallback invite : {0}") << info.attach_;

				TeamService::GetInstance()->InvokeAddTeam(tid, team_info);
				for (auto& id : info.ids_)
				{
					nim::Team::QueryTeamMemberAsync(tid, id, [tid](const nim::TeamMemberProperty& team_member_info) {
						TeamService::GetInstance()->InvokeAddTeamMember(tid, team_member_info);
					});
				}
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamKick)
			{
				QLOG_APP(L"UITeamEventCallback Kick : {0}") << info.attach_;

				for (auto& id : info.ids_)
				{
					TeamService::GetInstance()->InvokeRemoveTeamMember(tid, id);
					if (LoginManager::GetInstance()->IsEqual(id)) {
						TeamService::GetInstance()->InvokeRemoveTeam(tid);
					}
				}
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamLeave)
			{
				std::string uid = *info.ids_.begin();
				TeamService::GetInstance()->InvokeRemoveTeamMember(tid, uid);
				if (LoginManager::GetInstance()->IsEqual(uid)) {
					TeamService::GetInstance()->InvokeRemoveTeam(tid);
				}
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamOwnerTransfer)
			{
				std::string uid = *info.ids_.begin();
				TeamService::GetInstance()->InvokeSetTeamOwner(tid, uid);
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamAddManager)
			{
				for (auto id = info.ids_.begin(); id != info.ids_.end(); ++id)
				{
					TeamService::GetInstance()->InvokeChangeTeamAdmin(tid, *id, true);
				}
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamRemoveManager)
			{
				for (auto id = info.ids_.begin(); id != info.ids_.end(); ++id)
				{
					TeamService::GetInstance()->InvokeChangeTeamAdmin(tid, *id, false);
				}
			}
			else if (info.notification_id_ == nim::kNIMNotificationIdTeamMemberChanged)
			{
				std::string uid = *info.ids_.begin();
				//TODO(litianyi) 同步堵塞接口测试
				//nim::TeamMemberProperty prop = nim::Team::QueryTeamMemberBlock(tid, uid);
				//TeamService::GetInstance()->InvokeChangeTeamMember(tid, prop.GetAccountID(), prop.GetNick());

				nim::Team::QueryTeamMemberAsync(tid, uid, [](const nim::TeamMemberProperty& team_member_info) {
					TeamService::GetInstance()->InvokeChangeTeamMember(team_member_info.GetTeamID(), team_member_info.GetAccountID(), team_member_info.GetNick());
				});
			}
		}
	}
}

void TeamCallback::OnTeamEventCallback(const nim::TeamEvent& result)
{
	nim::Team::QueryTeamInfoAsync(result.team_id_, nbase::Bind(&TeamCallback::UITeamEventCallback, result, std::placeholders::_1, std::placeholders::_2));
	QLOG_APP(L"OnTeamEventCallback: notify_id={0} code={1} tid={2}") <<result.notification_id_ <<result.res_code_ <<result.team_id_;
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

void UIReceiveSysmsgCallback(nim::SysMessage& msg)
{
	if (msg.type_ == nim::kNIMSysMsgTypeCustomP2PMsg || msg.type_ == nim::kNIMSysMsgTypeCustomTeamMsg)
	{
		nim::IMMessage immsg;
		CustomSysMessageToIMMessage(msg, immsg);
		if (msg.msg_setting_.need_offline_ != nim::BS_TRUE)//只在线
		{
			if (msg.type_ == nim::kNIMSysMsgTypeCustomP2PMsg)
			{
				Json::Value json;
				if (StringToJson(msg.attach_, json) && json.isObject())
				{
					if (json.isMember("id"))
					{
						std::string id = json["id"].asString();
						if (id == "1")
						{
							std::string id = msg.sender_accid_;

							SessionBox* session = SessionManager::GetInstance()->FindSessionBox(id);
							if (session)
							{
								session->AddWritingMsg(immsg);
							}
							return;
						}
					}
				}
			}
		}
		if (msg.feature_ == nim::kNIMMessageFeatureRoamMsg || msg.feature_ == nim::kNIMMessageFeatureSyncMsg)
		{
			msg.status_ = nim::kNIMSysMsgStatusRead;
		}
		else
		{
			msg.status_ = nim::kNIMSysMsgStatusNone;
		}
		//bool add = true;
		MsgExDB::GetInstance()->InsertMsgData(msg);
		TeamEventForm* f = dynamic_cast<TeamEventForm*>(WindowsManager::GetInstance()->GetWindow(TeamEventForm::kClassName, TeamEventForm::kClassName));
		if (f)
		{
			f->OnOneCustomMsg(msg);
			//add = !f->IsCustomList();
		}
		//if (add)
		{
			std::string show_text;
			if (msg.msg_setting_.push_content_.empty())
			{
				Json::Value values;
				Json::Reader reader;
				if (reader.parse(msg.attach_, values) && values.isObject() && values.isMember("id") && values.isMember("content"))
					show_text = values["content"].asString();
				else
					show_text = msg.attach_;
			}
			else
				show_text = msg.msg_setting_.push_content_;
			std::wstring toast = nbase::StringPrintf(L"%s 收到 %s 发的自定义通知: %s", nbase::UTF8ToUTF16(msg.receiver_accid_).c_str(), nbase::UTF8ToUTF16(msg.sender_accid_).c_str(), nbase::UTF8ToUTF16(show_text).c_str());
			nim_ui::ShowToast(toast, 5000);
			//UpdateCustomSysmsgUnread(true);
		}

		return;
	}
	else if (msg.type_ == nim::kNIMSysMsgTypeFriendDel)
	{
		// 自己被好友删除，不显示提醒，直接从数据库删除该条消息。
		nim::SystemMsg::DeleteAsync(msg.id_, nim::SystemMsg::DeleteCallback());
		return;
	}

	UpdateSysmsgUnread(msg.total_unread_count_);

	std::string tid = msg.receiver_accid_;
	if (msg.type_ == nim::kNIMSysMsgTypeTeamApply)
	{
		TeamNotifyForm* tf = GetTeamNotifyForm(tid);
		if (tf)
			tf->OnAskJoin(msg);
	}
	else if (msg.type_ == nim::kNIMSysMsgTypeTeamInvite)
	{
		TeamNotifyForm* tf = GetTeamNotifyForm(tid);
		if (tf)
			tf->OnInviteYou(msg);
	}
	else if (msg.type_ == nim::kNIMSysMsgTypeTeamInviteReject)
	{
		TeamNotifyForm* tf = GetTeamNotifyForm(tid);
		if (tf)
		{
			tf->OnRejectInvite(msg);
		}
		TeamService::GetInstance()->InvokeRemoveTeamMember(tid, msg.sender_accid_);
	}
	else if (msg.type_ == nim::kNIMSysMsgTypeTeamReject)
	{
		TeamNotifyForm* tf = GetTeamNotifyForm(tid);
		if (tf)
			tf->OnRejectJoin(msg);
	}
	else
	{
		return;
	}

	TeamEventForm* f = dynamic_cast<TeamEventForm*>(WindowsManager::GetInstance()->GetWindow(TeamEventForm::kClassName, TeamEventForm::kClassName));
	if (f)
		f->OnOneTeamEvent(msg);
} 

void TeamCallback::OnReceiveSysmsgCallback( const nim::SysMessage& msg )
{
	QLOG_PRO(L"OnReceiveSysmsgCallback: content: {0} - attach : {1}") << msg.content_ << msg.attach_;
	StdClosure task = nbase::Bind(&UIReceiveSysmsgCallback, msg);
	Post2UI(task);
}

}
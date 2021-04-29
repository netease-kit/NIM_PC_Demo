#include "stdafx.h"
#include "session_list_cloud.h"
#include "export/nim_ui_session_list_manager.h"
#include "module/service/session_service.h"
#include "module/session/session_manager.h"

namespace nim_comp
{

using namespace ui;

SessionListCloud::SessionListCloud(ui::ListBox* session_list)
	: session_list_cloud_(session_list)
{
	auto changed_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnSessionChangeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	SessionManager::GetInstance()->RegSessionChangedCallback(changed_cb);

	auto userinfo_change_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnUserInfoChange, this, std::placeholders::_1));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(userinfo_change_cb));

	auto userphoto_ready_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(userphoto_ready_cb));

	auto team_name_change_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnTeamNameChange, this, std::placeholders::_1));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(team_name_change_cb));

	auto receive_event_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnReceiveEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	unregister_cb.Add(SubscribeEventManager::GetInstance()->RegReceiveEventCallback(receive_event_cb));

	auto notify_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnNotifyChangeCallback, this, std::placeholders::_1, std::placeholders::_2));
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetMuteCallback(notify_cb));

	auto team_notify_cb = session_list_cloud_->ToWeakCallback(std::bind(&SessionListCloud::OnTeamNotificationModeChangeCallback, this, std::placeholders::_1, std::placeholders::_2));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamNotification(team_notify_cb));

	auto friend_list_change_cb = session_list_cloud_->ToWeakCallback(nbase::Bind(&SessionListCloud::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2));
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));
}

void SessionListCloud::Show(bool show)
{
	session_list_cloud_->SetVisible(show);
}

void SessionListCloud::AddSessionItem(const nim::SessionData& session_data)
{
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(session_data.id_)));
	int item_index = session_list_cloud_->GetItemIndex(item);
	if (item && item_index == 1)
	{
		item->InitMsg(std::make_shared<nim::SessionData>( session_data)); //应该插入自己紧靠前面或后面的位置，就不用删除，直接更新。
	}
	else
	{
		if (item) session_list_cloud_->Remove(item);
		item = new SessionItem;
		std::wstring session_item_xml = L"main/session_item.xml";
		if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::Simplified_Chinese)
			session_item_xml = L"main/session_item.xml";
		if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
			session_item_xml = L"main/session_item_en.xml";
		GlobalManager::FillBoxWithCache(item, session_item_xml);
		int index = AdjustMsg(session_data);
		item->InitCtrl();
		item->InitMsg(std::make_shared<nim::SessionData>(session_data));
		item->SetOnlineSessionType(true);
		item->AttachAllEvents(nbase::Bind(&SessionListCloud::OnItemNotify, this, std::placeholders::_1));
		if (index >= 0)
		{
			session_list_cloud_->AddAt(item, index);
			session_list_cloud_->SelectItem(index);
		}
		else
		{
			session_list_cloud_->Add(item);
		}
	}
}

nim_comp::SessionItem* SessionListCloud::GetSessionItem(const std::string &session_id)
{
	std::wstring wid = nbase::UTF8ToUTF16(session_id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(wid));
	return item;
}

void SessionListCloud::DeleteSessionItem(SessionItem* item)
{
	assert(item);
	auto id = item->GetUTF8Name();
	session_list_cloud_->Remove(item);
}

void SessionListCloud::ResetUnreadCount(const std::string &id)
{
	std::wstring wid = nbase::UTF8ToUTF16(id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(wid));
	if (item)
	{
		item->ResetUnread();
	}
}

bool SessionListCloud::AddLoadMore(bool has_more)
{
	ui::Label* label_more = new ui::Label;
	label_more->SetHorAlignType(ui::kHorAlignCenter);
	label_more->SetFixedHeight(20);
	label_more->SetMargin({ 0,8,0,0 });
	if (has_more)
	{
		label_more->SetStateTextColor(kControlStateNormal, L"link_blue");
		label_more->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_LIST_CLOUD_LOAD_MORE"));
		session_list_cloud_->Add(label_more);
		label_more->AttachButtonUp([this, label_more](ui::EventArgs* args) {
			session_list_cloud_->Remove(label_more);
			nim_comp::SessionService::GetInstance()->InvokeLoadOnlineSessionList();
			return true;
		});
	}
	else
	{
		label_more->SetText(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_SESSION_LIST_CLOUD_NO_MORE_SESSION"));
		session_list_cloud_->Add(label_more);
	}

	return true;
}

void SessionListCloud::UpdateSessionInfo(const nim::UserNameCard& user_info)
{
	SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(user_info.GetAccId())));
	if (session_item != NULL)
		session_item->InitUserProfile();

	for (int i = 0; i < session_list_cloud_->GetCount(); i++)
	{
		SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_cloud_->GetItemAt(i));
		if (session_item && session_item->GetIsTeam())
			session_item->UpdateMsgContent(user_info.GetAccId()); //群通知消息内容中可能含有用户名
	}
}

int SessionListCloud::AdjustMsg(const nim::SessionData &msg)
{
	int count = session_list_cloud_->GetCount();
	for (int i = 0; i < count; i++)
	{
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->GetItemAt(i));
		if (item)
		{
			auto item_tiem = item->GetMsgTime();
			if (msg.msg_timetag_ > item_tiem)
				return i;
		}
	}
	return -1;
}

bool SessionListCloud::OnSwitchLocalSession(ui::EventArgs* msg)
{
	nim_ui::SessionListManager::GetInstance()->ShowCloudSession(false);
	return true;
}

bool SessionListCloud::OnItemNotify(ui::EventArgs* msg)
{
	if (msg->Type == kEventNotify)
	{
		SessionItem* item = dynamic_cast<SessionItem*>(msg->pSender);
		assert(item);
		if (msg->wParam == SET_DELETE)
		{
			this->DeleteSessionItem(item);
		}
	}
	return true;
}

void SessionListCloud::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	if (change_type == kChangeTypeUpdate || change_type == kChangeTypeAdd)
	{
		SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(accid)));
		if (session_item != NULL)
			session_item->InitUserProfile();
	}
}

void SessionListCloud::OnSessionChangeCallback(nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts)
{
	if (rescode != nim::kNIMResSuccess)
	{
		QLOG_APP(L"SessionList::OnChangeCallback Error! {0}, uid:{1}, unread_count: {2}") << rescode << data.id_ << total_unread_counts;
		assert(0);
		return;
	}
	QLOG_APP(L"SessionList::OnChangeCallback. command: {0}, uid: {1}, type: {2}, total unread_count: {3}") << data.command_ << data.id_ << data.type_ << total_unread_counts;
	switch (data.command_)
	{
	case nim::kNIMSessionCommandAdd:
	{
		SubscribeEventManager::GetInstance()->SubscribeSessionEvent(data);
	}
	// break; // 不要break，继续执行
	case nim::kNIMSessionCommandUpdate:
	case nim::kNIMSessionCommandMsgDeleted:
	{
		if (data.last_updated_msg_)
		{
			AddSessionItem(data);
		}
	}
	break;
	case nim::kNIMSessionCommandRemoveAllTeam:
	{
		for (int i = session_list_cloud_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->GetItemAt(i));
			if (item && item->GetIsTeam())
			{
				session_list_cloud_->RemoveAt(i);
			}
		}
	}
	break;
	case nim::kNIMSessionCommandRemove:
		break;
	case nim::kNIMSessionCommandAllMsgDeleted:
	case nim::kNIMSessionCommandAllP2PMsgDeleted:
	case nim::kNIMSessionCommandAllTeamMsgDeleted:
	{
		for (int i = session_list_cloud_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->GetItemAt(i));
			if (item)
			{
				if (data.command_ == nim::kNIMSessionCommandAllMsgDeleted || (item->GetIsTeam() == (data.command_ == nim::kNIMSessionCommandAllTeamMsgDeleted)))
				{
					item->ClearMsg();
				}
			}
		}
	}
	break;
	}
}

void SessionListCloud::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
		UpdateSessionInfo(*iter);
}

void SessionListCloud::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	if (type == kUser || type == kTeam || type == kRobot)
	{
		SessionItem* item = (SessionItem*)session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item)
			item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}

void SessionListCloud::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	SessionItem* item = (SessionItem*)session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(team_info.GetTeamID()));
	if (item)
		((Label*)item->FindSubControl(L"label_name"))->SetUTF8Text(team_info.GetName());
}

void SessionListCloud::OnNotifyChangeCallback(std::string id, bool mute)
{
	std::wstring wid = nbase::UTF8ToUTF16(id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(wid));
	if (item && !item->GetIsTeam())
		item->SetMute(mute);
}

void SessionListCloud::OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data)
{
	if (event_type == nim::kNIMEventTypeOnlineState)
	{
		SessionItem* item = (SessionItem*)session_list_cloud_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item == NULL || item->GetIsTeam())
			return;
		item->SetOnlineState(data);
	}
}

void SessionListCloud::OnTeamNotificationModeChangeCallback(const std::string &tid, const int64_t bits)
{
	std::wstring wid = nbase::UTF8ToUTF16(tid);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_cloud_->FindSubControl(wid));
	if (item && item->GetIsTeam())
		item->SetMute(SessionManager::GetInstance()->IsTeamMsgMuteShown(tid, bits));
}

bool SessionListCloud::OnReturnEventsClick(ui::EventArgs* param)
{
	if (param->Type == kEventReturn)
	{
		SessionItem* item = dynamic_cast<SessionItem*>(param->pSender);
		assert(item);
		if (item)
		{
			SessionManager::GetInstance()->OpenSessionBox(item->GetUTF8DataID(), item->GetIsTeam() ? nim::kNIMSessionTypeTeam : nim::kNIMSessionTypeP2P);
		}
	}

	return true;
}

}

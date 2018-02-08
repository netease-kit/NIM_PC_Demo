#include "session_list.h"
#include "module/session/session_manager.h"
#include "gui/main/team_event_form.h"
#include "duilib/Utils/MultiLangSupport.h"

namespace nim_comp
{

using namespace ui;

SessionList::SessionList(ui::ListBox* session_list)
{
	ASSERT(session_list != NULL);
	session_list_ = session_list;
	sys_msg_unread_ = 0;
	custom_msg_unread_ = 0;

	Box* multispot_and_events = (Box*)GlobalManager::CreateBox(L"main/main_session_multispot_and_event.xml");
	session_list->AddAt(multispot_and_events, 0);

	ButtonBox* btn_events = (ButtonBox*)multispot_and_events->FindSubControl(L"btn_events");
	ButtonBox* btn_multispot_info = (ButtonBox*)multispot_and_events->FindSubControl(L"multispot_info");
	btn_events->AttachClick(nbase::Bind(&SessionList::OnEventsClick, this, std::placeholders::_1));
	btn_multispot_info->AttachClick(nbase::Bind(&SessionList::OnMultispotClick, this, std::placeholders::_1));

	box_unread_sysmsg_ = (Box*)session_list->GetWindow()->FindControl(L"box_unread_sysmsg");
	label_unread_sysmsg_ = (Label*)session_list->GetWindow()->FindControl(L"label_unread_sysmsg");
	box_unread_sysmsg_->SetVisible(false);

	nim::Session::RegChangeCb(nbase::Bind(&SessionList::OnSessionChangeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	
	OnUserInfoChangeCallback cb1 = nbase::Bind(&SessionList::OnUserInfoChange, this, std::placeholders::_1);
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(cb1));
	OnPhotoReadyCallback cb2 = nbase::Bind(&SessionList::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(cb2));
	auto cb3 = nbase::Bind(&SessionList::OnTeamNameChange, this, std::placeholders::_1);
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(cb3));
	auto receive_event_cb = nbase::Bind(&SessionList::OnReceiveEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(SubscribeEventManager::GetInstance()->RegReceiveEventCallback(receive_event_cb));

	auto robot_list_change_cb = nbase::Bind(&SessionList::OnRobotChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	unregister_cb.Add(UserService::GetInstance()->RegRobotListChange(robot_list_change_cb));

	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetMuteCallback(nbase::Bind(&SessionList::OnNotifyChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamNotification(nbase::Bind(&SessionList::OnTeamNotificationModeChangeCallback, this, std::placeholders::_1, std::placeholders::_2)));

	auto friend_list_change_cb = nbase::Bind(&SessionList::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2);
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(friend_list_change_cb));

	session_list_->AttachBubbledEvent(kEventReturn, nbase::Bind(&SessionList::OnReturnEventsClick, this, std::placeholders::_1));
}

bool SessionList::OnReturnEventsClick(ui::EventArgs* param)
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

int SessionList::AdjustMsg(const nim::SessionData &msg)
{
	int count = session_list_->GetCount();
	for (int i = 0; i < count; i++)
	{
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
		if (item)
		{
			if (msg.msg_timetag_ == 0 || msg.msg_timetag_ > item->GetMsgTime())
				return i;
		}
	}
	return -1;
}

void SessionList::OnTeamNotificationModeChangeCallback(const std::string &tid, const int64_t bits)
{
	std::wstring wid = nbase::UTF8ToUTF16(tid);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
	if (item && item->GetIsTeam())
		item->SetMute(SessionManager::GetInstance()->IsTeamMsgMuteShown(tid, bits));
}

void SessionList::OnNotifyChangeCallback(std::string id, bool mute)
{
	std::wstring wid = nbase::UTF8ToUTF16(id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
	if (item && !item->GetIsTeam())
		item->SetMute(mute);
}

SessionItem* SessionList::AddSessionItem(const nim::SessionData &item_data)
{
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(nbase::UTF8ToUTF16(item_data.id_)));
	nim::SessionData item_data_new = item_data;
	if (item)
	{
		if (nim::kNIMSessionCommandMsgDeleted != item_data.command_ && item->GetMsgTime() > item_data.msg_timetag_)
		{
			item_data_new = item->GetSessionData();
			item_data_new.unread_count_ = item_data.unread_count_;
		}
	}

	int index = AdjustMsg(item_data_new);

	if (item && (session_list_->GetItemIndex(item) == index - 1 || session_list_->GetItemIndex(item) == index))
	{
		item->InitMsg(item_data_new); //应该插入自己紧靠前面或后面的位置，就不用删除，直接更新。
	}
	else
	{
		if (item)
			session_list_->Remove(item);
		item = new SessionItem;
		GlobalManager::FillBoxWithCache(item, L"main/session_item.xml");
		index = AdjustMsg(item_data_new); //删掉之后重新算一次
		if (index >= 0)
			session_list_->AddAt(item, index);
		else
			session_list_->Add(item);

		item->InitCtrl();
		item->InitMsg(item_data_new);
		item->AttachAllEvents(nbase::Bind(&SessionList::OnItemNotify, this, std::placeholders::_1));
	}
	
	InvokeUnreadCountChange();
	return item;
}

SessionItem* SessionList::GetSessionItem(const std::string &session_id)
{
	std::wstring wid = nbase::UTF8ToUTF16(session_id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
	return item;
}

void SessionList::DeleteSessionItem(SessionItem* item)
{
	assert(item);
	session_list_->Remove(item);
	InvokeUnreadCountChange();
}


void SessionList::RemoveAllSessionItem()
{
	//回话列表的第一项是多端同步和消息中心，所以要专门写个清除函数
	int count = session_list_->GetCount();
	for (int i = count - 1; i > 0; --i)
	{
		session_list_->RemoveAt(i);
	}
	InvokeUnreadCountChange();
}

UnregisterCallback SessionList::RegUnreadCountChange(const OnUnreadCountChangeCallback& callback)
{
	OnUnreadCountChangeCallback* new_callback = new OnUnreadCountChangeCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	unread_count_change_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		unread_count_change_cb_list_.erase(cb_id);
	});
	return cb;	
}

void SessionList::InvokeUnreadCountChange()
{
	if (unread_count_change_cb_list_.empty())
		return;

	int unread_count = sys_msg_unread_ + custom_msg_unread_;

	int count = session_list_->GetCount();
	for (int i = 0; i < count; i++)
	{
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
		if (item)
		{
			unread_count += item->GetUnread();
		}
	}

	for (auto& it : unread_count_change_cb_list_)
		(*(it.second))(unread_count);
}

void SessionList::AddUnreadCount(const std::string &id)
{
	std::wstring wid = nbase::UTF8ToUTF16(id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
	if (item)
	{		
		item->AddUnread();
		InvokeUnreadCountChange();
	}
}

void SessionList::ResetUnreadCount(const std::string &id)
{
	std::wstring wid = nbase::UTF8ToUTF16(id);
	SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
	if (item)
	{	
		item->ResetUnread();
		InvokeUnreadCountChange();
	}
}

void SessionList::UISysmsgUnread(int count)
{
	sys_msg_unread_ = count;
	int all_count = sys_msg_unread_ + custom_msg_unread_;
	if (all_count > 0)
	{
		label_unread_sysmsg_->SetText(nbase::StringPrintf(L"%d", all_count));
		box_unread_sysmsg_->SetVisible(true);
	}
	else
	{
		box_unread_sysmsg_->SetVisible(false);
	}
	InvokeUnreadCountChange();
}

void SessionList::UICustomSysmsgUnread(bool add)
{
	if (add)
	{
		custom_msg_unread_++;
	}
	else
	{
		custom_msg_unread_ = 0;
	}
	int all_count = sys_msg_unread_ + custom_msg_unread_;
	if (all_count > 0)
	{
		label_unread_sysmsg_->SetText(nbase::StringPrintf(L"%d", all_count));
		box_unread_sysmsg_->SetVisible(true);
	}
	else
	{
		box_unread_sysmsg_->SetVisible(false);
	}
	InvokeUnreadCountChange();
}

void SessionList::UpdateSessionInfo(const nim::UserNameCard& user_info)
{
	SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(nbase::UTF8ToUTF16(user_info.GetAccId())));
	if (session_item != NULL)
		session_item->InitUserProfile();

	for (int i = 0; i < session_list_->GetCount(); i++)
	{
		SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
		if (session_item && session_item->GetIsTeam())
			session_item->UpdateMsgContent(user_info.GetAccId()); //群通知消息内容中可能含有用户名
	}
}

//多端登录
//[{"app_account":"abc","client_os":"***","client_type":1,"device_id":"***","mac":"***"}]
void SessionList::OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients)
{
	for each (auto client in clients)
	{
		if (online)
		{ 
			map_multispot_infos_[client.client_type_] = client;
		}
		else
		{
			auto it = map_multispot_infos_.find(client.client_type_);
			if (it != map_multispot_infos_.end())
			{
				map_multispot_infos_.erase(it);
			}
		}
	}
	UpdateMultispotUI();
}

void SessionList::OnMultispotKickout(const std::list<std::string> &client_ids)
{
	for (auto it : client_ids)
	{
		for (auto it2 = map_multispot_infos_.begin(); it2 != map_multispot_infos_.end(); it2++)
		{
			if (it2->second.device_id_ == it)
			{
				map_multispot_infos_.erase(it2);
				break;
			}
		}
	}
	UpdateMultispotUI();
}

void SessionList::UpdateMultispotUI()
{
	ui::ButtonBox* show_btn = (ui::ButtonBox*)session_list_->GetWindow()->FindControl(L"multispot_info");
	if (show_btn)
	{
		if (map_multispot_infos_.size() > 0)
		{
			ui::Label *multi_des = (ui::Label*)session_list_->GetWindow()->FindControl(L"multi_des");
			std::wstring show_tip = ui::MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MAINWINDOW_USING_NIM_ON_MOBILEPHONE");
			std::wstring client_des;
			switch (map_multispot_infos_.begin()->second.client_type_) 
			{
			case nim::kNIMClientTypeAndroid:
				client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_AOS");
				break;
			case nim::kNIMClientTypeiOS:
				client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_IOS");
				break;
			case nim::kNIMClientTypePCWindows:
			case nim::kNIMClientTypeMacOS:
				client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_PC");
				break;
			case nim::kNIMClientTypeWeb:
				client_des = MutiLanSupport::GetInstance()->GetStringViaID(L"STRID_MULTISPOT_NIM_WEB");
				break;		
			default:
				client_des = nbase::UTF8ToUTF16(map_multispot_infos_.begin()->second.client_os_);
				break;
			}
			show_tip = nbase::StringPrintf(show_tip.c_str(), client_des.c_str());
			multi_des->SetText(show_tip);
			//std::wstring show_tip = L"登录帐号设备";
			//for (auto& it : map_multispot_infos_)
			//{
			//	show_tip.append(L"\r\n");
			//	show_tip.append(nbase::UTF8ToUTF16(it.second.client_os_));
			//	show_tip.append(L"\r\n");
			//	show_tip.append(GetMessageTime(it.second.login_time_, false));
			//}
			show_btn->SetVisible(true);
			//show_btn->SetToolTipText(show_tip);
		}
		else
		{
			show_btn->SetVisible(false);
		}
	}

	MultispotForm* form = dynamic_cast<MultispotForm*>(WindowsManager::GetInstance()->GetWindow(MultispotForm::kClassName, MultispotForm::kClassName));
	if (form)
	{
		if (map_multispot_infos_.size() > 0)
		{
			form->OnMultispotChange(map_multispot_infos_);
		}
		else
		{
			form->Close();
		}
	}
}

void SessionList::OnSessionChangeCallback(nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts)
{
	if (rescode != nim::kNIMResSuccess)
	{
		QLOG_APP(L"SessionList::OnChangeCallback Error! {0}, uid:{1}, unread_count: {2}") << rescode << data.id_<< total_unread_counts;
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
			SubscribeEventManager::GetInstance()->SubscribeSessionEvent(data);
			AddSessionItem(data);
			if (data.unread_count_ == 0
				|| SessionManager::GetInstance()->IsSessionBoxActive(data.id_))
			{
				ResetUnreadCount(data.id_);
			}
		}
	}
	break;
	case nim::kNIMSessionCommandRemoveAll:
	{
		std::list<nim::SessionData> unsubscribe_list;
		for (int i = session_list_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
			if (item && !item->GetIsTeam())
			{
				unsubscribe_list.push_back(item->GetSessionData());
			}
		}
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(unsubscribe_list);
		RemoveAllSessionItem();
	}	
	break;
	case nim::kNIMSessionCommandRemoveAllP2P:
	{		
		std::list<nim::SessionData> unsubscribe_list;
		for (int i = session_list_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
			if (item && !item->GetIsTeam())
			{
				unsubscribe_list.push_back(item->GetSessionData());
				session_list_->RemoveAt(i);
			}
		}
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(unsubscribe_list);
		InvokeUnreadCountChange();
	}
	break;
	case nim::kNIMSessionCommandRemoveAllTeam:
	{
		for (int i = session_list_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
			if (item && item->GetIsTeam())
			{
				session_list_->RemoveAt(i);
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
		for (int i = session_list_->GetCount() - 1; i >= 0; i--)
		{
			SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(i));
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

void SessionList::OnRobotChange(nim::NIMResCode rescode, nim::NIMRobotInfoChangeType type, const nim::RobotInfos& robots)
{
	if (rescode == nim::kNIMResSuccess)
	{
		for (auto &robot : robots)
		{
			SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(nbase::UTF8ToUTF16(robot.GetAccid())));
			if (session_item != NULL)
				session_item->InitRobotProfile();
		}
	}
}

void SessionList::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
		UpdateSessionInfo(*iter);
}

void SessionList::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	if (change_type == kChangeTypeUpdate)
	{
		SessionItem* session_item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(nbase::UTF8ToUTF16(accid)));
		if (session_item != NULL)
			session_item->InitUserProfile();
	}
}

void SessionList::OnUserPhotoReady(PhotoType type, const std::string& accid, const std::wstring &photo_path)
{
	if (type == kUser || type == kTeam || type == kRobot)
	{
		SessionItem* item = (SessionItem*)session_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item)
			item->FindSubControl(L"head_image")->SetBkImage(photo_path);
	}
}

void SessionList::OnTeamNameChange(const nim::TeamInfo& team_info)
{
	SessionItem* item = (SessionItem*)session_list_->FindSubControl(nbase::UTF8ToUTF16(team_info.GetTeamID()));
	if (item)
		((Label*)item->FindSubControl(L"label_name"))->SetUTF8Text(team_info.GetName());
}

void SessionList::OnReceiveEvent(int event_type, const std::string &accid, const EventDataEx &data)
{
	if (event_type == nim::kNIMEventTypeOnlineState)
	{
		SessionItem* item = (SessionItem*)session_list_->FindSubControl(nbase::UTF8ToUTF16(accid));
		if (item == NULL || item->GetIsTeam())
			return;
		item->SetOnlineState(data);
	}
}

bool SessionList::OnEventsClick(ui::EventArgs* param)
{
	if (param->pSender->GetName() == L"btn_events")
	{
		TeamEventForm* f = WindowsManager::SingletonShow<TeamEventForm>(TeamEventForm::kClassName);
		f->ReloadEvents();
	}

	return true;
}

bool SessionList::OnMultispotClick(ui::EventArgs* param)
{
	if (param->pSender->GetName() == L"multispot_info")
	{
		MultispotForm* f = WindowsManager::SingletonShow<MultispotForm>(MultispotForm::kClassName);
		f->OnMultispotChange(map_multispot_infos_);
	}
	return true;
}

bool SessionList::OnItemNotify(ui::EventArgs* msg)
{
	if(msg->Type == kEventNotify)
	{		
	 	SessionItem* item = dynamic_cast<SessionItem*>(msg->pSender);
	 	assert(item);
	 	if(msg->wParam == SET_DELETE)
	 	{
	 		this->DeleteSessionItem(item);
	 	}
	}
	return true;
}

}
#include "stdafx.h"
#include "session_list.h"
#include "module/session/session_manager.h"
#include "gui/main/team_event_form.h"
#include "duilib/Utils/MultiLangSupport.h"
#include "ui_kit_base/invoke_safe_callback.h"
#include "export/nim_ui_session_list_manager.h"

namespace nim_comp
{

using namespace ui;

SessionList::SessionList(ui::VirtualListBox* session_list, ui::ListBox* top_session_list)
{
	ASSERT(session_list != NULL);
	session_list_ = session_list;
	session_list_->SelectNextWhenActiveRemoved(false);
	top_session_list_ = top_session_list;
	cursel_id_ = "";
	session_list_->AttachSelect([this](ui::EventArgs* args) {
		int index = session_list_->GetCurSel();
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_->GetItemAt(index));
		if (item != nullptr)
			cursel_id_ = item->GetUTF8Name();
		return true;
		});
	int kRoomMemberItemHeight = 60;
	session_list_->SetElementHeight(ui::DpiManager::GetInstance()->ScaleInt(kRoomMemberItemHeight));
	session_list_->SetDataProvider(this);
	session_list_->InitElement(200);
	session_list_->Refresh();
	sys_msg_unread_ = 0;
	custom_msg_unread_ = 0;

	Box* multispot_and_events = (Box*)GlobalManager::CreateBox(L"main/main_session_multispot_and_event.xml");
	top_session_list_->AddAt(multispot_and_events, 0);

	ListContainerElement* cloud_session_list = (ListContainerElement*)GlobalManager::CreateBox(L"main/main_cloud_session.xml");
	top_session_list_->AddAt(cloud_session_list, 1);
	ListContainerElement* local_session_list = (ListContainerElement*)GlobalManager::CreateBox(L"main/main_local_session.xml");
	local_session_list->SetVisible(false);
	top_session_list_->AddAt(local_session_list, 2);
	
	cloud_session_list->AttachClick(ToWeakCallback([local_session_list, cloud_session_list](ui::EventArgs* msg) {
		nim_ui::SessionListManager::GetInstance()->ShowCloudSession();
		local_session_list->SetVisible(true);
		cloud_session_list->SetVisible(false);
		return true;
		}));

	
	local_session_list->AttachClick(ToWeakCallback([local_session_list,cloud_session_list](ui::EventArgs* msg) {
		nim_ui::SessionListManager::GetInstance()->ShowCloudSession(false);
		cloud_session_list->SetVisible(true);
		local_session_list->SetVisible(false);
		return true;
		}));

	ButtonBox* btn_events = (ButtonBox*)multispot_and_events->FindSubControl(L"btn_events");
	ButtonBox* btn_multispot_info = (ButtonBox*)multispot_and_events->FindSubControl(L"multispot_info");
	btn_events->AttachClick(nbase::Bind(&SessionList::OnEventsClick, this, std::placeholders::_1));
	btn_multispot_info->AttachClick(nbase::Bind(&SessionList::OnMultispotClick, this, std::placeholders::_1));

	box_unread_sysmsg_ = (Box*)top_session_list_->GetWindow()->FindControl(L"box_unread_sysmsg");
	label_unread_sysmsg_ = (Label*)top_session_list_->GetWindow()->FindControl(L"label_unread_sysmsg");
	box_unread_sysmsg_->SetVisible(false);
	
	auto changed_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnSessionChangeCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	SessionManager::GetInstance()->RegSessionChangedCallback(changed_cb);

	auto userinfo_change_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnUserInfoChange, this, std::placeholders::_1));
	unregister_cb.Add(UserService::GetInstance()->RegUserInfoChange(userinfo_change_cb));

	auto userphoto_ready_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnUserPhotoReady, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	unregister_cb.Add(PhotoService::GetInstance()->RegPhotoReady(userphoto_ready_cb));

	auto team_name_change_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnTeamNameChange, this, std::placeholders::_1));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamName(team_name_change_cb));
	
	auto receive_event_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnReceiveEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	unregister_cb.Add(SubscribeEventManager::GetInstance()->RegReceiveEventCallback(receive_event_cb));

	auto notify_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnNotifyChangeCallback, this, std::placeholders::_1, std::placeholders::_2));
	unregister_cb.Add(MuteBlackService::GetInstance()->RegSyncSetMuteCallback(notify_cb));

	auto team_notify_cb = session_list_->ToWeakCallback(std::bind(&SessionList::OnTeamNotificationModeChangeCallback, this, std::placeholders::_1, std::placeholders::_2));
	unregister_cb.Add(TeamService::GetInstance()->RegChangeTeamNotification(team_notify_cb));

	auto friend_list_change_cb = session_list_->ToWeakCallback(nbase::Bind(&SessionList::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2));
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

int SessionList::AddSessionItemData(const nim::SessionData& item_data)
{
	int ret = 0;
	std::shared_ptr<nim::SessionData> item_data_new = std::make_shared<nim::SessionData>(item_data);
	auto it = session_list_data_.find(item_data.id_);
	if (it != session_list_data_.end())
	{
		if (nim::kNIMSessionCommandMsgDeleted != item_data.command_ && it->second->msg_timetag_ > item_data.msg_timetag_)
		{
			item_data_new = it->second;
			item_data_new->unread_count_ = item_data.unread_count_;
		}
	}
	if (session_list_data_[item_data_new->id_] == nullptr)
		session_list_data_[item_data_new->id_] = item_data_new;
	else
		*session_list_data_[item_data_new->id_] = *item_data_new;
	auto sort_it = std::find_if(session_list_sort_data_.begin(), session_list_sort_data_.end(), [&](const std::shared_ptr<nim::SessionData>& item) {
		return item->id_.compare(item_data_new->id_) == 0;
		});
	if (sort_it == session_list_sort_data_.end())
	{
		ret = 1;
		session_list_sort_data_.emplace_back(session_list_data_[item_data_new->id_]);
	}
	return ret;
}

bool SessionList::DeleteSessionItemData(const std::string& session_id)
{
	auto it = session_list_data_.find(session_id);
	if (it == session_list_data_.end())
		return false;
	session_list_data_.erase(session_id);
	auto sort_it = std::find_if(session_list_sort_data_.begin(), session_list_sort_data_.end(), [&](const std::shared_ptr<nim::SessionData>& item) {
		return item->id_.compare(session_id) == 0;
		});
	if (sort_it != session_list_sort_data_.end())
		session_list_sort_data_.erase(sort_it);
	if (session_id.compare(cursel_id_) == 0)
	{
		cursel_id_.clear();
		session_list_->SelectItem(-1, false, false);
	}
	return true;
}
void SessionList::DeleteSessionDataByType(nim::NIMSessionType type, std::list<nim::SessionData>& unsubscribe_list)
{
	for (auto it = session_list_sort_data_.begin(); it != session_list_sort_data_.end(); )
	{
		if (it->operator->()->type_ == type)
		{
			unsubscribe_list.emplace_back(*(*it));
			session_list_data_.erase(it->operator->()->id_);
			it = session_list_sort_data_.erase(it);
			continue;
		}
		else
		{
			it++;
		}
	}
}
void SessionList::ResetAllSessionUnreadCount(int type)
{
	for (auto it : session_list_sort_data_)
	{
		if(type == 1 && it->type_ == nim::NIMSessionType::kNIMSessionTypeP2P)
			it->unread_count_ = 0;
		else if (type == 1 && it->type_ == nim::NIMSessionType::kNIMSessionTypeTeam)
			it->unread_count_ = 0;
		else
			it->unread_count_ = 0;
	}
}

void SessionList::Show(bool show)
{
	session_list_->SetVisible(show);
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
	auto it = session_list_data_.find(id);
	it->second->placed_on_top_;	
	if (it != session_list_data_.end() && (it->second->type_ == nim::NIMSessionType::kNIMSessionTypeTeam || it->second->type_ == nim::NIMSessionType::kNIMSessionTypeSuperTeam))
	{
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
		if (item && !item->GetIsTeam())
			item->SetMute(mute);
	}	
}

SessionItem* SessionList::AddSessionItem(const nim::SessionData &item_data)
{
	AddSessionItem(item_data, true);
	SortSessionItems();
	
	return nullptr;
}
SessionItem* SessionList::AddSessionItem(const nim::SessionData &item_data, bool notify_event)
{
	// 机器人功能下线，不显示机器人消息在会话列表
	if (item_data.msg_type_ == nim::kNIMMessageTypeRobot)
		return nullptr;
	// 此处有一个隐含问题，当从列表中删除一个正在传送文件的 session item 时
	// SDK 会回调到上层 SendCommand 来发送一条消息给对端告诉对端取消了文件传输
	// 当界面已经删除了这个 item 后又收到了消息回执会重新将 item 添加到列表中
	// 所以此处不响应取消传送文件请求的消息
	Json::Value values;
	Json::Reader reader;
	if (reader.parse(item_data.msg_attach_, values) && values.isObject())
	{
		if (values.isMember(kJsonKeyCommand))
		{
			if ((values[kJsonKeyCommand].asString() == kJsonKeyCancelTransferFile ||
				values[kJsonKeyCommand].asString() == kJsonKeyCancelReceiveFile))
			{
				return nullptr;
			}
		}
	}

	bool invoke_additem = false;
	invoke_additem = (AddSessionItemData(item_data) == 1 ? notify_event:false);
	SortSessionItems();
	InvokeUnreadCountChange();
	if (invoke_additem)
		for (auto it : add_item_cb_list_)
			(*it.second)(item_data.id_);
	return nullptr;
}
void SessionList::AddSessionItem(const std::list<nim::SessionData>& list)
{
	for (auto it : list)
		AddSessionItem(it, false);
	SortSessionItems();
}
bool SessionList::HasSession(const std::string &session_id)
{	
	return session_list_data_.find(session_id) != session_list_data_.end();
}
void SessionList::SelectSession(const std::string& session_id,bool sel, bool trigger)
{
	auto it = session_list_data_.find(session_id);
	if (it == session_list_data_.end())
	{
		return;
	}		
	else
	{
		cursel_id_ = session_id;
		SortSessionItems(true);
	}
		
}
int SessionList::GetCurSel()
{
	return session_list_->GetCurSel();
}
bool SessionList::IsSessionSelected(const std::string& session_id)
{
	return session_id.compare(cursel_id_) == 0;
}
void SessionList::DeleteSessionItem(const std::string& session_id)
{
	if (session_list_ == nullptr)
		return;
	DeleteSessionItemData(session_id);
	SortSessionItems();
	InvokeUnreadCountChange();	
	for (auto it : remove_item_cb_list_)
		(*it.second)(session_id);
}


void SessionList::RemoveAllSessionItem()
{
	session_list_->RemoveAll();
	session_list_sort_data_.clear();
	session_list_data_.clear();
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
UnregisterCallback SessionList::RegAddItem(const OnAddItemCallback& callback)
{
	OnAddItemCallback* new_callback = new OnAddItemCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	add_item_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		add_item_cb_list_.erase(cb_id);
	});
	return cb;
}

UnregisterCallback SessionList::RegRemoveItem(const OnRemoveItemCallback& callback)
{
	OnRemoveItemCallback* new_callback = new OnRemoveItemCallback(callback);
	int cb_id = (int)new_callback;
	assert(nbase::MessageLoop::current()->ToUIMessageLoop());
	remove_item_cb_list_[cb_id].reset(new_callback);
	auto cb = ToWeakCallback([this, cb_id]() {
		remove_item_cb_list_.erase(cb_id);
	});
	return cb;
}
ui::Control* SessionList::CreateElement()
{
	SessionItem* item = new SessionItem;
	std::wstring session_item_xml = L"main/session_item.xml";
	if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::Simplified_Chinese)
		session_item_xml = L"main/session_item.xml";
	if (ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		session_item_xml = L"main/session_item_en.xml";
	GlobalManager::FillBoxWithCache(item, session_item_xml);
	item->InitCtrl();
	item->DetachEvent(ui::EventType::kEventAll);
	item->AttachAllEvents(nbase::Bind(&SessionList::OnItemNotify, this, std::placeholders::_1));
	return item;
}
void SessionList::FillElement(ui::Control* control, int index)
{
	SessionItem* item = (SessionItem*)control;
	auto item_data_new = session_list_sort_data_.at(index);	
	item->InitMsg(item_data_new);	
	//item->SetUnread(item_data_new->unread_count_);
	if (cursel_id_.compare(item_data_new->id_) == 0)
	{
		item->Selected(true, false);
	}
	else
	{
		item->Selected(false, false);
	}
	//item->SetIndex(index);
}
int SessionList::GetElementtCount()
{
	return session_list_data_.size();
}
void SessionList::InvokeUnreadCountChange()
{
	if (unread_count_change_cb_list_.empty())
		return;

	int unread_count = sys_msg_unread_ + custom_msg_unread_;
	for (auto it : session_list_data_)
	{
		unread_count += it.second->unread_count_;
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
	}
	auto it = session_list_data_.find(id);
	if (it != session_list_data_.end())
	{
		it->second->unread_count_ += 1;
		InvokeUnreadCountChange();
	}
}

void SessionList::ResetUnreadCount(const std::string &id)
{	
	auto it = session_list_data_.find(id);
	if (it != session_list_data_.end())
	{
		it->second->unread_count_ = 0;
		std::wstring wid = nbase::UTF8ToUTF16(id);
		SessionItem* item = dynamic_cast<SessionItem*>(session_list_->FindSubControl(wid));
		if (item)
		{
			if (item->ResetUnread())
			{
				session_list_->Refresh();
				InvokeUnreadCountChange();
			}
		}		
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
		for (auto it : session_list_data_)
		{
			unsubscribe_list.emplace_back(*it.second);
		}
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(unsubscribe_list);
		RemoveAllSessionItem();
	}	
	break;
	case nim::kNIMSessionCommandRemoveAllP2P:
	{		
		std::list<nim::SessionData> unsubscribe_list;
		DeleteSessionDataByType(nim::NIMSessionType::kNIMSessionTypeP2P, unsubscribe_list);
		SortSessionItems();
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(unsubscribe_list);
		InvokeUnreadCountChange();
	}
	break;
	case nim::kNIMSessionCommandRemoveAllTeam:
	{
		std::list<nim::SessionData> unsubscribe_list;
		DeleteSessionDataByType(nim::NIMSessionType::kNIMSessionTypeTeam, unsubscribe_list);
		SortSessionItems();
		SubscribeEventManager::GetInstance()->UnSubscribeSessionEvent(unsubscribe_list);
		InvokeUnreadCountChange();		
	}
	break;
	case nim::kNIMSessionCommandRemove:
		break;
	case nim::kNIMSessionCommandAllMsgDeleted:
	case nim::kNIMSessionCommandAllP2PMsgDeleted:
	case nim::kNIMSessionCommandAllTeamMsgDeleted:
	{
		int type = 0;
		switch (data.command_ )
		{
		case nim::kNIMSessionCommandAllMsgDeleted:
			type = 0;
			break;
		case nim::kNIMSessionCommandAllP2PMsgDeleted:
			type = 1;
			break;
		case nim::kNIMSessionCommandAllTeamMsgDeleted:
			type = 2;
			break;
		}		
		ResetAllSessionUnreadCount(type);
		SortSessionItems();
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

void SessionList::OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos)
{
	for (auto iter = uinfos.cbegin(); iter != uinfos.cend(); iter++)
		UpdateSessionInfo(*iter);
}

void SessionList::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	if (change_type == kChangeTypeUpdate || change_type == kChangeTypeAdd)
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

bool SessionList::OnSwitchCloudSession(ui::EventArgs* msg)
{
	nim_ui::SessionListManager::GetInstance()->ShowCloudSession();
	return true;
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
			auto id = item->GetSessionData()->id_;
			Post2UI(ToWeakCallback([this,id]() {
				DeleteSessionItem(id);
			}));
	 	}
	}
	return true;
}
void SessionList::SortSessionItems(bool sel_force_visible)
{
	bool selected_visible = false;
	if (sel_force_visible)
	{
		selected_visible = true;
	}		
	else
	{
		if (!cursel_id_.empty())
		{
			std::vector<int> collection;
			session_list_->GetDisplayCollection(collection);
			int index = 0;
			for (auto it : session_list_sort_data_)
			{
				if (it->id_.compare(cursel_id_) == 0)
					break;
				index++;
			}
			selected_visible = collection.end() != std::find_if(collection.begin(), collection.end(), [&](int item) {return index == item; });
		}
	}	
	std::sort(session_list_sort_data_.begin(), session_list_sort_data_.end(), [](const std::shared_ptr<nim::SessionData>& item1, const std::shared_ptr<nim::SessionData>& item2) {
		
		auto user_type1 = item1->msg_timetag_;
		auto user_type2 = item2->msg_timetag_;
			return user_type2 < user_type1;
		});
	session_list_->Refresh();	
	if (selected_visible && !cursel_id_.empty())
	{
		int index = 0;
		for (auto it : session_list_sort_data_)
		{
			if (it->id_.compare(cursel_id_) == 0)
				break;
			index++;
		}
		session_list_->EnsureVisible(index, false);		
	}
}
}
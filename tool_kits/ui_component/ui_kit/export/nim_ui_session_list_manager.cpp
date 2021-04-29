#include "stdafx.h"
#include "nim_ui_session_list_manager.h"
#include "callback/session/session_callback.h"
#include "nim_service/module/service/session_service.h"
#include "gui/main/team_event_form.h"
#include "gui/main/session_list.h"

namespace nim_ui
{
SessionListManager::SessionListManager()
{
	nim_comp::SessionService::GetInstance()->RegSessionDataChangedCallback(nbase::Bind(&SessionListManager::OnQuerySessionListCallback, this, std::placeholders::_1));
	nim_comp::SessionService::GetInstance()->RegOnlineSessionDataChangedCallback(nbase::Bind(&SessionListManager::OnQueryOnlineSessionListCallback, this, std::placeholders::_1, std::placeholders::_2));
}

bool SessionListManager::AttachListBox(ui::VirtualListBox*list_box, ui::ListBox* top_function_list)
{
	if (NULL == list_box)
	{
		if (session_list_ != nullptr)
			session_list_ = nullptr;
	}
	else
	{
		session_list_.reset(new nim_comp::SessionList(list_box, top_function_list));
	}
	return true;
}

bool SessionListManager::AttachCloudListBox(ui::ListBox* list_box)
{
	if (NULL == list_box)
	{
		if (session_list_cloud_ != nullptr)
			session_list_cloud_ = nullptr;
	}
	else
	{
		session_list_cloud_.reset(new nim_comp::SessionListCloud(list_box));
	}
	return true;
}

void SessionListManager::ShowCloudSession(bool show /*= true*/)
{
	session_list_->Show(!show);
	session_list_cloud_->Show(show);
}

void SessionListManager::QueryUnreadSysMsgCount()
{
	nim::SystemMsg::QueryUnreadCount(nbase::Bind(&nim_ui::SessionListManager::OnQuerySysmsgUnreadCb, this, std::placeholders::_1, std::placeholders::_2));
}

UnregisterCallback SessionListManager::RegUnreadCountChange(const nim_comp::OnUnreadCountChangeCallback& callback)
{
	ASSERT(NULL != session_list_);
	return session_list_->RegUnreadCountChange(callback);
}
UnregisterCallback SessionListManager::RegAddItem(const nim_comp::OnAddItemCallback& callback)
{
	ASSERT(NULL != session_list_);
	return session_list_->RegAddItem(callback);
}

UnregisterCallback SessionListManager::RegRemoveItem(const nim_comp::OnRemoveItemCallback& callback)
{
	ASSERT(NULL != session_list_);
	return session_list_->RegRemoveItem(callback);
}
void SessionListManager::InvokeSelectSessionItem(const std::string &id, bool sel/* = true*/, bool trigger/* = true*/)
{
	if (session_list_ == nullptr)
		return;
	if (!session_list_->HasSession(id) || session_list_->IsSessionSelected(id))
		return;
	session_list_->SelectSession(id, sel,trigger);
	auto online_session_item = session_list_cloud_->GetSessionItem(id);
	if (online_session_item == nullptr || online_session_item->IsSelected() == sel)
		return;
	online_session_item->Selected(sel, trigger);
}
void SessionListManager::InvokeSelectSessionItem(const std::string &id, nim::NIMSessionType session_type, bool create/* = true*/, bool sel/* = true*/, bool trigger/* = true*/)
{
	if (session_list_ == nullptr)
		return;
	if (!session_list_ ->HasSession(id))
	{
		if (create)
		{
			InsertLocalMsg(ToWeakCallback([this, sel, trigger](nim::NIMResCode res_code, const std::string& msg_id){
			}), session_type, id, 1000 * nbase::Time::Now().ToTimeT());
		}			
	}
	else
	{
		if (session_list_->IsSessionSelected(id) != sel)
			session_list_->SelectSession(id, sel, trigger);
	}	
}
bool SessionListManager::AddSessionItem(const nim::SessionData &msg)
{
	if (session_list_ == nullptr)
		return false;
	session_list_->AddSessionItem(msg) != nullptr;
	return true;
}
bool SessionListManager::CheckSessionItem(const std::string &session_id)
{
	if (session_list_ == nullptr)
		return false;
	return session_list_->HasSession(session_id);
}
void SessionListManager::InvokeAddSessionUnread(const std::string &id)
{
	if (NULL == session_list_)
		return;
	session_list_->AddUnreadCount(id);
}

void SessionListManager::InvokeResetSessionUnread(const std::string &id)
{
	if (NULL == session_list_)
		return;
	session_list_->ResetUnreadCount(id);

	if (NULL == session_list_cloud_)
		return;
	session_list_cloud_->ResetUnreadCount(id);
}

void SessionListManager::UISysmsgUnread(int count)
{
	if (NULL == session_list_)
		return;
	session_list_->UISysmsgUnread(count);
}

void SessionListManager::UICustomSysmsgUnread(bool add)
{
	if (NULL == session_list_)
		return;
	session_list_->UICustomSysmsgUnread(add);
}

void SessionListManager::OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients)
{
	if (NULL == session_list_)
		return;
	session_list_->OnMultispotChange(online, clients);
}

void SessionListManager::OnMultispotKickout(const std::list<std::string> &client_ids)
{
	if (NULL == session_list_)
		return;
	session_list_->OnMultispotKickout(client_ids);
}

void SessionListManager::OnQuerySysmsgUnreadCb(nim::NIMResCode res_code, int unread_count)
{
	if (res_code == 200)
		nim_comp::UpdateSysmsgUnread(unread_count);
}

void SessionListManager::OnQuerySessionListCallback(const std::list<nim::SessionData>& sessions)
{
	if (sessions.empty())
		return;
	session_list_->AddSessionItem(sessions);	
}

void SessionListManager::OnQueryOnlineSessionListCallback(bool has_more, const nim::SessionOnLineServiceHelper::SessionList& session_list)
{
	if (session_list.empty())
		return;

	for each (auto& session in session_list)
	{
		Json::Value msg_values;
		Json::Reader reader;

		nim::SessionData session_data;
		session_data.id_ = session.id_;
		session_data.type_ = session.type_;

		if (reader.parse(session.last_message_, msg_values))
		{
			session_data.msg_attach_ = msg_values[nim::kNIMMsgKeyAttach].asString();
			session_data.msg_content_ = msg_values[nim::kNIMMsgKeyBody].asString();
			session_data.msg_id_ = msg_values[nim::kNIMMsgKeyClientMsgid].asString();
			session_data.msg_type_ = static_cast<nim::NIMMessageType>(msg_values[nim::kNIMMsgKeyType].asInt());
			session_data.msg_status_ = static_cast<nim::NIMMsgLogStatus>(msg_values[nim::kNIMMsgKeyLocalLogStatus].asInt());
			session_data.msg_sub_status_ = static_cast<nim::NIMMsgLogSubStatus>(msg_values[nim::kNIMMsgKeyLocalLogSubStatus].asInt());
			session_data.msg_sender_accid_ = msg_values[nim::kNIMMsgKeyFromAccount].asString();
			session_data.msg_timetag_ = msg_values[nim::kNIMMsgKeyTime].asUInt64();
			session_data.last_updated_msg_ = true;
		}

		session_list_cloud_->AddSessionItem(session_data);
	}

	// 增加获取更多按钮
	session_list_cloud_->AddLoadMore(has_more);
}

void SessionListManager::InsertLocalMsg(const nim_comp::InsertLocalMessageCallback& cb, nim::NIMSessionType session_type, const std::string& session_id, __int64 time, bool status_delete)
{
	nim::IMMessage msg;
	msg.session_type_ = session_type;
	msg.receiver_accid_ = session_id;
	msg.sender_accid_ = nim_comp::LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id_ = nim::Tool::GetUuid();
	msg.timetag_ = time > 0 ? time : 1000 * nbase::Time::Now().ToTimeT();
	if (status_delete)
		msg.status_ = nim::kNIMMsgLogStatusDeleted;
	msg.type_ = nim::NIMMessageType(nim::kNIMMessageTypeUnknown + nim::kNIMMessageTypeCustom);
	msg.content_ = "";
	msg.attach_ = "{\"virtual_local_message\":true}";
	nim::MsgLog::WriteMsglogToLocalAsync(session_id, msg, true, ToWeakCallback([status_delete, msg,cb](nim::NIMResCode res_code, const std::string& msg_id){
		if (res_code == nim::kNIMResSuccess)
		{
			QLOG_PRO(L"send local session selected message callback: session_id={0}") << msg.receiver_accid_;
			//nim::MsgLog::DeleteAsync(msg.receiver_accid_, msg.session_type_, msg.client_msg_id_, nim::MsgLog::DeleteCallback());
			if (status_delete)
				nim::MsgLog::SetStatusAsync(msg.client_msg_id_, nim::kNIMMsgLogStatusDeleted, nim::MsgLog::SetStatusCallback());
		}
		if (cb != nullptr)
			cb(res_code, msg_id);
	}));
}
}


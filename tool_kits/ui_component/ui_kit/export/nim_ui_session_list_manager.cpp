#include "nim_ui_session_list_manager.h"
#include "gui/main/session_list.h"
#include "callback/session/session_callback.h"
#include "gui/main/team_event_form.h"
#include "nim_service/module/service/session_service.h"

namespace nim_ui
{
SessionListManager::SessionListManager()
{
	nim_comp::SessionService::GetInstance()->RegSessionDataChangedCallback(nbase::Bind(&SessionListManager::OnQuerySessionListCallback, this, std::placeholders::_1));
}

bool SessionListManager::AttachListBox(ui::ListBox *list_box)
{
	if (NULL == list_box)
	{
		session_list_ = nullptr;
	}
	else
	{
		session_list_.reset(new nim_comp::SessionList(list_box));
	}
	return true;
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
	auto item = session_list_->GetSessionItem(id);
	if (item == nullptr || item->IsSelected() == sel)
		return;
	item->Selected(sel, trigger);
}
void SessionListManager::InvokeSelectSessionItem(const std::string &id, nim::NIMSessionType session_type, bool create/* = true*/, bool sel/* = true*/, bool trigger/* = true*/)
{
	if (session_list_ == nullptr)
		return;
	auto item = session_list_->GetSessionItem(id);
	if (item == nullptr)
	{
		if (create)
		{
			InsertLocalMsg(ToWeakCallback([this, sel, trigger](nim::NIMResCode res_code, const std::string& msg_id){
			}), session_type, id, 1000 * nbase::Time::Now().ToTimeT());
		}			
	}
	else
	{
		if(item->IsSelected() != sel)
			item->Selected(sel, trigger);
	}	
}
bool SessionListManager::AddSessionItem(const nim::SessionData &msg)
{
	if (session_list_ == nullptr)
		return false;
	return session_list_->AddSessionItem(msg) != nullptr;
}
bool SessionListManager::CheckSessionItem(const std::string &session_id)
{
	if (session_list_ == nullptr)
		return false;
	return session_list_->GetSessionItem(session_id) != nullptr;
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

	for each (auto& session in sessions)
	{
		session_list_->AddSessionItem(session,false);
	}
}
void SessionListManager::InsertLocalMsg(const nim_comp::InsertLocalMessageCallback& cb, nim::NIMSessionType session_type, const std::string& session_id, __int64 time, bool status_delete)
{
	nim::IMMessage msg;
	msg.session_type_ = session_type;
	msg.receiver_accid_ = session_id;
	msg.sender_accid_ = nim_comp::LoginManager::GetInstance()->GetAccount();
	msg.client_msg_id_ = QString::GetGUID();
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


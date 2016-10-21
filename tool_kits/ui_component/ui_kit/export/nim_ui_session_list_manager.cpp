#include "nim_ui_session_list_manager.h"
#include "gui/main/session_list.h"
#include "callback/session/session_callback.h"
#include "gui/main/team_event_form.h"

namespace nim_ui
{

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

void SessionListManager::InvokeLoadSessionList()
{
	nim::Session::QueryAllRecentSessionAsync(nbase::Bind(&nim_comp::TalkCallback::OnQuerySessionListCallback, std::placeholders::_1, std::placeholders::_2));
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

	for each (auto session in sessions)
	{
		session_list_->AddSessionItem(session);
	}
}

}


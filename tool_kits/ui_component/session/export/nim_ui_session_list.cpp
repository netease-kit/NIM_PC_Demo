#include "nim_ui_session_list.h"
#include "gui/main/session_list.h"
#include "callback/session/session_callback.h"
#include "gui/main/team_event_form.h"

namespace nim_ui
{

SessionListManager::SessionListManager()
{

}

SessionListManager::~SessionListManager()
{

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

void SessionListManager::InvokeLoadSessionList()
{
	nim::Session::QueryAllRecentSessionAsync(nbase::Bind(&nim_comp::TalkCallback::OnQuerySessionListCallback, std::placeholders::_1, std::placeholders::_2));
}

void SessionListManager::AddUnreadCount(const std::string &id)
{
	if (NULL == session_list_)
		return;
	session_list_->AddUnreadCount(id);
}

void SessionListManager::ResetSessionUnread(const std::string &id)
{
	if (NULL == session_list_)
		return;
	session_list_->ResetSessionUnread(id);
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

void SessionListManager::OnMultispotChange(bool online, Json::Value& json)
{
	if (NULL == session_list_)
		return;
	session_list_->OnMultispotChange(online, json);
}

void SessionListManager::OnMultispotKickout(const std::list<std::string> &client_ids)
{
	if (NULL == session_list_)
		return;
	session_list_->OnMultispotKickout(client_ids);
}

void SessionListManager::QueryUnreadCount()
{
	nim::SystemMsg::QueryUnreadCount(nbase::Bind(&nim_ui::SessionListManager::QuerySysmsgUnreadCb, std::placeholders::_1, std::placeholders::_2));
}

void SessionListManager::QuerySysmsgUnreadCb(nim::NIMResCode res_code, int unread_count)
{
	if (res_code == 200)
		nim_comp::UpdateSysmsgUnread(unread_count);
}



void SessionListManager::LoadSessionList(const std::string &str)
{
	if (NULL == session_list_)
		return;

	Json::Value value;
	Json::Reader reader;
	if (reader.parse(str, value))
	{
		Json::Value &content = value[nim::kNIMSessionListContent];
		int n = content.size();
		for (int i = 0; i < n; i++)
		{
			SessionMsgData data;
			data.session_id = content[i][nim::kNIMSessionId].asString();
			data.from_account = content[i][nim::kNIMSessionMsgFromAccount].asString();
			data.to_type = content[i][nim::kNIMSessionType].asString() == "0" ? nim::kNIMSessionTypeP2P : nim::kNIMSessionTypeTeam;
			data.msg_body = content[i][nim::kNIMSessionMsgBody].asString();
			data.msg_attach = content[i][nim::kNIMSessionMsgAttach].asString();
			data.msg_type = content[i][nim::kNIMSessionMsgType].asInt();
			data.msg_time = content[i][nim::kNIMSessionMsgTime].asInt64();
			data.unread_count = content[i][nim::kNIMSessionUnreadCount].asInt();
			data.msg_status = content[i][nim::kNIMSessionMsgStatus].asInt();

			session_list_->AddSessionItem(data);
		}
	}
	else
	{
		QLOG_ERR(L"parse session list fail: {0}") << str;
	}
}

}


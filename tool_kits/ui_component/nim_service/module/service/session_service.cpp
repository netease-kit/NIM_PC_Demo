#include "session_service.h"
#include "nim_service/module/subscribe_event/subscribe_event_manager.h"

namespace nim_comp
{
	void SessionService::InvokeLoadSessionList()
	{
		nim::Session::QueryAllRecentSessionAsync(nbase::Bind(&nim_comp::SessionService::OnQuerySessionListCallback, std::placeholders::_1, std::placeholders::_2));
	}

	nim::SessionData SessionService::GetSessionItem(const std::string &session_id)
	{
		auto iter = sessions_.find(session_id);
		if (iter != sessions_.end())
			return iter->second;
		else
			return nim::SessionData();
	}

	void SessionService::OnQuerySessionList(const std::list<nim::SessionData>& sessions)
	{
		if (sessions.empty())
			return;

		// 最近会话列表会在登录后较早的收到，这时批量的拉取涉及到的用户信息把这些用户添加到查询队列里
		// 避免创建会话列表项时，列表项控件查询用户信息而导致频繁调用用户信息获取接口
		std::list<std::string> user_ids;
		for each (auto& session in sessions)
		{
			if (session.type_ == nim::kNIMSessionTypeP2P)
			{
				user_ids.push_back(session.id_);
			}
			else
			{
				user_ids.push_back(session.msg_sender_accid_);
			}
		}
		nim_comp::UserService::GetInstance()->DoQueryUserInfos(user_ids);
		nim_comp::SubscribeEventManager::GetInstance()->SubscribeAllSessionEvent(sessions);

		for each (auto& session in sessions)
		{
			auto iter = sessions_.find(session.id_);
			if (iter != sessions_.end())
			{
				if (nim::kNIMSessionCommandMsgDeleted != session.command_ && iter->second.msg_timetag_ > session.msg_timetag_)
				{
					sessions_[session.id_] = session;
				}
				break;
			}
			else
			{
				sessions_[session.id_] = session;
			}
		}

		(*session_data_changed_callback_)(sessions);
	}

	void SessionService::OnQuerySessionListCallback(int unread_count, const nim::SessionDataList& session_list)
	{
		QLOG_PRO(L"local session list: count :{0} - unread :{1}") << session_list.count_ << session_list.unread_count_;
		SessionService::GetInstance()->OnQuerySessionList(session_list.sessions_);
	}
}
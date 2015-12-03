#include "session_callback.h"
#include "module/session/session_manager.h"
#include "export/nim_ui_session_list_manager.h"


namespace nim_comp
{
void TalkCallback::OnReceiveMsgCallback(const nim::IMMessage& message)
{
	QLOG_PRO(L"OnReceiveMsgCallback: {0}") << message.client_msg_id_;
	std::string id = GetSessionId(message);
	//会话窗口
	if (message.feature_ == nim::kNIMMessageFeatureDefault)
	{
		if (message.type_ == nim::kNIMMessageTypeNotification)
		{
			SessionForm* session = SessionManager::GetInstance()->Find(id);
			if (session)
			{
				session->AddNewMsg(message, false);
			}
		}
		else
		{
			SessionManager::GetInstance()->AddNewMsg(message);
		}
	}
	else if (message.feature_ == nim::kNIMMessageFeatureSyncMsg || message.feature_ == nim::kNIMMessageFeatureRoamMsg)
	{
		SessionForm* session = SessionManager::GetInstance()->Find(id);
		if (session)
		{
			session->AddNewMsg(message, false);
		}
	}
	else if (message.feature_ == nim::kNIMMessageFeatureCustomizedMsg)
	{
		SessionForm* session = SessionManager::GetInstance()->Find(id);
		if (session)
		{
			session->AddNewMsg(message, false);
		}
	}
}

void TalkCallback::OnSendMsgCallback(const nim::SendMessageArc& arc)
{
	QLOG_APP(L"OnSendMsgCallback: id={0} msg_id={1} code={2}") << arc.talk_id_ << arc.msg_id_ << arc.rescode_;

	SessionForm* session_form = SessionManager::GetInstance()->Find(arc.talk_id_);
	if (session_form)
		session_form->OnSendMsgCallback(arc.msg_id_, arc.rescode_);

	SessionManager::GetInstance()->RemoveFileUpProgressCb(arc.talk_id_);
}

void TalkCallback::OnSendCustomSysmsgCallback(const nim::SendMessageArc& arc)
{
	if (arc.rescode_ != nim::kNIMResSuccess)
	{
		QLOG_ERR(L"OnSendCustomSysmsgCallback: id={0} msg_id={1} code={2}") << arc.talk_id_ << arc.msg_id_ << arc.rescode_;
	}
}

void TalkCallback::OnQueryMsgCallback(nim::NIMResCode code, const std::string& query_id, nim::NIMSessionType query_type, const nim::QueryMsglogResult& result)
{
	QLOG_APP(L"query end: id={0} type={1} code={2}") <<query_id <<query_type <<code;

	std::vector<nim::IMMessage> vec;
	for each (auto msg in result.msglogs_)
	{
		vec.push_back(msg);
	}

	SessionForm* session_form = SessionManager::GetInstance()->Find(query_id);
	if (session_form)
		session_form->ShowMsgs(vec);
}

void TalkCallback::OnQuerySessionListCallback(int unread_count, const nim::SessionDataList& session_list)
{
	QLOG_PRO(L"local session list: count :{0} - unread :{1}") << session_list.count_ << session_list.unread_count_;
	nim_ui::SessionListManager::GetInstance()->LoadSessionList(session_list.sessions_);
}

}
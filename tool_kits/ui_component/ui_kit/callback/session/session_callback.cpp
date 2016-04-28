#include "session_callback.h"
#include "module/session/session_manager.h"
#include "export/nim_ui_session_list_manager.h"


namespace nim_comp
{
void TalkCallback::OnReceiveMsgCallback(const nim::IMMessage& message)
{
	QLOG_PRO(L"OnReceiveMsgCallback: {0}") << message.client_msg_id_;
	//QLOG_PRO(L"OnReceiveMsgCallback ext: {0}") << message.msg_setting_.server_ext_.toStyledString();
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

void TalkCallback::OnReceiveMsgsCallback(const std::list<nim::IMMessage>& messages)
{
	//例子 开发者可以根据需求实现(比如为了减少UI刷新频率，只在最后一条消息收到后刷新UI等)
	for (auto msg : messages)
	{
		OnReceiveMsgCallback(msg);
	}
}

void TalkCallback::OnSendMsgCallback(const nim::SendMessageArc& arc)
{
	QLOG_APP(L"OnSendMsgCallback: id={0} msg_id={1} code={2}") << arc.talk_id_ << arc.msg_id_ << arc.rescode_;

	//测试用代码
// 	nim::MsgLog::UpdateLocalExtAsync(arc.msg_id_, arc.msg_id_, [arc](nim::NIMResCode res_code, const std::string& msg_id){
// 		QLOG_APP(L"UpdateLocalExtAsync: id={0} msg_id={1} code={2} local={3}") << arc.talk_id_ << msg_id << res_code << arc.msg_id_;
// 		nim::MsgLog::QueryMsgByIDAysnc(arc.msg_id_, [arc](nim::NIMResCode res_code, const std::string& msg_id, const nim::IMMessage& msg){
// 			QLOG_APP(L"QueryMsgByIDAysnc: id={0} msg_id={1} code={2}, local:{3}") << arc.talk_id_ << msg_id << res_code << msg.local_ext_;
// 		});
// 	});

	SessionForm* session_form = SessionManager::GetInstance()->Find(arc.talk_id_);
	if (session_form)
		session_form->OnSendMsgCallback(arc.msg_id_, arc.rescode_, arc.msg_timetag_);

	SessionManager::GetInstance()->RemoveFileUpProgressCb(arc.talk_id_);
}

void TalkCallback::OnMsgStatusChangedCallback(const nim::MessageStatusChangedResult& res)
{
	QLOG_APP(L"TalkCallback：：OnMsgStatusChangedCallback");
	for (auto res : res.results_)
	{
		SessionForm *form = SessionManager::GetInstance()->Find(res.talk_id_);
		if (form)
			form->OnMsgStatusChangedCallback(res.talk_id_, res.msg_timetag_, res.status_);
	}
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
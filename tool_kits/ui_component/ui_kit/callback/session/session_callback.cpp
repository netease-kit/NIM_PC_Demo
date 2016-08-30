#include "session_callback.h"
#include "module/session/session_manager.h"
#include "export/nim_ui_session_list_manager.h"
#include "export/nim_ui_window_manager.h"

namespace nim_comp
{
void TalkCallback::OnReceiveMsgCallback(const nim::IMMessage& message)
{
	QLOG_PRO(L"OnReceiveMsgCallback: {0} from client:{1}") << message.client_msg_id_ << message.readonly_sender_client_type_;
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

void TalkCallback::OnReceiveRecallMsgCallback(nim::NIMResCode code, const std::list<nim::RecallMsgNotify>& message)
{
	for (auto notify : message)
	{
		UTF8String talk_id;
		if (notify.session_type_ == nim::kNIMSessionTypeTeam)
			talk_id = notify.to_id_;
		else
			talk_id = notify.from_id_ == LoginManager::GetInstance()->GetAccount() ? notify.to_id_ : notify.from_id_;
		SessionForm* session = SessionManager::GetInstance()->Find(talk_id);
		if (session)
		{
			session->RecallMsg(code, notify);
		}
		else
		{
			if (code != nim::kNIMResSuccess)
			{
				std::wstring toast = nbase::StringPrintf(L"recall msg error, code:%d, id:%s", code, nbase::UTF8ToUTF16(notify.msg_id_).c_str());
				nim_ui::ShowToast(toast, 5000);
				return;
			}

			//撤回本地不存在的消息的通知不在消息流中插入通知
			if (!notify.msglog_exist_)
				return;

			std::wstring notify_text;
			if (notify.from_id_ == LoginManager::GetInstance()->GetAccount())
			{
				notify_text = L"我撤回了一条消息";
			}
			else
			{
				if (notify.session_type_ == nim::kNIMSessionTypeP2P)
				{
					notify_text = L"对方撤回了一条消息";
				}
				else
				{
					auto info = nim::Team::QueryTeamMemberBlock(talk_id, notify.from_id_);
					UTF8String name = info.GetNick();
					if (name.empty())
					{
						nim::UserNameCard name_card;
						UserService::GetInstance()->GetUserInfo(notify.from_id_, name_card);
						name = name_card.GetName();
					}
					if (name.empty())
						name = notify.from_id_;
					notify_text = nbase::UTF8ToUTF16(name) + L" 撤回了一条消息";
				}
			}

			nim::IMMessage msg;
			msg.timetag_ = notify.notify_timetag_;
			msg.client_msg_id_ = QString::GetGUID();
			msg.receiver_accid_ = talk_id;
			msg.session_type_ = notify.session_type_;
			msg.sender_accid_ = notify.from_id_;
			msg.status_ = nim::kNIMMsgLogStatusSent;
			msg.type_ = nim::kNIMMessageTypeText;
			Json::Value values;
			values["comment"] = "is_recall_notification";
			values["notify_from"] = notify.from_id_;
			msg.attach_ = values.toStyledString();
			msg.content_ = nbase::UTF16ToUTF8(notify_text);
			msg.msg_setting_.push_need_badge_ = nim::BS_FALSE; //设置会话列表不需要计入未读数
			nim::MsgLog::WriteMsglogToLocalAsync(talk_id, msg, true, nim::MsgLog::WriteMsglogCallback());
		}
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
	QLOG_APP(L"TalkCallback: OnMsgStatusChangedCallback");
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
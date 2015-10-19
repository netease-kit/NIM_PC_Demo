#include "session_callback.h"
#include "module/session/session_manager.h"
#include "export/nim_ui_session_list.h"


namespace nim_comp
{
void TalkCallback::OnReceiveMsgCallback(const std::string& json_str)
{
	QLOG_PRO(L"OnReceiveMsgCallback: {0}") << json_str;

	Json::Value value;
	Json::Reader reader;
	if (reader.parse(json_str, value))
	{
		int code = value[nim::kNIMMsgKeyLocalRescode].asInt();
		int feature = value[nim::kNIMMsgKeyLocalMsgFeature].asInt();

		Json::Value json = value[nim::kNIMMsgKeyLocalReceiveMsgContent];
		json[nim::kNIMMsgKeyLocalRescode] = code;
		json[nim::kNIMMsgKeyLocalMsgFeature] = feature;

		MsgData msg;
		JsonToMsg(json, msg);
		std::string id = GetSessionId(msg);

		//会话窗口
		if (msg.feature == nim::kNIMMessageFeatureDefault)
		{
			if (msg.msg_type == nim::kNIMMessageTypeNotification)
			{
				SessionForm* session = SessionManager::GetInstance()->Find(id);
				if (session)
				{
					session->AddNewMsg(msg, false);
				}
			}
			else
			{
				SessionManager::GetInstance()->AddNewMsg(msg);
			}
		}
		else if (msg.feature == nim::kNIMMessageFeatureSyncMsg || msg.feature == nim::kNIMMessageFeatureRoamMsg)
		{
			SessionForm* session = SessionManager::GetInstance()->Find(id);
			if (session)
			{
				session->AddNewMsg(msg, false);
			}
		}
		else if (msg.feature == nim::kNIMMessageFeatureCustomizedMsg)
		{
			SessionForm* session = SessionManager::GetInstance()->Find(id);
			if (session)
			{
				session->AddNewMsg(msg, false);
			}
		}
	}
	else
	{
		QLOG_ERR(L"parse receive msg fail: {0}") << json_str;
	}
}

void TalkCallback::OnSendMsgCallback( const std::string& str )
{
	Json::Value msg;
	Json::Reader reader;
	if( reader.parse(str, msg) )
	{
		//会话窗口
		std::string id = msg[nim::kNIMSendArcKeyTalkId].asString();
		std::string msg_id = msg[nim::kNIMSendArcKeyMsgId].asString();
		int code = msg[nim::kNIMSendArcKeyRescode].asInt();

		QLOG_APP(L"OnSendMsgCallback: id={0} msg_id={1} code={2}") << id << msg_id << code;

		SessionForm* session_form = SessionManager::GetInstance()->Find(id);
		if (session_form)
			session_form->OnSendMsgCallback(msg_id, code);

		SessionManager::GetInstance()->RemoveFileUpProgressCb(id);
	}
	else
	{
		QLOG_ERR(L"parse send msg fail: {0}") <<str;
	}
}

void TalkCallback::OnSendCustomSysmsgCallback(const std::string& str)
{
	Json::Value msg;
	Json::Reader reader;
	if (reader.parse(str, msg))
	{
		int code = msg[nim::kNIMSendSysMsgArcKeyRescode].asInt();

		if (code != nim::kNIMResSuccess)
		{
			std::string id = msg[nim::kNIMSendSysMsgArcKeyTalkId].asString();
			std::string msg_id = msg[nim::kNIMSendSysMsgArcKeyMsgId].asString();
			QLOG_ERR(L"OnSendCustomSysmsgCallback: id={0} msg_id={1} code={2}") << id << msg_id << code;
		}
	}
	else
	{
		QLOG_ERR(L"parse send sysmsg fail: {0}") << str;
	}
}

void TalkCallback::OnQueryMsgCallback(nim::NIMResCode code, const std::string& query_id, nim::NIMSessionType query_type, const std::string& result)
{
	QLOG_APP(L"query end: id={0} type={1} code={2}") <<query_id <<query_type <<code;

	Json::Value value;
	Json::Reader reader;
	if(reader.parse(result, value))
	{
		std::vector<MsgData> vec;

		Json::Value& msg = value[nim::kNIMMsglogQueryKeyContent];
		int n = msg.size();
		for(int i = 0; i < n; i++)
		{
			MsgData md;
			JsonToMsg(msg[i], md);
			vec.push_back(md);
		}

		SessionForm* session_form = SessionManager::GetInstance()->Find(query_id);
		if (session_form)
			session_form->ShowMsgs(vec);
	}
	else
	{
		QLOG_ERR(L"parse msglog fail: {0}") <<result;
	}
}

void TalkCallback::OnQuerySessionListCallback(int unread_count, const std::string& json )
{
	QLOG_PRO(L"local session list: {0}") <<json;
	nim_ui::SessionListManager::GetInstance()->LoadSessionList(json);
}

}
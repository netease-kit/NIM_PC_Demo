#include "p2p_callback.h"
#include "module/session/session_manager.h"
#include "gui/session/msg_record.h"
#include "module/session/transfer_file_manager.h"

namespace nim_comp
{

void P2PCallback::OnTransferFileRequest(const RemoteFlagType remote_flag, TransferFileSessionID session_id, const char* file_info)
{
	SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(remote_flag));

	if (session_box)
	{
		nim::IMMessage msg;
		Json::Value root;
		Json::Value json_file_info;
		Json::Reader reader;

		root["type"] = CustomMsgType_TransferFile;
		if (reader.parse(file_info, json_file_info))
		{
			root["params"]["file_info"] = json_file_info;
			root["session_id"] = session_id;
		}

		// 注意这里构建消息自己是接收端，对端是 remote_flag
		msg.type_ = nim::kNIMMessageTypeCustom;
		msg.receiver_accid_ = LoginManager::GetInstance()->GetAccount();
		msg.sender_accid_ = remote_flag;
		msg.client_msg_id_ = session_id;
		msg.msg_setting_.resend_flag_ = nim::BS_FALSE;
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
		msg.status_ = nim::kNIMMsgLogStatusSent;

		msg.msg_setting_.server_history_saved_ = nim::BS_FALSE;//不存云端
		msg.msg_setting_.roaming_ = nim::BS_FALSE;//不漫游
		msg.msg_setting_.self_sync_ = nim::BS_FALSE;//不进行多端同步
		msg.msg_setting_.need_push_ = nim::BS_FALSE;//不推送
		msg.msg_setting_.push_need_badge_ = nim::BS_FALSE;//不计数
		msg.msg_setting_.need_offline_ = nim::BS_FALSE;//不需要支持离线
		msg.msg_setting_.routable_ = nim::BS_FALSE;//不需要抄送
		msg.attach_ = root.toStyledString();

		session_box->ShowMsg(msg, false, false);
	}
}

bool P2PCallback::SendCommandChannel(const RemoteFlagType remote_flag, const char* const command)
{
	QLOG_APP(L"[P2PCallback::SendCommandChannel] Send new command to {0}, command {1}") << remote_flag << command;

	Json::Value values;
	Json::Reader reader;
	if (reader.parse(command, values))
	{
		nim::IMMessage msg;
		std::string session_id = values["session_id"].asString();
		ASSERT(!session_id.empty());
		// 增加 sub type 类型为使用 P2P 传输文件
		values["type"] = CustomMsgType_TransferFile;

		msg.type_ = nim::kNIMMessageTypeCustom;
		msg.receiver_accid_ = remote_flag;
		msg.sender_accid_ = LoginManager::GetInstance()->GetAccount();
		// 确保发送出去的消息 id 和构建 bubble 使用的消息 id 都是相同的
		// 在加载历史消息时才能根据消息 id 匹配到对应的数据
		msg.client_msg_id_ = session_id;
		msg.msg_setting_.resend_flag_ = nim::BS_FALSE;
		msg.timetag_ = 1000 * nbase::Time::Now().ToTimeT();
		msg.status_ = nim::kNIMMsgLogStatusSending;

		msg.msg_setting_.server_history_saved_ = nim::BS_FALSE;//不存云端
		msg.msg_setting_.roaming_ = nim::BS_FALSE;//不漫游
		msg.msg_setting_.self_sync_ = nim::BS_FALSE;//不进行多端同步
		msg.msg_setting_.need_push_ = nim::BS_FALSE;//不推送
		msg.msg_setting_.push_need_badge_ = nim::BS_FALSE;//不计数
		msg.msg_setting_.need_offline_ = nim::BS_FALSE;//不需要支持离线
		msg.msg_setting_.routable_ = nim::BS_FALSE;//不需要抄送
		msg.attach_ = values.toStyledString();

		nim::Talk::SendMsg(msg.ToJsonString(true));
	}

	return true;
}

void P2PCallback::OnTransferFileSessionStateChangeCallback(const RemoteFlagType remote_flag, TransferFileSessionID session_id, enum TransferFileSessionState state)
{
	QLOG_APP(L"[P2PCallback::TransferFileSessionStateChangeCallback] session id = {0}, state = {1}") << nbase::UTF8ToUTF16(session_id) << state;
	TransferFileManager::GetInstance()->OnStateChangeCallback(session_id, state);
}

void P2PCallback::OnTransferFileProgressCallback(const RemoteFlagType remote_flag, TransferFileSessionID session_id, FilesizeType transferred, FilesizeType total)
{
	TransferFileManager::GetInstance()->OnProgressChangeCallback(session_id, transferred, total);
}

}
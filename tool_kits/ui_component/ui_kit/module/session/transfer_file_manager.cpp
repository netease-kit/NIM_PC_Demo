#include "stdafx.h"
#include "transfer_file_manager.h"

namespace nim_comp {

TransferFileManager::TransferFileManager()
{

}

TransferFileManager::~TransferFileManager()
{

}

bool TransferFileManager::AddItem(const std::string& session_box_id, std::string& transfer_file_session_id, MsgBubbleItem* msg_bubble)
{
	auto iterator = transfer_session_list_.find(transfer_file_session_id);
	if (iterator == transfer_session_list_.end())
	{
		TransferFileInfo transfer_file_info;
		transfer_file_info.session_box_ = session_box_id;
		transfer_file_info.bubble_weakflag_ = msg_bubble->GetWeakFlag();
		transfer_file_info.bubble_ = msg_bubble;
		// 如果不存在则插入新值
		transfer_session_list_[transfer_file_session_id] = transfer_file_info;
	}
	else
	{
		return false;
	}

	return true;
}

bool TransferFileManager::HasTransferTask()
{
	return transfer_session_list_.size() != 0;
}

bool TransferFileManager::HasTransferTask(const std::string& session_id)
{
	for (auto iterator : transfer_session_list_)
	{
		TransferFileInfo& transfer_file_info = iterator.second;
		if (transfer_file_info.session_box_ == session_id)
		{
			return true;
		}
	}

	return false;
}

void TransferFileManager::RemoveAllTaskBySessionBoxId(const std::string& session_id)
{
	for (auto iterator = transfer_session_list_.begin(); iterator != transfer_session_list_.end(); )
	{
		std::string			transfer_file_session_id	= iterator->first;
		TransferFileInfo	transfer_file_info			= iterator->second;

		if (transfer_file_info.session_box_ == session_id)
		{
			iterator = transfer_session_list_.erase(iterator);

			if (transfer_file_info.bubble_->GetSenderId() == session_id)
			{
				nim_p2p::NimP2PDvelopKit::GetInstance()->CancelReceiveFile(const_cast<TransferFileSessionID>(transfer_file_session_id.c_str()));
			}
			else
			{
				nim_p2p::NimP2PDvelopKit::GetInstance()->CancelTransferFile(const_cast<TransferFileSessionID>(transfer_file_session_id.c_str()));
			}
		}
		else
		{
			iterator++;
		}
	}
}

void TransferFileManager::OnStateChangeCallback(const std::string& transfer_file_session_id, enum TransferFileSessionState state)
{
	auto iterator = transfer_session_list_.find(transfer_file_session_id);
	if (iterator != transfer_session_list_.end())
	{
		TransferFileInfo& transfer_file_info = iterator->second;
		if (!transfer_file_info.bubble_weakflag_.expired())
		{
			auto msg_bubble = dynamic_cast<MsgBubbleTransferFile*>(transfer_file_info.bubble_);
			msg_bubble->SetBubbleStatus(state);
		}
		if (IsTransferFileSessionFinalState(state))
		{
			transfer_session_list_.erase(iterator);
		}
	}
}

void TransferFileManager::OnProgressChangeCallback(const std::string& transfer_file_session_id, int transferred, int total)
{
	auto iterator = transfer_session_list_.find(transfer_file_session_id);
	if (iterator != transfer_session_list_.end())
	{
		TransferFileInfo& transfer_file_info = iterator->second;
		if (!transfer_file_info.bubble_weakflag_.expired())
		{
			auto msg_bubble = dynamic_cast<MsgBubbleTransferFile*>(transfer_file_info.bubble_);
			msg_bubble->OnDownloadFileProgressCallback(total, transferred);
		}
	}
}

bool TransferFileManager::IsTransferFileSessionFinalState(TransferFileSessionState state)
{
	switch (state)
	{
	case TransferFileSessionState_Succeeded:
	case TransferFileSessionState_Failed:
	case TransferFileSessionState_CMDTimeout:
	case TransferFileSessionState_ReceiverRejected:
	case TransferFileSessionState_Rejected:
	case TransferFileSessionState_ReceiverCancel:
	case TransferFileSessionState_SenderCancel:
		return true;
	default:
		return false;
	}
}

}
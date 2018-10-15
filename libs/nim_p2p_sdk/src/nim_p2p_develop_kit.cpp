#include "nim_p2p_develop_kit.h"
#include <string>

namespace nim_p2p {

NimP2PDvelopKit::NimP2PDvelopKit()
{
}

NimP2PDvelopKit::~NimP2PDvelopKit()
{
}

bool NimP2PDvelopKit::Init(const std::wstring& dll_path,
	OnTransferFileRequest trans_file_request_callback,
	TransferFileSessionStateChangeCallback trans_file_state_change_callback,
	TransferFileProgressCallback trans_file_progress_callback,
	SendCommandChannel send_command_channel, const std::string& json_ext/* = ""*/)
{
	std::wstring dll_file(dll_path);
	if (!dll_file.empty() && (*dll_file.rbegin() != L'/' && *dll_file.rbegin() != L'\\'))
	{
		dll_file.append(1, L'/');
	}

#ifdef _DEBUG
	dll_file.append(kSDKP2PDll_d);
#else
	dll_file.append(kSDKP2PDll);
#endif

#ifdef _WIN32
	instance_p2p_ = ::LoadLibraryW(dll_file.c_str());
	DWORD dwError = GetLastError();
#else
	instance_p2p_ = dlopen(dll_file.c_str(), )
#endif

	if (instance_p2p_ == nullptr)
	{
		return false;
	}

	FunInitType f_init = Function<FunInitType>("Init");
	if (f_init)
	{
		receive_command_channel = f_init(trans_file_request_callback, 
			trans_file_state_change_callback, 
			trans_file_progress_callback, 
			send_command_channel, json_ext.c_str());
	}
	else
	{
		return false;
	}

	return true;
}

void NimP2PDvelopKit::UnInit()
{
	if (!instance_p2p_)
	{
		return;
	}

	FunClearType f_clear = Function<FunClearType>("Clear");
	if (f_clear)
	{
		f_clear();
	}

#ifdef _WIN32
	::FreeLibrary(instance_p2p_);
#else
	dlclose(instance_p2p_);
#endif
}

void NimP2PDvelopKit::RegTransferFileProcProg(const TransferFileProcProgRegInfo& info)
{
	if (!instance_p2p_)
	{
		return;
	}

	FunRegTransferFileProcProgType f_reg_transfer_file_proc_prog = 
		Function<FunRegTransferFileProcProgType>("RegTransferFileProcProg");
	if (f_reg_transfer_file_proc_prog)
	{
		f_reg_transfer_file_proc_prog(info);
	}
}

TransferFileSessionID NimP2PDvelopKit::TransferFile(const RemoteFlagType remote_flag, const FILE_INFO& file_info)
{
	if (!instance_p2p_)
	{
		return nullptr;
	}

	TransferFileSessionID transfer_session_id = nullptr;
	std::string file_info_json = "";
	std::string file_path = file_info.kFilePath;
	//nbase::StringReplaceAll("\\", "/", file_path);
	file_info_json += "{";
	file_info_json += R"("file_path":")" + file_path + R"(",)";
	file_info_json += R"("file_name":")" + file_info.kFileName + R"(",)";
	file_info_json += R"("file_size":)" + std::to_string(file_info.kFileSize) + R"(,)";
	file_info_json += R"("file_md5":")"  + file_info.kFileMD5 + R"(",)";
	file_info_json += R"("user_ext":")"  + file_info.kFileUserExt + R"("})";

	FunTransferFileType f_transfer_file = Function<FunTransferFileType>("TransferFile");
	if (f_transfer_file)
	{
		transfer_session_id = f_transfer_file(remote_flag, file_info_json.c_str());
	}

	return transfer_session_id;
}

bool NimP2PDvelopKit::CancelTransferFile(const TransferFileSessionID& session_id)
{
	if (!instance_p2p_)
	{
		return false;
	}

	FunCancelTransferFileType f_cancel_transfer_file = Function<FunCancelTransferFileType>("CancelTransferFile");
	if (f_cancel_transfer_file)
	{
		f_cancel_transfer_file(session_id);
		return true;
	}
	else
	{
		return false;
	}
}

bool NimP2PDvelopKit::ReceiveFile(const TransferFileSessionID& session_id, const std::string& file_path)
{
	if (!instance_p2p_)
	{
		return false;
	}

	FunReceiveFileType f_receive_file = Function<FunReceiveFileType>("ReceiveFile");
	if (f_receive_file)
	{
		f_receive_file(session_id, file_path.c_str());
		return true;
	}
	else
	{
		return false;
	}
}

bool NimP2PDvelopKit::RejectReceiveFile(const TransferFileSessionID& session_id)
{
	if (!instance_p2p_)
	{
		return false;
	}

	FunRejectReceiveFileType f_reject_receive_file = Function<FunRejectReceiveFileType>("RejectReceiveFile");
	if (f_reject_receive_file)
	{
		f_reject_receive_file(session_id);
		return true;
	}
	else
	{
		return false;
	}
}

bool NimP2PDvelopKit::CancelReceiveFile(const TransferFileSessionID& session_id)
{
	if (!instance_p2p_)
	{
		return false;
	}

	FunCancelReceiveFileType f_cancel_receive_file = Function<FunCancelReceiveFileType>("CancelReceiveFile");
	if (f_cancel_receive_file)
	{
		f_cancel_receive_file(session_id);
		return true;
	}
	else
	{
		return false;
	}
}

void NimP2PDvelopKit::OnReceiveChannelCommand(const RemoteFlagType& remote_flag, const std::string& command)
{
	if (!instance_p2p_)
	{
		return;
	}

	if (receive_command_channel)
	{
		receive_command_channel(remote_flag, command.c_str());
	}
}

}

#ifndef NIM_P2P_SDK_DEF_H
#define NIM_P2P_SDK_DEF_H
#include "nim_p2p_def.h"
#include "nim_p2p_sdk_file_def.h"
#include "nim_p2p_sdk_proc_prog_def.h"
#ifdef __cplusplus
extern"C"
{
#endif
typedef OnChannelCommand (*FunInitType)(OnTransferFileRequest trans_file_request_callback,
	TransferFileSessionStateChangeCallback trans_file_state_change_callback,
	TransferFileProgressCallback trans_file_progress_callback,
	SendCommandChannel channel, const char* json_exten );
typedef void (*FunClearType)();
typedef void (*FunRegTransferFileProcProgType)(const TransferFileProcProgRegInfo& info);

typedef TransferFileSessionID (*FunTransferFileType)(const RemoteFlagType remote_flag,const char* file_info);
typedef void (*FunCancelTransferFileType)(TransferFileSessionID session_id);

typedef void (*FunReceiveFileType)(TransferFileSessionID session_id, const char* file_path);
typedef void (*FunRejectReceiveFileType)(TransferFileSessionID session_id);
typedef void (*FunCancelReceiveFileType)(TransferFileSessionID session_id);
typedef void (*FunCloseTransferFileSessionType)(TransferFileSessionID session_id);

#ifdef __cplusplus
}
#endif
#endif //NIM_P2P_SDK_DEF_H

#ifndef NIM_P2P_SDK_FILE_DEF_H
#define NIM_P2P_SDK_FILE_DEF_H
#include "nim_p2p_def.h"
#ifdef __cplusplus
extern"C"
{
#endif
enum TransferFileState
{
    TransferFileState_NULL = 0,
    TransferFileState_Wait,//等待开始
    TransferFileState_Consulting,//协调传输方案中
    TransferFileState_Transferring,//传输中
    TransferFileState_Succeeded,//已成功
    TransferFileState_Failed,//已失败
    TransferFileState_Aborted//已中止
};
typedef void(*TransferFileStateChangeCallback)(RemoteFlagType remote_flag,TransferFileSessionID session_id, TransferFileProcProgID proc_id, enum TransferFileState state);
#ifdef __cplusplus
}
#endif
#endif //NIM_P2P_SDK_FILE_DEF_H

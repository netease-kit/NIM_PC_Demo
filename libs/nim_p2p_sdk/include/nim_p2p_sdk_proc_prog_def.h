#ifndef NIM_P2P_SDK_PROC_PROG_DEF_H
#define NIM_P2P_SDK_PROC_PROG_DEF_H
#include "nim_p2p_def.h"
#include "nim_p2p_sdk_file_def.h"
#ifdef __cplusplus
extern"C"
{
#endif
typedef void(*ProcProgCommandChannel)(const TransferFileSessionID,const RemoteFlagType, const char*);
typedef struct
{
    TransferFileProcProgID proc_prog_id_;
    void (*Init)(TransferFileProcProgID,const char*, const char*,ProcProgCommandChannel, TransferFileStateChangeCallback, TransferFileProgressCallback);
    void (*Run)(TransferFileProcProgID);
    void (*Cancel)(TransferFileProcProgID);
    void (*TransparentMessage)(TransferFileProcProgID,const char*);
}TransferFileProcProg;
typedef struct
{
    char* procprog_flag_;
    TransferFileProcProg* (*TransferFileProcProgCreator)();
    void(*TransferFileProcProgRelease)(TransferFileProcProgID);
} TransferFileProcProgRegInfo;
#ifdef __cplusplus
}
#endif
#endif //NIM_P2P_SDK_DEF_H

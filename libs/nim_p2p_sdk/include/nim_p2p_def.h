#ifndef NIM_P2P_DEF_H
#define NIM_P2P_DEF_H
#ifdef __cplusplus
extern"C"
{
#endif
typedef char* TransferFileSessionID;//传输文件会话ID
typedef void* TransferFileProcProgID;//传输文件方案实例ID
typedef char* RemoteFlagType;//对端信息(在IM中一般是accid)
typedef int FilesizeType;
enum TransferFileSessiontype
{
    TransferFileSessiontype_Sender = 0,//发送方
    TransferFileSessiontype_Receiver = 1,//接收方
};
enum TransferFileSessionState
{
    TransferFileSessionState_NULL = 0,
    TransferFileSessionState_Wait,//等待开始
    TransferFileSessionState_Consulting,//协商传输方案中
    TransferFileSessionState_Transferring,//传输中
    TransferFileSessionState_Succeeded,//已成功
    TransferFileSessionState_Failed,//已失败
	TransferFileSessionState_CMDTimeout,//信令超时，对方可能已离线

    TransferFileSessionState_ReceiverRejected,//接收方拒绝,
    TransferFileSessionState_Rejected,//已拒绝

    TransferFileSessionState_ReceiverCancel,//接收方取消,
    TransferFileSessionState_SenderCancel,//发送方取消

};

typedef void(*OnTransferFileRequest)(const RemoteFlagType remote_flag,TransferFileSessionID session_id,const char* file_info);
typedef void(*TransferFileSessionStateChangeCallback)(const RemoteFlagType remote_flag,TransferFileSessionID session_id, enum TransferFileSessionState state);
typedef void(*TransferFileProgressCallback)(const RemoteFlagType remote_flag,TransferFileSessionID session_id, FilesizeType total, FilesizeType transferred);
typedef void(*OnChannelCommand)(const RemoteFlagType remote_flag,const char* const);
typedef bool(*SendCommandChannel)(const RemoteFlagType, const char*);

#ifdef __cplusplus
}
#endif
#endif //NIM_P2P_DEF_H

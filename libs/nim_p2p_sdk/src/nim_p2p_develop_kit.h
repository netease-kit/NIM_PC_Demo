#ifndef NIM_P2P_DEVELOP_KIT_H
#define NIM_P2P_DEVELOP_KIT_H

#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h> // on Linux or macOS
#endif

// SDK header include
#include "include/nim_p2p_def.h"
#include "include/nim_p2p_sdk_def.h"
#include "include/nim_p2p_sdk_file_def.h"
#include "include/nim_p2p_sdk_proc_prog_def.h"

namespace nim_p2p {

static const std::wstring kSDKP2PDll_d = L"nim_p2p_sdk_d.dll";
static const std::wstring kSDKP2PDll = L"nim_p2p_sdk.dll";

#ifdef _WIN32
typedef HMODULE DYLIB_HINSTANCE;
#else
typedef void* DYLIB_HINSTANCE;
#endif

typedef struct
{
	std::string kFilePath;
	std::string kFileName;
	std::string kFileMD5;
	std::string kFileUserExt;
	__int64		kFileSize;
} FILE_INFO, *PFILE_INFO;

class NimP2PDvelopKit
{

public:
	NimP2PDvelopKit();
	~NimP2PDvelopKit();

	/**
	 * 将该类设置为单例
	 */
	static NimP2PDvelopKit* GetInstance()
	{
		static NimP2PDvelopKit nim_p2p_develop_kit;
		return &nim_p2p_develop_kit;
	}
	NimP2PDvelopKit(const NimP2PDvelopKit&) = delete;
	NimP2PDvelopKit& operator=(const NimP2PDvelopKit&) = delete;

	/**
	 * 初始化 p2p 库
	 * @param[in] dll_path 库文件路径
	 * @param[in] trans_file_request_callback 接收到文件传输的回调
	 * @param[in] channel 发送命令通道，由上层定义
	 * @param[in] json_ext json 拓展字段，暂时没有使用
	 * @result 返回是否初始化成功
	 */
	bool Init(const std::wstring& dll_path,
		OnTransferFileRequest trans_file_request_callback,
		TransferFileSessionStateChangeCallback trans_file_state_change_callback,
		TransferFileProgressCallback trans_file_progress_callback,
		SendCommandChannel send_command_channel, const std::string& json_ext = "");

	/**
	 * 释放 p2p 动态库
	 */
	void UnInit();

	/**
	 * 注册一个传输方案
	 * @param[in] info 由上层定义的传输方案结构
	 */
	void RegTransferFileProcProg(const TransferFileProcProgRegInfo& info);

	/**
	 * 发起传输一个文件
	 * @param[in] remote_flag 远端标志，IM 为 accid
	 * @param[in] file_info 要发送的文件信息结构
	 */
	TransferFileSessionID TransferFile(const RemoteFlagType remote_flag, const FILE_INFO& file_info);

	/**
	* 发送方取消传输文件
	* @param[in] session_id 文件传输的会话ID
	*/
	bool CancelTransferFile(const TransferFileSessionID& session_id);

	/**
	 * 开始接收一个文件
	 * @param[in] session_id 由 Init 方法中初始化的 OnTransferFileRequest 回调接收到的文件传输消息中携带的 TransferFileSessionID
	 * @param[in] file_path 要保存的文件路径
	 * @result 返回接收命令执行是否成功
	 */
	bool ReceiveFile(const TransferFileSessionID& session_id, const std::string& file_path);

	/**
	* 拒绝接收文件
	* @param[in] session_id 文件传输的会话ID
	*/
	bool RejectReceiveFile(const TransferFileSessionID& session_id);


	/**
	* 接收方取消接收文件
	* @param[in] session_id 文件传输的会话ID
	*/
	bool CancelReceiveFile(const TransferFileSessionID& session_id);

public:
	/**
	 * 接收到 IM 消息传递给 SDK
	 * @param[in] remote_flag 远端信息
	 * @param[in] command 要传递给 SDK 的命令
	 */
	void OnReceiveChannelCommand(const RemoteFlagType& remote_flag, const std::string& command);

private:
    template <typename F>
    F Function(const char* function_name)
    {
#ifdef _WIN32
        F f = (F) ::GetProcAddress((HMODULE)instance_p2p_, function_name);
#else
        F f = (F) dlsym(instance_p2p_, function_name);
#endif
        return f;
    }

private:
	DYLIB_HINSTANCE		instance_p2p_			= nullptr;
	OnChannelCommand	receive_command_channel	= nullptr;
};

}

#endif // NIM_P2P_DEVELOP_KIT_H

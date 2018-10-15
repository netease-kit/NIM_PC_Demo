#pragma once
#include "include/nim_p2p_include.h"

namespace nim_comp
{
/** @class P2PCallback
  * @brief p2p 请求事件回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @date 2018/9/15
  */
class P2PCallback
{
public:
	/**
	* p2p 收到传输文件请求的回调。
	* @param[in] remote_flag 对端信息（对应 IM 账户）
	* @param[in] session_id 会话 id（与 IM 不是一个会话）
	* @param[in] file_info 文件信息
	* @return void	无返回值
	*/
	static void OnTransferFileRequest(const RemoteFlagType remote_flag, TransferFileSessionID session_id, const char* file_info);

	/**
	* p2p 发送命令通道回调。
	* @param[in] remote_flag 对端信息（对应 IM 账户）
	* @param[in] command 命令内容
	* @return void	无返回值
	*/
	static bool SendCommandChannel(const RemoteFlagType remote_flag, const char* const command);

	/**
	* 文件下载完毕的回调函数
	* @param[in] remote_flag 对端信息（对应 IM 账户）
	* @param[in] session_id 文件传送的会话信息
	* @param[in] state 回话状态改变信息
	* @return void 无返回值
	*/
	static void OnTransferFileSessionStateChangeCallback(const RemoteFlagType remote_flag, TransferFileSessionID session_id, enum TransferFileSessionState state);

	/**
	* 文件下载进度的回调函数
	* @param[in] remote_flag 对端信息（对应 IM 账户）
	* @param[in] session_id	文件传送的会话信息
	* @param[in] total	总大小
	* @param[in] transferred 已传输大小
	* @return void 无返回值
	*/
	static void OnTransferFileProgressCallback(const RemoteFlagType remote_flag, TransferFileSessionID session_id, FilesizeType total, FilesizeType transferred);
};
}
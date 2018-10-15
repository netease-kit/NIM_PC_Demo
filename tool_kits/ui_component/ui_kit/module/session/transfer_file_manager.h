#pragma once
#include "base/memory/singleton.h"
#include "gui/session/control/bubbles/bubble_item.h"
#include "gui/session/control/bubbles/bubble_transferfile.h"

namespace nim_comp {

class TransferFileInfo;

class TransferFileManager
{
public:
	TransferFileManager();
	~TransferFileManager();

	SINGLETON_DEFINE(TransferFileManager);

	/**
	 * 添加一个传输任务到管理列表中
	 * param [in] session_id Session box ID
	 * param [in] msg_bubble 文件传输的 bubble 指针
	 */
	bool AddItem(const std::string& session_box_id, std::string& transfer_file_session_id, MsgBubbleItem* msg_bubble);

	/**
	 * 检查所有 session box 中是否含有传输中的任务
	 * return true 有，false 无
	 */
	bool HasTransferTask();

	/**
	* 检查所有某个 session box 是否有传输中的任务
	* param [in] session_id
	* return true 有，false 无
	*/
	bool HasTransferTask(const std::string& session_id);

	/**
	* 删除某个 session box 下的所有传输任务
	* param [in] session_id
	* 无返回值
	*/
	void RemoveAllTaskBySessionBoxId(const std::string& session_id);

	/**
	* 文件下载完毕的回调函数
	* @param[in] session_id 文件传送的会话信息
	* @param[in] state 回话状态改变信息
	* @return void 无返回值
	*/
	void OnStateChangeCallback(const std::string& transfer_file_session_id, enum TransferFileSessionState state);

	/**
	* 文件下载进度的回调函数
	* @param[in] session_id	文件传送的会话信息
	* @param[in] total	总大小
	* @param[in] transferred 已传输大小
	* @return void 无返回值
	*/
	void OnProgressChangeCallback(const std::string& transfer_file_session_id, int transferred, int total);
	/**
	* 判断是否为文件传输Session的终态（表示会话已结束）
	* @return void 无返回值
	*/
	bool IsTransferFileSessionFinalState(TransferFileSessionState state);
private:
	std::map<std::string, TransferFileInfo> transfer_session_list_;
};

class TransferFileInfo
{
public:
	std::string						session_box_;		// bubble 所属 session box
	std::weak_ptr<nbase::WeakFlag>	bubble_weakflag_;	// bubble weak flag
	MsgBubbleItem*					bubble_;			// bubble 指针
};

}

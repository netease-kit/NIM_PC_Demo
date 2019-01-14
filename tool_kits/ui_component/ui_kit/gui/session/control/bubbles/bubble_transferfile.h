#pragma once
#include "bubble_item.h"
#include "nim_p2p_develop_kit.h"

namespace nim_comp
{
/** @class MsgBubbleFile
  * @brief 会话窗体中聊天框内的P2P文件直传消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
 */

const std::string kJsonKeyCommand					= "command";
const std::string kJsonKeyTransferFileType			= "type";
const std::string kJsonKeyCancelTransferFile		= "cancel_transfer";
const std::string kJsonKeyCancelReceiveFile			= "transfer_cancel_receive";
const std::string kJsonKeyTransferFileRequest		= "transfer_request";
const std::string kJsonKeySupportedTransferFile		= "supported_transfer_file";
const std::string kJsonKeyTryTransferFileRequest	= "try_transfer_file_request";

class MsgBubbleTransferFile : public MsgBubbleItem
{
public:
	virtual ~MsgBubbleTransferFile();

	/**
	* 初始化控件内部指针
	* @param[in] bubble_right 是否显示到右侧
	* @return void 无返回值
	*/
	virtual void InitControl(bool bubble_right) override;

	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @return void 无返回值
	*/
	virtual void InitInfo(const nim::IMMessage &msg) override;

	/**
	* 设置此消息项的发送状态图标（发送中、发送失败等）
	* @param[in] status 消息状态
	* @return void 无返回值
	*/
	void SetBubbleStatus(TransferFileSessionState status, bool write_db = true);

	/**
	 * 父类继承下来的 bubble 状态管理方法，用来屏蔽或修改某些特定状态
	 * @param[in] status 消息状态
	 * @return void 无返回值
	 */
	virtual void SetMsgStatus(nim::NIMMsgLogStatus status) override;

	/**
	* 处理控件的菜单消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);

	/**
	 * 传输文件进度回调
	 * @param[in] total 传输文件总大小
	 * @param[in] transferred 已经传输的大小
	 */
	void OnDownloadFileProgressCallback(int total, int transferred);

	/**
	 * 返回当前 bubble 绑定的传输文件 sessionId
	 * @return TransferFileSessionID 该 bubble 绑定的传输文件 sessionId
	 */
	TransferFileSessionID GetTransferFileSessionId() { return const_cast<TransferFileSessionID>(transfer_file_session_id_.c_str()); }
protected:

	/** 
	* 把文件大小的数值转换为字符串
	* @param[in] size 文件大小(单位：字节) 
	* @return wstring 文件大小的字符串形式
	*/
	std::wstring GetFileSizeStr(int64_t size);

	/**
	* 根据文件名设置此消息项控件显示的图标
	* @param[in] file_name 文件名 
	* @return bool 返回值true: 设置成功， false: 设置失败
	*/ 
	bool SetFileIcon(const std::string& file_name);

	/**
	* 响应此消息项中包含的子控件的单击消息
	*@param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnEvent(ui::EventArgs* arg);

	/** 
	* 设置此消息项显示的文件上传(下载)进度
	* @param[in] prog_value 进度值（0-100）	
	* @return void 无返回值
	*/
	void SetProgressValue(int prog_value);

	/** 
	* 执行保存文件的操作	(从网络下载到本地)
	* @return void 无返回值
	*/
	void SaveAs();

	/** 
	* 保存文件对话框选择后的回调函数
	* @param[in] ret 是否要保存文件 
	* @param[in] file_path 文件保存路径			
	* @return void 无返回值
	*/
	void SaveFile(BOOL ret, std::wstring file_path);

	/** 
	* 开始保存文件，执行下载文件操作	
	* @return void 无返回值
	*/
	void StartDownload();
	/**
	* 判断是否为文件传输Session的终态（表示会话已结束）
	* @return void 无返回值
	*/
	bool IsTransferFileSessionFinalState(TransferFileSessionState state);
private:
	std::string		local_path_;
	std::string		local_path_temp_;
	std::string		file_name_;
	int64_t			file_size_ = 0;
	std::string		file_path_;
	ui::Box*		msg_file_;

	ui::Button*		file_image_;
	ui::Label*		file_info_name_;
	ui::Label*		file_info_size_;

	ui::VBox*		progress_vertlayout_;
	ui::Progress*	http_progress_;
	ui::Label*		http_status_;
	ui::Button*		file_receive_;
	ui::Button*		file_saveas_;
	ui::Button*		file_reject_;
	ui::Button*		file_open_;
	ui::Button*		file_openducu_;
	ui::Button*		file_cancel_;

	std::string		transfer_file_session_id_;
	TransferFileSessionState transfer_file_session_state_;
};

}
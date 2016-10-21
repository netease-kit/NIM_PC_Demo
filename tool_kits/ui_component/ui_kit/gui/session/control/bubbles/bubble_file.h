#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleFile
  * @brief 会话窗体中聊天框内的文件传输消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
 */
class MsgBubbleFile : public MsgBubbleItem
{
public:
	virtual ~MsgBubbleFile();

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
	virtual void SetMsgStatus(nim::NIMMsgLogStatus status) override;

	/** 
	* 获取此消息项的文件上传回调函数		
	* @return FileUpPrgCallback 回调函数
	*/
	nim::Talk::FileUpPrgCallback GetFileUpPrgCallback()
	{
		return nbase::Bind(&MsgBubbleFile::FileUpPrgCallback, this, std::placeholders::_1, std::placeholders::_2);
	}

	/**
	* 处理控件的菜单消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);
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
	* 文件上传进度的回调函数
	* @param[in] uploaded_size  已上传大小 
	* @param[in] file_size		文件总大小			
	* @return void 无返回值
	*/
	void FileUpPrgCallback(int64_t uploaded_size, int64_t file_size);

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
	* 文件下载完毕的回调函数
	* @param[in] res_code 错误码(200代表无错误)
	* @param[in] file_path 文件下载路径
	* @param[in] call_id 消息所属的会话id
	* @param[in] res_id 消息id
	* @return void 无返回值
	*/
	void DownloadResourceCallback1(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id);

	/**
	* 文件下载进度的回调函数
	* @param[in] downloaded_size 已下载大小
	* @param[in] file_size	文件总大小
	* @return void 无返回值
	*/
	void DownloadResourceProgressCallback1(__int64 downloaded_size, __int64 file_size);

	/**
	* 文件下载完毕的回调函数
	* @param[in] is_ok 是否下载成功
	* @param[in] res_code 错误码(200代表无错误)
	* @return void 无返回值
	*/
	void DownloadResourceCallback(bool is_ok, int response_code);

	/**
	* 文件下载进度的回调函数
	* @param[in] current_upload 已上传大小
	* @param[in] total_upload	总上传大小
	* @param[in] current_download	已下载大小
	* @param[in] total_downnload	总下载大小
	* @return void 无返回值
	*/
	void DownloadResourceProgressCallback(double current_upload, double total_upload, double current_download, double total_downnload);
private:
	std::string		local_path_;
	std::string		local_path_temp_;
	std::string		file_name_;
	int64_t			file_size_ = 0;
	std::string		file_url_;
	ui::Box*		msg_file_;

	ui::Button*		file_image_;
	ui::Label*		file_info_name_;
	ui::Label*		file_info_size_;

	ui::VBox*		progress_vertlayout_;
	ui::Progress*	http_progress_;
	ui::Label*		http_status_;
	ui::Button*		file_saveas_;
	//ui::Button*		file_save_;
	ui::Button*		file_open_;
	ui::Button*		file_openducu_;
	ui::Button*		file_cancel_;
	ui::Button*		file_redl_;
	ui::Button*		file_reup_;
	bool			loading_;
	bool			download_fail_;
	__int64			range_start_;
	int				download_request_id_;
	bool			download_cancel_;
};

}
#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleVideo
  * @brief 会话窗体中聊天框内的短视频消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleVideo : public MsgBubbleItem,public ThumbDecorate
{
public:
	/**
	* 初始化控件内部指针
	* @param[in] bubble_right 是否显示到右侧
	* @return void 无返回值
	*/
	virtual void InitControl(bool bubble_right);

	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @return void 无返回值
	*/
	virtual void InitInfo(const nim::IMMessage &msg);
	
	/**
	* 响应此消息项的单击消息，打开图片预览
	*@param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* arg);

	/**
	* 响应此消息项的右击消息，弹出菜单
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);

	/**
	* 是否需要下载图片消息文件
	* @return bool 返回值true: 需要， false: 不需要
	*/
	virtual bool NeedDownloadResource();
private:
	/**
	* 此消息接收的视频资源缩略图，是否成功下载到本地的回调函数
	* @param[in] success	是否下载成功
	* @param[in] file_path  文件下载路径
	* @return void 无返回值
	*/
	void OnDownloadCallback(bool success, const std::string& file_path);
	/**
	* 使用系统缺省设置的播放器播放视频文件
	* @return void 无返回值
	*/
	void PlayVideo();
	/**
	* 使下载视频文件回调
	* @param[in] res_code 返回码
	* @param[in] file_path 文件下载保存的路径
	* @param[in] call_id 会话id（好友id或群id）
	* @param[in] cid 消息id
	* @return void	无返回值
	*/
	void OnVideoDownLoadResult(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id);
	/**
	* 使下载视频文件进度回调
	* @return void 无返回值
	*/
	void OnVideoDownLoadProgress(int64_t completed_size, int64_t file_size);
protected:
	ui::ButtonBox*	msg_video_;
	ui::Control*	image_;
	ui::Label * prog_download_;
	ui::Box * prog_download_panel_;	
};
}
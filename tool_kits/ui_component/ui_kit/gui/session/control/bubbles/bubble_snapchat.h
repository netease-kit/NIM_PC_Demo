#pragma once
#include "bubble_image.h"

namespace nim_comp
{
/** @class MsgBubbleSnapChat
  * @brief 会话窗体中聊天框内的阅后即焚消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleSnapChat : public MsgBubbleImage
{
public:
	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @return void 无返回值
	*/
	void InitInfo(const nim::IMMessage &msg);

	/**
	* 响应此消息项的单击消息
	*@param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* arg) override;

	/**
	* 下载阅后即焚图片的回调函数
	* @param[in] res_code	返回的错误码
	* @param[in] file_path  文件下载路径
	* @param[in] call_id 会话id
	* @param[in] res_id  消息id
	* @return void 无返回值
	*/
	void OnDownloadCallback(int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id);

	/**
	* 查看阅后即焚图片
	* @return void 无返回值
	*/
	void ViewSnapchatImage();
};
}
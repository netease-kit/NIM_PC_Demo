#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleRobot
  * @brief 会话窗体中聊天框内的机器人消息项（可能复杂元素）
  * @copyright (c) 2015 - 2017, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2017/6/29
  */
class MsgBubbleRobot : public MsgBubbleItem
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
	* 设置此消息项显示的文本
	* @param[in] str 设置显示的文本 (可能包含表情信息)		
	* @return void 无返回值
	*/
	void SetMsgText(const std::wstring &str);

	/**
	* 显示消息发送者的头像
	* @return void 无返回值
	*/
	virtual void SetShowHeader() override;
protected:

	/**
	* 计算此消息项所需的控件大小
	* @param[in] szAvailable 布局提供的最大可用范围
	* @return CSize 控件需要的大小
	*/ 
	virtual ui::CSize EstimateSize(ui::CSize szAvailable) override;

	/**
	* 响应此消息项的右击消息，弹出菜单
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);

	/**
	* 处理“复制”菜单项的操作
	* @return void 无返回值
	*/
	virtual void OnMenuCopy();

	/**
	* 下载消息图片资源的回调函数
	* @param[in] success 是否成功
	* @param[in] file_path 本地资源路径
	* @return void 无返回值
	*/
	//virtual void OnDownloadCallback(bool success, const std::string& file_path) override;

	/**
	* 设置是否显示消息发送者的名字（或昵称）
	* @param[in] show 是否显示名字
	* @param[in] from_nick 指定要显示的名字
	* @return void 无返回值
	*/
	virtual void SetShowName(bool show, const std::string& from_nick);

	/**
	* 获取消息发送者id
	* @return string 用户id
	*/
	virtual std::string GetSenderId();


private:
	/**
	* 插入图片
	* @param[in] url 图片地址
	* @param[in] file_tag 文件附件内容
	* @return void 无返回值
	*/
	void InsertImage(const std::string &url, const std::wstring &file_tag = L"");

	/**
	* 下载图片
	* @param[in] url 图片地址
	* @param[in] photo_path 本地路径
	* @return void 无返回值
	*/
	void DownloadImage(const std::string &url, const std::wstring &photo_path, bool is_complex_element);

	/**
	* 下载消息图片资源的回调函数
	* @param[in] success 是否成功
	* @param[in] file_path 本地资源路径
	* @return void 无返回值
	*/
	void OnDownloadCallbackEx(bool success, const std::string& file_path, bool is_complex_element);

private:
	ui::Box*		msg_text_;
	ui::RichEdit*	text_;

	nim::IMBotRobot robot_attach_;

	Json::Value		complex_json_;
};

}
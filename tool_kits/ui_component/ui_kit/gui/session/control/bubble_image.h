#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleImage
  * @brief 会话窗体中聊天框内的图片消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleImage : public MsgBubbleItem
{
public:
	virtual void InitControl(bool bubble_right);
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
	* 设置图片消息项的图片是否可以预览
	* @param[in] can 是否可预览		
	* @return void 无返回值
	*/
	void SetCanView(bool can);

	/**
	* 此消息接收的图片资源，是否成功下载到本地的回调函数
	* @param[in] success	是否下载成功
	* @return void 无返回值
	*/
	void OnDownloadCallback( bool success );

	/** 
	* 设置图片消息项显示的缩略图	
	* @return void 无返回值
	*/
	void DoZoom();
protected:
	ui::ButtonBox*	msg_image_;
	ui::Control*	image_;

	std::wstring	thumb_;
	std::wstring	path_;
};
}
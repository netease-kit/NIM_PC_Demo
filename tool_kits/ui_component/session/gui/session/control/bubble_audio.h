#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleAudio
  * @brief 会话窗体中聊天框内的语音消息项
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleAudio : public MsgBubbleItem
{
public:
	MsgBubbleAudio();

	/**
	* 初始化控件内部指针
	* @return void 无返回值
	*/
	virtual void InitControl(bool bubble_right);

	/**
	* 初始化控件外观
	* @param[in] msg 消息信息结构体
	* @return void 无返回值
	*/
	virtual void InitInfo(const MsgData &msg);

	/**
	* 是否需要下载语音消息文件
	* @return bool 返回值true: 需要， false: 不需要
	*/
	virtual bool NeedDownloadResource();
public:
	/**
	* 单击了语音消息后，开始播放语音的回调函数
	* @param[in] cid	消息id
	* @param[in] code	错误码(200代表无错误)
	* @return void 无返回值
	*/
	void OnPlayCallback(int code);

	/**
	* 第二次单击了语音消息后，停止播放语音的回调函数
	* @param[in] cid	消息id
	* @param[in] code	错误码(200代表无错误)
	* @return void 无返回值
	*/
	void OnStopCallback(int code);
protected:

	/**
	* 此消息接收的语音资源，是否成功下载到本地的回调函数
	* @param[in] success	是否下载成功
	* @return void 无返回值
	*/
	virtual void OnDownloadCallback(bool success);
private:
	/**
	* 响应此消息项的单击消息，播放语音
	*@param[in] param 被单击的按钮的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* arg);

	/**
	* 响应此消息项的右击消息，弹出菜单
	* @param[in] param 被单击的菜单项的相关信息
	* @return bool 返回值true: 继续传递控件消息， false: 停止传递控件消息
	*/
	bool OnMenu(ui::EventArgs* arg);

	/** 
	* 语音播放时，更新播放进度的回调函数		
	* @return void 无返回值
	*/
	void OnTick();

	/** 
	* 初始化语音显示时间		
	* @return void 无返回值
	*/
	void InitTime();

	/**
	* 设置语音显示时间
	* @param sec 设置当前显示的播放时间（单位：秒）
	* @return void 无返回值
	*/
	void SetTime(int sec);

	/** 
	* 启用/禁用 语音播放功能
	* @param[in] can  启用/禁用	
	* @return void 无返回值
	*/
	void SetCanPlay(bool can);

	/** 
	* 开始语音播放的界面效果(只是界面变化，实际语音播放是通过单机触发)	
	* @return void 无返回值
	*/
	void DoPlay();

	/**
	* 停止语音播放的界面效果
	* @return void 无返回值
	*/
	void DoStop();
private:
	ui::ButtonBox*	msg_audio_;
	ui::Control*	audio_icon_;
	ui::Control*	play_icon_;
	ui::Label*		tip_time_;

	bool			is_playing_;

	int				time_;
	int				tick_;
	std::string		path_;

	nbase::WeakCallbackFlag timer_;
};

}
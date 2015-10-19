#pragma once

namespace nim_comp
{

/** @class CustomMsgBubble
  * @brief 消息中心“自定义消息”列表项UI类，当有新的自定义消息时，负责做出界面上的改变
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class CustomMsgBubble : public ui::ListContainerElement
{
public:
	void InitControl(const MsgData &msg);
protected:
	void InitInfo(const MsgData &msg);
	void SetMsgText(const std::wstring &str);

private:
	ui::RichEdit*	text_;
	ui::Label*		time_;
	ui::Label*		name_;
	ui::Button*		head_;

private:
	AutoUnregister unregister_cb;
};

}
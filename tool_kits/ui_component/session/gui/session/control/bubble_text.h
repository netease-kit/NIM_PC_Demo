#pragma once
#include "bubble_item.h"

namespace nim_comp
{
/** @class MsgBubbleText
  * @brief 会话窗体中聊天框内的文本消息项（可能包含表情）
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/11
  */
class MsgBubbleText : public MsgBubbleItem
{
public:
	virtual void InitControl(bool bubble_right);
	virtual void InitInfo(const MsgData &msg);

	/** 
	* 设置此消息项显示的文本
	* @param[in] str 设置显示的文本 (可能包含表情信息)		
	* @return void 无返回值
	*/
	void SetMsgText(const std::wstring &str);
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
private:
	ui::Box*		msg_text_;
	ui::RichEdit*	text_;
};

}
#pragma once
#include "shared/base.h"

namespace nim_comp
{
/** @class BroadForm
  * @brief 发布公告窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class BroadForm : public WindowEx
{
public:
	BroadForm();
	~BroadForm();
	
	//覆盖虚函数
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;

	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	/**
	* 设置群组id
	* @param[in] tid 群组id
	* @return void	无返回值
	*/
	void SetTid(const std::string &tid);
private:
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool Notify(ui::EventArgs* msg);

	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	bool OnClicked(ui::EventArgs* msg);

	/**
	* 响应发布公告的回调函数
	* @param[in] team_event 群组通知事件
	* @return void	无返回值
	*/
	void OnUpdateBroadCb(const nim::TeamEvent& team_event);
public:
	static const LPCTSTR kClassName;
private:
	std::string tid_;

	ui::RichEdit*	re_title_;
	ui::Label*		error_1_;

	ui::RichEdit*	re_content_;
	ui::Label*		error_2_;

	ui::Button*		btn_commit_;
};
}
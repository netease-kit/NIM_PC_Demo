#pragma once
#include "shared/base.h"

namespace nim_comp
{
/** @class TeamNotifyForm
  * @brief 显示群通知的窗口
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/21
  */
class TeamNotifyForm : public WindowEx
{
	enum TeamNotifyType
	{
		//as administrator
		TNP_ASK_JOIN		,	//xxx请求加入：拒绝or同意
		//as user
		TNP_INVITE_YOU		,	//xxx邀请你入群：忽略or拒绝or同意
		TNP_REJECT_INVITE	,	//xxx拒绝了你的邀请
		TNP_REJECT_JOIN		,	//xxx拒绝了你的加入申请
	};
public:
	TeamNotifyForm();
	~TeamNotifyForm();
	
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

	/**
	* 显示某人请求入群的提示信息
	* @return void	无返回值
	*/
	void SetAskJoinText();

	/**
	* 显示被邀请入群的提示信息
	* @return void	无返回值
	*/
	void SetInviteText();

	/**
	* 显示某人拒绝入群的提示信息
	* @return void	无返回值
	*/
	void SetRejectInviteText();

	/**
	* 显示被某群拒绝入群的提示信息
	* @return void	无返回值
	*/
	void SetRejectJoinText();

	/**
	* 根据群同志类型显示响应的提示信息
	* @return void	无返回值
	*/
	void RefreshText();

	/**
	* 响应某人请求入群的回调函数
	* @param[in] msg 系统消息
	* @return void	无返回值
	*/
	void OnAskJoin(const nim::SysMessage &msg);

	/**
	* 响应某群邀请你入群的回调函数
	* @param[in] msg 系统消息
	* @return void	无返回值
	*/
	void OnInviteYou(const nim::SysMessage &msg);

	/**
	* 响应某人拒绝入群的回调函数
	* @param[in] msg 系统消息
	* @return void	无返回值
	*/
	void OnRejectInvite(const nim::SysMessage &msg); 

	/**
	* 响应某群拒绝你的入群申请的回调函数
	* @param[in] msg 系统消息
	* @return void	无返回值
	*/
	void OnRejectJoin(const nim::SysMessage &msg);

	/**
	* 跳转到某个页面
	* @param[in] t 页面索引，0=ask_join; 1=invite_you; 2=reject
	* @return void	无返回值
	*/
	void GotoPage(int t);
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
private:
	/**
	* 响应群名改变的回调函数
	* @param[in] 
	* @param[in] 
	* @return void	无返回值
	*/
	void OnTeamNameChange(const nim::TeamInfo& team_info);
public:
	static const LPCTSTR kClassName;
private:
	ui::Control*	tnp_[3];

	ui::RichEdit*	re_ask_join_;
	ui::RichEdit*	re_invite_;
	ui::RichEdit*	re_reject_;

	__int64			msg_id_;

	std::string		uid_;
	std::string		tid_;

	TeamNotifyType	type_;

	AutoUnregister unregister_cb;
};
}
#pragma once

#include "nim_ui_dll.h"
#include "gui/main/session_list.h"

namespace nim_ui
{

/** @class SessionListManager
  * @brief 提供最近会话列表控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API SessionListManager : public nbase::SupportWeakCallback
{
public:
	SINGLETON_DEFINE(SessionListManager);

public:
	/**
	* 把传入的列表控件作为最近会话列表控件，控制列表控件来展示最近会话
	* @param[in] list_box 作为会话列表的列表控件指针(当控件被销毁时应该传入nullprt)
	* @return void	无返回值
	*/
	bool AttachListBox(ui::ListBox *list_box);

	/**
	* 主动向sdk查询之前保存的会话列表，然后逐个添加到界面的会话列表中
	* @return void 无返回值
	*/
	void InvokeLoadSessionList();

	/**
	* 从会话列表查找会话控件
	* @param[in] session_id 会话id
	* @return SessionItem* 会话控件的指针
	*/
	nim_comp::SessionItem* GetSessionItem(const std::string &session_id);

	/**
	* 主动向sdk查询未读的系统消息数
	* @return void 无返回值
	*/
	void QueryUnreadSysMsgCount();

	/**
	* 注册未读消息(会话消息、系统消息、自定义消息)总数改变的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegUnreadCountChange(const nim_comp::OnUnreadCountChangeCallback& callback);

	/**
	* 通知会话列表某个会话增加一条未读消息
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void InvokeAddSessionUnread(const std::string &id);

	/**
	* 通知会话列表某个会话重置未读消息数为0
	* @param[in] id 会话id
	* @return void 无返回值
	*/
	void InvokeResetSessionUnread(const std::string &id);

	/**
	* 通知会话列表改变系统未读消息总数
	* @param[in] count 未读消息总数
	* @return void 无返回值
	*/
	void UISysmsgUnread(int count);

	/**
	* 通知会话列表改变自定义未读消息总数
	* @param[in] add 增加或者减少未读自定义消息总数
	* @return void 无返回值
	*/
	void UICustomSysmsgUnread(bool add);
	
	/**
	* 移动端登录发生变化时的回调
	* @param[in] online 是否在线
	* @param[in] clients 其他客户端信息
	* @return void	无返回值
	*/
	void OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients);

	/**
	* 把移动端踢下线的结果回调
	* @param[in] client_ids 被踢的设备id
	* @return void	无返回值
	*/
	void OnMultispotKickout(const std::list<std::string> &client_ids);

	/**
	* 查询未读系统消息的回调
	* @param[in] res_code 错误码
	* @param[in] unread_count 未读总数
	* @return void 无返回值
	*/
	void OnQuerySysmsgUnreadCb(nim::NIMResCode res_code, int unread_count);

	/**
	* 查询到会话列表的回调，填充会话列表控件(不对外开放，提供给其他service调用)
	* @param[in] sessions 会话列表数据
	* @return void 无返回值
	*/
	void OnQuerySessionListCallback(const std::list<nim::SessionData>& sessions);

private:
	SessionListManager(){};
	~SessionListManager(){};
	std::unique_ptr<nim_comp::SessionList> session_list_;
};

}
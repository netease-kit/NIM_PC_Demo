#pragma once

#include "nim_ui_dll.h"
#include "gui/main/session_list.h"
#include "gui/main/session_list_cloud.h"

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
	bool AttachListBox(ui::VirtualListBox*list_box, ui::ListBox* top_function_list);

	/**
	* 把传入的列表控件作为云端会话列表控件，控制列表控件来展示云端会话
	* @param[in] list_box 作为会话列表的列表控件指针(当控件被销毁时应该传入nullprt)
	* @return void	无返回值
	*/
	bool AttachCloudListBox(ui::ListBox* list_box);

	/**
	 * 显示云端会话列表
	 * @param[in] show true 为显示云端，false 为显示本地的
	 * @return void 无返回值
	 */
	void ShowCloudSession(bool show = true);

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
	* 注册UI新插入项的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegAddItem(const nim_comp::OnAddItemCallback& callback);
	/**
	* 注册UI删除项的回调
	* @param[in] callback 回调函数
	* @return UnregisterCallback 反注册对象
	*/
	UnregisterCallback RegRemoveItem(const nim_comp::OnRemoveItemCallback& callback);
	/**
	* 选中会话列表某个会话
	* @param[in] id 会话id
	* @param[in] session_type 会话类型
	* @param[in] create 如果不存在该会话是否时行创建
	* @param[in] sel 是否进行选中
	* @param[in] trigger 是否触发通知事件 kEventSelect / kEventUnSelect
	* @return void 无返回值
	*/
	void InvokeSelectSessionItem(const std::string &id, nim::NIMSessionType session_type,bool create = true,bool sel = true, bool trigger = true);
	/**
	* 选中会话列表某个会话
	* @param[in] id 会话id
	* @param[in] sel 是否进行选中
	* @param[in] trigger 是否触发通知事件 kEventSelect / kEventUnSelect
	* @return void 无返回值
	*/
	void InvokeSelectSessionItem(const std::string &id, bool sel = true, bool trigger = true);
	/**
	* 添加一个会话控件到会话列表
	* @param[in] msg 会话数据
	* @return bool 是否添加成功
	*/
	bool AddSessionItem(const nim::SessionData &msg);

	/**
	* 判断会话列表是否存在指定会话
	* @param[in] session_id 会话id
	* @return bool 是否存在
	*/
	bool CheckSessionItem(const std::string &session_id);
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

	void OnQueryOnlineSessionListCallback(bool has_more, const nim::SessionOnLineServiceHelper::SessionList& session_list);

	/**
	* 插入一条本地消息,为了在SDK会话列表产生一条数据
	* @param[in] cb 结果回调
	* @param[in] session_type 类型
	* @param[in] session_id 会话ID
	* @param[in] time 消息的时间戳
	* @param[in] status_delete如果删除该消息，如果没有历史记录该列表项会被删除掉
	* @return void 无返回值
	*/
	void InsertLocalMsg(const nim_comp::InsertLocalMessageCallback& cb, nim::NIMSessionType session_type, const std::string& session_id, __int64 time, bool status_delete = true);
private:
	SessionListManager();
	~SessionListManager(){};
	std::unique_ptr<nim_comp::SessionList> session_list_;
	std::unique_ptr<nim_comp::SessionListCloud> session_list_cloud_;
};

}
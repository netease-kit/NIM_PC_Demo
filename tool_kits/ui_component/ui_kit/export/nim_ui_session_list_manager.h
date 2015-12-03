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
class NIM_UI_DLL_API SessionListManager
{
public:
	SINGLETON_DEFINE(SessionListManager);
public:
	SessionListManager();
	~SessionListManager();
public:
	bool AttachListBox(ui::ListBox *list_box);

	/**
	* 主动向sdk查询之前保存的会话列表，然后逐个添加到界面的会话列表session_list_中。
	* @return void 无返回值
	*/
	void InvokeLoadSessionList();
	void QueryUnreadCount();

public:
	void LoadSessionList(const std::list<nim::SessionData>& sessions);

	void AddUnreadCount(const std::string &id);
	void ResetSessionUnread(const std::string &id);

	void UISysmsgUnread(int count);
	void UICustomSysmsgUnread(bool add);
	
	static void QuerySysmsgUnreadCb(nim::NIMResCode res_code, int unread_count);

	//多端登录
	void OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients);
	void OnMultispotKickout(const std::list<std::string> &client_ids);

private:
	std::unique_ptr<nim_comp::SessionList> session_list_;
};

}
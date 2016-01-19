#pragma once
#include "control/session_item.h"
#include "gui/main/multispot_form.h"

namespace nim_comp
{
/** @class SessionList
  * @brief 用于操作和维护最近会话列表
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/14
  */
class SessionList : public nbase::SupportWeakCallback
{
public:
	SessionList(ui::ListBox* session_list);
	~SessionList();

	int AdjustMsg(const nim::SessionData &msg); //按每个会话最后一条消息的时间排序
	SessionItem* AddSessionItem(const nim::SessionData &msg);
	void RemoveAllSessionItem();

	void AddUnreadCount(const std::string &id);
	void ResetSessionUnread(const std::string &id);

	void UISysmsgUnread(int count);
	void UICustomSysmsgUnread(bool add);

	void UpdateSessionInfo(const nim::UserNameCard& user_info);
	void DeleteSessionItem(SessionItem* item);

	//多端登录
	void OnMultispotChange(bool online, const std::list<nim::OtherClientPres>& clients);
	void OnMultispotKickout(const std::list<std::string> &client_ids);
	void ShowMultispotUI();

	/**
	* 注册给sdk的回调函数，会话消息变化（如有新消息、删除会话记录）时执行，不需要手动调用。
	* @param[in] rescode	返回码
	* @param[in] result		消息的具体信息和内容，Json数据转换的字符串
	* @param[in] total_unread_counts	未读消息条数
	* @return void 无返回值
	*/
	void OnChangeCallback(nim::NIMResCode rescode, const nim::SessionData& data, int total_unread_counts);

	void OnUserInfoChange(const std::list<nim::UserNameCard>& uinfos);
	void OnUserPhotoReady(const std::string& accid, const std::wstring &photo_path);
	void OnTeamNameChange(const nim::TeamInfo& team_info);

private:
	bool OnItemNotify(ui::EventArgs* msg);
	bool OnEventsClick(ui::EventArgs* param);
	bool OnMultispotClick(ui::EventArgs* param);
	
private:
	ui::ListBox*	session_list_;

	ui::Box*		box_unread_sysmsg_;
	ui::Label*		label_unread_sysmsg_;

	int sys_msg_unread_;
	int custom_msg_unread_;

	AutoUnregister unregister_cb;
	std::map<nim::NIMClientType, nim::OtherClientPres> map_multispot_infos_;
};
}
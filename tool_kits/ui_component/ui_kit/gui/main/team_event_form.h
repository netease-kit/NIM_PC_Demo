#pragma once
#include "gui/main/control/event_item.h"

namespace nim_comp
{
/** @class TeamEventForm
  * @brief 点击主界面会话列表中“消息中心”时弹出的消息中心窗口，用列表形式显示系统消息（如xxx加你为好友，xxx邀请你加入xxx群）。
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TeamEventForm : public WindowEx
{
public:
	TeamEventForm();
	~TeamEventForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	
	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual void InitWindow() override;
	virtual bool Notify(ui::EventArgs* msg);
	virtual bool OnClicked(ui::EventArgs* msg);

	void ReloadEvents();

	/**
	* 向sdk查询所有的系统消息
	* @return void	无返回值
	*/
	void InvokeLoadEvents();
	void LoadEventsCb(int count, int unread, const std::list<nim::SysMessage> &result);
	void UpdateFarstTime();

	void OnTeamEventCb(__int64 msg_id, nim::NIMSysMsgStatus status);
	void OnOneTeamEvent(const nim::SysMessage &json);

	static void DeleteAllCb(nim::NIMResCode res_code, int unread);
	static void SysMsgReadAllCb(nim::NIMResCode code, int unread);

	//自定义通知
	void OnOneCustomMsg(const nim::SysMessage& msg) { AddCustomMsg(msg, true); }
	bool IsCustomList() { return custom_list_->IsVisible(); }
private:
	void AddEvent(const nim::SysMessage &json, bool first);
	void OpEventTip(bool add);

	void GetMoreCustomMsg();
	void AddCustomMsg(const nim::SysMessage& msg, bool first);
public:
	static const LPCTSTR kClassName;
private:
	ui::Button*		btn_recycle_;
	ui::ListBox*	event_list_;
	ui::ListBox*	custom_list_;

	__int64			farst_time_;
	std::map<__int64,TeamEventItem*>	id_item_pair_;

	bool has_more_;
	bool is_loading_;

	__int64 last_custom_msg_time_;//早的一条消息时间
	bool has_more_custom_;
};

/**
* 全局函数，更新未读系统消息条数
* @param[in]	count 未读条数
* @return void	无返回值
*/
void UpdateSysmsgUnread(int count);

/**
* 全局函数，更新未读自定义消息条数
* @param[in]	add 是否新增了未读消息，是则未读条数加1，否则未读条数清0。
* @return void	无返回值
*/
void UpdateCustomSysmsgUnread(bool add);
}
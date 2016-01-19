#pragma once


enum EventOpType
{
	EOT_DELETE
};

namespace nim_comp
{
/** @class TeamEventItem
  * @brief 消息中心系统消息列表项UI类，当有新的系统消息或用户操作时，负责做出界面上的改变
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class TeamEventItem : public ui::ListContainerElement
{
public:
	TeamEventItem();
	virtual ~TeamEventItem();
	void InitCtrl();
	bool InitInfo(const nim::SysMessage &json);

	__int64 GetMsgId();
	__int64 GetTime();
	void OnTeamEventCb(nim::NIMSysMsgStatus status);
	void OnUserInfoChange(const nim::UserNameCard &info);
	void OnUserPhotoReady(const std::string& accid, const std::wstring& photo_path);
	void OnTeamNameChange(const nim::TeamInfo& team_info);

	static void TeamEventCb(__int64 msg_id, const nim::TeamEvent& team_event);
	static void SetStatusCb(nim::NIMResCode code, __int64 msg_id, int unread);
	static void DeleteCb(nim::NIMResCode code, __int64 msg_id, int unread);
private:
	void PopupEventItemMenu(POINT point);
	bool DelEventItemMenuItemClick(ui::EventArgs* param);
private:
	bool OnClicked(ui::EventArgs* arg);
	bool OnEventItemMenu(ui::EventArgs* arg);
	
private:
	ui::Button*		btn_head_;
	ui::Label*		evt_team_;
	ui::Label*		evt_time_;
	ui::Label*		evt_tip_;

	ui::Button*		btn_ok_;
	ui::Button*		btn_no_;
	ui::Label*		result_;

	__int64		msg_time_;
	int			msg_type_;
	std::string	acc_id_;
	std::string	tid_;
	__int64		msg_id_;
	int			msg_status_;

	AutoUnregister unregister_cb;
};
}
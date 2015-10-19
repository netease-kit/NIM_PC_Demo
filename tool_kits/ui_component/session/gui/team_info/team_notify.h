#pragma once
#include "shared/base.h"

namespace nim_comp
{
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
	
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;
	
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void InitWindow() override;

	void SetTid(const std::string &tid);
	void SetAskJoinText();
	void SetInviteText();
	void SetRejectInviteText();
	void SetRejectJoinText();
	void RefreshText();

	void OnAskJoin(const Json::Value &json); //sb请求加入team
	void OnInviteYou(const Json::Value &json); //team邀请你加入
	void OnRejectInvite(const Json::Value &json); //sb拒绝加入team
	void OnRejectJoin(const Json::Value &json);	//team拒绝你的加入申请

	void GotoPage(int t); //0=ask_join; 1=invite_you; 2=reject_
private:
	bool Notify(ui::EventArgs* arg);
	bool OnClicked(ui::EventArgs* arg);
private:
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
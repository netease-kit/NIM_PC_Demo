// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
// Author: panqinke <panqinke@corp.netease.com>
// Date: 2013/8/14

#ifndef ADD_FRIEND_H_
#define ADD_FRIEND_H_

#include "util/window_ex.h"

namespace nim_comp
{
const LPCTSTR g_ADDFRIEND_SEARCH_PAGE	  = L"search_page";
const LPCTSTR g_ADDFRIEND_USERINFO_PAGE	  = L"userinfo_page";
const LPCTSTR g_ADDFRIEND_NOTFOUND_PAGE	  = L"notfound_page";
const LPCTSTR g_ADDFRIEND_SENDFRIENDAPPLY_PAGE	  = L"send_friend_apply_page";
const LPCTSTR g_ADDFRIEND_SENDCOMPLETE_PAGE	  = L"send_complete_page";
const LPCTSTR g_ADDFRIEND_SENDTOOFREQUENT_PAGE	  = L"send_too_frequent_page";
const LPCTSTR g_ADDFRIEND_ADDSUCCESS_PAGE	  = L"add_success_page";
const LPCTSTR g_ADDFRIEND_NET_ABNORMAL_PAGE = L"net_abnormal";

class AddFriendWindow : public WindowEx
{
public:
	AddFriendWindow();
	virtual ~AddFriendWindow();

	//接口实现
	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;

	//覆盖虚函数
	virtual std::wstring GetWindowClassName() const override { return kClassName; };
	virtual std::wstring GetWindowId() const override { return kClassName; };
	virtual UINT GetClassStyle() const override { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void InitWindow() override;
	bool Search(ui::EventArgs* param);
	//void FindUserCallBack(const nbiz::FindUserParam& msg_param);
	bool Cancel(ui::EventArgs* param);
	bool SearchTipWrong(ui::EventArgs* param);
	void StartSearch(const std::wstring& search_key);
	void InitUserProfile(const nim::UserNameCard& user_info);
	void InitEdit();

	enum INIT_TYPE
	{
		NONE,
		SearchAndSendApplyEdit,
		SendApplyEdit,
	};
	bool PreOrNextClick(ui::EventArgs* param, const std::wstring& page_name, INIT_TYPE init_edit);

public:
	static const LPCTSTR kClassName;

private:
	bool AddFriendClick(ui::EventArgs* param);
	//void AddFriendNewWithVerifyCallBack(const nbiz::AddFriendWithVerifyParam& msg_param);
	bool ChatClick(ui::EventArgs* param);
	bool RemoveFromBlack(ui::EventArgs* args);
	//void SendFriendApplyCallback(const nbiz::VerifyFriendParam& msg_param);
	bool SendFriendApplyClick(ui::EventArgs* param);
	bool OnSearchKeyEditSetFocus(void* param);
	void OnUserInfoChange(const std::list<nim::UserNameCard> &uinfos);
	void OnUserPhotoReady(const std::string& account, const std::wstring& photo_path);

private:
	ui::Button* headimage_;
	ui::Label* nick_name_;
	ui::RichEdit* search_key_;
	ui::RichEdit* apply_words_;
	ui::TabBox*	tablayout_;
	ui::TabBox* addfriend_or_chat_;
	ui::Button* userinfo_page_add_friend_;

private:
	UTF8String id_;
	bool da_key_;
	AutoUnregister unregister_cb;
};
}
#endif // ADD_FRIEND_H_

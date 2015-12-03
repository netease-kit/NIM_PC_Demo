#pragma once

#include "shared/pin_yin_helper.h"

namespace nim_comp
{
/** @class FriendItem
  * @brief 好友列表项的UI类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class FriendItem : public ui::TreeNode
{
public:
	FriendItem();
	virtual ~FriendItem();
	virtual void Init(bool is_team, const nim::UserNameCard &user_info);
	bool GetIsTeam() const
	{
		return is_team_;
	}
	std::string GetId()
	{
		return id_;
	}
	nim::UserNameCard GetUserInfo();

	bool Match(const UTF8String& search_key)
	{
		std::wstring ws_search_key = nbase::UTF8ToUTF16(search_key);
		ws_search_key = nbase::MakeLowerString(ws_search_key);
		if (nick_name_.find(ws_search_key) != std::wstring::npos
			|| nick_name_full_spell_.find(search_key) != UTF8String::npos
			|| nick_name_simple_spell_.find(search_key) != UTF8String::npos)
		{
			return true;
		}
		return false;
	}
	bool operator < (const FriendItem& elment2) const
	{
		static DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRCP);
		return ::CompareString(lcid, 0, this->nick_name_.c_str(), this->nick_name_.length(), elment2.nick_name_.c_str(),elment2.nick_name_.length()) == 1;
	}
	bool operator == (const FriendItem& elment2) const 
	{
		return this->id_ == elment2.id_;
	}

	bool OnDbClicked(ui::EventArgs* arg);
private:
	void OnTeamNameChange(const nim::TeamInfo& user_info);
private:
	ui::Label*		contact_;

	bool			is_team_;
	std::string		id_;

	std::wstring	nick_name_;
	UTF8String		nick_name_full_spell_;
	UTF8String		nick_name_simple_spell_;

	AutoUnregister unregister_cb;
};
}
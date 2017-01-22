#ifndef NIM_WIN_GUI_LIST_UTIL_LIST_ITEM_UTIL_H_
#define NIM_WIN_GUI_LIST_UTIL_LIST_ITEM_UTIL_H_

#include "duilib/UIlib.h"
#include "pin_yin_helper.h"

enum GroupType
{
	GT_CLOSE_FRIEND,
	GT_TEAM,
	GT_COMMON_FRIEND,
	GT_COMMON_NUMBER,
};

class GroupTitle: public ui::TreeNode
{

};

struct UserAllInfo
{
	UTF8String nim_account;
	UTF8String show_name;
	UTF8String nick_name;
	UTF8String icon;
	UTF8String id;
	bool is_close_friend;
};

class ListItemUtil
{
public:
	enum ListeItemType
	{
		FRIEND_LIST_TEAM,			//好友列表
		FRIEND_LIST_FRIEND,	

		START_CHAT_SEARCH_TEAM,		//发起聊天窗口搜索结果
		START_CHAT_SEARCH_FRIEND,
	};

	static int GetGroup(GroupType groupType, wchar_t letter);
	static ui::TreeNode* CreateFirstLetterListItem(const wstring& letter_name);
	
};



class ListItemUserData : public ui::UserDataBase
{
public:
	UTF8String GetShowNameFullSpell()
	{
		if (show_name_full_spell.empty())
		{
			show_name_full_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(show_name));
		}

		return show_name_full_spell;
	}

	UTF8String GetShowNameSimpleSpell()
	{
		if (show_name_simple_spell.empty())
		{
			show_name_simple_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(show_name));
		}

		return show_name_simple_spell;
	}

	UTF8String GetNickNameFullSpell()
	{
		if (nick_name_full_spell.empty())
		{
			nick_name_full_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(nick_name));
		}

		return nick_name_full_spell;
	}

	UTF8String GetNickNameSimpleSpell()
	{
		if (nick_name_simple_spell.empty())
		{
			nick_name_simple_spell = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(nick_name));
		}

		return nick_name_simple_spell;
	}

	UTF8String GetShowNameFirstLetter()
	{
		if (show_name_first_letter.empty())
		{
			show_name_first_letter = GetShowNameFullSpell().substr(0, 1);
		}

		return show_name_first_letter;
	}

	bool operator < (const ListItemUserData& elment2)
	{
		static DWORD lcid = MAKELCID(MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRCP);
		return ::CompareString(lcid, 0, this->show_name.c_str(), (int)this->show_name.length(), elment2.show_name.c_str(), (int)elment2.show_name.length()) == 1;
	}

	UTF8String show_name_first_letter;
	wstring show_name;
	UTF8String show_name_full_spell;
	UTF8String show_name_simple_spell;
	wstring nick_name;
	UTF8String nick_name_full_spell;
	UTF8String nick_name_simple_spell;
	bool team_or_friend;
};



#endif // NIM_WIN_GUI_LIST_UTIL_LIST_ITEM_UTIL_H_
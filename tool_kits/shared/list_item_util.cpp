#include "stdafx.h"
#include "list_item_util.h"
#include "shared/tool.h"
#include "pin_yin_helper.h"
#include "ui/ui_menu.h"

using namespace ui;
using namespace std;
using namespace nbase;


int ListItemUtil::GetGroup(GroupType groupType, wchar_t letter)
{
	int index = 0;
	if (groupType == GT_CLOSE_FRIEND)
	{
		index = 0;
	}
	else if (groupType == GT_TEAM)
	{
		index = 1;
	}
	else if (groupType == GT_COMMON_FRIEND) {
		if (letter >= L'A' && letter <= L'Z')
		{
			index = letter - 'A' + 3;
		}
		else if (letter >= L'a' && letter <= L'z')
		{
			index = letter - 'a' + 3;
		}
		else
		{
			index = 2;
		}
	}
	else if (groupType == GT_COMMON_NUMBER) {
		index = 2;
	}
	else {
		ASSERT(FALSE);
	}
	return index;
}

TreeNode* ListItemUtil::CreateFirstLetterListItem(const wstring& letter_name)
{
	GroupTitle* letterHeadContainer = new GroupTitle();
	letterHeadContainer->SetName(letter_name.c_str());
	letterHeadContainer->SetFixedHeight(26);
	RECT margin = {0, -1, 0, 0};
	letterHeadContainer->SetMargin(margin);
	letterHeadContainer->SetBkColor(L"bk_listitem_hovered");

	HBox* hor = new HBox;
	letterHeadContainer->Add(hor);

	Label* group_caption = new Label;
	if (letter_name == L"*" || letter_name == L"Èº×é")
	{
		MutiLanSupport* multilan = MutiLanSupport::GetInstance();
		std::wstring caption = multilan->GetStringViaID(letter_name.compare(L"*") == 0 ? L"STRID_MAINWINDOW_STAR_FRIENDS" : L"STRID_MAINWINDOW_TEAMS");
		group_caption->SetText(caption);
		group_caption->SetFont(1);
	}
	else
	{
		group_caption->SetText(letter_name.c_str());
		group_caption->SetFont(17);
	}
	group_caption->SetStateTextColor(kControlStateNormal, L"group_name");
	margin.left = 10;
	margin.top = 0;
 	margin.right = 0;
	margin.bottom = 0;
	group_caption->SetMargin(margin);
	group_caption->SetVerAlignType(kVerAlignCenter);
	hor->Add(group_caption);

	Control* splitLine = new Control();
	splitLine->SetName(L"top_splitLine");
	splitLine->SetFixedWidth(DUI_LENGTH_STRETCH);
	splitLine->SetClass(L"splitline_hor_level2");
	letterHeadContainer->Add(splitLine);

	Control* splitLine2 = new Control();
	splitLine2->SetFixedWidth(DUI_LENGTH_STRETCH);
	splitLine2->SetClass(L"splitline_hor_level2");
	splitLine2->SetVerAlignType(kVerAlignBottom);
	letterHeadContainer->Add(splitLine2);

	return letterHeadContainer;
}

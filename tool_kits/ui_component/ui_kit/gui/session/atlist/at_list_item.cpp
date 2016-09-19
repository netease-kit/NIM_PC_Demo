#include "at_list_item.h"
#include "shared/pin_yin_helper.h"

using namespace ui;

namespace nim_comp
{
const char *AtListItem::kLastFiveItemName = "last_five";

AtListItem::AtListItem(bool is_last_five)
	: head_icon_(NULL)
	, name_label_(NULL)
	, is_last_five_(is_last_five)
{

}

AtListItem::~AtListItem()
{

}

void AtListItem::InitControls()
{
	name_label_ = (Label*)FindSubControl(L"user_name");
	head_icon_ = (Control*)FindSubControl(L"user_icon");
	ASSERT(name_label_ != NULL && head_icon_ != NULL);
}

void AtListItem::Activate()
{
	__super::Activate();
	// 触发单击消息
	ButtonTemplate::Activate();
}

void AtListItem::SetUserID(const std::string& uid)
{
	uid_ = uid;

	if (is_last_five_)
	{
		//如果是最近发言的5人之一，就不设置本item的Name为uid，否则会影响到@列表的查找
		this->SetUTF8Name(kLastFiveItemName);
	}
	else
	{	
		this->SetUTF8Name(uid_);
	}

}

void AtListItem::SetAliasName(const std::wstring& name)
{
	alias_name_ = nbase::UTF16ToUTF8(name);
	alias_full_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(name));
	alias_simple_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(name));
}

void AtListItem::SetTeamCardName(const std::wstring& name)
{
	team_card_name_ = nbase::UTF16ToUTF8(name);
	team_card_full_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(name));
	team_card_simple_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(name));
}

void AtListItem::SetNickName(const std::wstring& name)
{
	nick_name_ = nbase::UTF16ToUTF8(name);
	nick_full_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToFullSpell(name));
	nick_simple_name_ = nbase::MakeLowerString(PinYinHelper::GetInstance()->ConvertToSimpleSpell(name));
}

void AtListItem::SetShowName(const std::wstring& name)
{
	name_label_->SetText(name);
}

void AtListItem::SetHeadImage(std::wstring head)
{	
	head_icon_->SetBkImage(head);
}

bool AtListItem::Match(const std::string& search_key)
{
	if (alias_name_.find(search_key) != string::npos
		|| alias_full_name_.find(search_key) != string::npos
		|| alias_simple_name_.find(search_key) != string::npos
		|| team_card_name_.find(search_key) != string::npos
		|| team_card_full_name_.find(search_key) != string::npos
		|| team_card_simple_name_.find(search_key) != string::npos
		|| nick_name_.find(search_key) != string::npos
		|| nick_full_name_.find(search_key) != string::npos
		|| nick_simple_name_.find(search_key) != string::npos
		|| uid_.find(search_key) != string::npos)
	{
		return true;
	}

	return false;
}
}
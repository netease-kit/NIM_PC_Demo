#include "stdafx.h"
#include "team_plugin_page.h"
#include "export\nim_ui_contacts_list_manager.h"
#include "gui/team_info/team_info_box.h"

using namespace nim_comp;

TeamPluginPage::TeamPluginPage() :
contact_teaminfo_container_(nullptr), detach_list_function_(nullptr)
{

}
TeamPluginPage::~TeamPluginPage()
{
	if (detach_list_function_ != nullptr)
		detach_list_function_();	
}
void TeamPluginPage::DoInit()
{
	contact_teaminfo_container_ = dynamic_cast<ui::TabBox*>(FindSubControl(L"team_profile_container"));
	ui::TreeView* group_list = dynamic_cast<ui::TreeView*>(FindSubControl(L"group_list"));
	group_list->SelectNextWhenActiveRemoved(false);
	detach_list_function_ = group_list->ToWeakCallback([](){
		nim_ui::ContactsListManager::GetInstance()->AttachGroupListBox(nullptr);
	});
	nim_ui::ContactsListManager::GetInstance()->AttachGroupListBox(group_list);
}
void TeamPluginPage::ShowTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
{
	SetActiveTeamInfoBox(create_or_display,type,team_id,team_info);
}
TeamInfoBox* TeamPluginPage::GetTeamInfoBoxByID(const std::string& uid) const
{
	TeamInfoBox* ret = nullptr;
	std::wstring id = nbase::UTF8ToUTF16(uid);
	for (int i = 0; i < contact_teaminfo_container_->GetCount(); i++)
	{
		Control *box_item = contact_teaminfo_container_->GetItemAt(i);
		if (box_item->GetName() == id)
		{
			ret = dynamic_cast<TeamInfoBox*>(box_item);
			break;
		}
	}
	return ret;
}
void TeamPluginPage::SetActiveTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info, TeamInfoBox* infobox)
{
	auto box = (infobox == nullptr ? GetTeamInfoBoxByID(team_id) : infobox);
	if (box != nullptr)
	{
		contact_teaminfo_container_->SelectItem(box);
		GetPlugin()->Selected(true, true);
	}
	else
	{
		SetActiveTeamInfoBox(create_or_display, type, team_id, team_info, CrateTeamInfoBox(create_or_display,type,team_id,team_info));
	}
}
TeamInfoBox* TeamPluginPage::CrateTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
{
	TeamInfoBox* ret = new TeamInfoBox(create_or_display,type,team_id,team_info,true);
	if (type == nim::kNIMTeamTypeAdvanced)
		ui::GlobalManager::FillBoxWithCache(ret, L"team_info/team_info_box_team_view.xml");
	else if (type == nim::kNIMTeamTypeNormal)
		ui::GlobalManager::FillBoxWithCache(ret, L"team_info/team_info_box_group_view.xml");
	ret->SetUTF8Name(team_id);
	ret->AttachClose([this, team_id](UINT){
		CloseInfoBox(team_id);
	});
	contact_teaminfo_container_->Add(ret);
	return ret;
}
std::string TeamPluginPage::GetActiveTeamInfoBox() const
{
	auto active = contact_teaminfo_container_->GetItemAt(contact_teaminfo_container_->GetCurSel());
	if (active != nullptr)
		return active->GetUTF8Name();
	return "";
}
void TeamPluginPage::CloseInfoBox(const std::string& team_id)
{
	auto task = [this, team_id](){
		auto box = GetTeamInfoBoxByID(team_id);
		if (box != nullptr)
		{
			if (IsActiveTeamInfoBox(box))
				contact_teaminfo_container_->SelectItem(0);
			contact_teaminfo_container_->Remove(box);
		}
	};
	Post2UI(ToWeakCallback(task));
}
bool TeamPluginPage::IsActiveTeamInfoBox(const TeamInfoBox *box)
{
	if (box == nullptr)
		return false;
	return contact_teaminfo_container_->GetCurSel() == contact_teaminfo_container_->GetItemIndex((ui::Control*)box);
}
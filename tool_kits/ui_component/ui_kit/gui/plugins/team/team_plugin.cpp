#include "stdafx.h"
#include "team_plugin.h"
namespace nim_comp
{
	const IMainPlugin::PluginFlagType TeamPlugin::kPLUGIN_NAME = "TeamPlugin";
	TeamPlugin::TeamPlugin()
	{

	}
	TeamPlugin::~TeamPlugin()
	{

	}
	IMainPlugin::PluginFlagType TeamPlugin::GetPluginFlag() const
	{
		return kPLUGIN_NAME;
	}
	ui::OptionBox* TeamPlugin::CreatePluginIcon()
	{
		auto ret = new MainPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/team/icon.xml");
		return ret;
	}
	ui::Box* TeamPlugin::CreatePluginPage()
	{
		auto ret = new TeamPluginPage;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/team/page.xml");

		return ret;
	}
	void TeamPlugin::ShowTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info)
	{
		page_->ShowTeamInfoBox(create_or_display, type, team_id, team_info);
	}
}
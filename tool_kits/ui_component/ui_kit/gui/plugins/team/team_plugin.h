#pragma once
#include "module/plugins/main_plugins_base.h"
#include "team_plugin_page.h"
namespace nim_comp
{
	class TeamPlugin : public MainPluginBase<IMainPlugin::PluginType::PluginType_Main,MainPluginIcon, TeamPluginPage>
	{
	public:
		TeamPlugin();
		virtual ~TeamPlugin();
		virtual PluginFlagType GetPluginFlag() const override;
	public:
		void ShowTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
	protected:
		virtual ui::OptionBox* CreatePluginIcon() override;
		virtual ui::Box* CreatePluginPage() override;
	public:
		static const PluginFlagType kPLUGIN_NAME;
	
	};
}
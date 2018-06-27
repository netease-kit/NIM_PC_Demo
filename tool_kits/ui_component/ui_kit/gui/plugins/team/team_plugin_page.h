#pragma once
#include "module/plugins/main_plugins_base.h"
namespace nim_comp
{
	class TeamInfoBox;
	class TeamPluginPage : public MainPluginPage
	{
	public:
		TeamPluginPage();
		virtual ~TeamPluginPage();
	public:
		virtual void DoInit() override;
		void ShowTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
		std::string GetActiveTeamInfoBox() const;
	private:
		void SetActiveTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info, TeamInfoBox* infobox = nullptr);
		TeamInfoBox* GetTeamInfoBoxByID(const std::string& uid) const;
		TeamInfoBox* CrateTeamInfoBox(bool create_or_display, nim::NIMTeamType type, const std::string& team_id, const nim::TeamInfo& team_info);
		void CloseInfoBox(const std::string& team_id);
		bool IsActiveTeamInfoBox(const TeamInfoBox *box);
	private:
		ui::TabBox* contact_teaminfo_container_;
		AutoUnregister unregister_cb;
	};
}
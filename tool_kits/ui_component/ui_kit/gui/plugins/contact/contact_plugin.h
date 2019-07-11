#pragma once
#include "module/plugins/main_plugins_base.h"
#include "gui/plugins/contact/contact_plugin_page.h"
namespace nim_comp
{
	class ContactPlugin : public MainPluginBase<IMainPlugin::PluginType::PluginType_Main,MainPluginIcon,ContactPluginPage>
	{
	public:
		ContactPlugin();
		virtual ~ContactPlugin();
		virtual PluginFlagType GetPluginFlag() const override;		
	public:
		void ShowProfileForm(UTF8String uid, bool is_robot);
		std::string GetActiveProfile() const;
	protected:
		virtual ui::OptionBox* CreatePluginIcon() override;
		virtual ui::Box* CreatePluginPage() override;
	public:
		static const PluginFlagType kPLUGIN_NAME;
	};
}
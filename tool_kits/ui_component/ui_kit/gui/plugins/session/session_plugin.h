#pragma once
#include "module/plugins/main_plugins_base.h"
#include "session_plugin_page.h"
#include "session_plugin_icon.h"
namespace nim_comp
{
	class ISessionDock;
	class SessionPlugin : public MainPluginBase<IMainPlugin::PluginType::PluginType_Main,SessionPluginIcon, SessionPluginPage>
	{
	public:
		SessionPlugin();
		virtual ~SessionPlugin();
		virtual PluginFlagType GetPluginFlag() const override;
		virtual void DoInit() override;
		ISessionDock* GetSessionDock() const;		
	protected:
		virtual ui::OptionBox* GetPluginIconAction() override;
		virtual ui::OptionBox* CreatePluginIcon() override;
		virtual ui::Box* CreatePluginPage() override;
	
	public:
		static const PluginFlagType kPLUGIN_NAME;
	private:
		std::function<void()> detach_list_function_;
	};
}
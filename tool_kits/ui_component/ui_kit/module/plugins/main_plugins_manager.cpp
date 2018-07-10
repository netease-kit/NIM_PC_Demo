#include "stdafx.h"
#include "main_plugins_manager.h"
#include "gui/plugins/session/session_plugin.h"
#include "gui/plugins/contact/contact_plugin.h"
#include "gui/plugins/team/team_plugin.h"
using namespace nim_comp;
MainPluginsManager::MainPluginsManager()
{
	RegPlugin();
}
MainPluginsManager::~MainPluginsManager()
{
}
void MainPluginsManager::RegPlugin()
{
	RegPlugin<SessionPlugin>(SessionPlugin::kPLUGIN_NAME);
	RegPlugin<ContactPlugin>(ContactPlugin::kPLUGIN_NAME);
	RegPlugin<TeamPlugin>(TeamPlugin::kPLUGIN_NAME);
}
void MainPluginsManager::LoadPlugins()
{
	plugin_list_ = std::move(shared::TemplatedObjectFactoryWrapper::InstantiateAllRegisteredSharedOjbect<IMainPlugin, IMainPlugin::PluginFlagType>());
}
MainPluginList MainPluginsManager::GetPluginList() const
{
	return plugin_list_;
}
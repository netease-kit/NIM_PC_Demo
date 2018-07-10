#pragma once
#include "main_plugins_base.h"
namespace nim_comp
{
	class MainPluginsManager
	{
	public:
		MainPluginsManager();
		~MainPluginsManager();	
		SINGLETON_DEFINE(MainPluginsManager);
	public:
		template<typename TPlugin = IMainPlugin>
		std::shared_ptr<TPlugin> GetPlugin(const IMainPlugin::PluginFlagType& flag) const
		{
			auto it = std::find_if(plugin_list_.begin(), plugin_list_.end(), [&](const MainPlugin& item){
				return flag == item->GetPluginFlag();
			});
			if (it != plugin_list_.end())
				return std::dynamic_pointer_cast<TPlugin> (*it);
			return nullptr;
		}
		MainPluginList GetPluginList() const;
		template<typename TPlugin>
		void RegPlugin(const IMainPlugin::PluginFlagType& flag)
		{
			shared::TemplatedObjectFactoryWrapper::RegisteredOjbect<IMainPlugin, TPlugin>(flag);
		}
		void LoadPlugins();
	private:		
		void RegPlugin();
	private:
		MainPluginList plugin_list_;
	};
}
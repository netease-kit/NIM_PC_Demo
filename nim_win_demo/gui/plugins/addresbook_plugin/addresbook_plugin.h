#include "module/plugins/main_plugins_base.h"
#include "gui/plugins/addresbook_plugin/addresbook_plugin_page.h"
class AddresBookPlugin : public nim_comp::MainPluginBase<nim_comp::IMainPlugin::PluginType::PluginType_Main,nim_comp::MainPluginIcon, AddresBookPluginPage>
{
public:
	AddresBookPlugin() = default;
	virtual ~AddresBookPlugin() = default;
	virtual PluginFlagType GetPluginFlag() const override
	{
		return "AddresBookPlugin";
	}
public:
	virtual bool ShowSearchBar() const override;
	virtual void DoInit() override;
protected:
	virtual ui::OptionBox* CreatePluginIcon() override;
	virtual ui::Box* CreatePluginPage() override;
public:
	static const PluginFlagType kPLUGIN_NAME;
};
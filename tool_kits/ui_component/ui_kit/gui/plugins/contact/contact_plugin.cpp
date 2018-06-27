#include "stdafx.h"
#include "contact_plugin.h"
namespace nim_comp
{
	const IMainPlugin::PluginFlagType ContactPlugin::kPLUGIN_NAME = "ContactPlugin";
	ContactPlugin::ContactPlugin()
	{

	}
	ContactPlugin::~ContactPlugin()
	{

	}
	IMainPlugin::PluginFlagType ContactPlugin::GetPluginFlag() const
	{
		return kPLUGIN_NAME;
	}
	ui::OptionBox* ContactPlugin::CreatePluginIcon()
	{
		auto ret = new MainPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/contact/icon.xml");
		return ret;
	}
	ui::Box* ContactPlugin::CreatePluginPage()
	{
		auto ret = new ContactPluginPage;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/contact/page.xml");
		return ret;
	}
	void ContactPlugin::ShowProfileForm(UTF8String uid, bool is_robot)
	{
		page_->ShowProfile(uid, is_robot);
	}
	std::string ContactPlugin::GetActiveProfile() const
	{
		return page_->GetActiveProfile();
	}
}
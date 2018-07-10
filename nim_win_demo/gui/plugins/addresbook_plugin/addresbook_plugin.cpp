#include "stdafx.h"
#include "addresbook_plugin.h"
bool AddresBookPlugin::ShowSearchBar() const 
{ 
	return true; 
}
void AddresBookPlugin::DoInit() 
{
	
}
ui::OptionBox* AddresBookPlugin::CreatePluginIcon()
{
	auto ret = new nim_comp::MainPluginIcon;
	ui::GlobalManager::FillBoxWithCache(ret, L"plugins/addresbook/icon.xml");
	return ret;
}
ui::Box* AddresBookPlugin::CreatePluginPage()
{
	auto ret = new AddresBookPluginPage;
	ui::GlobalManager::FillBoxWithCache(ret, L"plugins/addresbook/page.xml");
	return ret;
}
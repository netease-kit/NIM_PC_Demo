#pragma once
#include "module/plugins/main_plugins_base.h"
#include "gui/cef/cef_tip.h"
class CefTestPlugin : public nim_comp::MainPluginBase<>
{
public:
	CefTestPlugin() = default;
	virtual ~CefTestPlugin() = default;
	virtual PluginFlagType GetPluginFlag() const override
	{
		return "CefTestPlugin";
	}
public:
	virtual bool ShowSearchBar() const override{ return false; }
	virtual void DoInit() override
	{
		auto cef_tip_ = new CefTip;
		ui::GlobalManager::FillBoxWithCache(cef_tip_, L"ceftest/cef_test_tip.xml", &CefTip::CreateControl);
		auto cef_box = dynamic_cast<ui::Box*>(page_->FindSubControl(L"cef_box"));
		if (cef_box != nullptr)
			cef_box->Add(cef_tip_);
	}
protected:
	virtual ui::OptionBox* CreatePluginIcon() override
	{
		auto ret = new nim_comp::MainPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/ceftest/icon.xml");
		return ret;
	}
	virtual ui::Box* CreatePluginPage() override
	{
		auto ret = new nim_comp::MainPluginPage;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/ceftest/page.xml");
		return ret;
	}
public:
	static const PluginFlagType kPLUGIN_NAME;
};
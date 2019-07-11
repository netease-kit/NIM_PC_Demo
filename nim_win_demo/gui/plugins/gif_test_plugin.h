#pragma once
#include "module/plugins/main_plugins_base.h"
#include "gui\test_gif\test_gif_form.h"
class GifTestPlugin : public nim_comp::MainPluginBase<nim_comp::IMainPlugin::PluginType::PluginType_Simple>
{
public:
	virtual PluginFlagType GetPluginFlag() const
	{
		return "GifTestPlugin";
	}
	virtual bool IsActivePage() const { return false; }
	virtual void DoInit() override
	{
		auto test_gif = dynamic_cast<ui::ButtonBox*>(icon_->FindSubControl(L"test_gif"));
		if (test_gif)
			test_gif->AttachClick(test_gif->ToWeakCallback([](ui::EventArgs* param){
			nim_ui::WindowsManager::GetInstance()->SingletonShow<TestGifForm>(TestGifForm::kClassName);
					return true;
				})	);
	}
protected:
	virtual ui::OptionBox* CreatePluginIcon() override
	{
		auto ret = new nim_comp::MainPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/giftest/icon.xml");
		return ret;
	}
	virtual ui::Box* CreatePluginPage() override
	{
		return nullptr;
	}
};
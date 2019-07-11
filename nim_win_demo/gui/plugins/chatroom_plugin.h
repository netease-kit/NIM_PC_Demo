#pragma once
#include "module/plugins/main_plugins_base.h"
#include "gui\chatroom_frontpage.h"
class ChatroomPlugin : public nim_comp::MainPluginBase<nim_comp::IMainPlugin::PluginType::PluginType_Simple>
{
public:
	virtual PluginFlagType GetPluginFlag() const
	{
		return "ChatroomPlugin";
	}
	virtual bool IsActivePage() const { return false; }
	virtual void DoInit() override
	{
		auto goto_chatroom = dynamic_cast<ui::ButtonBox*>(icon_->FindSubControl(L"goto_chatroom"));
		if (goto_chatroom)
			goto_chatroom->AttachClick(goto_chatroom->ToWeakCallback([](ui::EventArgs* param){
					nim_ui::WindowsManager::GetInstance()->SingletonShow<nim_chatroom::ChatroomFrontpage>(nim_chatroom::ChatroomFrontpage::kClassName);
					return true;
				})	);
	}
protected:
	virtual ui::OptionBox* CreatePluginIcon() override
	{
		auto ret = new nim_comp::MainPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/chatroom/icon.xml");
		return ret;
	}
	virtual ui::Box* CreatePluginPage() override
	{
		return nullptr;
	}
};
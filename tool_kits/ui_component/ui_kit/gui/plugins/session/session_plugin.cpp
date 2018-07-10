#include "stdafx.h"
#include "session_plugin.h"
#include "export\nim_ui_session_list_manager.h"
#include "export\nim_ui_session_manager.h"
#include "session_plugin_page.h"
namespace nim_comp
{
	const IMainPlugin::PluginFlagType SessionPlugin::kPLUGIN_NAME = "SessionPlugin";
	SessionPlugin::SessionPlugin() :
		detach_list_function_(nullptr)
	{

	}
	SessionPlugin::~SessionPlugin()
	{
		if (detach_list_function_ != nullptr)
			detach_list_function_();
	}
	IMainPlugin::PluginFlagType SessionPlugin::GetPluginFlag() const
	{
		return kPLUGIN_NAME;
	}
	ui::OptionBox* SessionPlugin::CreatePluginIcon()
	{
		auto ret = new SessionPluginIcon;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/session/icon.xml", &SessionPluginIcon::CreateControl);
		return ret;
	}
	ui::Box* SessionPlugin::CreatePluginPage()
	{
		auto ret = new SessionPluginPage;
		ui::GlobalManager::FillBoxWithCache(ret, L"plugins/session/page.xml");		
		return ret;
	}
	void SessionPlugin::DoInit()
	{
		ui::ListBox* session_list = dynamic_cast<ui::ListBox*>(page_->FindSubControl(L"session_list"));
		session_list->SelectNextWhenActiveRemoved(false);
		detach_list_function_ = session_list->ToWeakCallback([](){
			nim_ui::SessionListManager::GetInstance()->AttachListBox(nullptr);
		});
		nim_ui::SessionListManager::GetInstance()->AttachListBox(session_list);		
		page_->OnSessionListAttached();
		icon_->OnSessionListAttached();
	}
	ISessionDock* SessionPlugin::GetSessionDock() const
	{
		return page_;
	}
	ui::OptionBox* SessionPlugin::GetPluginIconAction()
	{
		return icon_->GetPluginIconAction();
	}
}
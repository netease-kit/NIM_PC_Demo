#pragma once
#include "module/plugins/main_plugins_base.h"
namespace nim_comp
{
	class CustomButtonBox;
	class SessionPluginIcon : public MainPluginIcon
	{
	public:
		static ui::Control* CreateControl(const std::wstring& pstrClass);
	public:
		SessionPluginIcon();
		virtual ~SessionPluginIcon();
	public:
		virtual void DoInit() override;
		void OnSessionListAttached();	
		virtual ui::OptionBox* GetPluginIconAction() override;
	private:
		ui::Label*		label_unread_;
		nim_comp::CustomButtonBox*	box_unread_;
		ui::OptionBox* real_option_;
	};
}
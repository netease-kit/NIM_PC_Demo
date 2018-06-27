#pragma once
#include "ui_kit_base/auto_create_object_manager.h"
namespace nim_comp
{
	class IMainPlugin : public virtual nbase::SupportWeakCallback
	{
	public:
		using PluginFlagType = std::string;
	public:
		IMainPlugin();
		virtual ~IMainPlugin();
	public:
		virtual PluginFlagType GetPluginFlag() const = 0;
		virtual ui::OptionBox* GetPluginIcon() = 0;		
		virtual ui::Box* GetPluginPage() = 0;
		virtual bool SetGroup(const std::wstring& group_name)
		{
			auto action = GetPluginIconAction();
			if (action == nullptr)
				return false;
			action->SetGroup(group_name);
			return true;
		}
		virtual bool AttachSelect(const ui::EventCallback& callback)
		{
			auto action = GetPluginIconAction();
			if (action == nullptr)
				return false;
			action->AttachSelect(callback);
			return true;
		}
		virtual bool AttachUnSelect(const ui::EventCallback& callback)
		{
			auto action = GetPluginIconAction();
			if (action == nullptr)
				return false;
			action->AttachUnSelect(callback);
			return true;
		}
		virtual void Selected(bool sel, bool btrigger) { 
			auto action = GetPluginIconAction();
			if (action != nullptr)
				action->Selected(sel, btrigger);
		}
		virtual bool IsActivePage() const { return true; }
		virtual bool ShowSearchBar() const { return true; }
		virtual void DoInit() {}
	protected:
		virtual ui::OptionBox* GetPluginIconAction() { return GetPluginIcon(); };
	};
	using MainPlugin = std::shared_ptr<IMainPlugin>;
	using MainPluginList = std::list<MainPlugin>;
	
	class MainPluginComponentBase
	{
	public:
		MainPluginComponentBase();
		virtual ~MainPluginComponentBase();
	public:
		IMainPlugin* GetPlugin() const{ return plugin_; };
		void SetPlugin(IMainPlugin* plugin){ plugin_ = plugin; }
	private:
		IMainPlugin* plugin_;
	};
	class MainPluginIcon : public ui::OptionBox, public MainPluginComponentBase
	{
	public:
		MainPluginIcon() = default;
		virtual ~MainPluginIcon() = default;
		virtual ui::OptionBox* GetPluginIconAction() { return this; }
	};
	class MainPluginPage : public ui::Box, public MainPluginComponentBase
	{
	public:
		MainPluginPage() = default;
		virtual ~MainPluginPage() = default;
	};
	template<typename TIcon = MainPluginIcon, typename TPage = MainPluginPage>
	class MainPluginBase : public IMainPlugin
	{
	public:
		MainPluginBase() : icon_(nullptr), page_(nullptr)
		{
			static_assert(std::is_base_of<MainPluginIcon, TIcon>::value && std::is_base_of<MainPluginPage, TPage>::value, "icon or page type error");
		}
		virtual ~MainPluginBase()
		{
		}
	public:
		virtual ui::OptionBox* GetPluginIcon()
		{
			if (icon_ == nullptr)
			{
				icon_ = dynamic_cast<TIcon*>(CreatePluginIcon());
				if (icon_ != nullptr)
					((MainPluginIcon*)icon_)->SetPlugin(this);
			}				
			return icon_;
		}
		virtual ui::Box* GetPluginPage()
		{
			if (page_ == nullptr)
			{
				page_ = dynamic_cast<TPage*>(CreatePluginPage());
				if (page_ != nullptr)
					((MainPluginPage*)page_)->SetPlugin(this);
			}				
			return page_;
		}
	protected:
		virtual ui::OptionBox* CreatePluginIcon() = 0;
		virtual ui::Box* CreatePluginPage() = 0;
	protected:
		TIcon* icon_;
		TPage* page_;
	};
}
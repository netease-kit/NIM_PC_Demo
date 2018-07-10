#pragma once
#include "module/plugins/main_plugins_base.h"
namespace nim_comp
{
	class ProfileBox;
	class ContactPluginPage : public MainPluginPage
	{
	public:
		ContactPluginPage();
		virtual ~ContactPluginPage();
	public:
		virtual void DoInit() override;
		void ShowProfile(UTF8String uid, bool is_robot);
		std::string GetActiveProfile() const;
	private:
		void SetActiveProfile(const std::string &uid, bool is_robot, ProfileBox* profile = nullptr);
		ProfileBox* GetProfileByID(const std::string& uid) const;
		ProfileBox* CrateProfile(const std::string &uid, bool is_robot);
		void OnFriendListChange(FriendChangeType change_type, const std::string& accid);
		void RemoveProfile(const std::string& uid);
	private:
		ui::TabBox* contact_profile_container_;
		AutoUnregister unregister_cb;
		std::function<void()> detach_list_function_;
	};
}
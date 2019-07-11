#include "contact_plugin_page.h"
#include "export\nim_ui_contacts_list_manager.h"
#include "gui/profile_form/profile_box.h"
using namespace nim_comp;
ContactPluginPage::ContactPluginPage() : 
contact_profile_container_(nullptr), detach_list_function_(nullptr)
{

}
ContactPluginPage::~ContactPluginPage()
{
	if (detach_list_function_ != nullptr)
		detach_list_function_();	
}
void ContactPluginPage::DoInit()
{
	contact_profile_container_ = dynamic_cast<ui::TabBox*>(FindSubControl(L"contact_profile_container"));
	unregister_cb.Add(UserService::GetInstance()->RegFriendListChange(nbase::Bind(&ContactPluginPage::OnFriendListChange, this, std::placeholders::_1, std::placeholders::_2)));
	ui::TreeView* friend_list = dynamic_cast<ui::TreeView*>(FindSubControl(L"friend_list"));
	detach_list_function_ = friend_list->ToWeakCallback([](){
		nim_ui::ContactsListManager::GetInstance()->AttachFriendListBox(nullptr);
	});
	nim_ui::ContactsListManager::GetInstance()->AttachFriendListBox(friend_list);	
}
void ContactPluginPage::ShowProfile(UTF8String uid, bool is_robot)
{
	SetActiveProfile(uid, is_robot);
}
ProfileBox* ContactPluginPage::GetProfileByID(const std::string& uid) const
{
	ProfileBox* ret = nullptr;
	std::wstring id = nbase::UTF8ToUTF16(uid);
	for (int i = 0; i < contact_profile_container_->GetCount(); i++)
	{
		Control *box_item = contact_profile_container_->GetItemAt(i);
		if (box_item->GetName() == id)
		{
			ret = dynamic_cast<ProfileBox*>(box_item);
			break;
		}
	}
	return ret;
}
void ContactPluginPage::SetActiveProfile(const std::string &uid, bool is_robot, ProfileBox* profile)
{
	auto box = (profile == nullptr ?  GetProfileByID(uid) : profile);
	if (box != nullptr)
	{
		contact_profile_container_->SelectItem(box);
		GetPlugin()->Selected(true, true);
	}
	else
	{
		SetActiveProfile(uid, is_robot, CrateProfile(uid, is_robot));
	}
}
ProfileBox* ContactPluginPage::CrateProfile(const std::string &uid, bool is_robot)
{
	ProfileBox* ret = new ProfileBox;
	ui::GlobalManager::FillBoxWithCache(ret, L"profile_form/profile_box.xml");
	ret->SetUTF8Name(uid);
	contact_profile_container_->Add(ret);

	// 获取用户信息
	nim::UserNameCard info;
	UserService::GetInstance()->GetUserInfo(uid, info);
	ret->InitUserInfo(info);

	return ret;
}
std::string ContactPluginPage::GetActiveProfile() const
{
	auto active = contact_profile_container_->GetItemAt(contact_profile_container_->GetCurSel());
	if (active != nullptr)
		return active->GetUTF8Name();
	return "";
}
void ContactPluginPage::OnFriendListChange(FriendChangeType change_type, const std::string& accid)
{
	if (change_type == kChangeTypeDelete)
	{
		RemoveProfile(accid);
	}
}
void ContactPluginPage::RemoveProfile(const std::string& uid)
{
	if (GetActiveProfile().compare(uid) == 0)
		contact_profile_container_->SelectItem(0);
	auto profile = GetProfileByID(uid);
	if (profile != nullptr)
		contact_profile_container_->Remove(profile);
}
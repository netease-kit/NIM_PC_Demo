#pragma once
#include "module/plugins/main_plugins_base.h"
#include "gui/extern_ctrl/treectrl/treeview.h"
#include "contact_profile_tip.h"
class AddresBookPluginPage : public nim_comp::MainPluginPage
{
public:
	AddresBookPluginPage();
	virtual ~AddresBookPluginPage();
public:
	virtual void DoInit() override;
	void InitTreeData();
private:
	void LoadDataToTree(TreeComponent* tree, TiXmlElement* config_item, const TreeDocItemPtr& parent = nullptr);
	void ShowContact(const  std::shared_ptr<MyAddressbookContact>& contact_info);
private:
	ui::TabBox* contact_profile_container_;
	TreeComponent* addres_componet_;
	ContactProfileTip* profile_tip_;
};
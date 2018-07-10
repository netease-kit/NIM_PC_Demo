#include "stdafx.h"
#include "addresbook_plugin_page.h"
#include "shared\xml_util.h"
#include "mytree.h"

AddresBookPluginPage::AddresBookPluginPage() :
contact_profile_container_(nullptr), addres_componet_(nullptr), profile_tip_(nullptr)
{

}
AddresBookPluginPage::~AddresBookPluginPage()
{

}
void AddresBookPluginPage::DoInit()
{
	contact_profile_container_ = dynamic_cast<ui::TabBox*>(FindSubControl(L"addres_profile_container"));
	profile_tip_ = new ContactProfileTip;
	ui::GlobalManager::FillBoxWithCache(profile_tip_, L"plugins/addresbook/contact_box.xml");
	contact_profile_container_->Add(profile_tip_);
	InitTreeData();
}
void AddresBookPluginPage::ShowContact(const  std::shared_ptr<MyAddressbookContact>& contact_info)
{
	if (profile_tip_ == nullptr)
		return;
	profile_tip_->Refresh(contact_info);
	contact_profile_container_->SelectItem(profile_tip_);
}
void AddresBookPluginPage::InitTreeData()
{
	auto weke_flag = contact_profile_container_->GetWeakFlag();
	auto task = [this, weke_flag](){
		addres_componet_ = new TreeComponent;
		addres_componet_->SetWindow(this->GetWindow(), nullptr, false);		
		addres_componet_->RegisterStyleUI("MyAddresbookDepartmentUI", [this]() {
			auto item = new MyAddresbookDepartmentUI;
			item->SetVirtualParent(addres_componet_);
			ui::GlobalManager::FillBoxWithCache(item, L"plugins/addresbook/department.xml");
			item->SetWindow(addres_componet_->GetWindow(), nullptr);
			item->SetOwner(addres_componet_);
			return std::shared_ptr<MyAddresbookDepartmentUI>(item);
		});
		addres_componet_->RegisterStyleUI("MyAddressbookContactUI", [this]() {
			auto item = new MyAddressbookContactUI;
			item->SetVirtualParent(addres_componet_);
			ui::GlobalManager::FillBoxWithCache(item, L"plugins/addresbook/contact.xml");
			item->SetWindow(addres_componet_->GetWindow(), nullptr);
			item->SetOwner(addres_componet_);
			return std::shared_ptr<MyAddressbookContactUI>(item);
		});	

		std::string value;
		std::wstring addresbook_path = QPath::GetAppPath();
		addresbook_path.append(L"res/plugins/addresbook.xml");
		TiXmlDocument document;
		if (shared::LoadXmlFromFile(document, addresbook_path))
		{
			TiXmlElement* item = document.RootElement();
			LoadDataToTree(addres_componet_, item);
		}
		auto task_add = [this](){
			auto componet = dynamic_cast<ui::Box*>(FindSubControl(L"addresbook_componet"));
			componet->Add(addres_componet_);
			addres_componet_->AttachSelectChange([this](const std::shared_ptr<NulTreeItem>& sel){
				auto item_info = std::dynamic_pointer_cast<MyAddresbookItemInfo>(sel);
				if (item_info != nullptr && item_info->type_ == 1)
				{
					ShowContact(std::dynamic_pointer_cast<MyAddressbookContact>(sel));
				}
				else
				{
					contact_profile_container_->SelectItem(0);
				}
				return true;
			});
			nbase::ThreadManager::PostTask(ThreadId::kThreadUI, contact_profile_container_->ToWeakCallback([this](){
				addres_componet_->Update(true);
			}));			
			FindSubControl(L"wait")->SetVisible(false);
		};
		if (!weke_flag.expired())
			nbase::ThreadManager::PostTask(ThreadId::kThreadUI, contact_profile_container_->ToWeakCallback(task_add));
	};
	nbase::ThreadManager::PostTask(ThreadId::kThreadGlobalMisc, contact_profile_container_->ToWeakCallback(task));//初始化数据放在辅助线程去做
}
void AddresBookPluginPage::LoadDataToTree(TreeComponent* tree, TiXmlElement* config_item, const TreeDocItemPtr& parent)
{
	auto doc = tree->GetDoc();
	auto AddItemTask = doc->ToWeakCallback([doc](const TreeDocItemPtr& sub_item){
		doc->AddItem(sub_item);
	});
	auto AddItemTask2 = doc->ToWeakCallback([doc](const TreeDocItemPtr& sub_item, const TreeDocItemPtr& parent){
		doc->AddItem(sub_item, parent);
	});
	std::string string_type(config_item->Attribute("type"));
	int type;
	nbase::StringToInt(string_type, &type);
	if (string_type.empty())
		return;
	TreeDocItemPtr subitem = nullptr;
	if (type == 0)//部门
	{
		auto item = std::make_shared<MyAddressbookDepartment>();
		if (config_item->QueryStringAttribute("id", &item->id_) == TIXML_NO_ATTRIBUTE)
			return;
		item->type_ = type;
		config_item->QueryStringAttribute("name", &item->name_);
		item->SetItemID(item->id_);
		item->SetTreeComponent(tree);
		subitem = item;	
		if (parent == nullptr)
			AddItemTask(subitem);
		else
			AddItemTask2(subitem, parent);
		auto child_element = config_item->FirstChildElement();
		while (child_element != nullptr)
		{
			LoadDataToTree(tree, child_element, subitem);
			child_element = child_element->NextSiblingElement();
		}
	}
	else if (type == 1)//联系人
	{
		static int head_image_index = 1;
		auto item = std::make_shared<MyAddressbookContact>();

		if (config_item->QueryStringAttribute("id", &item->id_) == TIXML_NO_ATTRIBUTE)
			return;
		item->type_ = type;
		config_item->QueryStringAttribute("name", &item->name_);
		config_item->QueryStringAttribute("mobile", &item->moblie_);
		config_item->QueryStringAttribute("mail", &item->mail_);
		config_item->QueryStringAttribute("employeesa", &item->employeesa_);
		int sa;
		nbase::StringToInt(item->employeesa_, &sa);
		item->head_image_path_.append("../plugins/addresbook/head_portrait_").append(nbase::IntToString(head_image_index++)).append(".png");

		item->SetItemID(item->id_);
		item->SetTreeComponent(tree);
		subitem = item;
		if (parent == nullptr)
			AddItemTask(subitem);
		else
			AddItemTask2(subitem, parent);
		if (head_image_index > 10)
			head_image_index = 1;
	}

}
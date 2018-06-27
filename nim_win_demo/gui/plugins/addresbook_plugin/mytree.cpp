#include "stdafx.h"
#include "mytree.h"
std::string MyAddressbookDepartment::OnGetIUIStyleName() const 
{ 
	return "MyAddresbookDepartmentUI"; 
}
std::string MyAddressbookContact::OnGetIUIStyleName() const
{ 
	return "MyAddressbookContactUI"; 
}
int MyAddresbookDepartmentUI::GetHeight() 
{ 
	return 30; 
};
void MyAddresbookDepartmentUI::OnFill()
{
	auto&& item_data = std::dynamic_pointer_cast<MyAddresbookDepartmentInfo>(doc_item_);
	if (item_data == nullptr)
		return;
	auto label = dynamic_cast<ui::Label*>(FindSubControl(L"contact"));
	label->SetUTF8Text(item_data->name_);
}
int MyAddressbookContactUI::GetHeight() 
{ 
	return 48; 
};
void MyAddressbookContactUI::OnFill()
{
	auto&& item_data = std::dynamic_pointer_cast<MyAddresbookContactInfo>(doc_item_);
	if (item_data == nullptr)
		return;
	auto label = dynamic_cast<ui::Label*>(FindSubControl(L"contact"));
	if (label != nullptr)
		label->SetUTF8Text(item_data->name_);
	label = dynamic_cast<ui::Label*>(FindSubControl(L"mail"));
	if (label != nullptr)
		label->SetUTF8Text(item_data->mail_);
	auto head = FindSubControl(L"contact_image");
	if (head != nullptr)		
		head->SetUTF8BkImage(item_data->head_image_path_);
}
#pragma once
#include "mytree.h"
class ContactProfileTip : public ui::VBox 
{ 
public:
	ContactProfileTip() : head_image_(nullptr), name_(nullptr), moblie_(nullptr), mail_(nullptr), employeesa_(nullptr)
	{
	}
	~ContactProfileTip() = default;
public:
	virtual void DoInit() override
	{
		head_image_ = FindSubControl(L"head_image");
		name_ = dynamic_cast<ui::Label*>(FindSubControl(L"name"));
		moblie_ = dynamic_cast<ui::Label*>(FindSubControl(L"phone"));
		mail_ = dynamic_cast<ui::Label*>(FindSubControl(L"email"));
		employeesa_ = dynamic_cast<ui::Label*>(FindSubControl(L"employeesa"));
	}
	void Refresh(const  std::shared_ptr<MyAddressbookContact>& contact_info)
	{
		head_image_->SetUTF8BkImage(contact_info->head_image_path_);
		name_->SetUTF8Text(contact_info->name_);
		moblie_->SetUTF8Text(contact_info->moblie_);
		mail_->SetUTF8Text(contact_info->mail_);
		employeesa_->SetUTF8Text(contact_info->employeesa_);
	}
private:
	ui::Control* head_image_;
	ui::Label* name_;
	ui::Label* moblie_;
	ui::Label* mail_;
	ui::Label* employeesa_;
};
#include "profile_form.h"
#include "profile_box.h"
#include "module/plugins/main_plugins_manager.h"
#include "gui/plugins/contact/contact_plugin.h"
namespace nim_comp
{
const LPCTSTR ProfileForm::kClassName = L"ProfileForm";
void ProfileForm::ShowProfileForm(UTF8String uid, bool is_robot, bool inplugin_box)
{
	if (inplugin_box)
		MainPluginsManager::GetInstance()->GetPlugin<ContactPlugin>(ContactPlugin::kPLUGIN_NAME)->ShowProfileForm(uid,is_robot);
	else
		ShowProfileForm(uid, is_robot);
}
ProfileForm * ProfileForm::ShowProfileForm(UTF8String uid, bool is_robot/* = false*/)
{
	ProfileForm* form = (ProfileForm*)WindowsManager::GetInstance()->GetWindow(kClassName, kClassName);
	if (form != NULL && form->GetNameCard().GetAccId().compare(uid) == 0) //当前已经打开的名片正是希望打开的名片
		; // 直接显示
	else
	{
		if (form != NULL && form->GetNameCard().GetAccId().compare(uid) != 0)//当前已经打开的名片不是希望打开的名片
			::DestroyWindow(form->m_hWnd); //关闭重新创建
		form = new ProfileForm();
		form->Create(NULL, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0L);
		
		// 获取用户信息
		nim::UserNameCard info;
		UserService::GetInstance()->GetUserInfo(uid, info);
		form->InitUserInfo(info);
	}
	if (!::IsWindowVisible(form->m_hWnd))
	{
		::ShowWindow(form->m_hWnd, SW_SHOW);
		form->CenterWindow();
	}
	form->__super::ActiveWindow();
	return form;
}

ProfileForm *ProfileForm::ShowProfileForm(UTF8String tid, UTF8String uid, nim::NIMTeamUserType my_type)
{
	ProfileForm* form = (ProfileForm*)WindowsManager::GetInstance()->GetWindow(kClassName, kClassName);
	if (form != NULL && form->GetNameCard().GetAccId().compare(uid) == 0 && form->tid_ == tid) //当前已经打开的名片正是希望打开的名片
		; // 直接显示
	else
	{
		if (form != NULL && (form->GetNameCard().GetAccId().compare(uid) != 0 || form->tid_ != tid))//当前已经打开的名片不是希望打开的名片
			::DestroyWindow(form->m_hWnd); //关闭重新创建
		form = new ProfileForm(tid, uid, my_type);
		form->Create(NULL, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0L);

		// 获取用户信息
		nim::UserNameCard info;
		UserService::GetInstance()->GetUserInfo(uid, info);
		form->InitUserInfo(info);
	}
	if (!::IsWindowVisible(form->m_hWnd))
	{
		::ShowWindow(form->m_hWnd, SW_SHOW);
		form->CenterWindow();
	}
	form->__super::ActiveWindow();
	return form;
}

ProfileForm::ProfileForm()
{
	contant_ = new ProfileBox;
}
ProfileForm::ProfileForm(UTF8String tid, UTF8String uid, nim::NIMTeamUserType my_type)
{
	contant_ = new ProfileBox(tid, uid, my_type);
	contant_->AttachClose(std::bind(&ProfileForm::Close, this, std::placeholders::_1));
	contant_->AttachSetTaskbarTitle(std::bind(&ProfileForm::SetTaskbarTitle, this, std::placeholders::_1));
}
ProfileForm::~ProfileForm()
{

}

std::wstring ProfileForm::GetSkinFolder()
{
	return L"profile_form";
}

std::wstring ProfileForm::GetSkinFile()
{
	return L"profile_form.xml";
}

std::wstring ProfileForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring ProfileForm::GetWindowId() const
{
	return kClassName;
}

void ProfileForm::InitWindow()
{	
	std::wstring profile_box = L"profile_form/profile_box.xml";
	if(ui::GlobalManager::GetLanguageSetting().m_enumType == ui::LanguageType::American_English)
		profile_box = L"profile_form/profile_box_en.xml";
	ui::GlobalManager::FillBoxWithCache(contant_, profile_box);
	auto container = dynamic_cast<ui::Box*>(FindControl(L"container"));
	container->Add(contant_);
}
void ProfileForm::InitUserInfo(const nim::UserNameCard & info)
{
	contant_->InitUserInfo(info);
}
nim::UserNameCard	ProfileForm::GetNameCard() const
{
	return contant_->GetNameCard();
}
void ProfileForm::SetTaskbarTitle(const std::wstring &title)
{
	WindowEx::SetTaskbarTitle(title);
}
void ProfileForm::Close(UINT nRet/* = 0*/)
{
	WindowEx::Close(nRet);
}
void ProfileForm::OnMultiportPushConfigChange(bool switch_on)
{
	contant_->OnMultiportPushConfigChange(switch_on);
}
}
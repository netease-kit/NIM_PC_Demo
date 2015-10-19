#include "nim_ui_window_manager.h"
#include "gui/profile_form/profile_form.h"
#include "gui/team_info/team_info.h"
#include "gui/link/link_form.h"
#include "gui/rts/rts_replay.h"
#include "gui/team_info/team_search.h"
#include "shared/modal_wnd/async_do_modal.h"
#include "module/video/video_manager.h"
#include "module/login/login_manager.h"

// namespace nim_ui
// {
// 
// WindowsManager::WindowsManager()
// {
// 
// }
// 
// WindowsManager::~WindowsManager()
// {
// 
// }
// 
// // bool WindowsManager::RegisterWindow(const std::wstring wnd_class_name, const std::wstring wnd_id, nim_comp::WindowEx *wnd)
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->RegisterWindow(wnd_class_name, wnd_id, wnd);
// // }
// // 
// // void WindowsManager::UnRegisterWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id, nim_comp::WindowEx *wnd)
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->UnRegisterWindow(wnd_class_name, wnd_id, wnd);
// // }
// // 
// // nim_comp::WindowList WindowsManager::GetAllWindows()
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->GetAllWindows();
// // }
// // 
// // nim_comp::WindowEx* WindowsManager::GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id)
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->GetWindow(wnd_class_name, wnd_id);
// // }
// // 
// // nim_comp::WindowList WindowsManager::GetWindowsByClassName(LPCTSTR classname)
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->GetWindowsByClassName(classname);
// // }
// // 
// // void WindowsManager::DestroyAllWindows()
// // {
// // 	return nim_comp::WindowsManager::GetInstance()->DestroyAllWindows();
// // }
// // void SetStopRegister(bool stop)
// // { 
// // 	return nim_comp::WindowsManager::GetInstance()->SetStopRegister(stop);
// // }
// // 
// // bool IsStopRegister()
// // { 
// // 	return nim_comp::WindowsManager::GetInstance()->IsStopRegister();
// // }
// // 
// // template<typename WindowType>
// // WindowType* WindowsManager::SingletonShow(const std::wstring& window_id)
// // {
// // 	return nim_comp::WindowsManager::SingletonShow(window_id);
// // }
// 
// void WindowsManager::ShowProfileForm()
// {
// 	nim_comp::ProfileForm::ShowProfileForm(nim_comp::LoginManager::GetInstance()->GetAccount());
// }
// 
// void WindowsManager::ShowLinkForm()
// {
// 	nim_comp::ShowLinkForm();
// }
// 
// void WindowsManager::ShowRtsReplyForm()
// {
// 	/*nim_comp::*/WindowsManager::SingletonShow<nim_comp::RtsReplay>(nim_comp::RtsReplay::kClassName);
// }
// 
// void WindowsManager::ShowVideoSettingForm()
// {
// 	nim_comp::VideoManager::GetInstance()->ShowVideoSetting();
// }
// 
// void WindowsManager::ShowTeamSearchForm()
// {
// 	/*nim_comp::*/WindowsManager::SingletonShow<nim_comp::TeamSearchForm>(nim_comp::TeamSearchForm::kClassName);
// }
// 
// void WindowsManager::ShowTeamInfoForm(nim::NIMTeamType team_type, LPCWSTR caption)
// {
// 	if (team_type == nim::kNIMTeamTypeNormal)
// 	{
// 		nim_comp::TeamInfoForm * team_info_form = (nim_comp::TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
// 			(nim_comp::TeamInfoForm::kClassName, nim_comp::TeamInfoForm::kGroupInfoWindowId);
// 		if (team_info_form == NULL)
// 		{
// 			team_info_form = new nim_comp::TeamInfoForm(true, nim::kNIMTeamTypeNormal, "", nim::TeamInfo());
// 			team_info_form->Create(NULL, caption, WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
// 			team_info_form->CenterWindow();
// 			team_info_form->ShowWindow(true);
// 		}
// 		else
// 		{
// 			team_info_form->ActiveWindow();
// 		}
// 	}
// 	else if (team_type == nim::kNIMTeamTypeAdvanced)
// 	{
// 		nim_comp::TeamInfoForm* team_info_form = (nim_comp::TeamInfoForm*)WindowsManager::GetInstance()->GetWindow\
// 			(nim_comp::TeamInfoForm::kClassName, nim_comp::TeamInfoForm::kTeamInfoWindowId);
// 		if (team_info_form == NULL)
// 		{
// 			team_info_form = new nim_comp::TeamInfoForm(true, nim::kNIMTeamTypeAdvanced, "", nim::TeamInfo());
// 			team_info_form->Create(NULL, caption, WS_OVERLAPPEDWINDOW& ~WS_MAXIMIZEBOX, 0L);
// 			team_info_form->CenterWindow();
// 			team_info_form->ShowWindow(true);
// 		}
// 		else
// 		{
// 			team_info_form->ActiveWindow();
// 		}
// 	}
// }
// 
// void WindowsManager::CancelAllAsyncForms()
// {
// 	CancelAllAsyncModalDialogs();
// }
// 
// void WindowsManager::RegShowMainForm(const OnShowMainWindow& cb)
// {
// 	if (cb)
// 		cb_show_main_window_ = cb;
// }
// 
// void WindowsManager::InvokeShowMainForm()
// {
// 	if (cb_show_main_window_)
// 		cb_show_main_window_();
// }
// 
// 
// }
// 

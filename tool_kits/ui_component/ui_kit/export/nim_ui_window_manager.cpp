#include "nim_ui_window_manager.h"
#include "module/login/login_manager.h"
#include "gui/toast/toast.h"

namespace nim_ui
{

nim_comp::WindowList WindowsManager::GetAllWindows()
{
	return nim_comp::WindowsManager::GetInstance()->GetAllWindows();
}

nim_comp::WindowEx* WindowsManager::GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id)
{
	return nim_comp::WindowsManager::GetInstance()->GetWindow(wnd_class_name, wnd_id);
}

nim_comp::WindowList WindowsManager::GetWindowsByClassName(LPCTSTR classname)
{
	return nim_comp::WindowsManager::GetInstance()->GetWindowsByClassName(classname);
}

void WindowsManager::DestroyAllWindows()
{
	return nim_comp::WindowsManager::GetInstance()->DestroyAllWindows();
}

void WindowsManager::ShowProfileForm(UTF8String uid)
{
	nim_comp::ProfileForm::ShowProfileForm(uid);
}

void WindowsManager::ShowLinkForm()
{
	nim_comp::ShowLinkForm((nim::NIMResCode)nim_comp::LoginManager::GetInstance()->GetErrorCode(), true);
}

void WindowsManager::ShowVideoSettingForm()
{
	nim_comp::VideoManager::GetInstance()->ShowVideoSetting();
}

void ShowToast(const std::wstring &content, int duration, HWND parent)
{
	nim_comp::Toast::ShowToast(content, duration, parent);
}

}


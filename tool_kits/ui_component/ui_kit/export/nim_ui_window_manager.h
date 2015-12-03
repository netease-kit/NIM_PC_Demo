#pragma once
#include "nim_ui_dll.h"
#include "util/windows_manager.h"

//Demo可能会使用的ui_kit窗体
#include "gui/add_friend/add_friend_wnd.h"
#include "gui/black_list/black_list_wnd.h"
#include "gui/broad/broad_form.h"
#include "gui/main/team_event_form.h"
#include "gui/profile_form/profile_form.h"
#include "gui/link/link_form.h"
#include "gui/team_info/team_info.h"
#include "gui/rts/rts_replay.h"
#include "gui/team_info/team_search.h"
#include "module/video/video_manager.h"

namespace nim_ui
{

/** @class WindowsManager
  * @brief 提供所有窗体的控制接口
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2015/9/16
  */
class NIM_UI_DLL_API WindowsManager
{
public:
	SINGLETON_DEFINE(WindowsManager);
public:
	WindowsManager();
	~WindowsManager();
public:

 	//根据窗口类名和id获取窗口
 	nim_comp::WindowEx* GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id);
	//获取所有窗口
	nim_comp::WindowList GetAllWindows();
	//获取指定class对应的所有窗口
	nim_comp::WindowList GetWindowsByClassName(LPCTSTR classname);
	//关闭所有窗口
	void DestroyAllWindows();

 	template<typename WindowType> 
	static WindowType* SingletonShow(const std::wstring& window_id)
 	{
		return nim_comp::WindowsManager::SingletonShow<WindowType>(window_id);

 	}
	void ShowProfileForm(UTF8String uid);
 	void ShowLinkForm();
	void ShowVideoSettingForm();
};

}
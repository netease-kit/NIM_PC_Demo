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
	WindowsManager(){};
	~WindowsManager(){};
public:

	/**
	* 根据窗口类名和id获取窗口
	* @param[in] wnd_class_name 窗口类名
	* @param[in] wnd_id 窗口id
	* @return WindowEx* 窗口指针
	*/
	nim_comp::WindowEx* GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id);

	/**
	* 获取所有窗口获取窗口
	* @return WindowList 窗口列表
	*/
	nim_comp::WindowList GetAllWindows();

	/**
	* 根据窗口类名获取对应的所有窗口
	* @param[in] classname 窗口类名
	* @return WindowList 窗口列表
	*/
	nim_comp::WindowList GetWindowsByClassName(LPCTSTR classname);

	/**
	* 销毁所有窗口
	* @return void 无返回值
	*/
	void DestroyAllWindows();

	/**
	* 根据窗口id创建一个唯一存在的窗口
	* @param[in] window_id 窗口id
	* @return WindowType* 窗口指针
	*/
 	template<typename WindowType> 
	static WindowType* SingletonShow(const std::wstring& window_id)
 	{
		return nim_comp::WindowsManager::SingletonShow<WindowType>(window_id);

 	}

	/**
	* 显示用户信息窗口
	* @param[in] uid 用户id
	* @return void	无返回值
	*/
	void ShowProfileForm(UTF8String uid);

	/**
	* 显示断线重连窗口
	* @return void	无返回值
	*/
 	void ShowLinkForm();

	/**
	* 显示音视频设置窗口
	* @return void	无返回值
	*/
	void ShowVideoSettingForm();
};

/**
* 显示一个简易的提示框
* @param[in] content 显示内容
* @param[in] duration 显示时长(单位毫秒，0表示一直显示)
* @param[in] parent 父窗口句柄
* @return void	无返回值
*/
void ShowToast(const std::wstring &content, int duration = 0, HWND parent = NULL);
}
#pragma once
// 原本是作为ui组件分离窗体组件使用的,本文件暂时弃用
#include "nim_ui_dll.h"
#include "util/windows_manager.h"


// namespace nim_ui
// {
// 	typedef std::function<void()> OnShowMainWindow;
// 
// /** @class WindowsManager
//   * @brief 提供所有窗体的控制接口
//   * @copyright (c) 2015, NetEase Inc. All rights reserved
//   * @author Redrain
//   * @date 2015/9/16
//   */
// class NIM_UI_DLL_API WindowsManager
// {
// public:
// 	SINGLETON_DEFINE(WindowsManager);
// public:
// 	WindowsManager();
// 	~WindowsManager();
// public:
// 
// // 	//根据窗口类名和id注册窗口
// // 	bool RegisterWindow(const std::wstring wnd_class_name, const std::wstring wnd_id, nim_comp::WindowEx *wnd);
// // 	//根据窗口类名和id注销窗口
// // 	void UnRegisterWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id, nim_comp::WindowEx *wnd);
// // 	//根据窗口类名和id获取窗口
// // 	nim_comp::WindowEx* GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id);
// // 	//获取所有窗口
// // 	nim_comp::WindowList GetAllWindows();
// // 	//获取指定class对应的所有窗口
// // 	nim_comp::WindowList GetWindowsByClassName(LPCTSTR classname);
// // 	//关闭所有窗口
// // 	void DestroyAllWindows();
// // 	//设置禁止窗口创建
// // 	void SetStopRegister(bool stop = true);
// // 	//是否禁止窗口创建
// // 	bool IsStopRegister();
// // 
// // 	template<typename WindowType> 
// // 	static WindowType* SingletonShow(const std::wstring& window_id);
// 
// 	//注册显示主窗体的回调函数
// 	void RegShowMainForm(const OnShowMainWindow& cb);
// 	void InvokeShowMainForm();
// 
// 	void ShowProfileForm();
// 	void ShowLinkForm();
// 	void ShowRtsReplyForm();
// 	void ShowVideoSettingForm();
// 	void ShowTeamSearchForm();
// 	void ShowTeamInfoForm(nim::NIMTeamType team_type, LPCWSTR caption);
// 	void CancelAllAsyncForms();
// 
// private:
// 	OnShowMainWindow cb_show_main_window_;
// 
// };
// 
// }
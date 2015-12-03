#pragma once
#include "window_ex.h"

namespace nim_comp
{
//map<窗口类名，map<窗口id，窗口指针>>, 如果同一类只有一个窗口，使用类名作为id
typedef std::map<std::wstring, std::map<std::wstring, WindowEx*>> WindowsMap; 
typedef std::list<WindowEx *> WindowList;


class WindowsManager
{
public:
	SINGLETON_DEFINE(WindowsManager);
	WindowsManager();
	virtual ~WindowsManager();

	//根据窗口类名和id注册窗口
	bool RegisterWindow(const std::wstring wnd_class_name, const std::wstring wnd_id, WindowEx *wnd);
	//根据窗口类名和id注销窗口
	void UnRegisterWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id, WindowEx *wnd);
	//根据窗口类名和id获取窗口
	WindowEx* GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id);
	//获取所有窗口
	WindowList GetAllWindows();
	//获取指定class对应的所有窗口
	WindowList GetWindowsByClassName(LPCTSTR classname);
	//关闭所有窗口
	void DestroyAllWindows();
	//设置禁止窗口创建
	void SetStopRegister(bool stop=true){stop_register_ = stop;}
	//是否禁止窗口创建
	bool IsStopRegister(){return stop_register_;}

	template<typename WindowType>
	static WindowType* SingletonShow(const std::wstring& window_id)
	{
		WindowType *window = (WindowType*)(WindowsManager::GetInstance()->GetWindow(WindowType::kClassName, window_id));
		if (!window)
		{
			window = new WindowType;
			window->Create(NULL, WindowType::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
			window->CenterWindow();
			window->ShowWindow();
		}
		else
		{
			window->ActiveWindow();
		}

		return window;
	}

private:
	WindowsMap					windows_map_;	//所有窗口
	std::string					user_id_;
	bool						stop_register_;	//禁止窗口创建
};
}
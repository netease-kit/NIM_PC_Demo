#include "stdafx.h"
#include "windows_manager.h"
#include "shared/log.h"

namespace nim_comp
{
WindowsManager::WindowsManager()
{
	stop_register_ = false;
	windows_map_.clear();
}

WindowsManager::~WindowsManager()
{
	windows_map_.clear();
}

bool WindowsManager::RegisterWindow(const std::wstring wnd_class_name, const std::wstring wnd_id, WindowEx *wnd)
{
	if (IsStopRegister())
	{
		assert(!stop_register_);
		return false;
	}
	WindowsMap::iterator it = windows_map_.find(wnd_class_name);
	if (it != windows_map_.end())
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>>::iterator it2 = it->second.find(wnd_id);
		if (it2 != it->second.end())
		{
			QLOG_PRO(L"The window <class name: {0}, id: {1}> has already registered !")<<wnd_class_name <<wnd_id;
		}
		it->second[wnd_id] = std::make_pair(wnd->GetWeakFlag(),wnd);
	}
	else
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>> id_win;
		id_win[wnd_id] = std::make_pair(wnd->GetWeakFlag(), wnd);;
		windows_map_[wnd_class_name] = id_win;
	}
	return true;
}

void WindowsManager::UnRegisterWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id, WindowEx *wnd)
{
	WindowsMap::iterator it = windows_map_.find(wnd_class_name);
	if (it != windows_map_.end())
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>>::iterator it2 = it->second.find(wnd_id);
		if (it2 != it->second.end())
		{
			it->second.erase(it2);
		}
	}
}
	
WindowList WindowsManager::GetAllWindows()
{
	WindowList list;
	WindowsMap::iterator it = windows_map_.begin();
	for ( ; it != windows_map_.end(); ++it)
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>>::iterator it2 = it->second.begin();
		for ( ; it2 != it->second.end(); ++it2)
		{
			std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*> _wnd = it2->second;
			//WindowEx *wnd = (WindowEx *)(it2->second);
			if (!_wnd.first.expired() && _wnd.second != nullptr && ::IsWindow(_wnd.second->GetHWND()))
			{
				list.push_back(_wnd);
			}
		}
	}
	return list;
}

WindowEx* WindowsManager::GetWindow(const std::wstring &wnd_class_name, const std::wstring &wnd_id)
{
	WindowsMap::iterator it = windows_map_.find(wnd_class_name);
	if(it != windows_map_.end())
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>>::iterator it2 = it->second.find(wnd_id);
		if (it2 != it->second.end())
		{
			std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*> _wnd = it2->second;
			if (!_wnd.first.expired() && _wnd.second != nullptr && ::IsWindow(_wnd.second->GetHWND()))
			{
				return _wnd.second;
			}
		}
	}
	return nullptr;
}

WindowList WindowsManager::GetWindowsByClassName(LPCTSTR classname)
{
	WindowList wnd_list;
	WindowsMap::iterator it = windows_map_.find(classname);
	if(it != windows_map_.end())
	{
		std::map<std::wstring, std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*>>::iterator it2 = it->second.begin();
		for(; it2 != it->second.end(); it2++)
		{
			std::pair<std::weak_ptr<nbase::WeakFlag>, WindowEx*> _wnd = it2->second;
			if (!_wnd.first.expired() && _wnd.second != nullptr && ::IsWindow(_wnd.second->GetHWND()))
			{
				wnd_list.push_back(_wnd);
			}
		}
	}
	return wnd_list;
}

void WindowsManager::DestroyAllWindows()
{
	SetStopRegister();

	WindowList lst_wnd = GetAllWindows();
	WindowList::iterator it = lst_wnd.begin();
	for ( ; it != lst_wnd.end(); ++it)
	{
		if (!it->first.expired() && it->second != nullptr && ::IsWindow(it->second->GetHWND()))
		{
			::DestroyWindow(it->second->GetHWND());
		}
	}
}
}
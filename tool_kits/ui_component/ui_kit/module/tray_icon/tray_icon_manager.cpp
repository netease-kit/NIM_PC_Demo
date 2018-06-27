#include "stdafx.h"
#include "tray_icon_manager.h"
namespace nim_comp
{
	TrayIconManager::TrayIconManager()
	{
		tray_icon_.reset(new TrayIcon);
		tray_icon_->Init(this);
	}
	TrayIconManager::~TrayIconManager()
	{

	}

	void TrayIconManager::AddTrayIconEventHandler(ITrayIconEventHandler* handler)
	{
		handler_list_.emplace_back(std::make_pair(handler->GetWeakFlag(), handler));
	}
	void TrayIconManager::Destroy()
	{
		tray_icon_->Destroy();
		tray_icon_.release();
	}
	void TrayIconManager::SetTrayIcon(HICON icon, const std::wstring& tray_icon_text)
	{
		tray_icon_->SetTrayIcon(icon, tray_icon_text);
	}
	void TrayIconManager::LoadIconList(int icon_res_start_index, int count)
	{
		tray_icon_->LoadIconList(icon_res_start_index, count);
	}
	void TrayIconManager::SetAnimateTray(const std::vector<int>& aniIndex, int anim_escape_time)
	{
		tray_icon_->SetAnimateTray(aniIndex, anim_escape_time);
	}
	void TrayIconManager::StartTrayIconAnimate()
	{
		tray_icon_->StartTrayIconAnimate();
	}
	void TrayIconManager::StopTrayIconAnimate()
	{
		tray_icon_->StopTrayIconAnimate();
	}
	UINT TrayIconManager::GetTaskbarCreatedMsgId()
	{
		return tray_icon_->GetTaskbarCreatedMsgId();
	}

	void TrayIconManager::LeftClick()
	{
		auto it = handler_list_.begin();
		while (it != handler_list_.end())
		{
			if (it->first.expired())
				it = handler_list_.erase(it);
			else
				if (it++->second->LeftClick())
					break;
		}
	}
	void TrayIconManager::LeftDoubleClick()
	{
		auto it = handler_list_.begin();
		while (it != handler_list_.end())
		{
			if (it->first.expired())
				it = handler_list_.erase(it);
			else
				if (it++->second->LeftDoubleClick())
					break;
		}
	}
	void TrayIconManager::RightClick()
	{
		auto it = handler_list_.begin();
		while (it != handler_list_.end())
		{
			if (it->first.expired())
				it = handler_list_.erase(it);
			else
				if (it++->second->RightClick())
					break;
		}
	}
	void TrayIconManager::RightDoubleClick()
	{
		auto it = handler_list_.begin();
		while (it != handler_list_.end())
		{
			if (it->first.expired())
				it = handler_list_.erase(it);
			else
				if (it++->second->RightDoubleClick())
					break;
		}
	}
}
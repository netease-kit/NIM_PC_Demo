#include "stdafx.h"
#include "nim_ui_runtime_manager.h"
#include "module/runtime_data/runtime_data_manager.h"
namespace nim_ui
{
	void RunTimeDataManager::SetUIStyle(UIStyle style)
	{
		nim_comp::RunTimeDataManager::GetInstance()->SetUIStyle(style);
	}
	UIStyle RunTimeDataManager::GetUIStyle() const
	{
		return nim_comp::RunTimeDataManager::GetInstance()->GetUIStyle();
	}
	UnregisterCallback RunTimeDataManager::RegSDKInited(const SDKInitCallback& cb)
	{
		return sdk_init_cb_.AddCallback(cb);
	}
	void RunTimeDataManager::SetSDKInited()
	{
		nim_comp::RunTimeDataManager::GetInstance()->SetSDKInited(true);
		sdk_init_cb_();
	}

	bool RunTimeDataManager::IsSDKInited() const
	{
		return nim_comp::RunTimeDataManager::GetInstance()->IsSDKInited();
	}

}